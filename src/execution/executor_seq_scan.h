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

class SeqScanExecutor : public AbstractExecutor {
   private:
    std::string tab_name_;              // 表的名称
    std::vector<Condition> conds_;      // scan的条件
    RmFileHandle *fh_;                  // 表的数据文件句柄
    std::vector<ColMeta> cols_;         // scan后生成的记录的字段
    size_t len_;                        // scan后生成的每条记录的长度
    std::vector<Condition> fed_conds_;  // 同conds_，两个字段相同

    Rid rid_;
    std::unique_ptr<RecScan> scan_;     // table_iterator

    SmManager *sm_manager_;

    bool check_condition(RmRecord *record, const Condition &cond) {
        // 获取左值的列元数据
        auto lhs_col = get_col(cols_, cond.lhs_col);
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
            auto rhs_col = get_col(cols_, cond.rhs_col);
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

   public:
    SeqScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = std::move(tab_name);
        conds_ = std::move(conds);
        TabMeta &tab = sm_manager_->db_.get_table(tab_name_);
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab.cols;
        len_ = cols_.back().offset + cols_.back().len;

        context_ = context;

        fed_conds_ = conds_;
    }

    void beginTuple() override {
        scan_ = fh_->create_scan();
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
};