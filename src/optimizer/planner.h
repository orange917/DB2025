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

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "execution/execution_defs.h"
#include "execution/execution_manager.h"
#include "record/rm.h"
#include "system/sm.h"
#include "common/context.h"
#include "plan.h"
#include "parser/parser.h"
#include "common/common.h"
#include "analyze/analyze.h"

class Planner {
   private:
    SmManager *sm_manager_;

    bool enable_nestedloop_join = true;
    bool enable_sortmerge_join = false;

   public:
    Planner(SmManager *sm_manager) : sm_manager_(sm_manager) {}


    std::shared_ptr<Plan> do_planner(std::shared_ptr<Query> query, Context *context);

    void set_enable_nestedloop_join(bool set_val) { enable_nestedloop_join = set_val; }

    void set_enable_sortmerge_join(bool set_val) { enable_sortmerge_join = set_val; }

   private:
    std::shared_ptr<Query> logical_optimization(std::shared_ptr<Query> query, Context *context);
    std::shared_ptr<Plan> physical_optimization(std::shared_ptr<Query> query, Context *context);

    std::shared_ptr<Plan> make_one_rel(std::shared_ptr<Query> query);
    std::shared_ptr<Plan> build_join_plan(std::shared_ptr<Query> query, const std::vector<std::string>& tables);

    std::shared_ptr<Plan> generate_sort_plan(std::shared_ptr<Query> query, std::shared_ptr<Plan> plan);

    std::shared_ptr<Plan> generate_select_plan(std::shared_ptr<Query> query, Context *context);


    // int get_indexNo(std::string tab_name, std::vector<Condition> curr_conds);
    bool get_index_cols(std::string tab_name, std::vector<Condition> curr_conds, std::vector<std::string>& index_col_names);

    ColType interp_sv_type(ast::SvType sv_type) {
        std::map<ast::SvType, ColType> m = {
            {ast::SV_TYPE_INT, TYPE_INT}, {ast::SV_TYPE_FLOAT, TYPE_FLOAT}, {ast::SV_TYPE_STRING, TYPE_STRING}};
        return m.at(sv_type);
    }
    
    CompOp convert_sv_comp_op(ast::SvCompOp op);
    Value convert_sv_value(const std::shared_ptr<ast::Value> &sv_val);
};

struct OrderByCol {
    TabCol col;         // 排序列
    AggFunc agg;        // 聚合函数
    bool is_agg;        // 是否是聚合函数列
};

class AggPlan : public Plan
{
public:
    AggPlan(std::shared_ptr<Plan> subplan, const std::vector<AggFunc> &agg_funcs,
        const std::vector<TabCol> &group_by_cols,
        const std::vector<Condition> &having_conds,
        const std::vector<OrderByCol> &order_by_cols,
        const std::vector<bool> &order_by_directions,
        int limit_val)
    : agg_funcs_(agg_funcs), group_by_cols_(group_by_cols), having_conds_(having_conds),
    order_by_cols_(order_by_cols), order_by_directions_(order_by_directions), 
    limit_val_(limit_val), subplan_(std::move(subplan)) {}

    std::vector<AggFunc> agg_funcs_;
    std::vector<TabCol> group_by_cols_;
    std::vector<Condition> having_conds_;
    std::vector<OrderByCol> order_by_cols_;
    std::vector<bool> order_by_directions_;
    int limit_val_;
    std::shared_ptr<Plan> subplan_;
};
