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

class DeleteExecutor : public AbstractExecutor {
    private:
    TabMeta tab_;                   // 表的元数据
    std::vector<Condition> conds_;  // delete的条件
    RmFileHandle *fh_;              // 表的数据文件句柄
    std::vector<Rid> rids_;         // 需要删除的记录的位置
    std::string tab_name_;          // 表名称
    SmManager *sm_manager_;
    size_t current_index_;          // 当前处理的记录索引

   public:
    DeleteExecutor(SmManager *sm_manager, const std::string &tab_name, std::vector<Condition> conds,
                   std::vector<Rid> rids, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = tab_name;
        tab_ = sm_manager_->db_.get_table(tab_name);
        fh_ = sm_manager_->fhs_.at(tab_name).get();
        conds_ = conds;
        rids_ = rids;
        context_ = context;
        current_index_ = 0;
    }

    std::unique_ptr<RmRecord> Next() override {
        while (current_index_ < rids_.size()) {
            Rid rid = rids_[current_index_++];

            // 读取需要删除的记录，根据并发控制模式选择不同的方法
            std::unique_ptr<RmRecord> record_ptr;
            if (context_ != nullptr && context_->txn_mgr_ != nullptr && 
                context_->txn_mgr_->get_concurrency_mode() == ConcurrencyMode::MVCC) {
                // MVCC模式：使用get_record_mvcc，它会进行写-写冲突检测
                record_ptr = fh_->get_record_mvcc(rid, context_);
            } else {
                record_ptr = fh_->get_record(rid, context_);
            }
            
            if (!record_ptr) continue;
            RmRecord &record = *record_ptr;

            // 在MVCC模式下，需要创建撤销日志
            if (context_ != nullptr && context_->txn_mgr_ != nullptr && 
                context_->txn_mgr_->get_concurrency_mode() == ConcurrencyMode::MVCC) {
                // 获取当前记录的元数据
                TupleMeta tuple_meta;
                memcpy(&tuple_meta, record.data, sizeof(TupleMeta));
                
                // 创建撤销日志
                UndoLog undo_log;
                undo_log.ts_ = tuple_meta.ts_;
                undo_log.is_deleted_ = false;  // 删除前是未删除状态
                
                // 记录修改的字段（对于删除操作，所有字段都需要记录）
                undo_log.modified_fields_.assign(tab_.cols.size(), true);
                
                // 保存原始数据
                undo_log.tuple_.resize(tab_.cols.size());
                int tuple_data_offset = sizeof(TupleMeta);
                for (size_t i = 0; i < tab_.cols.size(); i++) {
                    const auto& col = tab_.cols[i];
                    const char* field_data = record.data + tuple_data_offset + col.offset;
                    
                    switch (col.type) {
                        case TYPE_INT:
                            undo_log.tuple_[i].set_int(*(int*)field_data);
                            break;
                        case TYPE_FLOAT:
                            undo_log.tuple_[i].set_float(*(float*)field_data);
                            break;
                        case TYPE_STRING:
                            undo_log.tuple_[i].set_str(std::string(field_data, col.len));
                            break;
                        default:
                            break;
                    }
                }
                
                // 获取当前版本链
                auto version_link = context_->txn_mgr_->GetVersionLink(rid);
                if (version_link.has_value()) {
                    undo_log.prev_version_ = version_link->prev_;
                } else {
                    undo_log.prev_version_ = UndoLink{};  // 创建无效的UndoLink
                }
                
                // 将撤销日志添加到事务
                auto undo_link = context_->txn_->AppendUndoLog(undo_log);
                
                // 更新版本链
                VersionUndoLink new_version_link;
                new_version_link.prev_ = undo_link;
                new_version_link.in_progress_ = true;
                context_->txn_mgr_->UpdateVersionLink(rid, new_version_link);
                
                // 更新记录的元数据
                TupleMeta new_meta;
                new_meta.ts_ = context_->txn_->get_start_ts();
                new_meta.is_deleted_ = true;
                
                // 创建新记录
                RmRecord new_record(record.size);
                memcpy(new_record.data, &new_meta, sizeof(TupleMeta));
                memcpy(new_record.data + sizeof(TupleMeta), record.data + sizeof(TupleMeta), record.size - sizeof(TupleMeta));
                
                // 更新记录
                fh_->update_record(rid, new_record.data, context_);
            } else {
                // 非MVCC模式的传统处理
                // 添加到事务的写集合中 - 在删除前记录原始数据
                if (context_ != nullptr && context_->txn_ != nullptr) {
                    // 创建记录副本用于事务回滚
                    RmRecord record_copy(record.size);
                    memcpy(record_copy.data, record.data, record.size);

                    // 创建写记录并添加到事务的写集合中
                    WriteRecord* write_record = new WriteRecord(
                        WType::DELETE_TUPLE,
                        tab_name_,
                        rid,
                        record_copy
                    );

                    // 添加到事务的写集合中
                    context_->txn_->append_write_record(write_record);
                }

                // 先删除记录的索引
                for (size_t i = 0; i < tab_.indexes.size(); ++i) {
                    auto& index = tab_.indexes[i];
                    auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();

                    // 创建索引键
                    char* key = new char[index.col_tot_len];
                    int offset = 0;
                    for (size_t j = 0; j < index.col_num; ++j) {
                        memcpy(key + offset, record.data + index.cols[j].offset, index.cols[j].len);
                        offset += index.cols[j].len;
                    }

                    // 删除索引项
                    ih->delete_entry(key, context_->txn_);
                    delete[] key;
                }

                // 再删除记录本身
                fh_->delete_record(rid, context_);
                // 更新 row_count
                tab_.row_count--;
                sm_manager_->update_table_row_count(tab_name_, tab_.row_count);
            }
        }

        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};