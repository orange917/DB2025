/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"
#include "transaction/txn_defs.h"  // 添加这个头文件以使用TransactionAbortException

class InsertExecutor : public AbstractExecutor {
   private:
    TabMeta tab_;                   // 表的元数据
    std::vector<Value> values_;     // 需要插入的数据
    RmFileHandle *fh_;              // 表的数据文件句柄
    std::string tab_name_;          // 表名称
    Rid rid_;                       // 插入的位置，由于系统默认插入时不指定位置，因此当前rid_在插入后才赋值
    SmManager *sm_manager_;

   public:
    InsertExecutor(SmManager *sm_manager, const std::string &tab_name, std::vector<Value> values, Context *context) {
        sm_manager_ = sm_manager;
        tab_ = sm_manager_->db_.get_table(tab_name);
        values_ = values;
        tab_name_ = tab_name;
        if (values.size() != tab_.cols.size()) {
            throw InvalidValueCountError();
        }
        fh_ = sm_manager_->fhs_.at(tab_name).get();
        context_ = context;
    };

    std::unique_ptr<RmRecord> Next() override {
        // Make record buffer
        RmRecord rec(fh_->get_file_hdr().record_size);

        // 在MVCC模式下，需要在记录前面预留TupleMeta空间
        int tuple_data_offset = 0;
        bool is_mvcc = (context_ != nullptr && context_->txn_mgr_ != nullptr && 
                       context_->txn_mgr_->get_concurrency_mode() == ConcurrencyMode::MVCC);
        
        if (is_mvcc) {
            // 在记录前面设置TupleMeta
            TupleMeta meta;

            // 如果有事务上下文，使用事务的开始时间戳；否则使用时间戳0
            if (context_->txn_ != nullptr) {
                meta.ts_ = context_->txn_->get_start_ts();
            } else {
                // 对于非事务的INSERT（自动提交），使用时间戳0
                meta.ts_ = 0;
            }

            meta.is_deleted_ = false;
            memcpy(rec.data, &meta, sizeof(TupleMeta));
            tuple_data_offset = sizeof(TupleMeta);
        }
        
        for (size_t i = 0; i < values_.size(); i++) {
            auto &col = tab_.cols[i];
            auto &val = values_[i];
            // 类型对齐与转换
            if (col.type != val.type) {
                if (col.type == TYPE_FLOAT && val.type == TYPE_INT) {
                    // 将 INT 转换为 FLOAT
                    val.float_val = static_cast<float>(val.int_val);
                    val.type = TYPE_FLOAT;
                } else if (col.type == TYPE_INT && val.type == TYPE_FLOAT) {
                    // 将 FLOAT 转换为 INT
                    val.int_val = static_cast<int>(val.float_val);
                    val.type = TYPE_INT;
                } else {
                    // 其他类型不匹配的情况仍然抛出异常
                    throw IncompatibleTypeError(coltype2str(col.type), coltype2str(val.type));
                }
            }

            val.init_raw(col.len);
            memcpy(rec.data + tuple_data_offset + col.offset, val.raw->data, col.len);
        }
        
        // **修复：在MVCC模式下进行写-写冲突检查**
        // 主要检查是否有并发事务正在操作相同的记录
        if (is_mvcc) {
            // 对于INSERT操作，主要的写-写冲突发生在：
            // 1. 尝试插入相同的主键值
            // 2. 尝试插入相同的唯一键值
            // 这种情况下，如果另一个事务正在修改具有相同键值的记录，就存在冲突
            
            for (size_t i = 0; i < tab_.indexes.size(); ++i) {
                auto& index = tab_.indexes[i];
                if (!index.unique) continue;  // 只检查唯一索引
                
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* key = new char[index.col_tot_len];
                int offset = 0;
                for (size_t j = 0; j < index.col_num; ++j) {
                    memcpy(key + offset, rec.data + tuple_data_offset + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                
                // 检查是否存在相同的键值
                std::vector<Rid> existing_rids;
                try {
                    ih->get_value(key, &existing_rids, context_->txn_);
                    
                    // 如果找到了相同的键值，检查是否存在写-写冲突
                    for (const Rid& existing_rid : existing_rids) {
                        // 检查该记录是否被并发事务锁定或修改
                        if (fh_->CheckWriteWriteConflict(existing_rid, context_)) {
                            std::cout << "[CONFLICT] Write-write conflict detected during INSERT operation" 
                                      << " - another transaction is modifying record with same unique key" << std::endl;
                            delete[] key;
                            throw TransactionAbortException(context_->txn_->get_transaction_id(), AbortReason::DEADLOCK_PREVENTION);
                        }
                    }
                } catch (const TransactionAbortException&) {
                    delete[] key;
                    throw;  // 重新抛出事务中止异常
                } catch (...) {
                    // 其他异常（如索引不存在等）忽略，继续正常的唯一性检查
                }
                
                delete[] key;
            }
        }
        
        // Insert into record file
        rid_ = fh_->insert_record(rec.data, context_);

        // 更新 row_count
        tab_.row_count++;
        sm_manager_->update_table_row_count(tab_name_, tab_.row_count);

        // 添加到事务的写集合中
        if (context_ != nullptr && context_->txn_ != nullptr) {
            if (is_mvcc) {
                // **MVCC模式：创建撤销日志**
                UndoLog undo_log;
                undo_log.ts_ = 0;  // INSERT操作的撤销日志使用时间戳0，表示删除该记录
                undo_log.is_deleted_ = true;  // 撤销INSERT就是删除
                undo_log.tuple_.clear();  // INSERT的撤销不需要保存数据
                undo_log.modified_fields_.assign(tab_.cols.size(), true);
                undo_log.prev_version_ = UndoLink{};  // INSERT没有前一个版本
                
                // 将撤销日志添加到事务
                auto undo_link = context_->txn_->AppendUndoLog(undo_log);
                
                // 创建版本链
                VersionUndoLink version_link;
                version_link.prev_ = undo_link;
                version_link.in_progress_ = true;
                context_->txn_mgr_->UpdateVersionLink(rid_, version_link);
            } else {
                // **非MVCC模式：传统的写记录**
                WriteRecord* write_record = new WriteRecord(WType::INSERT_TUPLE, tab_name_, rid_);
                context_->txn_->append_write_record(write_record);
            }
        }

        // Insert into index
        try {
            for(size_t i = 0; i < tab_.indexes.size(); ++i) {
                auto& index = tab_.indexes[i];
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* key = new char[index.col_tot_len];
                int offset = 0;
                for(size_t j = 0; j < index.col_num; ++j) {
                    memcpy(key + offset, rec.data + tuple_data_offset + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                ih->insert_entry(key, rid_, context_->txn_);
                delete[] key;
            }
        } catch (const std::exception& e) {
            // 回滚主表插入
            fh_->delete_record(rid_, context_);
            throw;
        }
        return nullptr;
    }
    Rid &rid() override { return rid_; }
};