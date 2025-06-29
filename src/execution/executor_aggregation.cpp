/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "executor_aggregation.h"
#include "errors.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iostream>

AggregationExecutor::AggregationExecutor(std::unique_ptr<AbstractExecutor> prev,
            const std::vector<AggFunc>& agg_funcs,
            const std::vector<TabCol>& group_by_cols,
            const std::vector<Condition>& having_conds,
            const std::vector<OrderByCol>& order_by_cols,
            const std::vector<bool>& order_by_directions,
            int limit_val)
        : prev_(std::move(prev)), agg_funcs_(agg_funcs), group_by_cols_(group_by_cols),
        having_conds_(having_conds), order_by_cols_(order_by_cols), 
        order_by_directions_(order_by_directions), limit_val_(limit_val), 
        current_result_idx_(0), is_initialized_(false) {

    // 计算输出列元数据
    size_t offset = 0;

    // 如果有GROUP BY，先添加分组列
    for (const auto& group_col : group_by_cols_) {
        ColMeta col_meta;
        col_meta.tab_name = group_col.tab_name;
        col_meta.name = group_col.col_name;
        col_meta.offset = offset;

        // 获取列类型和长度
        auto pos = get_col(prev_->cols(), group_col);
        col_meta.type = pos->type;
        col_meta.len = pos->len;

        cols_.push_back(col_meta);
        offset += col_meta.len;
    }

    // 添加聚合函数列
    for (const auto& agg_func : agg_funcs_) {
        ColMeta col_meta;
        col_meta.tab_name = "";
        col_meta.name = agg_func.alias.empty() ? "agg_" + std::to_string(cols_.size()) : agg_func.alias;
        col_meta.offset = offset;

        // 根据聚合函数类型确定输出类型
        if (agg_func.func_type == AGG_COUNT) {
            col_meta.type = TYPE_INT;
            col_meta.len = sizeof(int);
        } else if (agg_func.func_type == AGG_AVG) {
            col_meta.type = TYPE_FLOAT;
            col_meta.len = sizeof(float);
        } else {
            // MAX, MIN, SUM - 保持原列类型
            if (!agg_func.col.tab_name.empty() || !agg_func.col.col_name.empty()) {
                auto pos = get_col(prev_->cols(), agg_func.col);
                col_meta.type = pos->type;
                col_meta.len = pos->len;
            } else {
                col_meta.type = TYPE_INT;
                col_meta.len = sizeof(int);
            }
        }

        cols_.push_back(col_meta);
        offset += col_meta.len;
    }

    len_ = offset;

    // 初始化全局聚合状态（用于没有GROUP BY的情况）
    if (group_by_cols_.empty()) {
        AggState& global_state = group_states_["global"];
        global_state.count = 0;
        global_state.agg_values.clear();

        // 根据聚合函数类型初始化聚合值
        for (const auto& agg_func : agg_funcs_) {
            Value agg_val;
            if (agg_func.func_type == AGG_COUNT) {
                agg_val.set_int(0);
            } else if (agg_func.func_type == AGG_AVG) {
                agg_val.set_float(0.0f);
            } else {
                // MAX, MIN, SUM - 根据列类型初始化
                if (!agg_func.col.tab_name.empty() || !agg_func.col.col_name.empty()) {
                    auto pos = get_col(prev_->cols(), agg_func.col);
                    if (pos->type == TYPE_INT) {
                        agg_val.set_int(0);
                    } else if (pos->type == TYPE_FLOAT) {
                        agg_val.set_float(0.0f);
                    } else {
                        agg_val.set_str("");
                    }
                } else {
                    agg_val.set_int(0);
                }
            }
            global_state.agg_values.push_back(agg_val);
        }
    }
}

void AggregationExecutor::beginTuple() {
    prev_->beginTuple();
    current_result_idx_ = 0;
    is_initialized_ = false;
}

void AggregationExecutor::nextTuple() {
    if (!is_initialized_) {
        // 第一次调用，计算所有聚合结果
        compute_final_results();
        is_initialized_ = true;
    }
    
    // 聚合执行器的nextTuple不需要增加current_result_idx_
    // 因为Next()函数已经处理了
}

bool AggregationExecutor::is_end() const {
    if (!is_initialized_) {
        return false;
    }
    return current_result_idx_ >= results_.size();
}

std::unique_ptr<RmRecord> AggregationExecutor::Next() {
    if (current_result_idx_ >= results_.size()) {
        return nullptr;
    }
    
    // 创建输出记录
    auto record = std::make_unique<RmRecord>(len_);
    const auto& result = results_[current_result_idx_];
    
    // 填充分组列
    size_t offset = 0;
    for (size_t i = 0; i < group_by_cols_.size(); i++) {
        const auto& col_meta = cols_[i];
        const auto& value = result.group_key[i];
        
        if (col_meta.type == TYPE_INT) {
            *(int*)(record->data + offset) = value.int_val;
        } else if (col_meta.type == TYPE_FLOAT) {
            *(float*)(record->data + offset) = value.float_val;
        } else if (col_meta.type == TYPE_STRING) {
            memcpy(record->data + offset, value.str_val.c_str(), std::min(value.str_val.size(), (size_t)col_meta.len));
        }
        offset += col_meta.len;
    }
    
    // 填充聚合函数列
    for (size_t i = 0; i < agg_funcs_.size(); i++) {
        const auto& col_meta = cols_[group_by_cols_.size() + i];
        const auto& value = result.agg_values[i];
        
        if (col_meta.type == TYPE_INT) {
            *(int*)(record->data + offset) = value.int_val;
        } else if (col_meta.type == TYPE_FLOAT) {
            *(float*)(record->data + offset) = value.float_val;
        } else if (col_meta.type == TYPE_STRING) {
            memcpy(record->data + offset, value.str_val.c_str(), std::min(value.str_val.size(), (size_t)col_meta.len));
        }
        offset += col_meta.len;
    }
    
    // 增加索引，为下次调用做准备
    current_result_idx_++;
    
    return record;
}

const std::vector<ColMeta>& AggregationExecutor::cols() const {
    return cols_;
}

size_t AggregationExecutor::tupleLen() const {
    return len_;
}

Rid& AggregationExecutor::rid() {
    return _abstract_rid;
}

std::string AggregationExecutor::get_group_key(const std::unique_ptr<RmRecord>& record) {
    if (group_by_cols_.empty()) {
        return "global"; // 全局聚合
    }
    
    std::stringstream ss;
    for (const auto& group_col : group_by_cols_) {
        Value value = get_column_value(record, group_col);
        if (value.type == TYPE_INT) {
            ss << value.int_val << "|";
        } else if (value.type == TYPE_FLOAT) {
            ss << value.float_val << "|";
        } else {
            ss << value.str_val << "|";
        }
    }
    return ss.str();
}

void AggregationExecutor::update_agg_state(AggState& state, const std::unique_ptr<RmRecord>& record) {
    state.count++;
    
    // 确保聚合值数组和累加器数组有足够的元素
    while (state.agg_values.size() < agg_funcs_.size()) {
        Value agg_val;
        size_t i = state.agg_values.size();
        const auto& agg_func = agg_funcs_[i];
        
        if (agg_func.func_type == AGG_COUNT) {
            agg_val.set_int(0);
        } else if (agg_func.func_type == AGG_AVG) {
            agg_val.set_float(0.0f);
        } else {
            // MAX, MIN, SUM - 根据列类型初始化
            if (!agg_func.col.tab_name.empty() || !agg_func.col.col_name.empty()) {
                auto pos = get_col(prev_->cols(), agg_func.col);
                if (pos != prev_->cols().end() && pos->type == TYPE_INT) {
                    agg_val.set_int(0);
                } else if (pos != prev_->cols().end() && pos->type == TYPE_FLOAT) {
                    agg_val.set_float(0.0f);
                } else {
                    agg_val.set_str("");
                }
            } else {
                agg_val.set_int(0);
            }
        }
        state.agg_values.push_back(agg_val);
        state.float_accumulators.push_back(0.0);  // 初始化double累加器
    }
    
    for (size_t i = 0; i < agg_funcs_.size(); i++) {
        const auto& agg_func = agg_funcs_[i];
        
        if (agg_func.func_type == AGG_COUNT && agg_func.col.tab_name.empty() && agg_func.col.col_name.empty()) {
            // COUNT(*) - 已经在上面处理了count++
            continue;
        }
        
        // 只有当列名不为空时才调用 get_column_value
        Value value;
        if (!agg_func.col.col_name.empty()) {
            value = get_column_value(record, agg_func.col);
        } else {
            // 如果列名为空，设置默认值
            value.set_int(0);
        }
        
        switch (agg_func.func_type) {
            case AGG_COUNT:
                // COUNT(column) - 只统计非NULL值
                if (value.type == TYPE_INT || value.type == TYPE_FLOAT || 
                    (value.type == TYPE_STRING && !value.str_val.empty())) {
                    if (state.agg_values[i].type == TYPE_INT) {
                        state.agg_values[i].set_int(state.agg_values[i].int_val + 1);
                    } else {
                        state.agg_values[i].set_int(1);
                    }
                }
                break;
                
            case AGG_MAX:
                if (state.agg_values[i].type == TYPE_INT && value.type == TYPE_INT) {
                    if (state.count == 1 || value.int_val > state.agg_values[i].int_val) {
                        state.agg_values[i].set_int(value.int_val);
                    }
                } else if (state.agg_values[i].type == TYPE_FLOAT && value.type == TYPE_FLOAT) {
                    if (state.count == 1 || value.float_val > state.agg_values[i].float_val) {
                        state.agg_values[i].set_float(value.float_val);
                    }
                } else {
                    // 初始化聚合值
                    if (value.type == TYPE_INT) {
                        state.agg_values[i].set_int(value.int_val);
                    } else if (value.type == TYPE_FLOAT) {
                        state.agg_values[i].set_float(value.float_val);
                    } else {
                        state.agg_values[i].set_str(value.str_val);
                    }
                }
                break;
                
            case AGG_MIN:
                if (state.agg_values[i].type == TYPE_INT && value.type == TYPE_INT) {
                    if (state.count == 1 || value.int_val < state.agg_values[i].int_val) {
                        state.agg_values[i].set_int(value.int_val);
                    }
                } else if (state.agg_values[i].type == TYPE_FLOAT && value.type == TYPE_FLOAT) {
                    if (state.count == 1 || value.float_val < state.agg_values[i].float_val) {
                        state.agg_values[i].set_float(value.float_val);
                    }
                } else {
                    // 初始化聚合值
                    if (value.type == TYPE_INT) {
                        state.agg_values[i].set_int(value.int_val);
                    } else if (value.type == TYPE_FLOAT) {
                        state.agg_values[i].set_float(value.float_val);
                    } else {
                        state.agg_values[i].set_str(value.str_val);
                    }
                }
                break;
                
            case AGG_SUM:
                if (state.agg_values[i].type == TYPE_INT && value.type == TYPE_INT) {
                    state.agg_values[i].set_int(state.agg_values[i].int_val + value.int_val);
                } else if (state.agg_values[i].type == TYPE_FLOAT && value.type == TYPE_FLOAT) {
                    // 使用double累加器保持精度
                    state.float_accumulators[i] += static_cast<double>(value.float_val);
                    // 更新Value中的float值（用于显示）
                    state.agg_values[i].set_float(static_cast<float>(state.float_accumulators[i]));
                } else {
                    // 初始化聚合值
                    if (value.type == TYPE_INT) {
                        state.agg_values[i].set_int(value.int_val);
                    } else if (value.type == TYPE_FLOAT) {
                        state.agg_values[i].set_float(value.float_val);
                        state.float_accumulators[i] = static_cast<double>(value.float_val);
                    } else {
                        state.agg_values[i].set_str(value.str_val);
                    }
                }
                break;
                
                case AGG_AVG:
                // AVG需要特殊处理，先累加，最后在compute_final_results中计算平均值
                if (value.type == TYPE_INT) {
                    // 累加整数值到double累加器
                    state.float_accumulators[i] += static_cast<double>(value.int_val);
                } else if (value.type == TYPE_FLOAT) {
                    // 累加浮点数值到double累加器
                    state.float_accumulators[i] += static_cast<double>(value.float_val);
                }
                // 注意：这里不需要更新state.agg_values[i]，因为它只在最终计算时才有用
                break;
        }
    }
}

Value AggregationExecutor::get_agg_value(const AggState& state, AggFuncType func_type, const TabCol& col) {
    if (func_type == AGG_COUNT && col.tab_name.empty() && col.col_name.empty()) {
        // COUNT(*)
        Value result;
        result.set_int(state.count);
        return result;
    }
    
    // 查找对应的聚合函数索引
    size_t func_index = 0;
    for (size_t i = 0; i < agg_funcs_.size(); i++) {
        if (agg_funcs_[i].func_type == func_type && 
            agg_funcs_[i].col.tab_name == col.tab_name && 
            agg_funcs_[i].col.col_name == col.col_name) {
            func_index = i;
            break;
        }
    }
    
    if (func_index >= state.agg_values.size()) {
        // 如果没有找到对应的聚合值，返回默认值
        Value result;
        if (func_type == AGG_COUNT) {
            result.set_int(0);
        } else if (func_type == AGG_AVG) {
            result.set_float(0.0f);
        } else {
            result.set_int(0);
        }
        return result;
    }
    
    Value result = state.agg_values[func_index];
    
    if (func_type == AGG_AVG && state.count > 0) {
        // 计算平均值，使用double保持精度
        if (result.type == TYPE_FLOAT) {
            double sum_double = state.float_accumulators[func_index]; // 使用累加器里的总和
            double avg_double = sum_double / state.count;
            result.set_float(static_cast<float>(avg_double));
        }
    }
    
    return result;
}

bool AggregationExecutor::evaluate_having(const AggResult& result) {
    // 如果没有HAVING条件，直接返回true
    if (having_conds_.empty()) {
        return true;
    }
    
    std::cout << "DEBUG: Evaluating HAVING for group with count: " << result.count << std::endl;
    
    // 对每个HAVING条件进行评估
    for (size_t cond_idx = 0; cond_idx < having_conds_.size(); cond_idx++) {
        const auto& cond = having_conds_[cond_idx];
        bool cond_result = false;
        
        std::cout << "DEBUG: Processing condition " << cond_idx << std::endl;
        
        // 处理聚合条件左侧
        Value left_value;
        if (cond.lhs_col.tab_name.empty() && cond.lhs_col.col_name.empty() && cond.is_lhs_agg) {
            // 左侧是聚合函数
            int group_idx = -1;
            
            // 特殊处理 COUNT(*)
            if (cond.lhs_agg.func_type == AGG_COUNT && 
                cond.lhs_agg.col.tab_name.empty() && cond.lhs_agg.col.col_name.empty()) {
                // COUNT(*) - 直接使用分组计数
                left_value.set_int(result.count);
                std::cout << "DEBUG: LHS COUNT(*) = " << result.count << std::endl;
            } else {
                // 寻找匹配的聚合函数
                for (size_t i = 0; i < agg_funcs_.size(); i++) {
                    // 比较函数类型和列名，忽略表名的差异
                    if (agg_funcs_[i].func_type == cond.lhs_agg.func_type && 
                        agg_funcs_[i].col.col_name == cond.lhs_agg.col.col_name) {
                        group_idx = i;
                        break;
                    }
                }
                
                if (group_idx >= 0 && group_idx < result.agg_values.size()) {
                    left_value = result.agg_values[group_idx];
                    std::cout << "DEBUG: LHS agg function found at index " << group_idx << ", value: " << left_value.float_val << std::endl;
                } else {
                    // 找不到匹配的聚合函数，返回默认值
                    if (cond.lhs_agg.func_type == AGG_COUNT) {
                        left_value.set_int(0);
                    } else if (cond.lhs_agg.func_type == AGG_AVG) {
                        left_value.set_float(0.0f);
                    } else {
                        left_value.set_int(0);
                    }
                    std::cout << "DEBUG: LHS agg function not found, using default value: " << left_value.float_val << std::endl;
                }
            }
        } else if (!cond.lhs_col.tab_name.empty() || !cond.lhs_col.col_name.empty()) {
            // 左侧是分组列
            int group_idx = -1;
            
            // 寻找匹配的分组列
            for (size_t i = 0; i < group_by_cols_.size(); i++) {
                if (group_by_cols_[i].tab_name == cond.lhs_col.tab_name && 
                    group_by_cols_[i].col_name == cond.lhs_col.col_name) {
                    group_idx = i;
                    break;
                }
            }
            
            if (group_idx >= 0 && group_idx < result.group_key.size()) {
                left_value = result.group_key[group_idx];
            } else {
                // 找不到匹配的分组列，返回默认值
                left_value.set_int(0);
            }
        } else {
            // 左侧是常量值
            left_value = cond.lhs_value;
        }
        
        // 处理条件右侧
        Value right_value;
        if (cond.rhs_col.tab_name.empty() && cond.rhs_col.col_name.empty() && cond.is_rhs_agg) {
            // 右侧是聚合函数
            int group_idx = -1;
            
            // 特殊处理 COUNT(*)
            if (cond.rhs_agg.func_type == AGG_COUNT && 
                cond.rhs_agg.col.tab_name.empty() && cond.rhs_agg.col.col_name.empty()) {
                // COUNT(*) - 直接使用分组计数
                right_value.set_int(result.count);
            } else {
                // 寻找匹配的聚合函数
                for (size_t i = 0; i < agg_funcs_.size(); i++) {
                    // 比较函数类型和列名，忽略表名的差异
                    if (agg_funcs_[i].func_type == cond.rhs_agg.func_type && 
                        agg_funcs_[i].col.col_name == cond.rhs_agg.col.col_name) {
                        group_idx = i;
                        break;
                    }
                }

                if (group_idx >= 0 && group_idx < result.agg_values.size()) {
                    right_value = result.agg_values[group_idx];
                } else {
                    // 找不到匹配的聚合函数，返回默认值
                    if (cond.rhs_agg.func_type == AGG_COUNT) {
                        right_value.set_int(0);
                    } else if (cond.rhs_agg.func_type == AGG_AVG) {
                        right_value.set_float(0.0f);
                    } else {
                        right_value.set_int(0);
                    }
                }
            }
        } else if (!cond.rhs_col.tab_name.empty() || !cond.rhs_col.col_name.empty()) {
            // 右侧是分组列
            int group_idx = -1;

            // 寻找匹配的分组列
            for (size_t i = 0; i < group_by_cols_.size(); i++) {
                if (group_by_cols_[i].tab_name == cond.rhs_col.tab_name &&
                    group_by_cols_[i].col_name == cond.rhs_col.col_name) {
                    group_idx = i;
                    break;
                }
            }

            if (group_idx >= 0 && group_idx < result.group_key.size()) {
                right_value = result.group_key[group_idx];
            } else {
                // 找不到匹配的分组列，返回默认值
                right_value.set_int(0);
            }
        } else {
            // 右侧是常量值
            right_value = cond.rhs_val;
            std::cout << "DEBUG: RHS constant value = " << right_value.float_val << std::endl;
        }
        
        // 根据操作符计算条件结果
        if (left_value.type == TYPE_INT && right_value.type == TYPE_INT) {
            switch (cond.op) {
                case OP_EQ: cond_result = left_value.int_val == right_value.int_val; break;
                case OP_NE: cond_result = left_value.int_val != right_value.int_val; break;
                case OP_LT: cond_result = left_value.int_val < right_value.int_val; break;
                case OP_GT: cond_result = left_value.int_val > right_value.int_val; break;
                case OP_LE: cond_result = left_value.int_val <= right_value.int_val; break;
                case OP_GE: cond_result = left_value.int_val >= right_value.int_val; break;
                default: cond_result = false; break;
            }
            std::cout << "DEBUG: INT comparison: " << left_value.int_val << " " << cond.op << " " << right_value.int_val << " = " << cond_result << std::endl;
        } else if (left_value.type == TYPE_FLOAT && right_value.type == TYPE_FLOAT) {
            switch (cond.op) {
                case OP_EQ: cond_result = fabs(left_value.float_val - right_value.float_val) < 1e-6; break;
                case OP_NE: cond_result = fabs(left_value.float_val - right_value.float_val) >= 1e-6; break;
                case OP_LT: cond_result = left_value.float_val < right_value.float_val; break;
                case OP_GT: cond_result = left_value.float_val > right_value.float_val; break;
                case OP_LE: cond_result = left_value.float_val <= right_value.float_val; break;
                case OP_GE: cond_result = left_value.float_val >= right_value.float_val; break;
                default: cond_result = false; break;
            }
            std::cout << "DEBUG: FLOAT comparison: " << left_value.float_val << " " << cond.op << " " << right_value.float_val << " = " << cond_result << std::endl;
        } else if (left_value.type == TYPE_INT && right_value.type == TYPE_FLOAT) {
            // 将int转换为float进行比较
            float left_float = static_cast<float>(left_value.int_val);
            switch (cond.op) {
                case OP_EQ: cond_result = fabs(left_float - right_value.float_val) < 1e-6; break;
                case OP_NE: cond_result = fabs(left_float - right_value.float_val) >= 1e-6; break;
                case OP_LT: cond_result = left_float < right_value.float_val; break;
                case OP_GT: cond_result = left_float > right_value.float_val; break;
                case OP_LE: cond_result = left_float <= right_value.float_val; break;
                case OP_GE: cond_result = left_float >= right_value.float_val; break;
                default: cond_result = false; break;
            }
            std::cout << "DEBUG: INT->FLOAT comparison: " << left_float << " " << cond.op << " " << right_value.float_val << " = " << cond_result << std::endl;
        } else if (left_value.type == TYPE_FLOAT && right_value.type == TYPE_INT) {
            // 将int转换为float进行比较
            float right_float = static_cast<float>(right_value.int_val);
            switch (cond.op) {
                case OP_EQ: cond_result = fabs(left_value.float_val - right_float) < 1e-6; break;
                case OP_NE: cond_result = fabs(left_value.float_val - right_float) >= 1e-6; break;
                case OP_LT: cond_result = left_value.float_val < right_float; break;
                case OP_GT: cond_result = left_value.float_val > right_float; break;
                case OP_LE: cond_result = left_value.float_val <= right_float; break;
                case OP_GE: cond_result = left_value.float_val >= right_float; break;
                default: cond_result = false; break;
            }
            std::cout << "DEBUG: FLOAT->INT comparison: " << left_value.float_val << " " << cond.op << " " << right_float << " = " << cond_result << std::endl;
        } else if (left_value.type == TYPE_STRING && right_value.type == TYPE_STRING) {
            switch (cond.op) {
                case OP_EQ: cond_result = left_value.str_val == right_value.str_val; break;
                case OP_NE: cond_result = left_value.str_val != right_value.str_val; break;
                case OP_LT: cond_result = left_value.str_val < right_value.str_val; break;
                case OP_GT: cond_result = left_value.str_val > right_value.str_val; break;
                case OP_LE: cond_result = left_value.str_val <= right_value.str_val; break;
                case OP_GE: cond_result = left_value.str_val >= right_value.str_val; break;
                default: cond_result = false; break;
            }
        } else {
            // 类型不匹配，条件为false
            cond_result = false;
            std::cout << "DEBUG: Type mismatch: left=" << left_value.type << ", right=" << right_value.type << std::endl;
        }
        
        // 对于AND条件，一旦有一个条件为false，整个结果就为false
        if (!cond_result) {
            std::cout << "DEBUG: Condition " << cond_idx << " failed, returning false" << std::endl;
            return false;
        }
    }
    
    // 所有条件都满足
    std::cout << "DEBUG: All conditions satisfied, returning true" << std::endl;
    return true;
}

void AggregationExecutor::compute_final_results() {
    group_states_.clear();
    results_.clear();
    
    // 初始化子执行器
    prev_->beginTuple();
    
    // 遍历所有输入记录
    while (!prev_->is_end()) {
        auto record = prev_->Next();
        if (!record) break;
        
        std::string group_key = get_group_key(record);
        update_agg_state(group_states_[group_key], record);
        
        // 移动到下一条记录
        prev_->nextTuple();
    }
    
    // 如果没有数据，创建一个空的聚合状态
    if (group_states_.empty()) {
        AggState empty_state;
        empty_state.count = 0;
        // 初始化聚合值
        for (size_t i = 0; i < agg_funcs_.size(); i++) {
            Value agg_val;
            const auto& agg_func = agg_funcs_[i];
            if (agg_func.func_type == AGG_COUNT) {
                agg_val.set_int(0);
            } else if (agg_func.func_type == AGG_AVG) {
                agg_val.set_float(0.0f);
            } else {
                agg_val.set_int(0);
            }
            empty_state.agg_values.push_back(agg_val);
        }
        group_states_["global"] = empty_state;
    }
    
    // 计算最终结果
    std::vector<std::pair<std::string, AggState>> sorted_groups;
    for (const auto& pair : group_states_) {
        sorted_groups.push_back(pair);
    }
    
    // 如果没有ORDER BY，按照分组键的字典序排序，确保输出顺序稳定
    if (order_by_cols_.empty()) {
        std::sort(sorted_groups.begin(), sorted_groups.end(), 
                 [](const std::pair<std::string, AggState>& a, 
                    const std::pair<std::string, AggState>& b) {
                     return a.first < b.first;
                 });
    }
    
    for (const auto& pair : sorted_groups) {
        const auto& group_key_str = pair.first;
        const auto& state = pair.second;
        
        // 构建分组键值
        std::vector<Value> group_key_values;
        if (!group_by_cols_.empty()) {
            // 解析分组键字符串
            std::stringstream ss(group_key_str);
            std::string token;
            size_t i = 0;
            while (std::getline(ss, token, '|') && i < group_by_cols_.size()) {
                Value value;
                const auto& col_meta = cols_[i];
                if (col_meta.type == TYPE_INT) {
                    value.set_int(std::stoi(token));
                } else if (col_meta.type == TYPE_FLOAT) {
                    value.set_float(std::stof(token));
                } else {
                    value.set_str(token);
                }
                group_key_values.push_back(value);
                i++;
            }
        }
        
        // 计算聚合函数结果
        std::vector<Value> agg_values;
        for (size_t i = 0; i < agg_funcs_.size(); i++) {
            const auto& agg_func = agg_funcs_[i];
            Value agg_value;
            
            if (agg_func.func_type == AGG_COUNT && agg_func.col.tab_name.empty() && agg_func.col.col_name.empty()) {
                // COUNT(*)
                agg_value.set_int(state.count);
            } else if (i < state.agg_values.size()) {
                agg_value = state.agg_values[i];
                
                if (agg_func.func_type == AGG_AVG && state.count > 0) {
                    // 计算平均值，使用double累加器保持精度
                    if (i < state.float_accumulators.size()) {
                        double avg_double = state.float_accumulators[i] / state.count;
                        agg_value.set_float(static_cast<float>(avg_double));
                    }
                }
            } else {
                // 默认值
                if (agg_func.func_type == AGG_COUNT) {
                    agg_value.set_int(0);
                } else if (agg_func.func_type == AGG_AVG) {
                    agg_value.set_float(0.0f);
                } else {
                    agg_value.set_int(0);
                }
            }
            
            agg_values.push_back(agg_value);
        }
        
        AggResult result(group_key_values, agg_values, state.count);
        
        // 检查HAVING条件
        if (evaluate_having(result)) {
            results_.push_back(result);
        }
    }
    
    // 应用排序规则
    if (!order_by_cols_.empty()) {
        std::sort(results_.begin(), results_.end(), [this](const AggResult& a, const AggResult& b) {
            // 按照order_by_cols_中的顺序，依次比较各列
            for (size_t i = 0; i < order_by_cols_.size(); i++) {
                const auto& order_col = order_by_cols_[i];
                bool is_asc = order_by_directions_[i];  // true表示升序，false表示降序
                
                Value a_val, b_val;
                
                if (order_col.is_agg) {
                    // 排序列是聚合函数
                    int agg_idx = -1;
                    for (size_t j = 0; j < agg_funcs_.size(); j++) {
                        // 通过别名匹配聚合函数
                        if (agg_funcs_[j].alias == order_col.agg.alias) {
                            agg_idx = j;
                            break;
                        }
                    }
                    
                    if (agg_idx >= 0 && agg_idx < a.agg_values.size()) {
                        a_val = a.agg_values[agg_idx];
                        b_val = b.agg_values[agg_idx];
                    } else {
                        continue;  // 跳过无效的聚合函数
                    }
                } else {
                    // 排序列是分组列
                    int group_idx = -1;
                    for (size_t j = 0; j < group_by_cols_.size(); j++) {
                        if (group_by_cols_[j].tab_name == order_col.col.tab_name && 
                            group_by_cols_[j].col_name == order_col.col.col_name) {
                            group_idx = j;
                            break;
                        }
                    }
                    
                    if (group_idx >= 0 && group_idx < a.group_key.size()) {
                        a_val = a.group_key[group_idx];
                        b_val = b.group_key[group_idx];
                    } else {
                        continue;  // 跳过无效的列
                    }
                }
                
                // 根据值类型进行比较
                if (a_val.type == TYPE_INT && b_val.type == TYPE_INT) {
                    if (a_val.int_val != b_val.int_val) {
                        return is_asc ? (a_val.int_val < b_val.int_val) : (a_val.int_val > b_val.int_val);
                    }
                } else if (a_val.type == TYPE_FLOAT && b_val.type == TYPE_FLOAT) {
                    if (fabs(a_val.float_val - b_val.float_val) >= 1e-6) {
                        return is_asc ? (a_val.float_val < b_val.float_val) : (a_val.float_val > b_val.float_val);
                    }
                } else if (a_val.type == TYPE_STRING && b_val.type == TYPE_STRING) {
                    if (a_val.str_val != b_val.str_val) {
                        return is_asc ? (a_val.str_val < b_val.str_val) : (a_val.str_val > b_val.str_val);
                    }
                }
            }

            // 所有排序字段都相等，则认为相等
            return false;
        });
    }

    // 应用LIMIT
    if (limit_val_ > 0 && results_.size() > (size_t)limit_val_) {
        results_.resize(limit_val_);
    }
}

Value AggregationExecutor::get_column_value(const std::unique_ptr<RmRecord>& record, const TabCol& col) {
    // 添加调试信息
    if (col.tab_name.empty() && col.col_name.empty()) {
        std::cout << "DEBUG: get_column_value called with empty column name" << std::endl;
        Value value;
        value.set_int(0);
        return value;
    }
    
    if (col.col_name.empty()) {
        std::cout << "DEBUG: get_column_value called with empty col_name: tab_name=" << col.tab_name << std::endl;
        Value value;
        value.set_int(0);
        return value;
    }
    
    auto pos = get_col(prev_->cols(), col);
    Value value;
    
    if (pos == prev_->cols().end()) {
        // 如果找不到列，返回默认值
        std::cout << "DEBUG: Column not found: " << col.tab_name << "." << col.col_name << std::endl;
        value.set_int(0);
        return value;
    }
    
    if (pos->type == TYPE_INT) {
        value.set_int(*(int*)(record->data + pos->offset));
    } else if (pos->type == TYPE_FLOAT) {
        value.set_float(*(float*)(record->data + pos->offset));
    } else if (pos->type == TYPE_STRING) {
        value.set_str(std::string(record->data + pos->offset, pos->len));
    } else {
        // 未知类型，返回默认值
        value.set_int(0);
    }
    
    return value;
} 