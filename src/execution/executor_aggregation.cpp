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

AggregationExecutor::AggregationExecutor(std::unique_ptr<AbstractExecutor> prev,
                                       const std::vector<AggFunc>& agg_funcs,
                                       const std::vector<TabCol>& group_by_cols,
                                       const std::vector<Condition>& having_conds,
                                       int limit_val)
    : prev_(std::move(prev)), agg_funcs_(agg_funcs), group_by_cols_(group_by_cols),
      having_conds_(having_conds), limit_val_(limit_val), current_result_idx_(0), is_initialized_(false) {
    
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
    
    // 确保聚合值数组有足够的元素，并正确初始化
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
    }
    
    for (size_t i = 0; i < agg_funcs_.size(); i++) {
        const auto& agg_func = agg_funcs_[i];
        
        if (agg_func.func_type == AGG_COUNT && agg_func.col.tab_name.empty() && agg_func.col.col_name.empty()) {
            // COUNT(*) - 已经在上面处理了count++
            continue;
        }
        
        Value value = get_column_value(record, agg_func.col);
        
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
                    state.agg_values[i].set_float(state.agg_values[i].float_val + value.float_val);
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
                
            case AGG_AVG:
                // AVG需要特殊处理，先累加，最后计算平均值
                if (state.agg_values[i].type == TYPE_FLOAT && value.type == TYPE_FLOAT) {
                    state.agg_values[i].set_float(state.agg_values[i].float_val + value.float_val);
                } else {
                    // 初始化聚合值
                    if (value.type == TYPE_INT) {
                        state.agg_values[i].set_float((float)value.int_val);
                    } else if (value.type == TYPE_FLOAT) {
                        state.agg_values[i].set_float(value.float_val);
                    } else {
                        state.agg_values[i].set_float(0.0f);
                    }
                }
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
        // 计算平均值
        if (result.type == TYPE_FLOAT) {
            result.set_float(result.float_val / state.count);
        }
    }
    
    return result;
}

bool AggregationExecutor::evaluate_having(const AggResult& result) {
    // 简化实现：暂时返回true
    // TODO: 实现HAVING条件评估
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
    for (const auto& pair : group_states_) {
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
                    // 计算平均值
                    if (agg_value.type == TYPE_FLOAT) {
                        agg_value.set_float(agg_value.float_val / state.count);
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
        
        AggResult result(group_key_values, agg_values);
        
        // 检查HAVING条件
        if (evaluate_having(result)) {
            results_.push_back(result);
        }
    }
    
    // 应用LIMIT
    if (limit_val_ > 0 && results_.size() > (size_t)limit_val_) {
        results_.resize(limit_val_);
    }
}

Value AggregationExecutor::get_column_value(const std::unique_ptr<RmRecord>& record, const TabCol& col) {
    auto pos = get_col(prev_->cols(), col);
    Value value;
    
    if (pos == prev_->cols().end()) {
        // 如果找不到列，返回默认值
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