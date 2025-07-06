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
#include "transaction/transaction_manager.h" // For MVCC
#include "transaction/transaction.h"         // For MVCC

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

    // Helper function to create a Value object from a raw record field
    Value get_value_from_record(const RmRecord* record, const ColMeta& col) {
        Value val;
        val.type = col.type;
        
        // 注意：在MVCC模式下，get_record_mvcc返回的记录已经是ReconstructTuple的结果，
        // 不包含TupleMeta，所以不需要额外的偏移量
        const char* field_ptr = record->data + col.offset;
        switch (col.type) {
            case TYPE_INT:
                val.int_val = *reinterpret_cast<const int*>(field_ptr);
                break;
            case TYPE_FLOAT:
                val.float_val = *reinterpret_cast<const float*>(field_ptr);
                break;
            case TYPE_STRING:
                val.str_val = std::string(field_ptr, col.len);
                break;
            default:
                // Should not happen with a valid schema
                throw std::runtime_error("Unsupported column type for undo log creation.");
        }
        return val;
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
        // MVCC Check: Ensure we are in a transaction
        if (context_ == nullptr || context_->txn_ == nullptr || context_->txn_mgr_ == nullptr) {
            // Fallback or error for non-transactional updates, which shouldn't happen with MVCC.
            // For simplicity, we'll just throw an error.
            throw std::runtime_error("MVCC updates must be performed within a transaction.");
        }
        auto txn = context_->txn_;
        auto txn_mgr = context_->txn_mgr_;

        // 1. 收集所有满足条件的 rid
        rids_.clear();
        for (scanner_->beginTuple(); !scanner_->is_end(); scanner_->nextTuple()) {
            rids_.push_back(scanner_->rid());
        }

        // 2. 对 rids_ 做 update
        for (const Rid &rid : rids_) {
            // Get the visible version of the record for the current transaction.
            // This also performs the write-write conflict check.
            auto old_record_ptr = fh_->get_record_mvcc(rid, context_);
            if (!old_record_ptr) {
                // Record is not visible to this transaction (e.g., created by a concurrent uncommitted txn,
                // or deleted by a committed txn). Skip it.
                continue;
            }
            RmRecord &old_record = *old_record_ptr;

            // 注意：old_record是通过get_record_mvcc返回的，已经是ReconstructTuple的结果，
            // 不包含TupleMeta。我们需要从文件中读取原始记录来获取TupleMeta
            auto raw_record = fh_->get_record(rid, context_);
            TupleMeta old_meta;
            memcpy(&old_meta, raw_record->data, sizeof(TupleMeta));

            UndoLog undo_log;
            undo_log.ts_ = old_meta.ts_;
            undo_log.is_deleted_ = old_meta.is_deleted_;
            for(const auto& col : tab_.cols) {
                undo_log.tuple_.push_back(get_value_from_record(&old_record, col));
                undo_log.modified_fields_.push_back(true); // For simplicity, save all fields
            }
            
            // Link the new undo log to the previous version chain.
            auto prev_undo_link = txn_mgr->GetUndoLink(rid);
            if(prev_undo_link.has_value()) {
                undo_log.prev_version_ = *prev_undo_link;
            }
            auto new_undo_link = txn->AppendUndoLog(std::move(undo_log));

            // Generate the new record version.
            RmRecord new_record = *raw_record;
            // First, update the tuple's meta with the new version info.
            TupleMeta new_meta;
            new_meta.ts_ = txn->get_start_ts(); // Use start_ts as a temporary marker. This will be updated to commit_ts on commit.
            new_meta.is_deleted_ = false;
            memcpy(new_record.data, &new_meta, sizeof(TupleMeta));
            
            // 注意：new_record仍然包含TupleMeta（它是从get_record_mvcc获取的原始记录的副本），
            // 所以在更新时仍需要跳过TupleMeta
            int tuple_data_offset = sizeof(TupleMeta);
            
            // Second, apply the new values from the SET clauses.
            for (const auto &set_clause : set_clauses_) {
                auto col = get_col(tab_.cols, set_clause.lhs);
                char *data_ptr = new_record.data + tuple_data_offset + col->offset;
                
                Value val = analyzer_->evaluate_expr(set_clause.rhs, &old_record, tab_.cols);
                
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

            // Update unique indexes check
            for (auto &index : tab_.indexes) {
                if (!index.unique) continue;
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* new_key = new char[index.col_tot_len];
                char* old_key = new char[index.col_tot_len];
                int offset = 0;
                for (size_t j = 0; j < index.col_num; ++j) {
                    memcpy(new_key + offset, new_record.data + tuple_data_offset + index.cols[j].offset, index.cols[j].len);
                    memcpy(old_key + offset, old_record.data + index.cols[j].offset, index.cols[j].len);  // old_record没有TupleMeta
                    offset += index.cols[j].len;
                }
                if (memcmp(new_key, old_key, index.col_tot_len) != 0) {
                    std::vector<Rid> rids;
                    ih->get_value(new_key, &rids, context_->txn_);
                    if (!rids.empty()) {
                        delete[] new_key; delete[] old_key;
                        throw UniqueIndexViolationError(tab_name_, {});
                    }
                }
                delete[] new_key; delete[] old_key;
            }

            // Update indexes
            for (auto &index : tab_.indexes) {
                auto ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index.cols)).get();
                char* new_key = new char[index.col_tot_len];
                char* old_key = new char[index.col_tot_len];
                int offset = 0;
                for (size_t j = 0; j < index.col_num; ++j) {
                    memcpy(new_key + offset, new_record.data + tuple_data_offset + index.cols[j].offset, index.cols[j].len);
                    memcpy(old_key + offset, old_record.data + index.cols[j].offset, index.cols[j].len);  // old_record没有TupleMeta
                    offset += index.cols[j].len;
                }

                if (memcmp(new_key, old_key, index.col_tot_len) == 0) {
                    delete[] new_key; delete[] old_key;
                    continue;
                }

                ih->delete_entry(old_key, context_->txn_);
                ih->insert_entry(new_key, rid, context_->txn_);

                delete[] new_key;
                delete[] old_key;
            }

            // Write the new version to the main table (in-place update)
            fh_->update_record(rid, new_record.data, context_);
            
            // Point the tuple to the new head of its version chain
            VersionUndoLink version_link;
            version_link.prev_ = new_undo_link;
            version_link.in_progress_ = true;
            txn_mgr->UpdateVersionLink(rid, std::make_optional(version_link));
            
            // The old WriteRecord logic is now replaced by the UndoLog mechanism
        }
        return nullptr;
    }

    Rid &rid() override { return _abstract_rid; }
};