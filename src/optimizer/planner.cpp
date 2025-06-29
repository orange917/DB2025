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
            best_index_cols.clear();
            for (const auto& col : index.cols) {
                best_index_cols.push_back(col.name);
            }
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

    //TODO 实现逻辑优化规则

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
        if(index_exist){
            plan = std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, query->tables[0], query->conds, index_col_names);
        }
        else {
            // 没有索引，用顺序扫描
            index_col_names.clear();
            plan = std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, query->tables[0], query->conds, index_col_names);
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
    for (size_t i = 0; i < tables.size(); i++) {
        auto curr_conds = pop_conds(query->conds, tables[i]);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(tables[i], curr_conds, index_col_names);
        if (index_exist == false) {
            index_col_names.clear();
            table_scan_executors[i] = 
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, tables[i], curr_conds, index_col_names);
        } else {
            table_scan_executors[i] =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, tables[i], curr_conds, index_col_names);
        }
    }
    
    // 构建连接计划
    std::shared_ptr<Plan> current_plan = table_scan_executors[0];
    
    for (const auto& join_expr : query->jointree) {
        // 查找右表的扫描计划
        std::shared_ptr<Plan> right_plan = nullptr;
        for (size_t i = 0; i < tables.size(); i++) {
            if (tables[i] == join_expr->right) {
                right_plan = table_scan_executors[i];
                break;
            }
        }
        
        if (!right_plan) {
            throw InternalError("Right table not found in join expression");
        }
        
        // 转换连接条件
        std::vector<Condition> join_conds;
        for (const auto& cond : join_expr->conds) {
            Condition join_cond;
            
            // 处理左操作数
            if (auto lhs_col = std::dynamic_pointer_cast<ast::Col>(cond->lhs)) {
                join_cond.lhs_col = {.tab_name = lhs_col->tab_name, .col_name = lhs_col->col_name};
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
                join_cond.rhs_col = {.tab_name = rhs_col->tab_name, .col_name = rhs_col->col_name};
            } else {
                throw InternalError("Join condition rhs must be a column or value");
            }
            
            join_cond.op = convert_sv_comp_op(cond->op);
            
            join_conds.push_back(join_cond);
        }
        
        // 根据连接类型创建连接计划
        PlanTag join_tag = (join_expr->type == SEMI_JOIN) ? T_SemiJoin : T_NestLoop;
        
        current_plan = std::make_shared<JoinPlan>(
            join_tag,
            std::move(current_plan),
            std::move(right_plan),
            join_conds
        );
        
        // 设置连接类型
        auto join_plan = std::dynamic_pointer_cast<JoinPlan>(current_plan);
        if (join_plan) {
            join_plan->type = join_expr->type;
        }
    }
    
    return current_plan;
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
    
    // // Scan table , 生成表算子列表tab_nodes
    std::vector<std::shared_ptr<Plan>> table_scan_executors(tables.size());
    for (size_t i = 0; i < tables.size(); i++) {
        auto curr_conds = pop_conds(query->conds, tables[i]);
        // int index_no = get_indexNo(tables[i], curr_conds);
        std::vector<std::string> index_col_names;
        bool index_exist = get_index_cols(tables[i], curr_conds, index_col_names);
        if (index_exist == false) {  // 该表没有索引
            index_col_names.clear();
            table_scan_executors[i] = 
                std::make_shared<ScanPlan>(T_SeqScan, sm_manager_, tables[i], curr_conds, index_col_names);
        } else {  // 存在索引
            table_scan_executors[i] =
                std::make_shared<ScanPlan>(T_IndexScan, sm_manager_, tables[i], curr_conds, index_col_names);
        }
    }
    // 只有一个表，不需要join。
    if(tables.size() == 1)
    {
        return table_scan_executors[0];
    }
    // 获取where条件
    auto conds = std::move(query->conds);
    std::shared_ptr<Plan> table_join_executors;
    // 标记已经处理过的表
    int scantbl[tables.size()];
    for(size_t i = 0; i < tables.size(); i++)
    {
        scantbl[i] = -1; // -1表示未处理
    }
    // 存储已经加入连接的表名
    std::vector<std::string> joined_tables;
    // 如果有连接条件
    if(conds.size() >= 1)
    {
        // 先尝试找出连接条件（两个表的列相等）
        auto it = conds.begin();
        bool found_join_cond = false;

        while (it != conds.end() && !found_join_cond) {
            // 检查是否是连接条件（即条件的两边分别来自不同的表）
            if (!it->is_rhs_val &&
                it->op == OP_EQ &&
                it->lhs_col.tab_name != it->rhs_col.tab_name) {

                std::shared_ptr<Plan> left = nullptr, right = nullptr;

                // 查找左表对应的扫描计划
                for (size_t i = 0; i < tables.size(); i++) {
                    if (tables[i] == it->lhs_col.tab_name && scantbl[i] == -1) {
                        left = table_scan_executors[i];
                        scantbl[i] = 1; // 标记为已处理
                        joined_tables.push_back(tables[i]);
                        break;
                    }
                }

                // 查找右表对应的扫描计划
                for (size_t i = 0; i < tables.size(); i++) {
                    if (tables[i] == it->rhs_col.tab_name && scantbl[i] == -1) {
                        right = table_scan_executors[i];
                        scantbl[i] = 1; // 标记为已处理
                        joined_tables.push_back(tables[i]);
                        break;
                    }
                }

                // 如果找到了两个表的扫描计划，创建连接计划
                if (left && right) {
                    std::vector<Condition> join_conds{*it};

                    // 创建嵌套循环连接计划
                    table_join_executors = std::make_shared<JoinPlan>(
                        T_NestLoop,
                        std::move(left),
                        std::move(right),
                        join_conds
                    );

                    found_join_cond = true;
                    it = conds.erase(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }

        // 如果没有找到连接条件，使用第一个和第二个表创建笛卡尔积
        if (!found_join_cond && tables.size() >= 2) {
            table_join_executors = std::make_shared<JoinPlan>(
                T_NestLoop,
                table_scan_executors[0],
                table_scan_executors[1],
                std::vector<Condition>() // 空条件表示笛卡尔积
            );

            scantbl[0] = 1;
            scantbl[1] = 1;
            joined_tables.push_back(tables[0]);
            joined_tables.push_back(tables[1]);
        } else if (!found_join_cond) {
            // 只有一个表，直接使用它的扫描计划
            table_join_executors = table_scan_executors[0];
            scantbl[0] = 1;
            joined_tables.push_back(tables[0]);
        }

        // 处理剩余的连接条件
        it = conds.begin();
        while (it != conds.end()) {
            // 如果是连接条件
            if (!it->is_rhs_val && 
                it->op == OP_EQ && 
                it->lhs_col.tab_name != it->rhs_col.tab_name) {

                // 检查条件中的表是否已经加入了连接
                bool lhs_joined = std::find(joined_tables.begin(), joined_tables.end(), it->lhs_col.tab_name) != joined_tables.end();
                bool rhs_joined = std::find(joined_tables.begin(), joined_tables.end(), it->rhs_col.tab_name) != joined_tables.end();

                if (lhs_joined && rhs_joined) {
                    // 两个表都已经加入了连接，将条件加入到已有的连接条件中
                    auto join_plan = std::dynamic_pointer_cast<JoinPlan>(table_join_executors);
                    if (join_plan) {
                        join_plan->conds_.push_back(*it);
                    }
                    it = conds.erase(it);
                } else if (lhs_joined || rhs_joined) {
                    // 只有一个表加入了连接，需要添加另一个表
                    std::shared_ptr<Plan> new_table = nullptr;
                    std::string new_table_name;

                    if (lhs_joined) {
                        new_table_name = it->rhs_col.tab_name;
                    } else {
                        new_table_name = it->lhs_col.tab_name;
                    }

                    // 查找新表对应的扫描计划
                    for (size_t i = 0; i < tables.size(); i++) {
                        if (tables[i] == new_table_name && scantbl[i] == -1) {
                            new_table = table_scan_executors[i];
                            scantbl[i] = 1; // 标记为已处理
                            joined_tables.push_back(tables[i]);
                            break;
                        }
                    }

                    if (new_table) {
                        std::vector<Condition> join_conds{*it};

                        // 创建新的嵌套循环连接计划
                        table_join_executors = std::make_shared<JoinPlan>(
                            T_NestLoop,
                            std::move(table_join_executors),
                            std::move(new_table),
                            join_conds
                        );

                        it = conds.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    // 两个表都没有加入连接，暂时跳过这个条件
                    ++it;
                }
            } else {
                // 不是连接条件，暂时跳过
                ++it;
            }
        }
    } else {
        // 没有条件，创建笛卡尔积
        table_join_executors = table_scan_executors[0];
        scantbl[0] = 1;
        joined_tables.push_back(tables[0]);
    }

    // 连接剩余的表（笛卡尔积）
    for (size_t i = 0; i < tables.size(); i++) {
        if (scantbl[i] == -1) {
            table_join_executors = std::make_shared<JoinPlan>(
                T_NestLoop, 
                std::move(table_join_executors), 
                std::move(table_scan_executors[i]), 
                std::vector<Condition>() // 空条件表示笛卡尔积
            );
            joined_tables.push_back(tables[i]);
        }
    }

    // 将剩余的条件添加到连接计划中
    for (const auto& cond : conds) {
        if (!cond.is_rhs_val) {
            auto join_plan = std::dynamic_pointer_cast<JoinPlan>(table_join_executors);
            if (join_plan) {
                join_plan->conds_.push_back(cond);
            }
        }
    }

    return table_join_executors;
}


std::shared_ptr<Plan> Planner::generate_sort_plan(std::shared_ptr<Query> query, std::shared_ptr<Plan> plan)
{
    auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse);
    if(!x->has_sort) {
        return plan;
    }
    
    // 对于多列排序，我们需要创建多个SortPlan
    std::shared_ptr<Plan> current_plan = plan;
    
    // 从后往前处理，这样第一个排序列会成为最外层的排序
    for (int i = x->order->cols.size() - 1; i >= 0; i--) {
        std::vector<std::string> tables = query->tables;
        std::vector<ColMeta> all_cols;
        for (auto &sel_tab_name : tables) {
            const auto &sel_tab_cols = sm_manager_->db_.get_table(sel_tab_name).cols;
            all_cols.insert(all_cols.end(), sel_tab_cols.begin(), sel_tab_cols.end());
        }
        
        TabCol sel_col;
        for (auto &col : all_cols) {
            if(col.name.compare(x->order->cols[i]->col_name) == 0) {
                sel_col = {.tab_name = col.tab_name, .col_name = col.name};
                break;
            }
        }
        
        // 添加调试信息
        std::cout << "SortPlan: Ordering by column '" << x->order->cols[i]->col_name << "'" << std::endl;
        std::cout << "SortPlan: Selected column: " << sel_col.tab_name << "." << sel_col.col_name << std::endl;
        std::cout << "SortPlan: Sort direction: " << (x->order->orderby_dirs[i] == ast::OrderBy_DESC ? "DESC" : "ASC") << std::endl;
        
        current_plan = std::make_shared<SortPlan>(T_Sort, current_plan, sel_col, 
                                                x->order->orderby_dirs[i] == ast::OrderBy_DESC);
    }
    
    return current_plan;
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
    } else {
        // 对于非聚合查询，ORDER BY 在聚合之前处理
        if (query->has_order_by) {
            plannerRoot = generate_sort_plan(query, std::move(plannerRoot));
        }
    }

    // 创建ProjectionPlan，传递limit值
    plannerRoot = std::make_shared<ProjectionPlan>(T_Projection, std::move(plannerRoot), std::move(sel_cols), query->limit_val);
    
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