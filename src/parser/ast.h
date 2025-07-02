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

#include <vector>
#include <string>
#include <memory>

enum JoinType {
    INNER_JOIN, LEFT_JOIN, RIGHT_JOIN, FULL_JOIN, SEMI_JOIN
};
namespace ast {

enum SvType {
    SV_TYPE_INT, SV_TYPE_FLOAT, SV_TYPE_STRING, SV_TYPE_BOOL
};

enum SvCompOp {
    SV_OP_EQ, SV_OP_NE, SV_OP_LT, SV_OP_GT, SV_OP_LE, SV_OP_GE
};

// 添加聚合函数类型枚举
enum AggFuncType {
    AGG_COUNT, AGG_MAX, AGG_MIN, AGG_SUM, AGG_AVG
};

enum OrderByDir {
    OrderBy_DEFAULT,
    OrderBy_ASC,
    OrderBy_DESC
};

enum SetKnobType {
    EnableNestLoop, EnableSortMerge
};

// Base class for tree nodes
struct TreeNode {
    bool is_explain = false;
    virtual ~TreeNode() = default;  // enable polymorphism
};

struct Help : public TreeNode {
};

struct ShowTables : public TreeNode {
};

struct TxnBegin : public TreeNode {
};

struct TxnCommit : public TreeNode {
};

struct TxnAbort : public TreeNode {
};

struct TxnRollback : public TreeNode {
};

struct TypeLen : public TreeNode {
    SvType type;
    int len;

    TypeLen(SvType type_, int len_) : type(type_), len(len_) {}
};

struct Field : public TreeNode {
};

struct ColDef : public Field {
    std::string col_name;
    std::shared_ptr<TypeLen> type_len;

    ColDef(std::string col_name_, std::shared_ptr<TypeLen> type_len_) :
            col_name(std::move(col_name_)), type_len(std::move(type_len_)) {}
};

struct CreateTable : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<Field>> fields;

    CreateTable(std::string tab_name_, std::vector<std::shared_ptr<Field>> fields_) :
            tab_name(std::move(tab_name_)), fields(std::move(fields_)) {}
};

struct DropTable : public TreeNode {
    std::string tab_name;

    DropTable(std::string tab_name_) : tab_name(std::move(tab_name_)) {}
};

struct DescTable : public TreeNode {
    std::string tab_name;

    DescTable(std::string tab_name_) : tab_name(std::move(tab_name_)) {}
};

struct CreateIndex : public TreeNode {
    std::string tab_name;
    std::vector<std::string> col_names;

    CreateIndex(std::string tab_name_, std::vector<std::string> col_names_) :
            tab_name(std::move(tab_name_)), col_names(std::move(col_names_)) {}
};

struct DropIndex : public TreeNode {
    std::string tab_name;
    std::vector<std::string> col_names;

    DropIndex(std::string tab_name_, std::vector<std::string> col_names_) :
            tab_name(std::move(tab_name_)), col_names(std::move(col_names_)) {}
};

struct Expr : public TreeNode {
};

struct Value : public Expr {
};

struct IntLit : public Value {
    int val;

    IntLit(int val_) : val(val_) {}
};

struct FloatLit : public Value {
    float val;

    FloatLit(float val_) : val(val_) {}
};

struct StringLit : public Value {
    std::string val;

    StringLit(std::string val_) : val(std::move(val_)) {}
};

struct BoolLit : public Value {
    bool val;

    BoolLit(bool val_) : val(val_) {}
};

struct Col : public Expr {
    std::string tab_name;
    std::string col_name;
    std::string alias;

    Col(std::string tab_name_, std::string col_name_, std::string alias_ = "")
        : tab_name(std::move(tab_name_)), col_name(std::move(col_name_)), alias(std::move(alias_)) {}
};

// 添加聚合函数表达式
struct AggFunc : public Expr {
    AggFuncType func_type;
    std::shared_ptr<Col> col;  // 可以是列名或nullptr(对于COUNT(*))
    std::string alias;         // 别名

    AggFunc(AggFuncType func_type_, std::shared_ptr<Col> col_, std::string alias_ = "") :
            func_type(func_type_), col(std::move(col_)), alias(std::move(alias_)) {}
};

struct SetClause : public TreeNode {
    std::string col_name;
    std::shared_ptr<Value> val;

    SetClause(std::string col_name_, std::shared_ptr<Value> val_) :
            col_name(std::move(col_name_)), val(std::move(val_)) {}
};

struct BinaryExpr : public TreeNode {
    std::shared_ptr<Expr> lhs;
    SvCompOp op;
    std::shared_ptr<Expr> rhs;

    BinaryExpr(std::shared_ptr<Expr> lhs_, SvCompOp op_, std::shared_ptr<Expr> rhs_) :
            lhs(std::move(lhs_)), op(op_), rhs(std::move(rhs_)) {}
};

struct OrderBy : public TreeNode
{
    std::vector<std::shared_ptr<Col>> cols;
    std::vector<OrderByDir> orderby_dirs;
    OrderBy( std::vector<std::shared_ptr<Col>> cols_, std::vector<OrderByDir> orderby_dirs_) :
       cols(std::move(cols_)), orderby_dirs(std::move(orderby_dirs_)) {}
};

struct InsertStmt : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<Value>> vals;

    InsertStmt(std::string tab_name_, std::vector<std::shared_ptr<Value>> vals_) :
            tab_name(std::move(tab_name_)), vals(std::move(vals_)) {}
};

struct DeleteStmt : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<BinaryExpr>> conds;

    DeleteStmt(std::string tab_name_, std::vector<std::shared_ptr<BinaryExpr>> conds_) :
            tab_name(std::move(tab_name_)), conds(std::move(conds_)) {}
};

struct UpdateStmt : public TreeNode {
    std::string tab_name;
    std::vector<std::shared_ptr<SetClause>> set_clauses;
    std::vector<std::shared_ptr<BinaryExpr>> conds;

    UpdateStmt(std::string tab_name_,
               std::vector<std::shared_ptr<SetClause>> set_clauses_,
               std::vector<std::shared_ptr<BinaryExpr>> conds_) :
            tab_name(std::move(tab_name_)), set_clauses(std::move(set_clauses_)), conds(std::move(conds_)) {}
};

struct JoinExpr : public TreeNode {
    std::string left;
    std::string right;
    std::vector<std::shared_ptr<BinaryExpr>> conds;
    JoinType type;

    JoinExpr(std::string left_, std::string right_,
               std::vector<std::shared_ptr<BinaryExpr>> conds_, JoinType type_) :
            left(std::move(left_)), right(std::move(right_)), conds(std::move(conds_)), type(type_) {}
};

// 添加分组子句
struct GroupBy : public TreeNode {
    std::vector<std::shared_ptr<Col>> cols;

    GroupBy(std::vector<std::shared_ptr<Col>> cols_) : cols(std::move(cols_)) {}
};

// 添加HAVING子句
struct Having : public TreeNode {
    std::vector<std::shared_ptr<BinaryExpr>> conds;

    Having(std::vector<std::shared_ptr<BinaryExpr>> conds_) : conds(std::move(conds_)) {}
};

// 添加LIMIT子句
struct Limit : public TreeNode {
    int limit_val;

    Limit(int limit_val_) : limit_val(limit_val_) {}
};

struct SelectStmt : public TreeNode {
    std::vector<std::shared_ptr<Col>> cols;
    std::vector<std::shared_ptr<AggFunc>> agg_funcs;  // 添加聚合函数列表
    std::vector<std::string> tabs;
    std::vector<std::shared_ptr<BinaryExpr>> conds;
    std::vector<std::shared_ptr<JoinExpr>> jointree;
    
    bool has_sort;
    std::shared_ptr<OrderBy> order;
    
    // 添加分组、HAVING和LIMIT支持
    bool has_group_by;
    std::shared_ptr<GroupBy> group_by;
    bool has_having;
    std::shared_ptr<Having> having;
    bool has_limit;
    std::shared_ptr<Limit> limit;

    SelectStmt(std::vector<std::shared_ptr<Col>> cols_,
               std::vector<std::shared_ptr<AggFunc>> agg_funcs_,
               std::vector<std::string> tabs_,
               std::vector<std::shared_ptr<BinaryExpr>> conds_,
               std::shared_ptr<OrderBy> order_,
               std::shared_ptr<GroupBy> group_by_ = nullptr,
               std::shared_ptr<Having> having_ = nullptr,
               std::shared_ptr<Limit> limit_ = nullptr,
               std::vector<std::shared_ptr<JoinExpr>> jointree_ = std::vector<std::shared_ptr<JoinExpr>>()) :
            cols(std::move(cols_)), agg_funcs(std::move(agg_funcs_)), tabs(std::move(tabs_)), 
            conds(std::move(conds_)), order(std::move(order_)), group_by(std::move(group_by_)),
            having(std::move(having_)), limit(std::move(limit_)), jointree(std::move(jointree_)) {
                has_sort = (bool)order;
                has_group_by = (bool)group_by;
                has_having = (bool)having;
                has_limit = (bool)limit;
            }
};

// set enable_nestloop
struct SetStmt : public TreeNode {
    SetKnobType set_knob_type_;
    bool bool_val_;

    SetStmt(SetKnobType &type, bool bool_value) : 
        set_knob_type_(type), bool_val_(bool_value) { }
};

struct TableRef {
    std::string tab_name;
    std::string alias;
    TableRef(const std::string& tab, const std::string& alias_ = "") : tab_name(tab), alias(alias_) {}
};

struct TabCol {
    std::string tab_name;
    std::string col_name;
    std::string alias; // 可选
    TabCol(const std::string& t, const std::string& c, const std::string& a = "") : tab_name(t), col_name(c), alias(a) {}
    TabCol() = default;
};

std::shared_ptr<TableRef> new_table_ref(const std::string& tab, const std::string* alias);

// Semantic value
struct SemValue {
    int sv_int;
    float sv_float;
    std::string sv_str;
    bool sv_bool;
    OrderByDir sv_orderby_dir;
    std::vector<std::string> sv_strs;

    std::shared_ptr<TreeNode> sv_node;

    SvCompOp sv_comp_op;
    AggFuncType sv_agg_func_type;  // 添加聚合函数类型

    std::shared_ptr<TypeLen> sv_type_len;

    std::shared_ptr<Field> sv_field;
    std::vector<std::shared_ptr<Field>> sv_fields;

    std::shared_ptr<Expr> sv_expr;
    std::shared_ptr<AggFunc> sv_agg_func;  // 添加聚合函数
    std::vector<std::shared_ptr<AggFunc>> sv_agg_funcs;  // 添加聚合函数列表

    std::shared_ptr<Value> sv_val;
    std::vector<std::shared_ptr<Value>> sv_vals;

    std::shared_ptr<Col> sv_col;
    std::vector<std::shared_ptr<Col>> sv_cols;

    std::shared_ptr<SetClause> sv_set_clause;
    std::vector<std::shared_ptr<SetClause>> sv_set_clauses;

    std::shared_ptr<BinaryExpr> sv_cond;
    std::vector<std::shared_ptr<BinaryExpr>> sv_conds;

    std::shared_ptr<OrderBy> sv_orderby;
    std::shared_ptr<GroupBy> sv_groupby;  // 添加分组
    std::shared_ptr<Having> sv_having;    // 添加HAVING
    std::shared_ptr<Limit> sv_limit;      // 添加LIMIT

    SetKnobType sv_setKnobType;
    
    // 添加连接表达式支持
    std::shared_ptr<JoinExpr> sv_joinexpr;
    std::vector<std::shared_ptr<JoinExpr>> sv_joinexprs;
    
    // 添加混合选择器类型，用于存储普通列和聚合函数的组合
    std::pair<std::vector<std::shared_ptr<Col>>, std::vector<std::shared_ptr<AggFunc>>> sv_mixed_selector;
    std::pair<std::shared_ptr<Col>, OrderByDir> sv_order_col_with_dir;
    std::vector<std::pair<std::shared_ptr<Col>, OrderByDir>> sv_order_col_list;

    std::shared_ptr<TableRef> sv_table_ref;
};

extern std::shared_ptr<ast::TreeNode> parse_tree;

struct ShowIndex : public TreeNode {
    std::string tab_name;
    ShowIndex(std::string tab_name_) : tab_name(std::move(tab_name_)) {}
};

}

#define YYSTYPE ast::SemValue
