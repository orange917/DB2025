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

#include "parser/parser.h"
#include "system/sm.h"
#include "common/common.h"

// 前向声明
struct OrderByCol;

class Query{
    public:
    std::shared_ptr<ast::TreeNode> parse;
    // TODO jointree
    // where条件
    std::vector<Condition> conds;
    // 投影列
    std::vector<TabCol> cols;
    // 表名
    std::vector<std::string> tables;
    // update 的set 值
    std::vector<SetClause> set_clauses;
    //insert 的values值
    std::vector<Value> values;
    
    // 添加聚合函数和分组支持
    std::vector<AggFunc> agg_funcs;  // 聚合函数列表
    std::vector<TabCol> group_by_cols;  // GROUP BY 列
    std::vector<Condition> having_conds;  // HAVING 条件
    int limit_val;  // LIMIT 值
    bool has_agg;  // 是否有聚合函数
    bool has_group_by;  // 是否有GROUP BY
    bool has_having;  // 是否有HAVING
    bool has_limit;  // 是否有LIMIT
    
    // 添加 ORDER BY 支持
    std::vector<OrderByCol> order_by_cols;  // ORDER BY 列
    std::vector<bool> order_by_directions;  // ORDER BY 方向 (true=ASC, false=DESC)
    bool has_order_by;  // 是否有ORDER BY
    
    // 添加连接表达式支持
    std::vector<std::shared_ptr<ast::JoinExpr>> jointree;  // 连接表达式树
    bool has_join;  // 是否有连接

    Query(){
        has_agg = false;
        has_group_by = false;
        has_having = false;
        has_limit = false;
        has_order_by = false;
        has_join = false;
        limit_val = -1;
    }

};

class Analyze
{
private:
    SmManager *sm_manager_;
public:
    Analyze(SmManager *sm_manager) : sm_manager_(sm_manager){}
    ~Analyze(){}

    std::shared_ptr<Query> do_analyze(std::shared_ptr<ast::TreeNode> root);

private:
    TabCol check_column(const std::vector<ColMeta> &all_cols, TabCol target);
    void get_all_cols(const std::vector<std::string> &tab_names, std::vector<ColMeta> &all_cols);
    void get_clause(const std::vector<std::shared_ptr<ast::BinaryExpr>> &sv_conds, std::vector<Condition> &conds);
    void check_clause(const std::vector<std::string> &tab_names, std::vector<Condition> &conds);
    Value convert_sv_value(const std::shared_ptr<ast::Value> &sv_val);
    CompOp convert_sv_comp_op(ast::SvCompOp op);
    AggFuncType convert_agg_func_type(ast::AggFuncType sv_agg_func_type);
};

