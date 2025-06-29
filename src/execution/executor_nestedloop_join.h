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

class NestedLoopJoinExecutor : public AbstractExecutor {
   private:
    static constexpr float FLT_EPSILON = 1e-6; //容差
    std::unique_ptr<AbstractExecutor> left_;    // 左表执行器
    std::unique_ptr<AbstractExecutor> right_;   // 右表执行器
    size_t len_;                                // 输出元组长度
    std::vector<ColMeta> cols_;                 // 输出列元数据
    std::vector<Condition> fed_conds_;          // 连接条件
    bool isend;                                 // 是否已经到达结尾
    
    std::unique_ptr<RmRecord> left_tuple_;      // 当前左表元组
    std::unique_ptr<RmRecord> right_tuple_;     // 当前右表元组
    std::unique_ptr<RmRecord> next_tuple_;      // 连接后的结果元组

   public:
    NestedLoopJoinExecutor(std::unique_ptr<AbstractExecutor> left, std::unique_ptr<AbstractExecutor> right, 
                            std::vector<Condition> conds) {
        left_ = std::move(left);
        right_ = std::move(right);
        len_ = left_->tupleLen() + right_->tupleLen(); // 连接后元组的长度为左表和右表元组长度之和
        cols_ = left_->cols();
        auto right_cols = right_->cols();
        for (auto &col : right_cols) {
            col.offset += left_->tupleLen();
        }

        cols_.insert(cols_.end(), right_cols.begin(), right_cols.end());
        isend = false;
        fed_conds_ = std::move(conds); // 连接条件

    }

    void beginTuple() override {
        left_->beginTuple();
        // 如果左表为空，则连接结果为空
        if (left_->is_end()) {
            isend = true;
            return;
        }
        // 获取左表的第一个元组
        left_tuple_ = left_->Next();
        // 重置右表迭代器
        right_->beginTuple();
        // 寻找第一个匹配的元组
        nextTuple();
    }

    void nextTuple() override {
        // 循环直到找到匹配的元组或遍历完所有元组
        while (true) {
            while(!right_->is_end())
            {
                right_tuple_ = right_->Next();
                right_->nextTuple();

                if(satisfyConditions())
                {
                    // 找到了匹配项，就准备好元组返回
                    next_tuple_ = std::make_unique<RmRecord>(len_);
                    memcpy(next_tuple_->data, left_tuple_->data, left_->tupleLen());
                    memcpy(next_tuple_->data + left_->tupleLen(), right_tuple_->data, right_->tupleLen());
                    return;
                }
            }

            // 右表遍历完毕，推进左表
            left_->nextTuple();
            if (left_->is_end()) {
                isend = true; // 左表遍历完毕，结束
                return;
            }

            left_tuple_ = left_->Next(); // 获取下一个左表元组
            right_->beginTuple(); // 重置右表迭代器
    }
}

    bool is_end() const override {
        return isend;
    }

    std::unique_ptr<RmRecord> Next() override {
        return std::move(next_tuple_);
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
    bool satisfyConditions() {
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
                        lhs_tuple_data = right_tuple_->data;
                        break;
                    }
                }
            }

            // 如果找不到左操作数，继续检查下一个条件
            if (lhs_col_meta == nullptr) {
                continue;
            }

            // 根据右操作数是值还是列，进行处理
            if (cond.is_rhs_val) {
                // 右操作数是字面值
                char* lhs_val = lhs_tuple_data + lhs_col_meta->offset;
                bool result = false;

                if (lhs_col_meta->type == TYPE_INT) {
                    int lhs_int = *(int*)lhs_val;
                    int rhs_int = cond.rhs_val.int_val;

                    switch (cond.op) {
                        case OP_EQ: result = (lhs_int == rhs_int); break;
                        case OP_NE: result = (lhs_int != rhs_int); break;
                        case OP_LT: result = (lhs_int < rhs_int); break;
                        case OP_GT: result = (lhs_int > rhs_int); break;
                        case OP_LE: result = (lhs_int <= rhs_int); break;
                        case OP_GE: result = (lhs_int >= rhs_int); break;
                        default: result = false;
                    }
                } else if (lhs_col_meta->type == TYPE_FLOAT) {
                    float lhs_float = *(float*)lhs_val;
                    float rhs_float = cond.rhs_val.float_val;

                    switch (cond.op) {
                        case OP_EQ: result = std::fabs(lhs_float - rhs_float) <= FLT_EPSILON; break;
                        case OP_NE: result = std::fabs(lhs_float - rhs_float) > FLT_EPSILON; break;
                        case OP_LT: result = lhs_float - rhs_float < -FLT_EPSILON; break;
                        case OP_GT: result = lhs_float - rhs_float > FLT_EPSILON; break;
                        case OP_LE: result = lhs_float - rhs_float <= FLT_EPSILON; break;
                        case OP_GE: result = lhs_float - rhs_float >= -FLT_EPSILON; break;
                        default: result = false;
                    }
                } else if (lhs_col_meta->type == TYPE_STRING) {
                    std::string lhs_str(lhs_val, strnlen(lhs_val, lhs_col_meta->len));
                    std::string rhs_str = cond.rhs_val.str_val;

                    switch (cond.op) {
                        case OP_EQ: result = (lhs_str == rhs_str); break;
                        case OP_NE: result = (lhs_str != rhs_str); break;
                        case OP_LT: result = (lhs_str < rhs_str); break;
                        case OP_GT: result = (lhs_str > rhs_str); break;
                        case OP_LE: result = (lhs_str <= rhs_str); break;
                        case OP_GE: result = (lhs_str >= rhs_str); break;
                        default: result = false;
                    }
                }

                // 如果有一个条件不满足，则返回false
                if (!result) {
                    return false;
                }
            } else {
                // 右操作数也是列
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
                            rhs_tuple_data = right_tuple_->data;
                            break;
                        }
                    }
                }

                // 如果找不到对应列，继续检查下一个条件
                if (rhs_col_meta == nullptr) {
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

                // 如果有一个条件不满足，则返回false
                if (!result) {
                    return false;
                }
            }
        }

        // 所有条件都满足
        return true;
    }
};