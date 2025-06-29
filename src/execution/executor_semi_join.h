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
#include <cstring>
#include <cmath>

class SemiJoinExecutor : public AbstractExecutor {
   private:
    static constexpr float FLT_EPSILON = 1e-6; //容差
    std::unique_ptr<AbstractExecutor> left_;    // 左表执行器
    std::unique_ptr<AbstractExecutor> right_;   // 右表执行器
    size_t len_;                                // 输出元组长度（只包含左表列）
    std::vector<ColMeta> cols_;                 // 输出列元数据（只包含左表列）
    std::vector<Condition> fed_conds_;          // 连接条件
    bool isend;                                 // 是否已经到达结尾
    
    std::unique_ptr<RmRecord> left_tuple_;      // 当前左表元组
    std::unique_ptr<RmRecord> next_tuple_;      // 半连接后的结果元组

   public:
    SemiJoinExecutor(std::unique_ptr<AbstractExecutor> left, std::unique_ptr<AbstractExecutor> right, 
                     std::vector<Condition> conds) {
        left_ = std::move(left);
        right_ = std::move(right);
        len_ = left_->tupleLen(); // 半连接只返回左表的列
        cols_ = left_->cols();    // 只包含左表的列元数据
        isend = false;
        fed_conds_ = std::move(conds); // 连接条件
    }

    void beginTuple() override {
        left_->beginTuple();
        // 如果左表为空，则半连接结果为空
        if (left_->is_end()) {
            isend = true;
            return;
        }
        // 获取左表的第一个元组
        left_tuple_ = left_->Next();
        // 初始化next_tuple_为nullptr，表示还没有准备好下一个元组
        next_tuple_ = nullptr;
    }

    void nextTuple() override {
        // 如果已经结束，直接返回
        if (isend) {
            return;
        }
        
        // 循环直到找到匹配的元组或遍历完所有元组
        while (true) {
            // 重置右表迭代器
            right_->beginTuple();
            
            // 检查当前左表元组是否在右表中有匹配
            bool found_match = false;
            while (!right_->is_end()) {
                auto right_tuple = right_->Next();
                right_->nextTuple();

                if (satisfyConditions(right_tuple.get())) {
                    found_match = true;
                    break;
                }
            }

            if (found_match) {
                // 找到了匹配项，准备返回左表元组
                next_tuple_ = std::make_unique<RmRecord>(len_);
                memcpy(next_tuple_->data, left_tuple_->data, left_->tupleLen());
                
                // 推进到下一个左表元组，为下次调用做准备
                left_->nextTuple();
                if (left_->is_end()) {
                    isend = true; // 左表遍历完毕，结束
                } else {
                    left_tuple_ = left_->Next(); // 获取下一个左表元组
                }
                return;
            }

            // 当前左表元组没有匹配，推进到下一个左表元组
            left_->nextTuple();
            if (left_->is_end()) {
                isend = true; // 左表遍历完毕，结束
                return;
            }

            // 获取下一个左表元组
            left_tuple_ = left_->Next();
        }
    }

    bool is_end() const override {
        return isend;
    }

    std::unique_ptr<RmRecord> Next() override {
        // 如果还没有准备好下一个元组，先调用nextTuple准备
        if (next_tuple_ == nullptr && !isend) {
            nextTuple();
        }
        // 保存当前元组并重置next_tuple_，以便下次调用时能准备下一个元组
        auto result = std::move(next_tuple_);
        next_tuple_ = nullptr;
        return result;
    }

    size_t tupleLen() const override {
        return len_;
    }

    const std::vector<ColMeta> &cols() const override {
        return cols_;
    }

    Rid &rid() override { return _abstract_rid; }

private:
    // 检查是否满足所有连接条件
    bool satisfyConditions(RmRecord* right_tuple) {
        // 如果没有条件，则返回笛卡尔积（所有组合都满足）
        if (fed_conds_.empty()) {
            return true;
        }

        // 检查每个连接条件
        for (const auto& cond : fed_conds_) {
            const ColMeta *lhs_col_meta = nullptr, *rhs_col_meta = nullptr;
            char *lhs_tuple_data = nullptr, *rhs_tuple_data = nullptr;

            // 查找左操作数（lhs_col）的元数据和数据来源
            for (const auto& col : left_->cols()) {
                if (col.tab_name == cond.lhs_col.tab_name && col.name == cond.lhs_col.col_name) {
                    lhs_col_meta = &col;
                    lhs_tuple_data = left_tuple_->data;
                    break;
                }
            }
            if (lhs_col_meta == nullptr) {
                for (const auto& col : right_->cols()) {
                    if (col.tab_name == cond.lhs_col.tab_name && col.name == cond.lhs_col.col_name) {
                        lhs_col_meta = &col;
                        lhs_tuple_data = right_tuple->data;
                        break;
                    }
                }
            }

            // 查找右操作数（rhs_col）的元数据和数据来源
            for (const auto& col : left_->cols()) {
                if (col.tab_name == cond.rhs_col.tab_name && col.name == cond.rhs_col.col_name) {
                    rhs_col_meta = &col;
                    rhs_tuple_data = left_tuple_->data;
                    break;
                }
            }
            if (rhs_col_meta == nullptr) {
                for (const auto& col : right_->cols()) {
                    if (col.tab_name == cond.rhs_col.tab_name && col.name == cond.rhs_col.col_name) {
                        rhs_col_meta = &col;
                        rhs_tuple_data = right_tuple->data;
                        break;
                    }
                }
            }

            // 如果找不到对应列，继续检查下一个条件
            if (lhs_col_meta == nullptr || rhs_col_meta == nullptr) {
                continue;
            }

            // 获取两列的值进行比较
            char* left_val = lhs_tuple_data + lhs_col_meta->offset;
            char* right_val = rhs_tuple_data + rhs_col_meta->offset;
            ColType type = lhs_col_meta->type; // 假设 equi-join 的类型相同

            // 根据类型比较值
            bool result = false;

            if (type == TYPE_INT) {
                int left_int = *(int*)left_val;
                int right_int = *(int*)right_val;

                switch (cond.op) {
                    case OP_EQ: result = (left_int == right_int); break;
                    case OP_NE: result = (left_int != right_int); break;
                    case OP_LT: result = (left_int < right_int); break;
                    case OP_GT: result = (left_int > right_int); break;
                    case OP_LE: result = (left_int <= right_int); break;
                    case OP_GE: result = (left_int >= right_int); break;
                    default: result = false;
                }
            } else if (type == TYPE_FLOAT) {
                float left_float = *(float*)left_val;
                float right_float = *(float*)right_val;

                switch (cond.op) {
                    case OP_EQ: result = std::fabs(left_float - right_float) <= FLT_EPSILON; break;
                    case OP_NE: result = std::fabs(left_float - right_float) > FLT_EPSILON; break;
                    case OP_LT: result = left_float - right_float < -FLT_EPSILON; break;
                    case OP_GT: result = left_float - right_float > FLT_EPSILON; break;
                    case OP_LE: result = left_float - right_float <= FLT_EPSILON; break;
                    case OP_GE: result = left_float - right_float >= -FLT_EPSILON; break;
                    default: result = false;
                }
            } else if (type == TYPE_STRING) {
                std::string left_str(left_val, strnlen(left_val, lhs_col_meta->len));
                std::string right_str(right_val, strnlen(right_val, rhs_col_meta->len));

                switch (cond.op) {
                    case OP_EQ: result = (left_str == right_str); break;
                    case OP_NE: result = (left_str != right_str); break;
                    case OP_LT: result = (left_str < right_str); break;
                    case OP_GT: result = (left_str > right_str); break;
                    case OP_LE: result = (left_str <= right_str); break;
                    case OP_GE: result = (left_str >= right_str); break;
                    default: result = false;
                }
            }

            // 如果有一个条件不满足，则整个连接条件不满足
            if (!result) {
                return false;
            }
        }

        return true;
    }
}; 