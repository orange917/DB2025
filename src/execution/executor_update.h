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
#include "analyze/analyze.h"

class UpdateExecutor : public AbstractExecutor {
   private:
    TabMeta tab_;
    std::vector<Condition> conds_;
    RmFileHandle *fh_;
    std::vector<Rid> rids_;
    std::string tab_name_;
    std::vector<SetClause> set_clauses_;
    std::unique_ptr<AbstractExecutor> scanner_;
    SmManager *sm_manager_;
    Analyze *analyzer_;  // 添加分析器用于表达式求值

    // Helper function to evaluate conditions on a record
    bool eval_conds(const RmRecord *record, const std::vector<ColMeta> &cols) {
        for (const auto &cond : conds_) {
            if (!eval_cond(record, cols, cond)) {
                return false;
            }
        }
        return true;
    }

    // Helper function to evaluate a single condition
    bool eval_cond(const RmRecord *record, const std::vector<ColMeta> &cols, const Condition &cond) {
        auto col_iter = std::find_if(cols.begin(), cols.end(), 
            [&cond](const ColMeta &col) { 
                return col.tab_name == cond.lhs_col.tab_name && col.name == cond.lhs_col.col_name; 
            });
        
        if (col_iter == cols.end()) {
            return false;
        }

        char *lhs_val = const_cast<char*>(record->data) + col_iter->offset;
        
        if (cond.is_rhs_val) {
            char *rhs_val = cond.rhs_val.raw->data;
            int cmp_res = ix_compare(lhs_val, rhs_val, col_iter->type, col_iter->len);
            
            switch (cond.op) {
                case OP_EQ: return (cmp_res == 0);
                case OP_NE: return (cmp_res != 0);
                case OP_LT: return (cmp_res < 0);
                case OP_GT: return (cmp_res > 0);
                case OP_LE: return (cmp_res <= 0);
                case OP_GE: return (cmp_res >= 0);
                default: return false;
            }
        } else {
            // Handle column comparison (not implemented for simplicity)
            return false;
        }
    }

public:
    UpdateExecutor(SmManager *sm_manager, const std::string &tab_name,
                   std::vector<SetClause> set_clauses, std::vector<Condition> conds,
                   std::unique_ptr<AbstractExecutor> scanner, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = tab_name;
        set_clauses_ = std::move(set_clauses);
        tab_ = sm_manager_->db_.get_table(tab_name);
        fh_ = sm_manager_->fhs_.at(tab_name).get();
        conds_ = std::move(conds);
        scanner_ = std::move(scanner);
        AbstractExecutor::context_ = context;
        analyzer_ = new Analyze(sm_manager);  // 创建分析器实例
    }

    ~UpdateExecutor() {
        delete analyzer_;
    }

    std::unique_ptr<RmRecord> Next() override {
        // 1. 收集所有满足条件的 rid
        rids_.clear();
        for (scanner_->beginTuple(); !scanner_->is_end(); scanner_->nextTuple()) {
            rids_.push_back(scanner_->rid());
        }
        // 2. 对 rids_ 做 update（用你前面写的 update 逻辑即可）
        for (const Rid &rid : rids_) {
            std::unique_ptr<RmRecord> record_ptr = fh_->get_record(rid, context_);
            if (!record_ptr) continue;
            RmRecord &old_record = *record_ptr;
            // 生成新记录（先拷贝原数据，再根据 set_clauses 修改）
            RmRecord new_record = old_record;
            for (const auto &set_clause : set_clauses_) {
                auto col = get_col(tab_.cols, set_clause.lhs);
                char *data_ptr = new_record.data + col->offset;
                
                // 使用表达式求值
                Value val = analyzer_->evaluate_expr(set_clause.rhs, &old_record, tab_.cols);
                
                // 类型转换
                if (col->type != val.type) {
                    if (col->type == TYPE_FLOAT && val.type == TYPE_INT) {
                        val.float_val = static_cast<float>(val.int_val);
                        val.type = TYPE_FLOAT;
                    } else if (col->type == TYPE_INT && val.type == TYPE_FLOAT) {
                        val.int_val = static_cast<int>(val.float_val);
                        val.type = TYPE_INT;
                    }
                }
                if (col->type != val.type) {
                    throw IncompatibleTypeError(coltype2str(col->type), coltype2str(val.type));
                }
                val.init_raw(col->len);
                memcpy(data_ptr, val.raw->data, col->len);
            }
            // 先做唯一性检查（只对新旧key不同才查）
            for (auto &index : tab_.indexes) {
                if (!index.unique) continue;
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* new_key = new char[index.col_tot_len];
                char* old_key = new char[index.col_tot_len];
                int offset = 0;
                for (size_t j = 0; j < index.col_num; ++j) {
                    memcpy(new_key + offset, new_record.data + index.cols[j].offset, index.cols[j].len);
                    memcpy(old_key + offset, old_record.data + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }
                if (memcmp(new_key, old_key, index.col_tot_len) != 0) {
                    std::vector<Rid> rids;
                    ih->get_value(new_key, &rids, context_ ? context_->txn_ : nullptr);
                    if (!rids.empty()) {
                        delete[] new_key; delete[] old_key;
                        throw UniqueIndexViolationError(tab_name_, {});
                    }
                }
                delete[] new_key; delete[] old_key;
            }
            // 只有当索引列被修改时，才更新索引
            for (auto &index : tab_.indexes) {
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* new_key = new char[index.col_tot_len];
                char* old_key = new char[index.col_tot_len];
                int offset = 0;
                for (size_t j = 0; j < index.col_num; ++j) {
                    memcpy(new_key + offset, new_record.data + index.cols[j].offset, index.cols[j].len);
                    memcpy(old_key + offset, old_record.data + index.cols[j].offset, index.cols[j].len);
                    offset += index.cols[j].len;
                }

                // 如果索引键没有变化，则跳过此索引
                if (memcmp(new_key, old_key, index.col_tot_len) == 0) {
                    delete[] new_key;
                    delete[] old_key;
                    continue;
                }

                // 索引键有变化，先删除旧的，再插入新的
                ih->delete_entry(old_key, context_ ? context_->txn_ : nullptr);
                // 注意：这里直接插入 new_key，而不是在后面的循环中做
                ih->insert_entry(new_key, rid, context_ ? context_->txn_ : nullptr);

                delete[] new_key;
                delete[] old_key;
            }
            // 写主表新数据
            fh_->update_record(rid, new_record.data, context_);
            // 事务日志
            if (context_ != nullptr && context_->txn_ != nullptr) {
                RmRecord record_copy(old_record.size);
                memcpy(record_copy.data, old_record.data, old_record.size);
                WriteRecord* write_record = new WriteRecord(WType::UPDATE_TUPLE, tab_name_, rid, record_copy);
                context_->txn_->append_write_record(write_record);
            }
        }
        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};