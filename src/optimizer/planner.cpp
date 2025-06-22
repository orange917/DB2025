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

// 目前的索引匹配规则为：完全匹配索引字段，且全部为单点查询，不会自动调整where条件的顺序
bool Planner::get_index_cols(std::string tab_name, std::vector<Condition> curr_conds, std::vector<std::string>& index_col_names) {
    index_col_names.clear();
    for(auto& cond: curr_conds) {
        if(cond.is_rhs_val && cond.op == OP_EQ && cond.lhs_col.tab_name.compare(tab_name) == 0)
            index_col_names.push_back(cond.lhs_col.col_name);
    }
    TabMeta& tab = sm_manager_->db_.get_table(tab_name);
    if(tab.is_index(index_col_names)) return true;
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
    // 处理orderby
    plan = generate_sort_plan(query, std::move(plan)); 

    return plan;
}



std::shared_ptr<Plan> Planner::make_one_rel(std::shared_ptr<Query> query)
{
    auto x = std::dynamic_pointer_cast<ast::SelectStmt>(query->parse);
    std::vector<std::string> tables = query->tables;
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
    std::vector<std::string> tables = query->tables;
    std::vector<ColMeta> all_cols;
    for (auto &sel_tab_name : tables) {
        // 这里db_不能写成get_db(), 注意要传指针
        const auto &sel_tab_cols = sm_manager_->db_.get_table(sel_tab_name).cols;
        all_cols.insert(all_cols.end(), sel_tab_cols.begin(), sel_tab_cols.end());
    }
    TabCol sel_col;
    for (auto &col : all_cols) {
        if(col.name.compare(x->order->cols->col_name) == 0 )
        sel_col = {.tab_name = col.tab_name, .col_name = col.name};
    }
    return std::make_shared<SortPlan>(T_Sort, std::move(plan), sel_col, 
                                    x->order->orderby_dir == ast::OrderBy_DESC);
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
    plannerRoot = std::make_shared<ProjectionPlan>(T_Projection, std::move(plannerRoot), 
                                                        std::move(sel_cols));

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
    } else {
        throw InternalError("Unexpected AST root");
    }
    return plannerRoot;
}