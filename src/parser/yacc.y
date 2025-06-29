%{
#include "ast.h"
#include "yacc.tab.h"
#include <iostream>
#include <memory>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

using namespace ast;
%}

// request a pure (reentrant) parser

%define api.pure full
// enable location in error handler
%locations
// enable verbose syntax error message
%define parse.error verbose

// 添加 %union 声明
%union {
    int sv_int;
    float sv_float;
    std::string sv_str;
    bool sv_bool;
    OrderByDir sv_orderby_dir;
    std::vector<std::string> sv_strs;

    std::shared_ptr<TreeNode> sv_node;

    SvCompOp sv_comp_op;
    AggFuncType sv_agg_func_type;

    std::shared_ptr<TypeLen> sv_type_len;

    std::shared_ptr<Field> sv_field;
    std::vector<std::shared_ptr<Field>> sv_fields;

    std::shared_ptr<Expr> sv_expr;
    std::shared_ptr<AggFunc> sv_agg_func;
    std::vector<std::shared_ptr<AggFunc>> sv_agg_funcs;

    std::shared_ptr<Value> sv_val;
    std::vector<std::shared_ptr<Value>> sv_vals;

    std::shared_ptr<Col> sv_col;
    std::vector<std::shared_ptr<Col>> sv_cols;

    std::shared_ptr<SetClause> sv_set_clause;
    std::vector<std::shared_ptr<SetClause>> sv_set_clauses;

    std::shared_ptr<BinaryExpr> sv_cond;
    std::vector<std::shared_ptr<BinaryExpr>> sv_conds;

    std::shared_ptr<OrderBy> sv_orderby;
    std::shared_ptr<GroupBy> sv_groupby;
    std::shared_ptr<Having> sv_having;
    std::shared_ptr<Limit> sv_limit;

    SetKnobType sv_setKnobType;
    
    std::pair<std::vector<std::shared_ptr<Col>>, std::vector<std::shared_ptr<AggFunc>>> sv_mixed_selector;
    std::pair<std::shared_ptr<Col>, OrderByDir> sv_order_col_with_dir;
    std::vector<std::pair<std::shared_ptr<Col>, OrderByDir>> sv_order_col_list;

    std::shared_ptr<JoinExpr> sv_joinexpr;
    std::vector<std::shared_ptr<JoinExpr>> sv_joinexprs;
}

// keywords
%token SHOW TABLES CREATE TABLE DROP DESC INSERT INTO VALUES DELETE FROM ASC
WHERE UPDATE SET SELECT INT CHAR FLOAT INDEX AND JOIN SEMI EXIT HELP TXN_BEGIN TXN_COMMIT TXN_ABORT TXN_ROLLBACK ENABLE_NESTLOOP ENABLE_SORTMERGE
// 添加聚合函数和分组关键字
COUNT MAX MIN SUM AVG GROUP HAVING LIMIT AS ORDER BY ON
// non-keywords
%token LEQ NEQ GEQ T_EOF

// type-specific tokens
%token <sv_str> IDENTIFIER VALUE_STRING
%token <sv_int> VALUE_INT
%token <sv_float> VALUE_FLOAT
%token <sv_bool> VALUE_BOOL

// specify types for non-terminal symbol
%type <sv_node> stmt dbStmt ddl dml txnStmt setStmt
%type <sv_field> field
%type <sv_fields> fieldList
%type <sv_type_len> type
%type <sv_comp_op> op
%type <sv_expr> expr
%type <sv_val> value
%type <sv_vals> valueList
%type <sv_str> tbName colName
%type <sv_strs> tableList colNameList
%type <sv_col> col
%type <sv_cols> colList selector
%type <sv_set_clause> setClause
%type <sv_set_clauses> setClauses
%type <sv_cond> condition
%type <sv_conds> whereClause optWhereClause
%type <sv_orderby>  order_clause opt_order_clause
%type <sv_orderby_dir> opt_asc_desc
%type <sv_setKnobType> set_knob_type

// 添加聚合函数和分组相关的类型声明
%type <sv_agg_func> agg_func
%type <sv_agg_func_type> agg_func_type
%type <sv_groupby> opt_group_by_clause
%type <sv_having> opt_having_clause
%type <sv_limit> opt_limit_clause
%type <sv_agg_funcs> agg_func_list
%type <sv_mixed_selector> mixed_selector
%type <sv_order_col_list> order_col_list
%type <sv_order_col_with_dir> order_col_with_dir
%type <sv_joinexpr> join_clause

// 添加新的非终结符来处理连接表达式
%type <sv_strs> tableListWithJoin
%type <sv_joinexprs> join_expr_list

// 为终结符添加类型声明
%type <sv_str> indexName

%%
start:
        stmt ';'
    {
        parse_tree = $1;
        YYACCEPT;
    }
    |   HELP
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
    |   EXIT
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
    |   T_EOF
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
    ;

stmt:
        dbStmt
    {
        $$ = $1;
    }
    |   ddl
    {
        $$ = $1;
    }
    |   dml
    {
        $$ = $1;
    }
    |   txnStmt
    {
        $$ = $1;
    }
    |   setStmt
    {
        $$ = $1;
    }
    ;

txnStmt:
        TXN_BEGIN
    {
        $$ = std::make_shared<TxnBegin>();
    }
    |   TXN_COMMIT
    {
        $$ = std::make_shared<TxnCommit>();
    }
    |   TXN_ABORT
    {
        $$ = std::make_shared<TxnAbort>();
    }
    | TXN_ROLLBACK
    {
        $$ = std::make_shared<TxnRollback>();
    }
    ;

dbStmt:
        SHOW TABLES
    {
        $$ = std::make_shared<ShowTables>();
    }
    |   CREATE TABLE tbName '(' fieldList ')'
    {
        $$ = std::make_shared<CreateTable>($3, $5);
    }
    |   DROP TABLE tbName
    {
        $$ = std::make_shared<DropTable>($3);
    }
    |   DESC tbName
    {
        $$ = std::make_shared<DescTable>($2);
    }
    |   CREATE INDEX indexName ON tbName '(' colNameList ')'
    {
        $$ = std::make_shared<CreateIndex>($5, $7);
    }
    |   DROP INDEX indexName ON tbName
    {
        $$ = std::make_shared<DropIndex>($5, std::vector<std::string>{});
    }
    |   SHOW INDEX tbName
    {
        $$ = std::make_shared<ShowIndex>($3);
    }
    ;

setStmt:
        SET set_knob_type '=' VALUE_BOOL
    {
        $$ = std::make_shared<SetStmt>($2, $4);
    }
    ;

ddl:
        CREATE TABLE tbName '(' fieldList ')'
    {
        $$ = std::make_shared<CreateTable>($3, $5);
    }
    |   DROP TABLE tbName
    {
        $$ = std::make_shared<DropTable>($3);
    }
    |   DESC tbName
    {
        $$ = std::make_shared<DescTable>($2);
    }
    |   CREATE INDEX tbName '(' colNameList ')'
    {
        $$ = std::make_shared<CreateIndex>($3, $5);
    }
    |   DROP INDEX tbName '(' colNameList ')'
    {
        $$ = std::make_shared<DropIndex>($3, $5);
    }
    |   SHOW INDEX FROM tbName
    {
        $$ = std::make_shared<ShowIndex>($4);
    }
    ;

dml:
        INSERT INTO tbName VALUES '(' valueList ')'
    {
        $$ = std::make_shared<InsertStmt>($3, $6);
    }
    |   DELETE FROM tbName optWhereClause
    {
        $$ = std::make_shared<DeleteStmt>($3, $4);
    }
    |   UPDATE tbName SET setClauses optWhereClause
    {
        $$ = std::make_shared<UpdateStmt>($2, $4, $5);
    }
    |   SELECT selector FROM tableListWithJoin join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause
    {
        $$ = std::make_shared<SelectStmt>($2, std::vector<std::shared_ptr<AggFunc>>(), $4, $6, $9, $7, $8, $10, $5);
    }
    |   SELECT agg_func_list FROM tableListWithJoin join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause
    {
        $$ = std::make_shared<SelectStmt>(std::vector<std::shared_ptr<Col>>(), $2, $4, $6, $9, $7, $8, $10, $5);
    }
    |   SELECT mixed_selector FROM tableListWithJoin join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause
    {
        $$ = std::make_shared<SelectStmt>($2.first, $2.second, $4, $6, $9, $7, $8, $10, $5);
    }
    ;

fieldList:
        field
    {
        $$ = std::vector<std::shared_ptr<Field>>{$1};
    }
    |   fieldList ',' field
    {
        $$.push_back($3);
    }
    ;

colNameList:
        colName
    {
        $$ = std::vector<std::string>{$1};
    }
    | colNameList ',' colName
    {
        $$.push_back($3);
    }
    ;

field:
        colName type
    {
        $$ = std::make_shared<ColDef>($1, $2);
    }
    ;

type:
        INT
    {
        $$ = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
    |   CHAR '(' VALUE_INT ')'
    {
        $$ = std::make_shared<TypeLen>(SV_TYPE_STRING, $3);
    }
    |   FLOAT
    {
        $$ = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
    ;

valueList:
        value
    {
        $$ = std::vector<std::shared_ptr<Value>>{$1};
    }
    |   valueList ',' value
    {
        $$.push_back($3);
    }
    ;

value:
        VALUE_INT
    {
        $$ = std::make_shared<IntLit>($1);
    }
    |   VALUE_FLOAT
    {
        $$ = std::make_shared<FloatLit>($1);
    }
    |   VALUE_STRING
    {
        $$ = std::make_shared<StringLit>($1);
    }
    |   VALUE_BOOL
    {
        $$ = std::make_shared<BoolLit>($1);
    }
    ;

condition:
        col op expr
    {
        $$ = std::make_shared<BinaryExpr>($1, $2, $3);
    }
    |   agg_func op expr
    {
        // 创建一个临时的 Col 对象来存储聚合函数信息
        // 这里需要修改 BinaryExpr 结构以支持聚合函数作为左操作数
        $$ = std::make_shared<BinaryExpr>($1, $2, $3);
    }
    ;

optWhereClause:
        /* epsilon */ { $$ = std::vector<std::shared_ptr<BinaryExpr>>(); }
    |   WHERE whereClause
    {
        $$ = $2;
    }
    ;

whereClause:
        condition 
    {
        $$ = std::vector<std::shared_ptr<BinaryExpr>>{$1};
    }
    |   whereClause AND condition
    {
        $$.push_back($3);
    }
    ;

col:
        tbName '.' colName
    {
        $$ = std::make_shared<Col>($1, $3);
    }
    |   colName
    {
        $$ = std::make_shared<Col>("", $1);
    }
    |   tbName '.' colName AS IDENTIFIER
    {
        $$ = std::make_shared<Col>($1, $3);
        // 注意：这里我们暂时忽略别名，因为Col结构中没有alias字段
        // 在实际实现中，需要扩展Col结构以支持别名
    }
    |   colName AS IDENTIFIER
    {
        $$ = std::make_shared<Col>("", $1);
        // 注意：这里我们暂时忽略别名，因为Col结构中没有alias字段
        // 在实际实现中，需要扩展Col结构以支持别名
    }
    ;

colList:
        col
    {
        $$ = std::vector<std::shared_ptr<Col>>{$1};
    }
    |   colList ',' col
    {
        $$.push_back($3);
    }
    ;

op:
        '='
    {
        $$ = SV_OP_EQ;
    }
    |   '<'
    {
        $$ = SV_OP_LT;
    }
    |   '>'
    {
        $$ = SV_OP_GT;
    }
    |   NEQ
    {
        $$ = SV_OP_NE;
    }
    |   LEQ
    {
        $$ = SV_OP_LE;
    }
    |   GEQ
    {
        $$ = SV_OP_GE;
    }
    ;

expr:
        value
    {
        $$ = std::static_pointer_cast<Expr>($1);
    }
    |   col
    {
        $$ = std::static_pointer_cast<Expr>($1);
    }
    |   agg_func
    {
        $$ = std::static_pointer_cast<Expr>($1);
    }
    |   '(' dml ')'
    {
        // 子查询：将SELECT语句包装为表达式
        $$ = std::static_pointer_cast<Expr>($2);
    }
    ;

setClauses:
        setClause
    {
        $$ = std::vector<std::shared_ptr<SetClause>>{$1};
    }
    |   setClauses ',' setClause
    {
        $$.push_back($3);
    }
    ;

setClause:
        colName '=' value
    {
        $$ = std::make_shared<SetClause>($1, $3);
    }
    ;

selector:
        '*'
    {
        $$ = std::vector<std::shared_ptr<Col>>();
    }
    |   colList
    ;

tableList:
        tbName
    {
        $$ = std::vector<std::string>{$1};
    }
    |   tableList ',' tbName
    {
        $$.push_back($3);
    }
    ;

tableListWithJoin:
        tableList
    {
        $$ = $1;
    }
    ;

join_expr_list:
        /* epsilon */
    {
        $$ = std::vector<std::shared_ptr<JoinExpr>>();
    }
    |   join_expr_list join_clause
    {
        $$ = $1;
        $$.push_back($2);
    }
    ;

join_clause:
    JOIN tbName ON whereClause
    {
        $$ = std::make_shared<JoinExpr>("", $2, $4, ::INNER_JOIN);
    }
    | SEMI JOIN tbName ON whereClause
    {
        $$ = std::make_shared<JoinExpr>("", $3, $5, ::SEMI_JOIN);
    }
    ;

opt_order_clause:
    ORDER BY order_clause      
    { 
        $$ = $3; 
    }
    |   /* epsilon */ { $$ = nullptr; }
    ;

order_clause:
      order_col_list
    { 
        // 从order_col_list中提取列和方向
        std::vector<std::shared_ptr<Col>> cols;
        std::vector<OrderByDir> dirs;
        for (const auto& col_with_dir : $1) {
            cols.push_back(col_with_dir.first);
            dirs.push_back(col_with_dir.second);
        }
        $$ = std::make_shared<OrderBy>(cols, dirs);
    }
    ;   

order_col_list:
    order_col_with_dir
    {
        $$ = std::vector<std::pair<std::shared_ptr<Col>, OrderByDir>>{$1};
    }
    |   order_col_list ',' order_col_with_dir
    {
        $$.push_back($3);
    }
    ;

order_col_with_dir:
    col opt_asc_desc
    {
        $$ = std::make_pair($1, $2);
    }
    ;

opt_asc_desc:
    ASC          { $$ = OrderBy_ASC;     }
    |  DESC      { $$ = OrderBy_DESC;    }
    |       { $$ = OrderBy_DEFAULT; }
    ;    

set_knob_type:
    ENABLE_NESTLOOP { $$ = EnableNestLoop; }
    |   ENABLE_SORTMERGE { $$ = EnableSortMerge; }
    ;

// 添加聚合函数相关的语法规则
agg_func_list:
    agg_func
    {
        $$ = std::vector<std::shared_ptr<AggFunc>>{$1};
    }
    |   agg_func_list ',' agg_func
    {
        $$.push_back($3);
    }
    ;

agg_func:
    COUNT '(' '*' ')'
    {
        $$ = std::make_shared<AggFunc>(AGG_COUNT, nullptr, "");
    }
    |   COUNT '(' '*' ')' AS IDENTIFIER
    {
        $$ = std::make_shared<AggFunc>(AGG_COUNT, nullptr, $6);
    }
    |   COUNT '(' col ')'
    {
        $$ = std::make_shared<AggFunc>(AGG_COUNT, $3, "");
    }
    |   COUNT '(' col ')' AS IDENTIFIER
    {
        $$ = std::make_shared<AggFunc>(AGG_COUNT, $3, $6);
    }
    |   agg_func_type '(' col ')'
    {
        $$ = std::make_shared<AggFunc>($1, $3, "");
    }
    |   agg_func_type '(' col ')' AS IDENTIFIER
    {
        $$ = std::make_shared<AggFunc>($1, $3, $6);
    }
    ;

agg_func_type:
    MAX { $$ = AGG_MAX; }
    |   MIN { $$ = AGG_MIN; }
    |   SUM { $$ = AGG_SUM; }
    |   AVG { $$ = AGG_AVG; }
    ;

opt_group_by_clause:
    /* epsilon */ { $$ = nullptr; }
    |   GROUP BY colList
    {
        $$ = std::make_shared<GroupBy>($3);
    }
    ;

opt_having_clause:
    /* epsilon */ { $$ = nullptr; }
    |   HAVING whereClause
    {
        $$ = std::make_shared<Having>($2);
    }
    ;

opt_limit_clause:
    /* epsilon */ { $$ = nullptr; }
    |   LIMIT VALUE_INT
    {
        $$ = std::make_shared<Limit>($2);
    }
    ;

// 添加混合选择器规则，支持普通列和聚合函数的组合
mixed_selector:
    col ',' agg_func
    {
        $$ = std::make_pair(std::vector<std::shared_ptr<Col>>{$1}, std::vector<std::shared_ptr<AggFunc>>{$3});
    }
    |   agg_func ',' col
    {
        $$ = std::make_pair(std::vector<std::shared_ptr<Col>>{$3}, std::vector<std::shared_ptr<AggFunc>>{$1});
    }
    |   col ',' col
    {
        $$ = std::make_pair(std::vector<std::shared_ptr<Col>>{$1, $3}, std::vector<std::shared_ptr<AggFunc>>());
    }
    |   agg_func ',' agg_func
    {
        $$ = std::make_pair(std::vector<std::shared_ptr<Col>>(), std::vector<std::shared_ptr<AggFunc>>{$1, $3});
    }
    |   mixed_selector ',' col
    {
        $$.first.push_back($3);
    }
    |   mixed_selector ',' agg_func
    {
        $$.second.push_back($3);
    }
    ;

tbName: IDENTIFIER;

colName: IDENTIFIER;

indexName: IDENTIFIER;
%%
