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

class IndexScanExecutor : public AbstractExecutor {
   private:
    std::string tab_name_;                      // 表名称
    TabMeta tab_;                               // 表的元数据
    std::vector<Condition> conds_;              // 扫描条件
    RmFileHandle *fh_;                          // 表的数据文件句柄
    std::vector<ColMeta> cols_;                 // 需要读取的字段
    size_t len_;                                // 选取出来的一条记录的长度
    std::vector<Condition> fed_conds_;          // 扫描条件，和conds_字段相同

    std::vector<std::string> index_col_names_;  // index scan涉及到的索引包含的字段
    IndexMeta index_meta_;                      // index scan涉及到的索引元数据

    Rid rid_;
    std::unique_ptr<RecScan> scan_;

    SmManager *sm_manager_;

   public:
    IndexScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, std::vector<std::string> index_col_names,
                    Context *context) {
        sm_manager_ = sm_manager;
        context_ = context;
        tab_name_ = std::move(tab_name);
        tab_ = sm_manager_->db_.get_table(tab_name_);
        conds_ = std::move(conds);
        index_col_names_ = index_col_names;
        index_meta_ = *tab_.get_index_meta(index_col_names_);
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab_.cols;
        len_ = cols_.back().offset + cols_.back().len;
        fed_conds_ = conds_;
    }

    void beginTuple() override {
        // 创建索引扫描器
        std::string index_name = sm_manager_->get_ix_manager()->get_index_name(tab_name_, index_col_names_);
        auto ih = sm_manager_->ihs_.at(index_name).get();
        Iid lower = Iid{-1, -1};  // 从第一个记录开始
        Iid upper = Iid{-1, -1};  // 到最后一个记录结束
        scan_ = std::make_unique<IxScan>(ih, lower, upper, sm_manager_->get_bpm());
        scan_->next();
    }

    void nextTuple() override {
        scan_->next();
    }

    std::unique_ptr<RmRecord> Next() override {
        while (!scan_->is_end()) {
            auto record = fh_->get_record(scan_->rid(), context_);
            bool match = true;
            for (const auto &cond : conds_) {
                if (!check_condition(record.get(), cond)) {
                    match = false;
                    break;
                }
            }
            if (match) {
                rid_ = scan_->rid();
                scan_->next();
                return record;
            }
            scan_->next();
        }
        return nullptr;
    }

    Rid &rid() override { return rid_; }

   private:
    bool check_condition(RmRecord *record, const Condition &cond) {
        // 获取左值的列元数据
        const ColMeta *lhs_col = nullptr;
        auto lhs_iter = get_col(cols_, cond.lhs_col);
        if (lhs_iter != cols_.end()) {
            lhs_col = &(*lhs_iter);
        } else {
            // 如果找不到列，尝试去掉表名前缀
            std::string col_name = cond.lhs_col.tab_name.empty() ? cond.lhs_col.col_name : cond.lhs_col.col_name;
            for (const auto &col : cols_) {
                if (col.name == col_name) {
                    lhs_col = &col;
                    break;
                }
            }
        }
        if (!lhs_col) {
            return false;
        }
        char *lhs_data = record->data + lhs_col->offset;

        // 获取右值
        char *rhs_data;
        if (cond.is_rhs_val) {
            // 如果右值是常量
            if (cond.rhs_val.type == TYPE_INT) {
                int val = cond.rhs_val.int_val;
                return compare_int(lhs_data, &val, cond.op);
            } else if (cond.rhs_val.type == TYPE_FLOAT) {
                float val = cond.rhs_val.float_val;
                return compare_float(lhs_data, &val, cond.op);
            } else if (cond.rhs_val.type == TYPE_STRING) {
                return compare_string(lhs_data, cond.rhs_val.str_val.c_str(), cond.op, lhs_col->len);
            }
        } else {
            // 如果右值是列
            const ColMeta *rhs_col = nullptr;
            auto rhs_iter = get_col(cols_, cond.rhs_col);
            if (rhs_iter != cols_.end()) {
                rhs_col = &(*rhs_iter);
            } else {
                // 如果找不到列，尝试去掉表名前缀
                std::string col_name = cond.rhs_col.tab_name.empty() ? cond.rhs_col.col_name : cond.rhs_col.col_name;
                for (const auto &col : cols_) {
                    if (col.name == col_name) {
                        rhs_col = &col;
                        break;
                    }
                }
            }
            if (!rhs_col) {
                return false;
            }
            rhs_data = record->data + rhs_col->offset;
            return compare_data(lhs_data, rhs_data, cond.op, lhs_col->len);
        }
        return false;
    }

    bool compare_int(char *lhs, const int *rhs, CompOp op) {
        int lhs_val = *(int *)lhs;
        switch (op) {
            case OP_EQ: return lhs_val == *rhs;
            case OP_NE: return lhs_val != *rhs;
            case OP_LT: return lhs_val < *rhs;
            case OP_GT: return lhs_val > *rhs;
            case OP_LE: return lhs_val <= *rhs;
            case OP_GE: return lhs_val >= *rhs;
            default: return false;
        }
    }

    bool compare_float(char *lhs, const float *rhs, CompOp op) {
        float lhs_val = *(float *)lhs;
        switch (op) {
            case OP_EQ: return lhs_val == *rhs;
            case OP_NE: return lhs_val != *rhs;
            case OP_LT: return lhs_val < *rhs;
            case OP_GT: return lhs_val > *rhs;
            case OP_LE: return lhs_val <= *rhs;
            case OP_GE: return lhs_val >= *rhs;
            default: return false;
        }
    }

    bool compare_string(char *lhs, const char *rhs, CompOp op, int len) {
        int cmp = memcmp(lhs, rhs, len);
        switch (op) {
            case OP_EQ: return cmp == 0;
            case OP_NE: return cmp != 0;
            case OP_LT: return cmp < 0;
            case OP_GT: return cmp > 0;
            case OP_LE: return cmp <= 0;
            case OP_GE: return cmp >= 0;
            default: return false;
        }
    }

    bool compare_data(char *lhs, char *rhs, CompOp op, int len) {
        int cmp = memcmp(lhs, rhs, len);
        switch (op) {
            case OP_EQ: return cmp == 0;
            case OP_NE: return cmp != 0;
            case OP_LT: return cmp < 0;
            case OP_GT: return cmp > 0;
            case OP_LE: return cmp <= 0;
            case OP_GE: return cmp >= 0;
            default: return false;
        }
    }
};