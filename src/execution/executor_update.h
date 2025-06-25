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
#include "system/sm_meta.h"
#include "index/ix_manager.h"

class UpdateExecutor : public AbstractExecutor {
   private:
    TabMeta tab_;
    std::vector<Condition> conds_;
    RmFileHandle *fh_;
    std::vector<Rid> rids_;
    std::string tab_name_;
    std::vector<SetClause> set_clauses_;
    SmManager *sm_manager_;


private:
    // Helper function to evaluate conditions on a record
    bool eval_conds(const RmRecord *record, const std::vector<ColMeta> &cols) {
        // TODO: Implement actual condition evaluation logic.
        return true;
    }

public:
    UpdateExecutor(SmManager *sm_manager, const std::string &tab_name, std::vector<SetClause> set_clauses,
                std::vector<Condition> conds, std::vector<Rid> rids, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = tab_name;
        set_clauses_ = std::move(set_clauses);
        tab_ = sm_manager_->db_.get_table(tab_name);
        fh_ = sm_manager_->fhs_.at(tab_name).get();
        conds_ = std::move(conds);
        rids_ = std::move(rids);
        // 判断是否满足 where 条件
        // (Removed invalid use of 'old_record' here; condition evaluation is handled in Next())
    }

    std::unique_ptr<RmRecord> Next() override {
        for (const Rid &rid : rids_) {
            std::unique_ptr<RmRecord> record_ptr = fh_->get_record(rid, context_);
            if (!record_ptr) continue;
            RmRecord &old_record = *record_ptr;

            // 判断是否满足 where 条件
            if (!eval_conds(&old_record, tab_.cols)) continue;

            // 添加到事务的写集合中 - 在修改前记录原始数据
            if (context_ != nullptr && context_->txn_ != nullptr) {
                // 创建记录副本用于事务回滚
                RmRecord record_copy(old_record.size);
                memcpy(record_copy.data, old_record.data, old_record.size);

                // 创建写记录并添加到事务的写集合中
                WriteRecord* write_record = new WriteRecord(
                    WType::UPDATE_TUPLE,
                    tab_name_,
                    rid,
                    record_copy
                );

                // 添加到事务的写集合中
                context_->txn_->append_write_record(write_record);
            }

             // 生成新记录（先拷贝原数据，再根据 set_clauses 修改）
            RmRecord new_record = old_record;
            for (const auto &set_clause : set_clauses_) {
                auto col = get_col(tab_.cols, set_clause.lhs);
                char *data_ptr = new_record.data + col->offset;
                Value val = set_clause.rhs;

                // 类型对齐与转换
                if (col->type != val.type) {
                    if (col->type == TYPE_FLOAT && val.type == TYPE_INT) {
                        // 将 INT 转换为 FLOAT
                        val.float_val = static_cast<float>(val.int_val);
                        val.type = TYPE_FLOAT;
                    } else if (col->type == TYPE_INT && val.type == TYPE_FLOAT) {
                        // 将 FLOAT 转换为 INT
                        val.int_val = static_cast<int>(val.float_val);
                        val.type = TYPE_INT;
                    }
                }

                // 确认类型匹配，以防万一
                if (col->type != val.type) {
                    throw IncompatibleTypeError(coltype2str(col->type), coltype2str(val.type));
                }

                val.init_raw(col->len);
                memcpy(data_ptr, val.raw->data, col->len);
            }

            // 先删除旧索引项
            for (auto &index : tab_.indexes) {
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* key = new char[index.col_tot_len];
                int offset = 0;
                for (size_t j = 0; j < index.col_num; ++j) {
                    memcpy(key + offset, old_record.data + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                ih->delete_entry(key, context_->txn_);
                delete[] key;
            }

            // 写回新数据
            fh_->update_record(rid, new_record.data, context_);

            // 再插入新索引项
            for (auto &index : tab_.indexes) {
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* key = new char[index.col_tot_len];
                int offset = 0;
                for (size_t j = 0; j < index.col_num; ++j) {
                    memcpy(key + offset, new_record.data + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                ih->insert_entry(key, rid, context_->txn_);
                delete[] key;
            }
        }
        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};