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
        
        // **关键修复：在MVCC模式下进行写-写冲突检查**
        // 解决问题：当事务A删除记录但未提交时，事务B插入相同ID应该抛出abort
        if (is_mvcc) {
            // 检查要插入的主键是否与现有记录冲突
            if (!tab_.cols.empty()) {
                // 假设第一列是主键（id列）
                const auto& first_col = tab_.cols[0];
                Value key_value;
                
                // 提取要插入记录的主键值
                switch (first_col.type) {
                    case TYPE_INT:
                        key_value.set_int(*reinterpret_cast<int*>(rec.data + tuple_data_offset + first_col.offset));
                        break;
                    case TYPE_FLOAT:
                        key_value.set_float(*reinterpret_cast<float*>(rec.data + tuple_data_offset + first_col.offset));
                        break;
                    case TYPE_STRING:
                        key_value.set_str(std::string(rec.data + tuple_data_offset + first_col.offset, first_col.len));
                        break;
                    default:
                        break;
                }
                
                // 扫描表查找是否存在相同主键的记录
                auto scanner = std::make_unique<RmScan>(fh_);
                // RmScan构造函数会自动定位到第一个记录，不需要reset_pos()
                for (; !scanner->is_end(); scanner->next()) {
                    Rid scan_rid = scanner->rid();
                    
                    // **关键：直接检查原始记录，不使用get_record_mvcc**
                    // 因为我们需要检查所有记录，包括被其他事务删除但未提交的记录
                    auto raw_record = fh_->get_record(scan_rid, context_);
                    if (!raw_record) continue;
                    
                    // 提取现有记录的主键值进行比较
                    bool matches = false;
                    const char* existing_field_data = raw_record->data + sizeof(TupleMeta) + first_col.offset;
                    
                    switch (first_col.type) {
                        case TYPE_INT:
                            matches = (*reinterpret_cast<const int*>(existing_field_data) == key_value.int_val);
                            break;
                        case TYPE_FLOAT:
                            matches = (*reinterpret_cast<const float*>(existing_field_data) == key_value.float_val);
                            break;
                        case TYPE_STRING:
                            matches = (std::string(existing_field_data, first_col.len) == key_value.str_val);
                            break;
                        default:
                            break;
                    }
                    
                    if (matches) {
                        // 找到相同主键的记录，检查是否存在写-写冲突
                        if (fh_->CheckWriteWriteConflict(scan_rid, context_)) {
                            std::cout << "[CONFLICT] Write-write conflict detected during INSERT operation" 
                                      << " - attempting to insert primary key " << key_value.int_val 
                                      << " that is being modified/deleted by another transaction" << std::endl;
                            throw TransactionAbortException(context_->txn_->get_transaction_id(), AbortReason::DEADLOCK_PREVENTION);
                        }
                        
                        // 检查记录是否对当前事务可见
                        auto visible_record = fh_->get_record_mvcc(scan_rid, context_);
                        if (visible_record) {
                            // 记录对当前事务可见，说明存在主键冲突
                            std::cout << "[CONFLICT] Primary key violation during INSERT operation" 
                                      << " - record with primary key " << key_value.int_val << " already exists and is visible" << std::endl;
                            throw InternalError("Primary key constraint violation");
                        }
                        
                        // 如果执行到这里，说明找到了相同主键的记录，但该记录：
                        // 1. 不存在写-写冲突（不是被其他事务正在操作）
                        // 2. 对当前事务不可见（可能是被当前事务删除，或被其他已提交事务删除）
                        // 这种情况下允许插入
                        std::cout << "[DEBUG] Found existing record with same primary key " << key_value.int_val 
                                  << " but it's not visible to current transaction, allowing INSERT" << std::endl;
                    }
                }
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
                for(int j = 0; j < index.col_num; ++j) {
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