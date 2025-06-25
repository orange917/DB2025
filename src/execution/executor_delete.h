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

            // 读取需要删除的记录，以便后续删除索引
            std::unique_ptr<RmRecord> record_ptr = fh_->get_record(rid, context_);
            if (!record_ptr) continue;

            RmRecord &record = *record_ptr;

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
        }

        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};