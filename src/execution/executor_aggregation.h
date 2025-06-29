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
#include "executor_abstract.h"
#include "common/common.h"
#include "system/sm.h"
#include "optimizer/planner.h"  // 包含OrderByCol定义
#include <unordered_map>
#include <vector>
#include <memory>

// 聚合结果结构
struct AggResult {
    std::vector<Value> group_key;  // 分组键
    std::vector<Value> agg_values; // 聚合函数结果
    int count;                     // 分组计数（用于COUNT(*)）
    
    AggResult() : count(0) {}
    AggResult(std::vector<Value> group_key, std::vector<Value> agg_values, int count = 0) 
        : group_key(std::move(group_key)), agg_values(std::move(agg_values)), count(count) {}
};

// 聚合状态结构
struct AggState {
    int count;
    std::vector<Value> agg_values;
    std::vector<double> float_accumulators;  // 用于浮点数聚合的double累加器
    
    AggState() : count(0) {}
};

class AggregationExecutor : public AbstractExecutor {
private:
    std::vector<OrderByCol> order_by_cols_; // ORDER BY列
    std::vector<bool> order_by_directions_; // 排序方向，true为ASC，false为DESC
    std::unique_ptr<AbstractExecutor> prev_;           // 子执行器
    std::vector<AggFunc> agg_funcs_;                   // 聚合函数列表
    std::vector<TabCol> group_by_cols_;                // GROUP BY列
    std::vector<Condition> having_conds_;              // HAVING条件
    int limit_val_;                                    // LIMIT值
    std::vector<ColMeta> cols_;                        // 输出列元数据
    size_t len_;                                       // 输出记录长度
    
    // 聚合相关
    std::unordered_map<std::string, AggState> group_states_; // 分组状态
    std::vector<AggResult> results_;                   // 最终结果
    size_t current_result_idx_;                        // 当前结果索引
    bool is_initialized_;                              // 是否已初始化
    
    // 辅助函数
    std::string get_group_key(const std::unique_ptr<RmRecord>& record);
    void update_agg_state(AggState& state, const std::unique_ptr<RmRecord>& record);
    Value get_agg_value(const AggState& state, AggFuncType func_type, const TabCol& col);
    bool evaluate_having(const AggResult& result);
    void compute_final_results();
    Value get_column_value(const std::unique_ptr<RmRecord>& record, const TabCol& col);

public:
    AggregationExecutor(std::unique_ptr<AbstractExecutor> prev,
            const std::vector<AggFunc>& agg_funcs,
            const std::vector<TabCol>& group_by_cols,
            const std::vector<Condition>& having_conds,
            const std::vector<OrderByCol>& order_by_cols,
            const std::vector<bool>& order_by_directions,
            int limit_val);

    ~AggregationExecutor() = default;

    void beginTuple() override;
    void nextTuple() override;
    bool is_end() const override;
    std::unique_ptr<RmRecord> Next() override;
    const std::vector<ColMeta>& cols() const override;
    size_t tupleLen() const override;
    Rid& rid() override;
};