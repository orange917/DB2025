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

#include "common/context.h"
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"
#include <memory>

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

    // 初始化迭代器
    void beginTuple() override {
        // 创建表扫描器
        scan_ = fh_->create_scan();

        // 如果没有记录，就跳过
        if(scan_->is_end()) {
            return;
        }

        rid_ = scan_->rid();  // 获取当前记录的rid
    }

    void nextTuple() override {
        // 移动到下一个
        scan_->next();  // 移动到下一个记录
        if (scan_->is_end()) {
            rid_.page_no = RM_NO_PAGE;  // 设置为无效页码，表示没有更多记录了
            rid_.slot_no = -1;          // 设置为无效slot号
            return;
        }
        rid_ = scan_->rid();  // 更新当前记录的rid
    }

    bool is_end() const override {
        return scan_->is_end();
    }


    std::unique_ptr<RmRecord> Next() override {
        // 获取记录
        auto rec = fh_->get_record(rid_, context_);
        // 检查记录是否满足条件
        if (!conds_.empty() && !eval_conds(rec.get(), cols_)) {
            // 如果不满足条件，继续查找下一条记录
            while (!is_end()) {
                nextTuple();
                if (is_end()) break;
                rec = fh_->get_record(rid_, context_);
                if (eval_conds(rec.get(), cols_)) {
                    return rec;
                }
            }
            return nullptr;
        }
        return rec;
    }

    // 条件判断函数
    bool eval_conds(const RmRecord *rec, const std::vector<ColMeta> &cols) {
        for (auto &cond : conds_) {
            if (!eval_cond(rec, cols, cond)) {
                return false;
            }
        }
        return true;
    }

    bool eval_cond(const RmRecord *rec, const std::vector<ColMeta> &cols, const Condition &cond) {
        // 实现条件判断逻辑
        // 获取左操作数值
        auto lhs_col = get_col(cols, cond.lhs_col);
        char *lhs_value = rec->data + lhs_col->offset;

        // 如果右操作数是值
        if (cond.is_rhs_val) {
            // 根据列类型比较
            return compare_values(lhs_col->type, lhs_value, cond.rhs_val.raw->data, cond.op);
        } else {
            // 右操作数也是列
            auto rhs_col = get_col(cols, cond.rhs_col);
            char *rhs_value = rec->data + rhs_col->offset;
            return compare_values(lhs_col->type, lhs_value, rhs_value, cond.op);
        }
    }

    // 比较两个值是否符合指定的比较操作符
    bool compare_values(ColType type, const char* lhs, const char* rhs, CompOp op) {
        switch (type) {
            case TYPE_INT: {
                int lhs_val = *(int*)lhs;
                int rhs_val = *(int*)rhs;
                switch (op) {
                    case OP_EQ: return lhs_val == rhs_val;
                    case OP_NE: return lhs_val != rhs_val;
                    case OP_LT: return lhs_val < rhs_val;
                    case OP_GT: return lhs_val > rhs_val;
                    case OP_LE: return lhs_val <= rhs_val;
                    case OP_GE: return lhs_val >= rhs_val;
                    default: return false;
                }
            }
            case TYPE_FLOAT: {
                float lhs_val = *(float*)lhs;
                float rhs_val = *(float*)rhs;
                switch (op) {
                    case OP_EQ: return lhs_val == rhs_val;
                    case OP_NE: return lhs_val != rhs_val;
                    case OP_LT: return lhs_val < rhs_val;
                    case OP_GT: return lhs_val > rhs_val;
                    case OP_LE: return lhs_val <= rhs_val;
                    case OP_GE: return lhs_val >= rhs_val;
                    default: return false;
                }
            }
            case TYPE_STRING: {
                // Assuming strings are null-terminated or have known length
                int cmp_result = std::strcmp(lhs, rhs);
                switch (op) {
                    case OP_EQ: return cmp_result == 0;
                    case OP_NE: return cmp_result != 0;
                    case OP_LT: return cmp_result < 0;
                    case OP_GT: return cmp_result > 0;
                    case OP_LE: return cmp_result <= 0;
                    case OP_GE: return cmp_result >= 0;
                    default: return false;
                }
            }
            default:
                return false;
        }
    }

    const std::vector<ColMeta> &cols() const override {
        return cols_;
    }

    Rid &rid() override { return rid_; }
};