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
#include <cstring>
#include <memory>
#include <cmath>

class FilterExecutor : public AbstractExecutor {
   private:
    static constexpr float EPSILON = 1e-6; //容差
    std::unique_ptr<AbstractExecutor> prev_;  // 子执行器
    std::vector<Condition> conds_;            // 过滤条件
    std::vector<ColMeta> cols_;               // 列元数据
    size_t len_;                              // 记录长度

   public:
    FilterExecutor(std::unique_ptr<AbstractExecutor> prev, std::vector<Condition> conds, Context *context) {
        if (!prev) {
            throw InternalError("FilterExecutor: prev is null");
        }
        prev_ = std::move(prev);
        conds_ = std::move(conds);
        cols_ = prev_->cols();
        len_ = prev_->tupleLen();
        context_ = context;
    }

    // 初始化迭代器
    void beginTuple() override {
        prev_->beginTuple();
    }

    void nextTuple() override {
        // 不断获取下一个记录，直到找到满足条件的记录或到达末尾
        do {
            prev_->nextTuple();
        } while (!prev_->is_end() && !eval_conds());
    }

    bool is_end() const override {
        return prev_->is_end();
    }

    std::unique_ptr<RmRecord> Next() override {
        // 获取当前记录
        auto record = prev_->Next();
        if (!record) {
            return nullptr;
        }
        
        // 检查是否满足过滤条件
        if (eval_conds(record.get())) {
            return record;
        }
        
        // 如果不满足条件，继续查找下一个
        nextTuple();
        if (is_end()) {
            return nullptr;
        }
        return prev_->Next();
    }

    const std::vector<ColMeta> &cols() const override {
        return cols_;
    }

    size_t tupleLen() const override {
        return len_;
    }

    Rid &rid() override { 
        return prev_->rid(); 
    }

private:
    // 条件判断函数（基于当前子执行器状态）
    bool eval_conds() {
        if (!prev_ || prev_->is_end()) {
            return false;
        }
        auto record = prev_->Next();
        if (!record) {
            return false;
        }
        return eval_conds(record.get());
    }

    // 条件判断函数（基于给定记录）
    bool eval_conds(const RmRecord *rec) {
        for (auto &cond : conds_) {
            if (!eval_cond(rec, cols_, cond)) {
                return false;
            }
        }
        return true;
    }

    bool eval_cond(const RmRecord *rec, const std::vector<ColMeta> &cols, const Condition &cond) {
        // 获取左操作数值
        auto lhs_col = get_col(cols, cond.lhs_col);
        
        // 注意：在MVCC模式下，通过执行器返回的记录已经是ReconstructTuple的结果，
        // 不包含TupleMeta，所以不需要额外的偏移量
        const char *lhs_value = rec->data + lhs_col->offset;

        // 根据右操作数是值还是列，进行处理
        if (cond.is_rhs_val) {
            // 右操作数是字面值
            if (lhs_col->type == TYPE_FLOAT) {
                float lhs_val = *(float*)lhs_value;
                float rhs_val = cond.rhs_val.float_val;
                switch (cond.op) {
                    case OP_EQ: return std::fabs(lhs_val - rhs_val) <= EPSILON;
                    case OP_NE: return std::fabs(lhs_val - rhs_val) > EPSILON;
                    case OP_LT: return rhs_val - lhs_val > EPSILON;
                    case OP_GT: return lhs_val - rhs_val > EPSILON;
                    case OP_LE: return lhs_val - rhs_val <= EPSILON;
                    case OP_GE: return rhs_val - lhs_val <= EPSILON;
                    default: return false;
                }
            }
            // 对于其他类型的字面值，使用通用的比较函数
            return compare_values(lhs_col->type, lhs_value, cond.rhs_val.raw->data, cond.op, lhs_col->len);
        } else {
            // 右操作数也是列
            auto rhs_col = get_col(cols, cond.rhs_col);
            const char *rhs_value = rec->data + rhs_col->offset;
            return compare_values(lhs_col->type, lhs_value, rhs_value, cond.op, lhs_col->len);
        }
    }

    // 比较两个值是否符合指定的比较操作符
    bool compare_values(ColType type, const char* lhs, const char* rhs, CompOp op, int len) {
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
                    case OP_EQ: return std::fabs(lhs_val - rhs_val) <= EPSILON;
                    case OP_NE: return std::fabs(lhs_val - rhs_val) > EPSILON;
                    case OP_LT: return rhs_val - lhs_val > EPSILON;
                    case OP_GT: return lhs_val - rhs_val > EPSILON;
                    case OP_LE: return lhs_val - rhs_val <= EPSILON;
                    case OP_GE: return rhs_val - lhs_val <= EPSILON;
                    default: return false;
                }
            }
            case TYPE_STRING: {
                int cmp_result = memcmp(lhs, rhs, len);
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
}; 