/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "planner.h"
#include "errors.h"
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <set>
#include <sstream>

#include "execution/executor_delete.h"
#include "execution/executor_index_scan.h"
#include "execution/executor_insert.h"
#include "execution/executor_nestedloop_join.h"
#include "execution/executor_projection.h"
#include "execution/executor_seq_scan.h"
#include "execution/executor_update.h"
#include "index/ix.h"
#include "optimizer/plan.h"
#include "record_printer.h"
#include "common/common.h"
#include "defs.h"

// comp_op2str 实现
static std::string comp_op2str(CompOp op) {
    switch (op) {
        case OP_EQ: return "=";
        case OP_NE: return "!=";
        case OP_LT: return "<";
        case OP_LE: return "<=";
        case OP_GT: return ">";
        case OP_GE: return ">=";
        default: return "?";
    }
}

// 目前的索引匹配规则为：完全匹配索引字段，且全部为单点查询，不会自动调整where条件的顺序
bool Planner::get_index_cols(std::string tab_name, std::vector<Condition> curr_conds, std::vector<std::string>& index_col_names) {
    index_col_names.clear();
    TabMeta& tab = sm_manager_->db_.get_table(tab_name);
    int best_match = 0;
    std::vector<std::string> best_index_cols;

    // 存储条件中出现的列以及对应的操作符
    std::map<std::string, std::vector<std::pair<CompOp, bool>>> col_conditions;

    // 收集条件中的所有列名和操作符类型
    for (const auto& cond : curr_conds) {
        // 处理条件左侧为该表列的情况
        if (cond.lhs_col.tab_name == tab_name && cond.is_rhs_val) {
            // 存储列名及其操作符
            col_conditions[cond.lhs_col.col_name].push_back({cond.op, true});
        }
        // 处理条件左侧为该表列，右侧也是列的情况
        else if (cond.lhs_col.tab_name == tab_name && !cond.is_rhs_val) {
            col_conditions[cond.lhs_col.col_name].push_back({cond.op, false});
        }
        // 处理条件右侧为该表列的情况，需要翻转操作符
        else if (!cond.is_rhs_val && cond.rhs_col.tab_name == tab_name) {
            CompOp reversed_op;
            switch (cond.op) {
                case OP_EQ: reversed_op = OP_EQ; break;
                case OP_NE: reversed_op = OP_NE; break;
                case OP_LT: reversed_op = OP_GT; break;
                case OP_GT: reversed_op = OP_LT; break;
                case OP_LE: reversed_op = OP_GE; break;
                case OP_GE: reversed_op = OP_LE; break;
                default: reversed_op = cond.op;
            }
            col_conditions[cond.rhs_col.col_name].push_back({reversed_op, false});
        }
    }

    // 遍历所有可用索引
    for (const auto& index : tab.indexes) {
        int match = 0;
        bool can_continue = true;
        std::vector<std::string> matched_cols;

        // 遍历索引中的每个列，检查最左前缀匹配
        for (size_t i = 0; i < index.cols.size() && can_continue; ++i) {
            const auto& idx_col = index.cols[i];
            auto it = col_conditions.find(idx_col.name);

            if (it != col_conditions.end()) {
                // 检查是否有可用于索引的操作符
                bool col_usable = false;

                for (const auto& op_pair : it->second) {
                    CompOp op = op_pair.first;

                    // 第一个列允许任何比较操作，后续列只有在前面都是等值条件时才能继续最左前缀匹配
                    if (i == 0 || (match == i && op == OP_EQ)) {
                        if (op == OP_EQ || op == OP_LT || op == OP_LE || op == OP_GT || op == OP_GE) {
                            col_usable = true;
                            break;
                        }
                    }
                }

                if (col_usable) {
                    match++;
                    matched_cols.push_back(idx_col.name);
                } else if (i == 0) {
                    // 第一列必须可用，否则不满足最左前缀原则
                    can_continue = false;
                } else {
                    // 如果当前列不可用，停止匹配后续列，但保留已匹配的前缀
                    break;
                }
            } else {
                // 如果索引列不在条件中，停止匹配后续列，但保留已匹配的前缀
                break;
            }
        }

        // 更新最佳匹配
        if (match > best_match) {
            best_match = match;
            best_index_cols = matched_cols;  // 只返回实际匹配的列
        }
    }

    if (best_match > 0) {
        index_col_names = best_index_cols;
        return true;
    }
    return false;
}

/**
 * @brief 表算子条件谓词生成
 *
 * @param conds 条件
 * @param tab_names 表名
 * @return std::vector<Condition>
 */
std::vector<Condition> pop_conds(std::vector<Condition> &conds, std::string tab_names) {
    // auto has_tab = [&](const std::string &tab_name) {
    //     return std::find(tab_names.begin(), tab_names.end(), tab_name) != tab_names.end();
    // };
    std::vector<Condition> solved_conds;
    auto it = conds.begin();
    while (it != conds.end()) {
        if ((tab_names.compare(it->lhs_col.tab_name) == 0 && it->is_rhs_val) || (it->lhs_col.tab_name.compare(it->rhs_col.tab_name) == 0)) {
            solved_conds.emplace_back(std::move(*it));
            it = conds.erase(it);
        } else {
            it++;
        }
    }
    return solved_conds;
}

int push_conds(Condition *cond, std::shared_ptr<Plan> plan)
{
    if(auto x = std::dynamic_pointer_cast<ScanPlan>(plan))
    {
        if(x->tab_name_.compare(cond->lhs_col.tab_name) == 0) {
            return 1;
        } else if(x->tab_name_.compare(cond->rhs_col.tab_name) == 0){
            return 2;
        } else {
            return 0;
        }
    }
    else if(auto x = std::dynamic_pointer_cast<JoinPlan>(plan))
    {
        int left_res = push_conds(cond, x->left_);
        // 条件已经下推到左子节点
        if(left_res == 3){
            return 3;
        }
        int right_res = push_conds(cond, x->right_);
        // 条件已经下推到右子节点
        if(right_res == 3){
            return 3;
        }
        // 左子节点或右子节点有一个没有匹配到条件的列
        if(left_res == 0 || right_res == 0) {
            return left_res + right_res;
        }
        // 左子节点匹配到条件的右边
        if(left_res == 2) {
            // 需要将左右两边的条件变换位置
            std::map<CompOp, CompOp> swap_op = {
                {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT}, {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},
            };
            std::swap(cond->lhs_col, cond->rhs_col);
            cond->op = swap_op.at(cond->op);
        }
        x->conds_.emplace_back(std::move(*cond));
        return 3;
    }
    return false;
}

std::shared_ptr<Plan> pop_scan(int *scantbl, std::string table, std::vector<std::string> &joined_tables, 
                std::vector<std::shared_ptr<Plan>> plans)
{
    for (size_t i = 0; i < plans.size(); i++) {
        auto x = std::dynamic_pointer_cast<ScanPlan>(plans[i]);
        if(x->tab_name_.compare(table) == 0)
        {
            scantbl[i] = 1;
            joined_tables.emplace_back(x->tab_name_);
            return plans[i];
        }
    }
    return nullptr;
}


std::shared_ptr<Query> Planner::logical_optimization(std::shared_ptr<Query> query, Context *context)
{
    // 投影下推优化：分析每个表需要的列，但是当SELECT *时不进行投影下推
    if (query->has_join && !query->jointree.empty() && !query->is_select_all) {
        // 为每个表分别收集不同用途的列
        std::map<std::string, std::set<std::string>> select_cols;  // 最终SELECT需要的列
        std::map<std::string, std::set<std::string>> join_cols;    // JOIN条件需要的列
        std::map<std::string, std::set<std::string>> filter_cols;  // WHERE条件需要的列
        
        // 初始化
        for (const auto& table : query->tables) {
            select_cols[table] = std::set<std::string>();
            join_cols[table] = std::set<std::string>();
            filter_cols[table] = std::set<std::string>();
        }
        
        // 1. 从SELECT列表中收集需要的列
        for (const auto& col : query->cols) {
            if (!col.tab_name.empty()) {
                select_cols[col.tab_name].insert(col.col_name);
            }
        }
        
        // 2. 从WHERE条件中收集需要的列（这些列只用于过滤）
        for (const auto& cond : query->conds) {
            if (!cond.lhs_col.tab_name.empty()) {
                filter_cols[cond.lhs_col.tab_name].insert(cond.lhs_col.col_name);
            }
            if (!cond.is_rhs_val && !cond.rhs_col.tab_name.empty()) {
                filter_cols[cond.rhs_col.tab_name].insert(cond.rhs_col.col_name);
            }
        }
        
        // 3. 从JOIN条件中收集需要的列（这些列用于连接）
        for (const auto& join_expr : query->jointree) {
            for (const auto& cond_expr : join_expr->conds) {
                if (auto lhs_col = std::dynamic_pointer_cast<ast::Col>(cond_expr->lhs)) {
                    std::string alias_name = lhs_col->tab_name;
                    std::string real_tab_name = alias_name;
                    // 查找对应的真实表名
                    for (const auto& pair : query->table_to_alias) {
                        if (pair.second == alias_name) {
                            real_tab_name = pair.first;
                            break;
                        }
                    }
                    join_cols[real_tab_name].insert(lhs_col->col_name);
                }
                if (auto rhs_col = std::dynamic_pointer_cast<ast::Col>(cond_expr->rhs)) {
                    std::string alias_name = rhs_col->tab_name;
                    std::string real_tab_name = alias_name;
                    // 查找对应的真实表名
                    for (const auto& pair : query->table_to_alias) {
                        if (pair.second == alias_name) {
                            real_tab_name = pair.first;
                            break;
                        }
                    }
                    join_cols[real_tab_name].insert(rhs_col->col_name);
                }
            }
        }
        
        // 4. 计算每个表在Filter之后需要的列（去除只用于Filter的列）
        for (const auto& table : query->tables) {
            query->table_projections[table] = std::vector<TabCol>();
            
            // 添加SELECT需要的列
            for (const auto& col_name : select_cols[table]) {
                std::string original_tab_name = query->table_to_alias.count(table) ? query->table_to_alias[table] : table;
                query->table_projections[table].push_back(TabCol(table, col_name, "", original_tab_name));
            }
            
            // 添加JOIN需要的列（但不在SELECT中的）
            for (const auto& col_name : join_cols[table]) {
                if (select_cols[table].find(col_name) == select_cols[table].end()) {
                    std::string original_tab_name = query->table_to_alias.count(table) ? query->table_to_alias[table] : table;
                    query->table_projections[table].push_back(TabCol(table, col_name, "", original_tab_name));
                }
            }
            
            // 注意：filter_cols中的列如果不在select_cols或join_cols中，则在Filter后就不再需要了，所以不添加到投影中
        }
    }

    return query;
}

std::shared_ptr<Plan> Planner::physical_optimization(std::shared_ptr<Query> query, Context *context)
{
    std::shared_ptr<Plan> plan;

    // 其他物理优化
    // 只有一个表就不用连接
    if(query->tables.size() == 1)
    {
        // 找这个表的索引
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(query->tables[0], query->conds, index_col_names);
        std::string original_tab_name = query->table_to_alias.count(query->tables[0]) ? query->table_to_alias[query->tables[0]] : query->tables[0];
        if(index_exist){
            plan = std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, query->tables[0], std::vector<Condition>(), index_col_names, original_tab_name);
        }
        else {
            // 没有索引，用顺序扫描
            index_col_names.clear();
            plan = std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, query->tables[0], std::vector<Condition>(), index_col_names, original_tab_name);
        }
        
        // 如果有条件，创建FilterPlan
        if (!query->conds.empty()) {
            plan = std::make_shared<FilterPlan>(plan, query->conds);
        }
    }else if(query->tables.size() > 1){
        // 多表连接
        plan = make_one_rel(query);
    }
    // 移除这里的 ORDER BY 处理，因为它在 generate_select_plan 中处理
    // plan = generate_sort_plan(query, std::move(plan)); 

    return plan;
}

std::shared_ptr<Plan> Planner::build_join_plan(std::shared_ptr<Query> query, const std::vector<std::string>& tables) {
    // 创建表扫描计划
    std::vector<std::shared_ptr<Plan>> table_scan_executors(tables.size());
    std::vector<int> table_row_counts(tables.size());
    for (size_t i = 0; i < tables.size(); i++) {
        auto curr_conds = pop_conds(query->conds, tables[i]);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(tables[i], curr_conds, index_col_names);
        std::shared_ptr<Plan> scan_plan;
        std::string original_tab_name = query->table_to_alias.count(tables[i]) ? query->table_to_alias[tables[i]] : tables[i];
        if (index_exist == false) {
            index_col_names.clear();
            scan_plan = std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, tables[i], std::vector<Condition>(), index_col_names, original_tab_name);
        } else {
            scan_plan = std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, tables[i], std::vector<Condition>(), index_col_names, original_tab_name);
        }
        
        // 如果有条件，创建FilterPlan
        if (!curr_conds.empty()) {
            scan_plan = std::make_shared<FilterPlan>(scan_plan, curr_conds);
        }
        
        // 投影下推：在Scan上方添加Project节点，但是当SELECT *时不添加
        if (!query->is_select_all && query->table_projections.count(tables[i]) && !query->table_projections[tables[i]].empty()) {
            scan_plan = std::make_shared<ProjectionPlan>(T_Projection, scan_plan, query->table_projections[tables[i]]);
        }
        
        table_scan_executors[i] = scan_plan;
        // 获取 row_count
        table_row_counts[i] = sm_manager_->db_.get_table(tables[i]).row_count;
    }
    
    // 收集所有连接条件
    std::vector<Condition> all_join_conds;
    for (const auto& join_expr : query->jointree) {
        for (const auto& cond : join_expr->conds) {
            Condition join_cond;
            
            // 处理左操作数
            if (auto lhs_col = std::dynamic_pointer_cast<ast::Col>(cond->lhs)) {
                std::string original_tab_name = lhs_col->tab_name;
                std::string actual_tab_name = original_tab_name;
                // 如果使用的是别名，需要解析为实际表名
                for (const auto& pair : query->table_to_alias) {
                    if (pair.second == original_tab_name) {
                        actual_tab_name = pair.first;
                        break;
                    }
                }
                join_cond.lhs_col = TabCol(actual_tab_name, lhs_col->col_name, "", original_tab_name);
                join_cond.is_lhs_agg = false;
            } else {
                throw InternalError("Join condition lhs must be a column");
            }
            
            // 处理右操作数，允许是列或常数
            if (auto rhs_val = std::dynamic_pointer_cast<ast::Value>(cond->rhs)) {
                join_cond.is_rhs_val = true;
                join_cond.is_rhs_agg = false;
                join_cond.rhs_val = convert_sv_value(rhs_val);
            } else if (auto rhs_col = std::dynamic_pointer_cast<ast::Col>(cond->rhs)) {
                join_cond.is_rhs_val = false;
                join_cond.is_rhs_agg = false;
                std::string original_tab_name = rhs_col->tab_name;
                std::string actual_tab_name = original_tab_name;
                // 如果使用的是别名，需要解析为实际表名
                for (const auto& pair : query->table_to_alias) {
                    if (pair.second == original_tab_name) {
                        actual_tab_name = pair.first;
                        break;
                    }
                }
                join_cond.rhs_col = TabCol(actual_tab_name, rhs_col->col_name, "", original_tab_name);
            } else {
                throw InternalError("Join condition rhs must be a column or value");
            }
            
            join_cond.op = convert_sv_comp_op(cond->op);
            all_join_conds.push_back(join_cond);
        }
    }
    
    // 如果只有一个表，直接返回
    if (tables.size() == 1) {
        return table_scan_executors[0];
    }
    
    // 贪心连接顺序优化
    std::vector<bool> used(tables.size(), false);
    std::vector<std::shared_ptr<Plan>> join_order;
    std::vector<std::string> join_order_names;
    
    // 1. 先选基数最小的两个表
    int min1 = -1, min2 = -1;
    for (size_t i = 0; i < tables.size(); ++i) {
        if (min1 == -1 || table_row_counts[i] < table_row_counts[min1]) min1 = i;
    }
    for (size_t i = 0; i < tables.size(); ++i) {
        if (i != min1 && (min2 == -1 || table_row_counts[i] < table_row_counts[min2])) min2 = i;
    }
    used[min1] = used[min2] = true;
    join_order.push_back(table_scan_executors[min1]);
    join_order.push_back(table_scan_executors[min2]);
    join_order_names.push_back(tables[min1]);
    join_order_names.push_back(tables[min2]);
    
    // 2. 每次从剩余表中选择基数最小的表加入
    for (size_t k = 2; k < tables.size(); ++k) {
        int min_next = -1;
        for (size_t i = 0; i < tables.size(); ++i) {
            if (!used[i] && (min_next == -1 || table_row_counts[i] < table_row_counts[min_next])) min_next = i;
        }
        used[min_next] = true;
        join_order.push_back(table_scan_executors[min_next]);
        join_order_names.push_back(tables[min_next]);
    }
    
    // 3. 按顺序左深树 join
    std::shared_ptr<Plan> plan = join_order[0];
    
    for (size_t i = 1; i < join_order.size(); ++i) {
        // 查找连接条件：新表与左子树中所有表的连接条件
        std::vector<Condition> join_conds;
        std::string new_table = join_order_names[i];
        
        // 收集左子树中的所有表名
        std::set<std::string> left_tables;
        std::function<void(const Plan*)> collect_left_tables = [&](const Plan* p) {
            if (auto scan = dynamic_cast<const ScanPlan*>(p)) {
                left_tables.insert(scan->tab_name_);
            } else if (auto filter = dynamic_cast<const FilterPlan*>(p)) {
                collect_left_tables(filter->subplan_.get());
            } else if (auto projection = dynamic_cast<const ProjectionPlan*>(p)) {
                collect_left_tables(projection->subplan_.get());
            } else if (auto join = dynamic_cast<const JoinPlan*>(p)) {
                collect_left_tables(join->left_.get());
                collect_left_tables(join->right_.get());
            }
        };
        collect_left_tables(plan.get());
        
        // 查找涉及新表和左子树中任意表的连接条件
        for (const auto& cond : all_join_conds) {
            if (!cond.is_rhs_val) {
                // 检查条件是否涉及新表和左子树中的表
                bool lhs_is_new = (cond.lhs_col.tab_name == new_table);
                bool rhs_is_new = (cond.rhs_col.tab_name == new_table);
                bool lhs_in_left = left_tables.count(cond.lhs_col.tab_name) > 0;
                bool rhs_in_left = left_tables.count(cond.rhs_col.tab_name) > 0;
                
                // 条件连接新表和左子树中的表
                if ((lhs_is_new && rhs_in_left) || (rhs_is_new && lhs_in_left)) {
                    join_conds.push_back(cond);
                }
            }
        }
        
        plan = std::make_shared<JoinPlan>(T_NestLoop, plan, join_order[i], join_conds);
    }
    
    return plan;
}

std::shared_ptr<Plan> Planner::make_one_rel(std::shared_ptr<Query> query)
{
    auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse);
    std::vector<std::string> tables = query->tables;
    // 处理连接表达式
    if (query->has_join && !query->jointree.empty()) {
        // 如果有连接表达式，使用连接表达式来构建查询计划
        return build_join_plan(query, tables);
    }
    // Scan table , 生成表算子列表tab_nodes
    std::vector<std::shared_ptr<Plan>> table_scan_executors(tables.size());
    std::vector<int> table_row_counts(tables.size());
    for (size_t i = 0; i < tables.size(); i++) {
        auto curr_conds = pop_conds(query->conds, tables[i]);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(tables[i], curr_conds, index_col_names);
        std::shared_ptr<Plan> scan_plan;
        std::string original_tab_name = query->table_to_alias.count(tables[i]) ? query->table_to_alias[tables[i]] : tables[i];
        if (index_exist == false) {
            index_col_names.clear();
            scan_plan = std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, tables[i], std::vector<Condition>(), index_col_names, original_tab_name);
        } else {
            scan_plan = std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, tables[i], std::vector<Condition>(), index_col_names, original_tab_name);
        }
        
        // 如果有条件，创建FilterPlan
        if (!curr_conds.empty()) {
            scan_plan = std::make_shared<FilterPlan>(scan_plan, curr_conds);
        }
        
        // 投影下推：在Scan上方添加Project节点，但是当SELECT *时不添加
        if (!query->is_select_all && query->table_projections.count(tables[i]) && !query->table_projections[tables[i]].empty()) {
            scan_plan = std::make_shared<ProjectionPlan>(T_Projection, scan_plan, query->table_projections[tables[i]]);
        }
        
        table_scan_executors[i] = scan_plan;
        // 获取 row_count
        table_row_counts[i] = sm_manager_->db_.get_table(tables[i]).row_count;
    }
    if(tables.size() == 1) {
        return table_scan_executors[0];
    }
    // 贪心连接顺序优化
    std::vector<bool> used(tables.size(), false);
    std::vector<std::shared_ptr<Plan>> join_order;
    std::vector<std::string> join_order_names;
    // 1. 先选基数最小的两个表
    int min1 = -1, min2 = -1;
    for (size_t i = 0; i < tables.size(); ++i) {
        if (min1 == -1 || table_row_counts[i] < table_row_counts[min1]) min1 = i;
    }
    for (size_t i = 0; i < tables.size(); ++i) {
        if (i != min1 && (min2 == -1 || table_row_counts[i] < table_row_counts[min2])) min2 = i;
    }
    used[min1] = used[min2] = true;
    join_order.push_back(table_scan_executors[min1]);
    join_order.push_back(table_scan_executors[min2]);
    join_order_names.push_back(tables[min1]);
    join_order_names.push_back(tables[min2]);
    // 2. 每次从剩余表中选择基数最小的表加入
    for (size_t k = 2; k < tables.size(); ++k) {
        int min_next = -1;
        for (size_t i = 0; i < tables.size(); ++i) {
            if (!used[i] && (min_next == -1 || table_row_counts[i] < table_row_counts[min_next])) min_next = i;
        }
        used[min_next] = true;
        join_order.push_back(table_scan_executors[min_next]);
        join_order_names.push_back(tables[min_next]);
    }
    // 3. 按顺序左深树 join
    std::shared_ptr<Plan> plan = join_order[0];
    
    for (size_t i = 1; i < join_order.size(); ++i) {
        // 查找连接条件：新表与左子树中所有表的连接条件
        std::vector<Condition> join_conds;
        std::string new_table = join_order_names[i];
        
        // 收集左子树中的所有表名
        std::set<std::string> left_tables;
        std::function<void(const Plan*)> collect_left_tables = [&](const Plan* p) {
            if (auto scan = dynamic_cast<const ScanPlan*>(p)) {
                left_tables.insert(scan->tab_name_);
            } else if (auto filter = dynamic_cast<const FilterPlan*>(p)) {
                collect_left_tables(filter->subplan_.get());
            } else if (auto projection = dynamic_cast<const ProjectionPlan*>(p)) {
                collect_left_tables(projection->subplan_.get());
            } else if (auto join = dynamic_cast<const JoinPlan*>(p)) {
                collect_left_tables(join->left_.get());
                collect_left_tables(join->right_.get());
            }
        };
        collect_left_tables(plan.get());
        
        // 查找涉及新表和左子树中任意表的连接条件
        auto it = query->conds.begin();
        while (it != query->conds.end()) {
            bool found_condition = false;
            if (!it->is_rhs_val) {
                // 检查条件是否涉及新表和左子树中的表
                bool lhs_is_new = (it->lhs_col.tab_name == new_table);
                bool rhs_is_new = (it->rhs_col.tab_name == new_table);
                bool lhs_in_left = left_tables.count(it->lhs_col.tab_name) > 0;
                bool rhs_in_left = left_tables.count(it->rhs_col.tab_name) > 0;
                
                // 条件连接新表和左子树中的表
                if ((lhs_is_new && rhs_in_left) || (rhs_is_new && lhs_in_left)) {
                    join_conds.push_back(*it);
                    it = query->conds.erase(it);
                    found_condition = true;
                }
            }
            
            if (!found_condition) {
                ++it;
            }
        }
        
        plan = std::make_shared<JoinPlan>(T_NestLoop, plan, join_order[i], join_conds);
    }
    // 剩余条件加到最外层 join
    for (const auto& cond : query->conds) {
        if (!cond.is_rhs_val) {
            auto join_plan = std::dynamic_pointer_cast<JoinPlan>(plan);
            if (join_plan) {
                join_plan->conds_.push_back(cond);
            }
        }
    }
    return plan;
}


std::shared_ptr<Plan> Planner::generate_sort_plan(std::shared_ptr<Query> query, std::shared_ptr<Plan> plan) {
    if (query->order_by_cols.empty()) {
        return plan;
    }

    std::vector<TabCol> order_by_cols;
    for (const auto& order_by_item : query->order_by_cols) {
        // order_by_item is of type OrderByCol, not a pointer.
        // We only handle non-aggregate columns for sorting here.
        if (!order_by_item.is_agg) {
            order_by_cols.push_back(order_by_item.col);
        }
    }

    // The is_desc information is in query->order_by_directions
    return std::make_shared<SortPlan>(std::move(plan), order_by_cols, query->order_by_directions, query->limit_val);
}



/**
 * @brief select plan 生成
 *
 * @param sel_cols select plan 选取的列
 * @param tab_names select plan 目标的表
 * @param conds select plan 选取条件
 */
std::shared_ptr<Plan> Planner::generate_select_plan(std::shared_ptr<Query> query, Context *context) {
    //逻辑优化
    query = logical_optimization(std::move(query), context);

    //物理优化
    auto sel_cols = query->cols;
    std::shared_ptr<Plan> plannerRoot = physical_optimization(query, context);

    // 新增：如果有聚合或分组，插入AggPlan
    if (query->has_agg || query->has_group_by) {
        // 对于聚合查询，ORDER BY 应该在聚合之后处理
        // 所以这里不调用 generate_sort_plan
        plannerRoot = std::make_shared<AggPlan>(
            plannerRoot, query->agg_funcs, query->group_by_cols, query->having_conds, 
            query->order_by_cols, query->order_by_directions, query->limit_val
        );
        
        // 为聚合查询创建正确的投影列
        std::vector<TabCol> agg_sel_cols;
        
        // 如果有GROUP BY，先添加分组列
        for (const auto& group_col : query->group_by_cols) {
            agg_sel_cols.push_back(group_col);
        }
        
        // 添加聚合函数列（使用别名）
        for (const auto& agg_func : query->agg_funcs) {
            TabCol agg_col;
            agg_col.tab_name = "";
            
            // 如果没有别名，使用原列名作为列名
            if (agg_func.alias.empty()) {
                if (agg_func.func_type == AGG_COUNT && agg_func.col.tab_name.empty() && agg_func.col.col_name.empty()) {
                    // COUNT(*) 的情况
                    agg_col.col_name = "count";
                } else if (!agg_func.col.col_name.empty()) {
                    // 有列名的情况，使用原列名
                    agg_col.col_name = agg_func.col.col_name;
                } else {
                    // 其他情况，使用默认的agg_序号
                    agg_col.col_name = "agg_" + std::to_string(agg_sel_cols.size());
                }
            } else {
                agg_col.col_name = agg_func.alias;
            }
            
            agg_sel_cols.push_back(agg_col);
        }
        
        sel_cols = std::move(agg_sel_cols);
        
        // 对于聚合查询，总是需要ProjectionPlan
        plannerRoot = std::make_shared<ProjectionPlan>(T_Projection, std::move(plannerRoot), std::move(sel_cols), query->limit_val, query->is_select_all);
    } else {
        // 对于非聚合查询，ORDER BY 在聚合之前处理
        if (query->has_order_by) {
            plannerRoot = generate_sort_plan(query, std::move(plannerRoot));
        }
        
        // 无论是否select *，根节点都需要是ProjectionPlan
        plannerRoot = std::make_shared<ProjectionPlan>(T_Projection, std::move(plannerRoot), std::move(sel_cols), query->limit_val, query->is_select_all);
    }
    
    return plannerRoot;
}

// 生成DDL语句和DML语句的查询执行计划
std::shared_ptr<Plan> Planner::do_planner(std::shared_ptr<Query> query, Context *context)
{
    std::shared_ptr<Plan> plannerRoot;
    if (auto x = std::dynamic_pointer_cast<ast::CreateTable>(query->parse)) {
        // create table;
        std::vector<ColDef> col_defs;
        for (auto &field : x->fields) {
            if (auto sv_col_def = std::dynamic_pointer_cast<ast::ColDef>(field)) {
                ColDef col_def = {.name = sv_col_def->col_name,
                                  .type = interp_sv_type(sv_col_def->type_len->type),
                                  .len = sv_col_def->type_len->len};
                col_defs.push_back(col_def);
            } else {
                throw InternalError("Unexpected field type");
            }
        }
        plannerRoot = std::make_shared<DDLPlan>(T_CreateTable, x->tab_name, std::vector<std::string>(), col_defs);
    } else if (auto x = std::dynamic_pointer_cast<ast::DropTable>(query->parse)) {
        // drop table;
        plannerRoot = std::make_shared<DDLPlan>(T_DropTable, x->tab_name, std::vector<std::string>(), std::vector<ColDef>());
    } else if (auto x = std::dynamic_pointer_cast<ast::CreateIndex>(query->parse)) {
        // create index;
        plannerRoot = std::make_shared<DDLPlan>(T_CreateIndex, x->tab_name, x->col_names, std::vector<ColDef>());
    } else if (auto x = std::dynamic_pointer_cast<ast::DropIndex>(query->parse)) {
        // drop index
        plannerRoot = std::make_shared<DDLPlan>(T_DropIndex, x->tab_name, x->col_names, std::vector<ColDef>());
    } else if (auto x = std::dynamic_pointer_cast<ast::InsertStmt>(query->parse)) {
        // insert;
        plannerRoot = std::make_shared<DMLPlan>(T_Insert, std::shared_ptr<Plan>(),  x->tab_name,  
                                                    query->values, std::vector<Condition>(), std::vector<SetClause>());
    } else if (auto x = std::dynamic_pointer_cast<ast::DeleteStmt>(query->parse)) {
        // delete;
        // 生成表扫描方式
        std::shared_ptr<Plan> table_scan_executors;
        // 只有一张表，不需要进行物理优化了
        // int index_no = get_indexNo(x->tab_name, query->conds);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(x->tab_name, query->conds, index_col_names);
        
        if (index_exist == false) {  // 该表没有索引
            index_col_names.clear();
            table_scan_executors = 
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        }

        plannerRoot = std::make_shared<DMLPlan>(T_Delete, table_scan_executors, x->tab_name,  
                                                std::vector<Value>(), query->conds, std::vector<SetClause>());
    } else if (auto x = std::dynamic_pointer_cast<ast::UpdateStmt>(query->parse)) {
        // update;
        // 生成表扫描方式
        std::shared_ptr<Plan> table_scan_executors;
        // 只有一张表，不需要进行物理优化了
        // int index_no = get_indexNo(x->tab_name, query->conds);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(x->tab_name, query->conds, index_col_names);

        if (index_exist == false) {  // 该表没有索引
        index_col_names.clear();
            table_scan_executors = 
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, x->tab_name, query->conds, index_col_names);
        }
        
        // 注意：条件已经在ScanPlan中处理，不需要额外的FilterPlan
        // 因为我们在scan阶段就应该过滤掉不满足条件的记录
        plannerRoot = std::make_shared<DMLPlan>(T_Update, table_scan_executors, x->tab_name,
                                                     std::vector<Value>(), query->conds, 
                                                     query->set_clauses);
    } else if (auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse)) {

        std::shared_ptr<plannerInfo> root = std::make_shared<plannerInfo>(x);
        // 生成select语句的查询执行计划
        std::shared_ptr<Plan> projection = generate_select_plan(std::move(query), context);
        plannerRoot = std::make_shared<DMLPlan>(T_select, projection, std::string(), std::vector<Value>(),
                                                    std::vector<Condition>(), std::vector<SetClause>());
    } else if (auto x = std::dynamic_pointer_cast<ast::ShowIndex>(query->parse)) {
        plannerRoot = std::make_shared<OtherPlan>(T_ShowIndex, x->tab_name);
    } else {
        throw InternalError("Unexpected AST root");
    }
    return plannerRoot;
}

CompOp Planner::convert_sv_comp_op(ast::SvCompOp op) {
    switch (op) {
        case ast::SV_OP_EQ: return OP_EQ;
        case ast::SV_OP_NE: return OP_NE;
        case ast::SV_OP_LT: return OP_LT;
        case ast::SV_OP_GT: return OP_GT;
        case ast::SV_OP_LE: return OP_LE;
        case ast::SV_OP_GE: return OP_GE;
        default: throw InternalError("Unknown comparison operator");
    }
}

Value Planner::convert_sv_value(const std::shared_ptr<ast::Value> &sv_val) {
    Value val;
    if (auto int_lit = std::dynamic_pointer_cast<ast::IntLit>(sv_val)) {
        val.set_int(int_lit->val);
    } else if (auto float_lit = std::dynamic_pointer_cast<ast::FloatLit>(sv_val)) {
        val.set_float(float_lit->val);
    } else if (auto str_lit = std::dynamic_pointer_cast<ast::StringLit>(sv_val)) {
        val.set_str(str_lit->val);
    } else {
        throw InternalError("Unexpected sv value type");
    }
    return val;
}

// Explain 实现
void print_indent(std::ostream& os, int indent) {
    for (int i = 0; i < indent; ++i) os << '\t';  // 一个tab缩进
}

void ScanPlan::Explain(std::ostream& os, int indent) const {
    print_indent(os, indent);
    os << "Scan(table=" << tab_name_ << ")\n";  // 使用真实表名，不是别名
}

void FilterPlan::Explain(std::ostream& os, int indent) const {
    print_indent(os, indent);
    os << "Filter(condition=[";
    std::vector<std::string> cond_strs;
    for (const auto& cond : conds_) {
        std::string lhs_tab_name = cond.lhs_col.original_tab_name.empty() ? cond.lhs_col.tab_name : cond.lhs_col.original_tab_name;
        std::string cond_str = lhs_tab_name + "." + cond.lhs_col.col_name;
        cond_str += comp_op2str(cond.op);
        if (cond.is_rhs_val) {
            if (cond.rhs_val.type == TYPE_STRING) {
                cond_str += "'" + cond.rhs_val.str_val + "'";
            } else if (cond.rhs_val.type == TYPE_INT) {
                cond_str += std::to_string(cond.rhs_val.int_val);
            } else if (cond.rhs_val.type == TYPE_FLOAT) {
                // 保持原始SQL格式，使用固定6位小数
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << cond.rhs_val.float_val;
                cond_str += oss.str();
            }
        } else {
            std::string rhs_tab_name = cond.rhs_col.original_tab_name.empty() ? cond.rhs_col.tab_name : cond.rhs_col.original_tab_name;
            cond_str += rhs_tab_name + "." + cond.rhs_col.col_name;
        }
        cond_strs.push_back(cond_str);
    }
    std::sort(cond_strs.begin(), cond_strs.end());
    for (size_t i = 0; i < cond_strs.size(); ++i) {
        if (i > 0) os << ",";
        os << cond_strs[i];
    }
    os << "])\n";
    if (subplan_) subplan_->Explain(os, indent + 1);
}

void JoinPlan::Explain(std::ostream& os, int indent) const {
    print_indent(os, indent);
    
    // 收集所有表名
    std::vector<std::string> tables;
    std::function<void(const Plan*, std::vector<std::string>&)> collect_tables = [&](const Plan* p, std::vector<std::string>& ts) {
        if (auto scan = dynamic_cast<const ScanPlan*>(p)) {
            ts.push_back(scan->tab_name_);  // 使用真实表名，不是别名
        } else if (auto filter = dynamic_cast<const FilterPlan*>(p)) {
            collect_tables(filter->subplan_.get(), ts);
        } else if (auto projection = dynamic_cast<const ProjectionPlan*>(p)) {
            collect_tables(projection->subplan_.get(), ts);
        } else if (auto join = dynamic_cast<const JoinPlan*>(p)) {
            collect_tables(join->left_.get(), ts);
            collect_tables(join->right_.get(), ts);
        }
    };
    collect_tables(this, tables);
    std::sort(tables.begin(), tables.end());
    
    os << "Join(tables=[";
    for (size_t i = 0; i < tables.size(); ++i) {
        if (i) os << ",";
        os << tables[i];
    }
    os << "]";
    
    // 输出连接条件
    std::vector<std::string> conds_str;
    for (const auto& cond : conds_) {
        std::string lhs_tab_name = cond.lhs_col.original_tab_name.empty() ? cond.lhs_col.tab_name : cond.lhs_col.original_tab_name;
        std::string s = lhs_tab_name + "." + cond.lhs_col.col_name;
        if (!cond.lhs_col.alias.empty()) s += " AS " + cond.lhs_col.alias;
        s += comp_op2str(cond.op);
        if (cond.is_rhs_val) {
            if (cond.rhs_val.type == TYPE_STRING) {
                s += "'" + cond.rhs_val.str_val + "'";
            } else if (cond.rhs_val.type == TYPE_INT) {
                s += std::to_string(cond.rhs_val.int_val);
            } else if (cond.rhs_val.type == TYPE_FLOAT) {
                // 保持原始SQL格式，使用固定6位小数
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << cond.rhs_val.float_val;
                s += oss.str();
            }
        } else {
            std::string rhs_tab_name = cond.rhs_col.original_tab_name.empty() ? cond.rhs_col.tab_name : cond.rhs_col.original_tab_name;
            s += rhs_tab_name + "." + cond.rhs_col.col_name;
            if (!cond.rhs_col.alias.empty()) s += " AS " + cond.rhs_col.alias;
        }
        conds_str.push_back(s);
    }
    std::sort(conds_str.begin(), conds_str.end());
    os << ",condition=[";
    for (size_t i = 0; i < conds_str.size(); ++i) {
        if (i) os << ",";
        os << conds_str[i];
    }
    os << "]";
    os << ")\n";
    
    // 递归输出子节点，按照Filter Join Project Scan的顺序输出
    std::vector<std::pair<int, std::shared_ptr<Plan>>> child_plans;
    
    // 定义节点类型的优先级
    auto get_plan_priority = [](const Plan* p) -> int {
        if (dynamic_cast<const FilterPlan*>(p)) return 1;
        if (dynamic_cast<const JoinPlan*>(p)) return 2;
        if (dynamic_cast<const ProjectionPlan*>(p)) return 3;
        if (dynamic_cast<const ScanPlan*>(p)) return 4;
        return 5; // 其他类型
    };
    
    // 收集子节点的表名（用于排序）
    std::function<std::string(const Plan*)> get_first_table = [&](const Plan* p) -> std::string {
        if (auto scan = dynamic_cast<const ScanPlan*>(p)) {
            return scan->tab_name_;
        } else if (auto filter = dynamic_cast<const FilterPlan*>(p)) {
            return get_first_table(filter->subplan_.get());
        } else if (auto projection = dynamic_cast<const ProjectionPlan*>(p)) {
            return get_first_table(projection->subplan_.get());
        } else if (auto join = dynamic_cast<const JoinPlan*>(p)) {
            return get_first_table(join->left_.get());
        }
        return "";
    };
    
    if (left_) {
        child_plans.emplace_back(get_plan_priority(left_.get()), left_);
    }
    if (right_) {
        child_plans.emplace_back(get_plan_priority(right_.get()), right_);
    }
    
    // 按优先级排序，同类型按表名升序排序
    std::sort(child_plans.begin(), child_plans.end(), [&](const auto& a, const auto& b) {
        if (a.first != b.first) {
            return a.first < b.first; // 按类型优先级排序
        }
        // 同类型按表名升序排序
        return get_first_table(a.second.get()) < get_first_table(b.second.get());
    });
    
    // 输出子节点
    for (const auto& child : child_plans) {
        child.second->Explain(os, indent + 1);
    }
}

void ProjectionPlan::Explain(std::ostream& os, int indent) const {
    print_indent(os, indent);
    os << "Project(columns=[";
    // 检查是否为select *
    if (is_select_all_) {
        os << "*";
    } else {
        std::vector<std::string> cols;
        for (const auto& c : sel_cols_) {
            std::string col_str;
            // 使用别名引用（如果有别名的话）
            if (!c.original_tab_name.empty() && c.original_tab_name != c.tab_name) {
                // 有别名，使用别名
                col_str = c.original_tab_name + "." + c.col_name;
            } else if (!c.tab_name.empty()) {
                col_str = c.tab_name + "." + c.col_name;
            } else {
                col_str = c.col_name;
            }
            if (!c.alias.empty()) col_str += " AS " + c.alias;
            cols.push_back(col_str);
        }
        std::sort(cols.begin(), cols.end());
        for (size_t i = 0; i < cols.size(); ++i) {
            if (i) os << ",";
            os << cols[i];
        }
    }
    os << "])\n";
    if (subplan_) subplan_->Explain(os, indent + 1);
}

void SortPlan::Explain(std::ostream& os, int indent) const {
    os << std::string(indent, ' ') << "Sort";
    if (limit_val_ != -1) {
        os << " (limit=" << limit_val_ << ")";
    }
    os << std::endl;
    os << std::string(indent + 2, ' ') << "-> Sort By (";
    for (size_t i = 0; i < order_by_cols_.size(); ++i) {
        os << order_by_cols_[i].col_name << (is_desc_[i] ? " desc" : " asc");
        if (i != order_by_cols_.size() - 1) {
            os << ", ";
        }
    }
    os << ")" << std::endl;
    subplan_->Explain(os, indent + 2);
}