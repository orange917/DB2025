/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */
#include "errors.h"
#include "analyze.h"
#include "defs.h"
#include "optimizer/planner.h"  // 包含 OrderByCol 的完整定义

/**
 * @description: 分析器，进行语义分析和查询重写，需要检查不符合语义规定的部分
 * @param {shared_ptr<ast::TreeNode>} parse parser生成的结果集
 * @return {shared_ptr<Query>} Query
 */
std::shared_ptr<Query> Analyze::do_analyze(std::shared_ptr<ast::TreeNode> parse)
{
    std::shared_ptr<Query> query = std::make_shared<Query>();
    if (auto x = std::dynamic_pointer_cast<ast::SelectStmt>(parse))
    {
        // 处理表名
        query->tables = std::move(x->tabs);

        // 处理连接表达式 - 从表名中提取连接信息
        // 由于语法分析器的限制，我们需要在这里处理连接表达式
        // 假设表名格式为: table1, table2 JOIN table3 ON condition
        // 我们需要解析这种格式并提取连接信息

        /** 检查表是否存在 */
        for(auto &tab_name : query->tables)
        {
            if(!sm_manager_->db_.is_table(tab_name))
            {
                throw TableNotFoundError(tab_name);
            }
        }

        // 处理聚合函数
        if (!x->agg_funcs.empty()) {
            query->has_agg = true;
            for (auto &sv_agg_func : x->agg_funcs) {
                AggFuncType func_type = convert_agg_func_type(sv_agg_func->func_type);
                TabCol col = {.tab_name = "", .col_name = ""};
                if (sv_agg_func->col) {
                    col = {.tab_name = sv_agg_func->col->tab_name, .col_name = sv_agg_func->col->col_name};
                }
                AggFunc agg_func(func_type, col, sv_agg_func->alias);
                query->agg_funcs.push_back(agg_func);
            }
        }
        
        // 处理target list，再target list中添加上表名，例如 a.id
        for (auto &sv_sel_col : x->cols) {
            TabCol sel_col = {.tab_name = sv_sel_col->tab_name, .col_name = sv_sel_col->col_name};
            query->cols.push_back(sel_col);
        }
        
        std::vector<ColMeta> all_cols;
        get_all_cols(query->tables, all_cols);
        if (query->cols.empty() && query->agg_funcs.empty()) {
            // select all columns
            for (auto &col : all_cols) {
                TabCol sel_col = {.tab_name = col.tab_name, .col_name = col.name};
                query->cols.push_back(sel_col);
            }
        } else {
            // infer table name from column name
            for (auto &sel_col : query->cols) {
                sel_col = check_column(all_cols, sel_col);  // 列元数据校验
            }
            // 检查聚合函数中的列
            for (auto &agg_func : query->agg_funcs) {
                if (!agg_func.col.tab_name.empty() || !agg_func.col.col_name.empty()) {
                    agg_func.col = check_column(all_cols, agg_func.col);
                }
            }
        }
        
        // 处理GROUP BY
        if (x->has_group_by && x->group_by) {
            query->has_group_by = true;
            for (auto &sv_group_col : x->group_by->cols) {
                TabCol group_col = {.tab_name = sv_group_col->tab_name, .col_name = sv_group_col->col_name};
                group_col = check_column(all_cols, group_col);
                query->group_by_cols.push_back(group_col);
            }
        }
        
        // 处理HAVING
        if (x->has_having && x->having) {
            query->has_having = true;
            get_clause(x->having->conds, query->having_conds);
            check_clause(query->tables, query->having_conds);
        }
        
        // 处理ORDER BY
        if (x->has_sort && x->order) {
            query->has_order_by = true;
            for (size_t i = 0; i < x->order->cols.size(); i++) {
                OrderByCol order_col;
                order_col.col = {.tab_name = x->order->cols[i]->tab_name, .col_name = x->order->cols[i]->col_name};
                
                // 检查排序列是否是聚合函数的别名
                bool is_agg_alias = false;
                for (const auto& agg_func : query->agg_funcs) {
                    if (agg_func.alias == order_col.col.col_name) {
                        order_col.is_agg = true;
                        order_col.agg = agg_func;
                        is_agg_alias = true;
                        break;
                    }
                }
                
                if (!is_agg_alias) {
                    // 不是聚合函数别名，按普通列处理
                    order_col.is_agg = false;
                    order_col.col = check_column(all_cols, order_col.col);
                }
                
                query->order_by_cols.push_back(order_col);
                
                // 设置排序方向
                bool is_asc = (x->order->orderby_dirs[i] == ast::OrderBy_ASC || x->order->orderby_dirs[i] == ast::OrderBy_DEFAULT);
                query->order_by_directions.push_back(is_asc);
            }
        }
        
        // 处理LIMIT
        if (x->has_limit && x->limit) {
            query->has_limit = true;
            query->limit_val = x->limit->limit_val;
        }
        
        //处理where条件
        get_clause(x->conds, query->conds);
        check_clause(query->tables, query->conds);
        
        // 处理连接表达式
        if (!x->jointree.empty()) {
            query->has_join = true;
            
            // 简单处理：直接复制连接表达式，不修改左表名
            query->jointree = x->jointree;
            
            // 检查连接表达式中的表是否存在，并添加到表列表中
            for (const auto& join_expr : query->jointree) {
                // 检查右表是否存在
                if (!sm_manager_->db_.is_table(join_expr->right)) {
                    throw TableNotFoundError(join_expr->right);
                }
                
                // 将右表添加到表列表中（如果还没有的话）
                bool found = false;
                for (const auto& existing_table : query->tables) {
                    if (existing_table == join_expr->right) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    query->tables.push_back(join_expr->right);
                }
            }
        }
        
        // 语义检查：检测SELECT列表中的非聚合列和WHERE子句中的聚合函数
        if (query->has_group_by) {
            // 检查1：SELECT 列表中不能出现没有在 GROUP BY 子句中的非聚集列
            for (const auto& sel_col : query->cols) {
                bool found_in_group_by = false;
                for (const auto& group_col : query->group_by_cols) {
                    if (sel_col.tab_name == group_col.tab_name && sel_col.col_name == group_col.col_name) {
                        found_in_group_by = true;
                        break;
                    }
                }
                if (!found_in_group_by) {
                    std::cout << "failure" << std::endl;
                    throw InternalError("Column '" + sel_col.col_name + "' must appear in GROUP BY clause or be used in an aggregate function");
                }
            }
        }
        
        // 检查2：WHERE 子句中不能用聚集函数作为条件表达式
        for (const auto& cond : query->conds) {
            if (cond.is_lhs_agg || cond.is_rhs_agg) {
                std::cout << "failure" << std::endl;
                throw InternalError("Aggregate functions are not allowed in WHERE clause");
            }
        }
    } else if (auto x = std::dynamic_pointer_cast<ast::UpdateStmt>(parse)) {
        /** TODO: */
        query->tables = {x->tab_name};
        if (!sm_manager_->db_.is_table(x->tab_name)) {
            throw TableNotFoundError(x->tab_name);
        }
        for (auto &set_clause : x->set_clauses) {
            SetClause clause;
            clause.lhs = {.tab_name = x->tab_name, .col_name = set_clause->col_name};
            clause.rhs = convert_sv_value(set_clause->val);
            query->set_clauses.push_back(clause);
        }
        get_clause(x->conds, query->conds);
        check_clause(query->tables, query->conds);
    } else if (auto x = std::dynamic_pointer_cast<ast::DeleteStmt>(parse)) {
        //处理where条件
        get_clause(x->conds, query->conds);
        check_clause({x->tab_name}, query->conds);        
    } else if (auto x = std::dynamic_pointer_cast<ast::InsertStmt>(parse)) {
        // 处理insert 的values值
        for (auto &sv_val : x->vals) {
            query->values.push_back(convert_sv_value(sv_val));
        }
    } else {
        // do nothing
    }
    query->parse = std::move(parse);
    return query;
}


TabCol Analyze::check_column(const std::vector<ColMeta> &all_cols, TabCol target) {
    if (target.tab_name.empty()) {
        // Table name not specified, infer table name from column name
        std::string tab_name;
        for (auto &col : all_cols) {
            if (col.name == target.col_name) {
                if (!tab_name.empty()) {
                    throw AmbiguousColumnError(target.col_name);
                }
                tab_name = col.tab_name;
            }
        }
        if (tab_name.empty()) {
            throw ColumnNotFoundError(target.col_name);
        }
        target.tab_name = tab_name;
    } else {
        /** TODO: Make sure target column exists */
        if(sm_manager_->db_.is_table(target.tab_name)) {
            auto &tab = sm_manager_->db_.get_table(target.tab_name);
            if (!tab.is_col(target.col_name)) {
                throw ColumnNotFoundError(target.col_name);
            }
        } else {
            throw TableNotFoundError(target.tab_name);
    }
}
    return target;
}

void Analyze::get_all_cols(const std::vector<std::string> &tab_names, std::vector<ColMeta> &all_cols) {
    for (auto &sel_tab_name : tab_names) {
        // 这里db_不能写成get_db(), 注意要传指针
        const auto &sel_tab_cols = sm_manager_->db_.get_table(sel_tab_name).cols;
        all_cols.insert(all_cols.end(), sel_tab_cols.begin(), sel_tab_cols.end());
    }
}

void Analyze::get_clause(const std::vector<std::shared_ptr<ast::BinaryExpr>> &sv_conds, std::vector<Condition> &conds) {
    conds.clear();
    for (auto &expr : sv_conds) {
        Condition cond;
        
        // 处理左操作数，可能是 Col 或 AggFunc
        if (auto lhs_col = std::dynamic_pointer_cast<ast::Col>(expr->lhs)) {
            cond.lhs_col = {.tab_name = lhs_col->tab_name, .col_name = lhs_col->col_name};
            cond.is_lhs_agg = false;
        } else if (auto lhs_agg = std::dynamic_pointer_cast<ast::AggFunc>(expr->lhs)) {
            // 处理聚合函数作为左操作数
            cond.is_lhs_agg = true;
            cond.lhs_agg.func_type = convert_agg_func_type(lhs_agg->func_type);
            if (lhs_agg->col) {
                cond.lhs_agg.col = {.tab_name = lhs_agg->col->tab_name, .col_name = lhs_agg->col->col_name};
            } else {
                cond.lhs_agg.col = {.tab_name = "", .col_name = ""}; // COUNT(*)的情况
            }
            // 设置空的lhs_col表示这是聚合函数
            cond.lhs_col = {.tab_name = "", .col_name = ""};
        } else {
            throw InternalError("Unexpected lhs type in BinaryExpr");
        }
        
        cond.op = convert_sv_comp_op(expr->op);
        if (auto rhs_val = std::dynamic_pointer_cast<ast::Value>(expr->rhs)) {
            cond.is_rhs_val = true;
            cond.is_rhs_agg = false;
            cond.rhs_val = convert_sv_value(rhs_val);
        } else if (auto rhs_col = std::dynamic_pointer_cast<ast::Col>(expr->rhs)) {
            cond.is_rhs_val = false;
            cond.is_rhs_agg = false;
            cond.rhs_col = {.tab_name = rhs_col->tab_name, .col_name = rhs_col->col_name};
        } else if (auto rhs_agg = std::dynamic_pointer_cast<ast::AggFunc>(expr->rhs)) {
            // 处理聚合函数作为右操作数
            cond.is_rhs_val = false;
            cond.is_rhs_agg = true;
            cond.rhs_agg.func_type = convert_agg_func_type(rhs_agg->func_type);
            if (rhs_agg->col) {
                cond.rhs_agg.col = {.tab_name = rhs_agg->col->tab_name, .col_name = rhs_agg->col->col_name};
            } else {
                cond.rhs_agg.col = {.tab_name = "", .col_name = ""}; // COUNT(*)的情况
            }
            // 设置空的rhs_col表示这是聚合函数
            cond.rhs_col = {.tab_name = "", .col_name = ""};
        }
        conds.push_back(cond);
    }
}

void Analyze::check_clause(const std::vector<std::string> &tab_names, std::vector<Condition> &conds) {
    // auto all_cols = get_all_cols(tab_names);
    std::vector<ColMeta> all_cols;
    get_all_cols(tab_names, all_cols);
    // Get raw values in where clause
    for (auto &cond : conds) {
        // 跳过聚合函数条件的类型检查，因为聚合函数在HAVING阶段才计算
        if (cond.is_lhs_agg || cond.is_rhs_agg) {
            continue;
        }
        
        // Infer table name from column name
        cond.lhs_col = check_column(all_cols, cond.lhs_col);
        if (!cond.is_rhs_val) {
            cond.rhs_col = check_column(all_cols, cond.rhs_col);
        }
        TabMeta &lhs_tab = sm_manager_->db_.get_table(cond.lhs_col.tab_name);
        auto lhs_col = lhs_tab.get_col(cond.lhs_col.col_name);
        ColType lhs_type = lhs_col->type;
        ColType rhs_type;
        if (cond.is_rhs_val) {
            cond.rhs_val.init_raw(lhs_col->len);
            rhs_type = cond.rhs_val.type;
        } else {
            // 列和列比较的情况
            TabMeta &rhs_tab = sm_manager_->db_.get_table(cond.rhs_col.tab_name);
            auto rhs_col = rhs_tab.get_col(cond.rhs_col.col_name);
            rhs_type = rhs_col->type;
        }
        if (lhs_type != rhs_type) {
            if(lhs_type == TYPE_FLOAT && rhs_type == TYPE_INT) {
                float float_val = static_cast<float>(cond.rhs_val.int_val);
                cond.rhs_val.set_float(float_val);
                rhs_type = TYPE_FLOAT;
            } else if(lhs_type == TYPE_INT && rhs_type == TYPE_FLOAT) {
                int int_val = static_cast<int>(cond.rhs_val.float_val);
                cond.rhs_val.set_int(int_val);
                rhs_type = TYPE_INT;
            } else {
                throw IncompatibleTypeError(coltype2str(lhs_type), coltype2str(rhs_type));
            }
        }
    }
}


Value Analyze::convert_sv_value(const std::shared_ptr<ast::Value> &sv_val) {
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

CompOp Analyze::convert_sv_comp_op(ast::SvCompOp op) {
    std::map<ast::SvCompOp, CompOp> m = {
        {ast::SV_OP_EQ, OP_EQ}, {ast::SV_OP_NE, OP_NE}, {ast::SV_OP_LT, OP_LT},
        {ast::SV_OP_GT, OP_GT}, {ast::SV_OP_LE, OP_LE}, {ast::SV_OP_GE, OP_GE},
    };
    return m.at(op);
}

AggFuncType Analyze::convert_agg_func_type(ast::AggFuncType sv_agg_func_type) {
    std::map<ast::AggFuncType, AggFuncType> m = {
        {ast::AGG_COUNT, AGG_COUNT}, {ast::AGG_MAX, AGG_MAX}, {ast::AGG_MIN, AGG_MIN},
        {ast::AGG_SUM, AGG_SUM}, {ast::AGG_AVG, AGG_AVG},
    };
    return m.at(sv_agg_func_type);
}
