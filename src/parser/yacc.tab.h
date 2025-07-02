/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_CODE_DB2025_SRC_PARSER_YACC_TAB_H_INCLUDED
# define YY_YY_HOME_CODE_DB2025_SRC_PARSER_YACC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SHOW = 258,                    /* SHOW  */
    TABLES = 259,                  /* TABLES  */
    CREATE = 260,                  /* CREATE  */
    TABLE = 261,                   /* TABLE  */
    DROP = 262,                    /* DROP  */
    DESC = 263,                    /* DESC  */
    INSERT = 264,                  /* INSERT  */
    INTO = 265,                    /* INTO  */
    VALUES = 266,                  /* VALUES  */
    DELETE = 267,                  /* DELETE  */
    FROM = 268,                    /* FROM  */
    ASC = 269,                     /* ASC  */
    WHERE = 270,                   /* WHERE  */
    UPDATE = 271,                  /* UPDATE  */
    SET = 272,                     /* SET  */
    SELECT = 273,                  /* SELECT  */
    INT = 274,                     /* INT  */
    CHAR = 275,                    /* CHAR  */
    FLOAT = 276,                   /* FLOAT  */
    INDEX = 277,                   /* INDEX  */
    AND = 278,                     /* AND  */
    JOIN = 279,                    /* JOIN  */
    SEMI = 280,                    /* SEMI  */
    EXIT = 281,                    /* EXIT  */
    HELP = 282,                    /* HELP  */
    TXN_BEGIN = 283,               /* TXN_BEGIN  */
    TXN_COMMIT = 284,              /* TXN_COMMIT  */
    TXN_ABORT = 285,               /* TXN_ABORT  */
    TXN_ROLLBACK = 286,            /* TXN_ROLLBACK  */
    ENABLE_NESTLOOP = 287,         /* ENABLE_NESTLOOP  */
    ENABLE_SORTMERGE = 288,        /* ENABLE_SORTMERGE  */
    COUNT = 289,                   /* COUNT  */
    MAX = 290,                     /* MAX  */
    MIN = 291,                     /* MIN  */
    SUM = 292,                     /* SUM  */
    AVG = 293,                     /* AVG  */
    GROUP = 294,                   /* GROUP  */
    HAVING = 295,                  /* HAVING  */
    LIMIT = 296,                   /* LIMIT  */
    AS = 297,                      /* AS  */
    ORDER = 298,                   /* ORDER  */
    BY = 299,                      /* BY  */
    ON = 300,                      /* ON  */
    LEQ = 301,                     /* LEQ  */
    NEQ = 302,                     /* NEQ  */
    GEQ = 303,                     /* GEQ  */
    T_EOF = 304,                   /* T_EOF  */
    EXPLAIN = 305,                 /* EXPLAIN  */
    IDENTIFIER = 306,              /* IDENTIFIER  */
    VALUE_STRING = 307,            /* VALUE_STRING  */
    VALUE_INT = 308,               /* VALUE_INT  */
    VALUE_FLOAT = 309,             /* VALUE_FLOAT  */
    VALUE_BOOL = 310               /* VALUE_BOOL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 31 "/home/code/db2025/src/parser/yacc.y"

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

    std::shared_ptr<TableRef> sv_table_ref;
    std::shared_ptr<JoinExpr> join_expr;

#line 171 "/home/code/db2025/src/parser/yacc.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int yyparse (void);


#endif /* !YY_YY_HOME_CODE_DB2025_SRC_PARSER_YACC_TAB_H_INCLUDED  */
