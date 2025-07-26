/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "/home/code/db2025/src/parser/yacc.y"

#include "parser/ast.h"
#include "yacc.tab.h"
#include <iostream>
#include <memory>
#include <string>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

// new_table_ref 辅助函数
std::shared_ptr<ast::TableRef> new_table_ref(const std::string& tab, const std::string* alias) {
    return std::make_shared<ast::TableRef>(tab, alias ? *alias : "");
}

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

using namespace ast;

#line 92 "/home/code/db2025/src/parser/yacc.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SHOW = 3,                       /* SHOW  */
  YYSYMBOL_TABLES = 4,                     /* TABLES  */
  YYSYMBOL_CREATE = 5,                     /* CREATE  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_DROP = 7,                       /* DROP  */
  YYSYMBOL_DESC = 8,                       /* DESC  */
  YYSYMBOL_INSERT = 9,                     /* INSERT  */
  YYSYMBOL_INTO = 10,                      /* INTO  */
  YYSYMBOL_VALUES = 11,                    /* VALUES  */
  YYSYMBOL_DELETE = 12,                    /* DELETE  */
  YYSYMBOL_FROM = 13,                      /* FROM  */
  YYSYMBOL_ASC = 14,                       /* ASC  */
  YYSYMBOL_WHERE = 15,                     /* WHERE  */
  YYSYMBOL_UPDATE = 16,                    /* UPDATE  */
  YYSYMBOL_SET = 17,                       /* SET  */
  YYSYMBOL_SELECT = 18,                    /* SELECT  */
  YYSYMBOL_INT = 19,                       /* INT  */
  YYSYMBOL_CHAR = 20,                      /* CHAR  */
  YYSYMBOL_FLOAT = 21,                     /* FLOAT  */
  YYSYMBOL_INDEX = 22,                     /* INDEX  */
  YYSYMBOL_AND = 23,                       /* AND  */
  YYSYMBOL_JOIN = 24,                      /* JOIN  */
  YYSYMBOL_SEMI = 25,                      /* SEMI  */
  YYSYMBOL_EXIT = 26,                      /* EXIT  */
  YYSYMBOL_HELP = 27,                      /* HELP  */
  YYSYMBOL_TXN_BEGIN = 28,                 /* TXN_BEGIN  */
  YYSYMBOL_TXN_COMMIT = 29,                /* TXN_COMMIT  */
  YYSYMBOL_TXN_ABORT = 30,                 /* TXN_ABORT  */
  YYSYMBOL_TXN_ROLLBACK = 31,              /* TXN_ROLLBACK  */
  YYSYMBOL_ENABLE_NESTLOOP = 32,           /* ENABLE_NESTLOOP  */
  YYSYMBOL_ENABLE_SORTMERGE = 33,          /* ENABLE_SORTMERGE  */
  YYSYMBOL_COUNT = 34,                     /* COUNT  */
  YYSYMBOL_MAX = 35,                       /* MAX  */
  YYSYMBOL_MIN = 36,                       /* MIN  */
  YYSYMBOL_SUM = 37,                       /* SUM  */
  YYSYMBOL_AVG = 38,                       /* AVG  */
  YYSYMBOL_GROUP = 39,                     /* GROUP  */
  YYSYMBOL_HAVING = 40,                    /* HAVING  */
  YYSYMBOL_LIMIT = 41,                     /* LIMIT  */
  YYSYMBOL_AS = 42,                        /* AS  */
  YYSYMBOL_ORDER = 43,                     /* ORDER  */
  YYSYMBOL_BY = 44,                        /* BY  */
  YYSYMBOL_ON = 45,                        /* ON  */
  YYSYMBOL_LEQ = 46,                       /* LEQ  */
  YYSYMBOL_NEQ = 47,                       /* NEQ  */
  YYSYMBOL_GEQ = 48,                       /* GEQ  */
  YYSYMBOL_T_EOF = 49,                     /* T_EOF  */
  YYSYMBOL_EXPLAIN = 50,                   /* EXPLAIN  */
  YYSYMBOL_IDENTIFIER = 51,                /* IDENTIFIER  */
  YYSYMBOL_VALUE_STRING = 52,              /* VALUE_STRING  */
  YYSYMBOL_VALUE_INT = 53,                 /* VALUE_INT  */
  YYSYMBOL_VALUE_FLOAT = 54,               /* VALUE_FLOAT  */
  YYSYMBOL_VALUE_BOOL = 55,                /* VALUE_BOOL  */
  YYSYMBOL_56_ = 56,                       /* '+'  */
  YYSYMBOL_57_ = 57,                       /* '-'  */
  YYSYMBOL_58_ = 58,                       /* '*'  */
  YYSYMBOL_59_ = 59,                       /* '/'  */
  YYSYMBOL_UMINUS = 60,                    /* UMINUS  */
  YYSYMBOL_61_ = 61,                       /* ';'  */
  YYSYMBOL_62_ = 62,                       /* '('  */
  YYSYMBOL_63_ = 63,                       /* ')'  */
  YYSYMBOL_64_ = 64,                       /* '='  */
  YYSYMBOL_65_ = 65,                       /* ','  */
  YYSYMBOL_66_ = 66,                       /* '.'  */
  YYSYMBOL_67_ = 67,                       /* '<'  */
  YYSYMBOL_68_ = 68,                       /* '>'  */
  YYSYMBOL_YYACCEPT = 69,                  /* $accept  */
  YYSYMBOL_start = 70,                     /* start  */
  YYSYMBOL_stmt = 71,                      /* stmt  */
  YYSYMBOL_txnStmt = 72,                   /* txnStmt  */
  YYSYMBOL_dbStmt = 73,                    /* dbStmt  */
  YYSYMBOL_setStmt = 74,                   /* setStmt  */
  YYSYMBOL_ddl = 75,                       /* ddl  */
  YYSYMBOL_dml = 76,                       /* dml  */
  YYSYMBOL_fieldList = 77,                 /* fieldList  */
  YYSYMBOL_colNameList = 78,               /* colNameList  */
  YYSYMBOL_field = 79,                     /* field  */
  YYSYMBOL_type = 80,                      /* type  */
  YYSYMBOL_valueList = 81,                 /* valueList  */
  YYSYMBOL_value = 82,                     /* value  */
  YYSYMBOL_condition = 83,                 /* condition  */
  YYSYMBOL_optWhereClause = 84,            /* optWhereClause  */
  YYSYMBOL_whereClause = 85,               /* whereClause  */
  YYSYMBOL_col = 86,                       /* col  */
  YYSYMBOL_colList = 87,                   /* colList  */
  YYSYMBOL_op = 88,                        /* op  */
  YYSYMBOL_expr = 89,                      /* expr  */
  YYSYMBOL_setClauses = 90,                /* setClauses  */
  YYSYMBOL_setClause = 91,                 /* setClause  */
  YYSYMBOL_selector = 92,                  /* selector  */
  YYSYMBOL_table_ref_list = 93,            /* table_ref_list  */
  YYSYMBOL_join_expr_list = 94,            /* join_expr_list  */
  YYSYMBOL_join_clause = 95,               /* join_clause  */
  YYSYMBOL_opt_order_clause = 96,          /* opt_order_clause  */
  YYSYMBOL_order_clause = 97,              /* order_clause  */
  YYSYMBOL_order_col_list = 98,            /* order_col_list  */
  YYSYMBOL_order_col_with_dir = 99,        /* order_col_with_dir  */
  YYSYMBOL_opt_asc_desc = 100,             /* opt_asc_desc  */
  YYSYMBOL_set_knob_type = 101,            /* set_knob_type  */
  YYSYMBOL_agg_func_list = 102,            /* agg_func_list  */
  YYSYMBOL_agg_func = 103,                 /* agg_func  */
  YYSYMBOL_agg_func_type = 104,            /* agg_func_type  */
  YYSYMBOL_opt_group_by_clause = 105,      /* opt_group_by_clause  */
  YYSYMBOL_opt_having_clause = 106,        /* opt_having_clause  */
  YYSYMBOL_opt_limit_clause = 107,         /* opt_limit_clause  */
  YYSYMBOL_mixed_selector = 108,           /* mixed_selector  */
  YYSYMBOL_tbName = 109,                   /* tbName  */
  YYSYMBOL_colName = 110,                  /* colName  */
  YYSYMBOL_indexName = 111,                /* indexName  */
  YYSYMBOL_table_ref = 112                 /* table_ref  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  56
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   294

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  131
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  257

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   311


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      62,    63,    58,    56,    65,    57,    66,    59,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    61,
      67,    64,    68,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    60
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   149,   149,   154,   161,   166,   171,   179,   183,   187,
     191,   195,   202,   206,   210,   214,   221,   225,   229,   233,
     237,   241,   245,   252,   259,   263,   267,   271,   275,   279,
     286,   290,   294,   298,   302,   306,   313,   317,   324,   328,
     335,   342,   346,   350,   357,   361,   368,   372,   376,   380,
     387,   391,   400,   401,   408,   412,   419,   423,   427,   431,
     438,   442,   449,   453,   457,   461,   465,   469,   476,   480,
     484,   488,   493,   498,   502,   506,   510,   516,   523,   527,
     534,   541,   545,   567,   571,   579,   582,   590,   594,   598,
     602,   609,   613,   617,   631,   635,   642,   649,   650,   651,
     655,   656,   661,   665,   672,   676,   680,   684,   688,   692,
     699,   700,   701,   702,   706,   707,   714,   715,   722,   723,
     731,   735,   739,   743,   747,   751,   757,   759,   761,   764,
     765,   766
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SHOW", "TABLES",
  "CREATE", "TABLE", "DROP", "DESC", "INSERT", "INTO", "VALUES", "DELETE",
  "FROM", "ASC", "WHERE", "UPDATE", "SET", "SELECT", "INT", "CHAR",
  "FLOAT", "INDEX", "AND", "JOIN", "SEMI", "EXIT", "HELP", "TXN_BEGIN",
  "TXN_COMMIT", "TXN_ABORT", "TXN_ROLLBACK", "ENABLE_NESTLOOP",
  "ENABLE_SORTMERGE", "COUNT", "MAX", "MIN", "SUM", "AVG", "GROUP",
  "HAVING", "LIMIT", "AS", "ORDER", "BY", "ON", "LEQ", "NEQ", "GEQ",
  "T_EOF", "EXPLAIN", "IDENTIFIER", "VALUE_STRING", "VALUE_INT",
  "VALUE_FLOAT", "VALUE_BOOL", "'+'", "'-'", "'*'", "'/'", "UMINUS", "';'",
  "'('", "')'", "'='", "','", "'.'", "'<'", "'>'", "$accept", "start",
  "stmt", "txnStmt", "dbStmt", "setStmt", "ddl", "dml", "fieldList",
  "colNameList", "field", "type", "valueList", "value", "condition",
  "optWhereClause", "whereClause", "col", "colList", "op", "expr",
  "setClauses", "setClause", "selector", "table_ref_list",
  "join_expr_list", "join_clause", "opt_order_clause", "order_clause",
  "order_col_list", "order_col_with_dir", "opt_asc_desc", "set_knob_type",
  "agg_func_list", "agg_func", "agg_func_type", "opt_group_by_clause",
  "opt_having_clause", "opt_limit_clause", "mixed_selector", "tbName",
  "colName", "indexName", "table_ref", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-151)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-129)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     134,     6,    14,    23,   -29,    24,    43,   -29,    46,   191,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,   163,    60,    26,
    -151,  -151,  -151,  -151,  -151,  -151,    -5,   -29,    39,   -29,
      39,  -151,  -151,   -29,   -29,    58,  -151,  -151,    29,    61,
    -151,  -151,  -151,  -151,    74,  -151,    82,   108,   113,   -11,
     117,   128,    -7,   141,   167,   149,  -151,  -151,   -29,  -151,
     168,   194,   181,   199,  -151,   183,   202,   237,   235,   200,
     197,     8,    81,   203,   204,   204,   161,    81,   203,   204,
      81,   200,   205,  -151,  -151,   200,   200,   -29,   200,   -29,
     195,    81,  -151,  -151,    -8,  -151,   189,  -151,   196,   198,
    -151,  -151,  -151,    10,   193,  -151,   193,  -151,  -151,  -151,
     201,   193,  -151,  -151,   218,  -151,     2,  -151,   115,    35,
    -151,   206,    94,  -151,    54,  -151,   239,    57,    57,   200,
    -151,   151,   221,   223,   215,  -151,   204,    17,    17,   225,
      17,   219,  -151,   200,  -151,   207,  -151,  -151,  -151,   200,
     200,  -151,  -151,  -151,  -151,  -151,   104,  -151,    81,  -151,
    -151,  -151,  -151,  -151,  -151,   151,   151,  -151,   151,    76,
    -151,  -151,   160,  -151,   220,   222,  -151,  -151,   204,   248,
     236,  -151,   236,   226,   236,  -151,  -151,   227,  -151,   111,
    -151,    54,  -151,   160,   160,  -151,   211,   178,   151,   151,
     151,   151,  -151,  -151,    -2,   204,   232,   238,   238,  -151,
     238,   216,  -151,  -151,  -151,  -151,    95,    95,  -151,  -151,
      81,    81,    -1,   203,    81,   240,   240,   240,  -151,   239,
     239,    81,    81,  -151,   108,   239,   241,   243,   243,   243,
     239,   239,   203,   228,  -151,  -151,  -151,    75,  -151,   217,
    -151,  -151,  -151,  -151,  -151,   203,  -151
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     4,    12,    13,    14,    15,     6,     0,     0,     0,
      10,     7,    11,     8,     9,    16,     0,     0,     0,     0,
       0,   126,    19,     0,     0,     0,   100,   101,     0,     0,
     110,   111,   112,   113,   127,    81,    60,    82,     0,     0,
     102,     0,     0,     0,    57,     0,     1,     2,     0,    22,
       0,   126,     0,     0,    18,     0,     0,     0,    52,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     3,    29,     0,     0,     0,     0,     0,
       0,     0,    31,   127,    52,    78,     0,    23,     0,     0,
     122,   120,    61,   129,    85,    83,    85,   103,   121,   123,
       0,    85,   124,   125,    56,    59,     0,    36,     0,     0,
      38,     0,     0,    21,     0,    54,    53,     0,     0,     0,
      32,     0,   104,   106,     0,   130,     0,    52,    52,   108,
      52,     0,    17,     0,    41,     0,    43,    40,    27,     0,
       0,    28,    48,    46,    47,    49,     0,    44,     0,    66,
      65,    67,    62,    63,    64,     0,     0,    79,     0,     0,
      68,    69,    80,    70,     0,     0,   131,    84,     0,     0,
     114,    86,   114,     0,   114,    58,    37,     0,    39,     0,
      30,     0,    55,    50,    51,    76,     0,     0,     0,     0,
       0,     0,   105,   107,     0,     0,     0,   116,   116,   109,
     116,     0,    20,    45,    71,    77,    72,    73,    74,    75,
       0,     0,     0,     0,     0,    92,    92,    92,    42,    88,
      87,     0,     0,    60,   115,   117,     0,   118,   118,   118,
      90,    89,     0,     0,    33,    34,    35,    99,    91,    93,
      94,   119,    98,    97,    96,     0,    95
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -151,  -151,   269,  -151,  -151,  -151,  -151,   118,  -151,   -77,
     145,  -151,  -151,  -121,   131,   -41,   -76,    -9,    67,   164,
    -150,  -151,   162,  -151,   -40,   -94,  -151,   -49,  -151,  -151,
      38,  -151,  -151,  -151,     0,  -151,   -62,    30,   -38,  -151,
      -3,   -48,   264,  -131
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    18,    19,    20,    21,    22,    23,    24,   116,   119,
     117,   147,   156,   170,   125,    92,   126,   171,    47,   165,
     172,    94,    95,    48,   104,   137,   181,   237,   248,   249,
     250,   254,    38,    49,   173,    51,   207,   225,   244,    52,
      53,    54,    63,   105
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      46,    32,    75,   157,    35,   177,    79,    91,    58,    50,
      25,   122,   138,   220,   231,   193,   194,   140,   195,   197,
      27,    96,    31,    59,    60,    62,    64,    65,    26,    29,
      67,    68,    91,   114,    33,   106,    28,   118,   120,   111,
     120,   178,   179,   221,   232,    30,    31,   204,   216,   217,
     218,   219,   134,   130,    76,    84,    34,   129,    80,    44,
      56,   135,    99,   100,   102,   142,    98,   143,   108,   110,
     213,   112,   101,   189,   222,    69,   107,   109,    36,    37,
     113,    96,   127,   252,   121,     5,   123,    57,     6,   253,
      61,   128,     7,    70,     9,   118,   180,   182,   148,   184,
     149,   188,   120,   159,   160,   161,   152,   153,   154,   155,
      39,    40,    41,    42,    43,    39,    40,    41,    42,    43,
     208,   162,   210,    71,   163,   164,    74,    44,   152,   153,
     154,   155,    44,   168,   144,   145,   146,     1,   169,     2,
    -126,     3,     4,     5,   229,   230,     6,    72,   235,   127,
       7,     8,     9,   200,   201,   240,   241,   151,   128,   149,
      10,    11,    12,    13,    14,    15,     1,   190,     2,   191,
       3,     4,     5,    73,   212,     6,   149,   238,   239,     7,
       8,     9,    77,    16,    17,    39,    40,    41,    42,    43,
      78,    12,    13,    14,    15,    39,    40,    41,    42,    43,
     245,   246,    44,   152,   153,   154,   155,    81,   168,    82,
      83,   127,   127,   169,   233,   127,   198,   199,   200,   201,
     128,   128,   127,   127,   128,    39,    40,    41,    42,    43,
      85,   128,   128,   247,   198,   199,   200,   201,   226,  -128,
     227,   215,    44,    86,    87,    88,   247,    89,    90,    45,
      91,    93,    97,   131,    44,   103,   115,   124,   136,   132,
     141,   133,   158,   174,   139,   175,   176,   183,   150,   187,
     185,   202,   205,   203,   214,   206,   223,   209,   224,   228,
     211,   251,   255,   236,   243,   242,    55,   196,   186,   192,
     234,   167,   166,   256,    66
};

static const yytype_uint8 yycheck[] =
{
       9,     4,    13,   124,     7,   136,    13,    15,    13,     9,
       4,    88,   106,    15,    15,   165,   166,   111,   168,   169,
       6,    69,    51,    26,    27,    28,    29,    30,    22,     6,
      33,    34,    15,    81,    10,    75,    22,    85,    86,    79,
      88,    24,    25,    45,    45,    22,    51,   178,   198,   199,
     200,   201,    42,    94,    65,    58,    13,    65,    65,    51,
       0,    51,    71,    72,    73,    63,    58,    65,    77,    78,
     191,    80,    72,   150,   205,    17,    76,    77,    32,    33,
      80,   129,    91,     8,    87,     9,    89,    61,    12,    14,
      51,    91,    16,    64,    18,   143,   137,   138,    63,   140,
      65,   149,   150,    46,    47,    48,    52,    53,    54,    55,
      34,    35,    36,    37,    38,    34,    35,    36,    37,    38,
     182,    64,   184,    62,    67,    68,    13,    51,    52,    53,
      54,    55,    51,    57,    19,    20,    21,     3,    62,     5,
      66,     7,     8,     9,   220,   221,    12,    65,   224,   158,
      16,    17,    18,    58,    59,   231,   232,    63,   158,    65,
      26,    27,    28,    29,    30,    31,     3,    63,     5,    65,
       7,     8,     9,    65,    63,    12,    65,   226,   227,    16,
      17,    18,    65,    49,    50,    34,    35,    36,    37,    38,
      62,    28,    29,    30,    31,    34,    35,    36,    37,    38,
     238,   239,    51,    52,    53,    54,    55,    66,    57,    42,
      61,   220,   221,    62,   223,   224,    56,    57,    58,    59,
     220,   221,   231,   232,   224,    34,    35,    36,    37,    38,
      62,   231,   232,   242,    56,    57,    58,    59,   208,    45,
     210,    63,    51,    62,    45,    62,   255,    45,    11,    58,
      15,    51,    55,    64,    51,    51,    51,    62,    65,    63,
      42,    63,    23,    42,    63,    42,    51,    42,    62,    62,
      51,    51,    24,    51,    63,    39,    44,    51,    40,    63,
      53,    53,    65,    43,    41,    44,    17,   169,   143,   158,
     223,   129,   128,   255,    30
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     7,     8,     9,    12,    16,    17,    18,
      26,    27,    28,    29,    30,    31,    49,    50,    70,    71,
      72,    73,    74,    75,    76,     4,    22,     6,    22,     6,
      22,    51,   109,    10,    13,   109,    32,    33,   101,    34,
      35,    36,    37,    38,    51,    58,    86,    87,    92,   102,
     103,   104,   108,   109,   110,    71,     0,    61,    13,   109,
     109,    51,   109,   111,   109,   109,   111,   109,   109,    17,
      64,    62,    65,    65,    13,    13,    65,    65,    62,    13,
      65,    66,    42,    61,   109,    62,    62,    45,    62,    45,
      11,    15,    84,    51,    90,    91,   110,    55,    58,    86,
      86,   103,    86,    51,    93,   112,    93,   103,    86,   103,
      86,    93,    86,   103,   110,    51,    77,    79,   110,    78,
     110,   109,    78,   109,    62,    83,    85,    86,   103,    65,
      84,    64,    63,    63,    42,    51,    65,    94,    94,    63,
      94,    42,    63,    65,    19,    20,    21,    80,    63,    65,
      62,    63,    52,    53,    54,    55,    81,    82,    23,    46,
      47,    48,    64,    67,    68,    88,    88,    91,    57,    62,
      82,    86,    89,   103,    42,    42,    51,   112,    24,    25,
      84,    95,    84,    42,    84,    51,    79,    62,   110,    78,
      63,    65,    83,    89,    89,    89,    76,    89,    56,    57,
      58,    59,    51,    51,   112,    24,    39,   105,   105,    51,
     105,    53,    63,    82,    63,    63,    89,    89,    89,    89,
      15,    45,   112,    44,    40,   106,   106,   106,    63,    85,
      85,    15,    45,    86,    87,    85,    43,    96,    96,    96,
      85,    85,    44,    41,   107,   107,   107,    86,    97,    98,
      99,    53,     8,    14,   100,    65,    99
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    69,    70,    70,    70,    70,    70,    71,    71,    71,
      71,    71,    72,    72,    72,    72,    73,    73,    73,    73,
      73,    73,    73,    74,    75,    75,    75,    75,    75,    75,
      76,    76,    76,    76,    76,    76,    77,    77,    78,    78,
      79,    80,    80,    80,    81,    81,    82,    82,    82,    82,
      83,    83,    84,    84,    85,    85,    86,    86,    86,    86,
      87,    87,    88,    88,    88,    88,    88,    88,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    90,    90,
      91,    92,    92,    93,    93,    94,    94,    95,    95,    95,
      95,    96,    96,    97,    98,    98,    99,   100,   100,   100,
     101,   101,   102,   102,   103,   103,   103,   103,   103,   103,
     104,   104,   104,   104,   105,   105,   106,   106,   107,   107,
     108,   108,   108,   108,   108,   108,   109,   110,   111,   112,
     112,   112
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     6,     3,     2,
       8,     5,     3,     4,     6,     3,     2,     6,     6,     4,
       7,     4,     5,    10,    10,    10,     1,     3,     1,     3,
       2,     1,     4,     1,     1,     3,     1,     1,     1,     1,
       3,     3,     0,     2,     1,     3,     3,     1,     5,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     2,     3,     1,     3,
       3,     1,     1,     1,     3,     0,     2,     4,     4,     5,
       5,     3,     0,     1,     1,     3,     2,     1,     1,     0,
       1,     1,     1,     3,     4,     6,     4,     6,     4,     6,
       1,     1,     1,     1,     0,     3,     0,     2,     0,     2,
       3,     3,     3,     3,     3,     3,     1,     1,     1,     1,
       2,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* start: stmt ';'  */
#line 150 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = (yyvsp[-1].sv_node);
        YYACCEPT;
    }
#line 1778 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 3: /* start: EXPLAIN stmt ';'  */
#line 155 "/home/code/db2025/src/parser/yacc.y"
    {
        // 标记为 explain
        if ((yyvsp[-1].sv_node)) (yyvsp[-1].sv_node)->is_explain = true;
        parse_tree = (yyvsp[-1].sv_node);
        YYACCEPT;
    }
#line 1789 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 4: /* start: HELP  */
#line 162 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
#line 1798 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 5: /* start: EXIT  */
#line 167 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1807 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 6: /* start: T_EOF  */
#line 172 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1816 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 7: /* stmt: dbStmt  */
#line 180 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1824 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 8: /* stmt: ddl  */
#line 184 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1832 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 9: /* stmt: dml  */
#line 188 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1840 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 10: /* stmt: txnStmt  */
#line 192 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1848 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 11: /* stmt: setStmt  */
#line 196 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1856 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 12: /* txnStmt: TXN_BEGIN  */
#line 203 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnBegin>();
    }
#line 1864 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 13: /* txnStmt: TXN_COMMIT  */
#line 207 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnCommit>();
    }
#line 1872 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 14: /* txnStmt: TXN_ABORT  */
#line 211 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnAbort>();
    }
#line 1880 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 15: /* txnStmt: TXN_ROLLBACK  */
#line 215 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnRollback>();
    }
#line 1888 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 16: /* dbStmt: SHOW TABLES  */
#line 222 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowTables>();
    }
#line 1896 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 17: /* dbStmt: CREATE TABLE tbName '(' fieldList ')'  */
#line 226 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1904 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 18: /* dbStmt: DROP TABLE tbName  */
#line 230 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1912 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 19: /* dbStmt: DESC tbName  */
#line 234 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1920 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 20: /* dbStmt: CREATE INDEX indexName ON tbName '(' colNameList ')'  */
#line 238 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1928 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 21: /* dbStmt: DROP INDEX indexName ON tbName  */
#line 242 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[0].sv_str), std::vector<std::string>{});
    }
#line 1936 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 22: /* dbStmt: SHOW INDEX tbName  */
#line 246 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 1944 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 23: /* setStmt: SET set_knob_type '=' VALUE_BOOL  */
#line 253 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetStmt>((yyvsp[-2].sv_setKnobType), (yyvsp[0].sv_bool));
    }
#line 1952 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 24: /* ddl: CREATE TABLE tbName '(' fieldList ')'  */
#line 260 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1960 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 25: /* ddl: DROP TABLE tbName  */
#line 264 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1968 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 26: /* ddl: DESC tbName  */
#line 268 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1976 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 27: /* ddl: CREATE INDEX tbName '(' colNameList ')'  */
#line 272 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1984 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 28: /* ddl: DROP INDEX tbName '(' colNameList ')'  */
#line 276 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1992 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 29: /* ddl: SHOW INDEX FROM tbName  */
#line 280 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 2000 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 30: /* dml: INSERT INTO tbName VALUES '(' valueList ')'  */
#line 287 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<InsertStmt>((yyvsp[-4].sv_str), (yyvsp[-1].sv_vals));
    }
#line 2008 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 31: /* dml: DELETE FROM tbName optWhereClause  */
#line 291 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DeleteStmt>((yyvsp[-1].sv_str), (yyvsp[0].sv_conds));
    }
#line 2016 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 32: /* dml: UPDATE tbName SET setClauses optWhereClause  */
#line 295 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<UpdateStmt>((yyvsp[-3].sv_str), (yyvsp[-1].sv_set_clauses), (yyvsp[0].sv_conds));
    }
#line 2024 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 33: /* dml: SELECT selector FROM table_ref_list join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 299 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>((yyvsp[-8].sv_cols), std::vector<std::shared_ptr<AggFunc>>(), (yyvsp[-6].sv_table_refs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit), (yyvsp[-5].sv_joinexprs));
    }
#line 2032 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 34: /* dml: SELECT agg_func_list FROM table_ref_list join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 303 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>(std::vector<std::shared_ptr<Col>>(), (yyvsp[-8].sv_agg_funcs), (yyvsp[-6].sv_table_refs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit), (yyvsp[-5].sv_joinexprs));
    }
#line 2040 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 35: /* dml: SELECT mixed_selector FROM table_ref_list join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 307 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>((yyvsp[-8].sv_mixed_selector).first, (yyvsp[-8].sv_mixed_selector).second, (yyvsp[-6].sv_table_refs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit), (yyvsp[-5].sv_joinexprs));
    }
#line 2048 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 36: /* fieldList: field  */
#line 314 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_fields) = std::vector<std::shared_ptr<Field>>{(yyvsp[0].sv_field)};
    }
#line 2056 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 37: /* fieldList: fieldList ',' field  */
#line 318 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_fields).push_back((yyvsp[0].sv_field));
    }
#line 2064 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 38: /* colNameList: colName  */
#line 325 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 2072 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 39: /* colNameList: colNameList ',' colName  */
#line 329 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 2080 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 40: /* field: colName type  */
#line 336 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_field) = std::make_shared<ColDef>((yyvsp[-1].sv_str), (yyvsp[0].sv_type_len));
    }
#line 2088 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 41: /* type: INT  */
#line 343 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
#line 2096 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 42: /* type: CHAR '(' VALUE_INT ')'  */
#line 347 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_STRING, (yyvsp[-1].sv_int));
    }
#line 2104 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 43: /* type: FLOAT  */
#line 351 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
#line 2112 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 44: /* valueList: value  */
#line 358 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_vals) = std::vector<std::shared_ptr<Value>>{(yyvsp[0].sv_val)};
    }
#line 2120 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 45: /* valueList: valueList ',' value  */
#line 362 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_vals).push_back((yyvsp[0].sv_val));
    }
#line 2128 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 46: /* value: VALUE_INT  */
#line 369 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>((yyvsp[0].sv_int));
    }
#line 2136 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 47: /* value: VALUE_FLOAT  */
#line 373 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>((yyvsp[0].sv_float));
    }
#line 2144 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 48: /* value: VALUE_STRING  */
#line 377 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<StringLit>((yyvsp[0].sv_str));
    }
#line 2152 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 49: /* value: VALUE_BOOL  */
#line 381 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<BoolLit>((yyvsp[0].sv_bool));
    }
#line 2160 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 50: /* condition: col op expr  */
#line 388 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_col), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_node)));
    }
#line 2168 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 51: /* condition: agg_func op expr  */
#line 392 "/home/code/db2025/src/parser/yacc.y"
    {
        // 创建一个临时的 Col 对象来存储聚合函数信息
        // 这里需要修改 BinaryExpr 结构以支持聚合函数作为左操作数
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_agg_func), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_node)));
    }
#line 2178 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 52: /* optWhereClause: %empty  */
#line 400 "/home/code/db2025/src/parser/yacc.y"
                      { (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>(); }
#line 2184 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 53: /* optWhereClause: WHERE whereClause  */
#line 402 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds) = (yyvsp[0].sv_conds);
    }
#line 2192 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 54: /* whereClause: condition  */
#line 409 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2200 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 55: /* whereClause: whereClause AND condition  */
#line 413 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2208 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 56: /* col: tbName '.' colName  */
#line 420 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2216 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 57: /* col: colName  */
#line 424 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[0].sv_str));
    }
#line 2224 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 58: /* col: tbName '.' colName AS IDENTIFIER  */
#line 428 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-4].sv_str), (yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2232 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 59: /* col: colName AS IDENTIFIER  */
#line 432 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2240 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 60: /* colList: col  */
#line 439 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)};
    }
#line 2248 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 61: /* colList: colList ',' col  */
#line 443 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols).push_back((yyvsp[0].sv_col));
    }
#line 2256 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 62: /* op: '='  */
#line 450 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_EQ;
    }
#line 2264 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 63: /* op: '<'  */
#line 454 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LT;
    }
#line 2272 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 64: /* op: '>'  */
#line 458 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GT;
    }
#line 2280 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 65: /* op: NEQ  */
#line 462 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_NE;
    }
#line 2288 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 66: /* op: LEQ  */
#line 466 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LE;
    }
#line 2296 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 67: /* op: GEQ  */
#line 470 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GE;
    }
#line 2304 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 68: /* expr: value  */
#line 477 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_val);
    }
#line 2312 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 69: /* expr: col  */
#line 481 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_col);
    }
#line 2320 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 70: /* expr: agg_func  */
#line 485 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_agg_func);
    }
#line 2328 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 71: /* expr: '(' dml ')'  */
#line 489 "/home/code/db2025/src/parser/yacc.y"
    {
        // 子查询：将SELECT语句包装为表达式
        (yyval.sv_node) = (yyvsp[-1].sv_node);
    }
#line 2337 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 72: /* expr: expr '+' expr  */
#line 494 "/home/code/db2025/src/parser/yacc.y"
    {
        // BinaryExpr是TreeNode的子类
        (yyval.sv_node) = std::make_shared<BinaryExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_node)), SV_OP_ADD, std::static_pointer_cast<Expr>((yyvsp[0].sv_node)));
    }
#line 2346 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 73: /* expr: expr '-' expr  */
#line 499 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<BinaryExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_node)), SV_OP_SUB, std::static_pointer_cast<Expr>((yyvsp[0].sv_node)));
    }
#line 2354 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 74: /* expr: expr '*' expr  */
#line 503 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<BinaryExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_node)), SV_OP_MUL, std::static_pointer_cast<Expr>((yyvsp[0].sv_node)));
    }
#line 2362 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 75: /* expr: expr '/' expr  */
#line 507 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<BinaryExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_node)), SV_OP_DIV, std::static_pointer_cast<Expr>((yyvsp[0].sv_node)));
    }
#line 2370 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 76: /* expr: '-' expr  */
#line 511 "/home/code/db2025/src/parser/yacc.y"
    {
        // 负号表达式，创建一个0减去expr的表达式
        auto zero = std::make_shared<IntLit>(0);
        (yyval.sv_node) = std::make_shared<BinaryExpr>(zero, SV_OP_SUB, std::static_pointer_cast<Expr>((yyvsp[0].sv_node)));
    }
#line 2380 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 77: /* expr: '(' expr ')'  */
#line 517 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[-1].sv_node);
    }
#line 2388 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 78: /* setClauses: setClause  */
#line 524 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses) = std::vector<std::shared_ptr<SetClause>>{(yyvsp[0].sv_set_clause)};
    }
#line 2396 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 79: /* setClauses: setClauses ',' setClause  */
#line 528 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses).push_back((yyvsp[0].sv_set_clause));
    }
#line 2404 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 80: /* setClause: colName '=' expr  */
#line 535 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>((yyvsp[-2].sv_str), (yyvsp[0].sv_node));
    }
#line 2412 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 81: /* selector: '*'  */
#line 542 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>();
    }
#line 2420 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 83: /* table_ref_list: table_ref  */
#line 568 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_table_refs) = std::vector<std::shared_ptr<ast::TableRef>>{(yyvsp[0].sv_table_ref)};
    }
#line 2428 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 84: /* table_ref_list: table_ref_list ',' table_ref  */
#line 572 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_table_refs).push_back((yyvsp[0].sv_table_ref));
    }
#line 2436 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 85: /* join_expr_list: %empty  */
#line 579 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexprs) = std::vector<std::shared_ptr<JoinExpr>>();
    }
#line 2444 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 86: /* join_expr_list: join_expr_list join_clause  */
#line 583 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexprs) = (yyvsp[-1].sv_joinexprs);
        (yyval.sv_joinexprs).push_back((yyvsp[0].sv_joinexpr));
    }
#line 2453 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 87: /* join_clause: JOIN table_ref ON whereClause  */
#line 591 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexpr) = std::make_shared<JoinExpr>("", (yyvsp[-2].sv_table_ref)->tab_name, (yyvsp[0].sv_conds), ::INNER_JOIN, "", (yyvsp[-2].sv_table_ref)->alias);
    }
#line 2461 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 88: /* join_clause: JOIN table_ref WHERE whereClause  */
#line 595 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexpr) = std::make_shared<JoinExpr>("", (yyvsp[-2].sv_table_ref)->tab_name, (yyvsp[0].sv_conds), ::INNER_JOIN, "", (yyvsp[-2].sv_table_ref)->alias);
    }
#line 2469 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 89: /* join_clause: SEMI JOIN table_ref ON whereClause  */
#line 599 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexpr) = std::make_shared<JoinExpr>("", (yyvsp[-2].sv_table_ref)->tab_name, (yyvsp[0].sv_conds), ::SEMI_JOIN, "", (yyvsp[-2].sv_table_ref)->alias);
    }
#line 2477 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 90: /* join_clause: SEMI JOIN table_ref WHERE whereClause  */
#line 603 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexpr) = std::make_shared<JoinExpr>("", (yyvsp[-2].sv_table_ref)->tab_name, (yyvsp[0].sv_conds), ::SEMI_JOIN, "", (yyvsp[-2].sv_table_ref)->alias);
    }
#line 2485 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 91: /* opt_order_clause: ORDER BY order_clause  */
#line 610 "/home/code/db2025/src/parser/yacc.y"
    { 
        (yyval.sv_orderby) = (yyvsp[0].sv_orderby); 
    }
#line 2493 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 92: /* opt_order_clause: %empty  */
#line 613 "/home/code/db2025/src/parser/yacc.y"
                      { (yyval.sv_orderby) = nullptr; }
#line 2499 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 93: /* order_clause: order_col_list  */
#line 618 "/home/code/db2025/src/parser/yacc.y"
    { 
        // 从order_col_list中提取列和方向
        std::vector<std::shared_ptr<Col>> cols;
        std::vector<OrderByDir> dirs;
        for (const auto& col_with_dir : (yyvsp[0].sv_order_col_list)) {
            cols.push_back(col_with_dir.first);
            dirs.push_back(col_with_dir.second);
        }
        (yyval.sv_orderby) = std::make_shared<OrderBy>(cols, dirs);
    }
#line 2514 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 94: /* order_col_list: order_col_with_dir  */
#line 632 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_list) = std::vector<std::pair<std::shared_ptr<Col>, OrderByDir>>{(yyvsp[0].sv_order_col_with_dir)};
    }
#line 2522 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 95: /* order_col_list: order_col_list ',' order_col_with_dir  */
#line 636 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_list).push_back((yyvsp[0].sv_order_col_with_dir));
    }
#line 2530 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 96: /* order_col_with_dir: col opt_asc_desc  */
#line 643 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_with_dir) = std::make_pair((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2538 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 97: /* opt_asc_desc: ASC  */
#line 649 "/home/code/db2025/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_ASC;     }
#line 2544 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 98: /* opt_asc_desc: DESC  */
#line 650 "/home/code/db2025/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_DESC;    }
#line 2550 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 99: /* opt_asc_desc: %empty  */
#line 651 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_orderby_dir) = OrderBy_DEFAULT; }
#line 2556 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 100: /* set_knob_type: ENABLE_NESTLOOP  */
#line 655 "/home/code/db2025/src/parser/yacc.y"
                    { (yyval.sv_setKnobType) = EnableNestLoop; }
#line 2562 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 101: /* set_knob_type: ENABLE_SORTMERGE  */
#line 656 "/home/code/db2025/src/parser/yacc.y"
                         { (yyval.sv_setKnobType) = EnableSortMerge; }
#line 2568 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 102: /* agg_func_list: agg_func  */
#line 662 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_funcs) = std::vector<std::shared_ptr<AggFunc>>{(yyvsp[0].sv_agg_func)};
    }
#line 2576 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 103: /* agg_func_list: agg_func_list ',' agg_func  */
#line 666 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_funcs).push_back((yyvsp[0].sv_agg_func));
    }
#line 2584 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 104: /* agg_func: COUNT '(' '*' ')'  */
#line 673 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, nullptr, "");
    }
#line 2592 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 105: /* agg_func: COUNT '(' '*' ')' AS IDENTIFIER  */
#line 677 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, nullptr, (yyvsp[0].sv_str));
    }
#line 2600 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 106: /* agg_func: COUNT '(' col ')'  */
#line 681 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, (yyvsp[-1].sv_col), "");
    }
#line 2608 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 107: /* agg_func: COUNT '(' col ')' AS IDENTIFIER  */
#line 685 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, (yyvsp[-3].sv_col), (yyvsp[0].sv_str));
    }
#line 2616 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 108: /* agg_func: agg_func_type '(' col ')'  */
#line 689 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>((yyvsp[-3].sv_agg_func_type), (yyvsp[-1].sv_col), "");
    }
#line 2624 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 109: /* agg_func: agg_func_type '(' col ')' AS IDENTIFIER  */
#line 693 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>((yyvsp[-5].sv_agg_func_type), (yyvsp[-3].sv_col), (yyvsp[0].sv_str));
    }
#line 2632 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 110: /* agg_func_type: MAX  */
#line 699 "/home/code/db2025/src/parser/yacc.y"
        { (yyval.sv_agg_func_type) = AGG_MAX; }
#line 2638 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 111: /* agg_func_type: MIN  */
#line 700 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_MIN; }
#line 2644 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 112: /* agg_func_type: SUM  */
#line 701 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_SUM; }
#line 2650 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 113: /* agg_func_type: AVG  */
#line 702 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_AVG; }
#line 2656 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 114: /* opt_group_by_clause: %empty  */
#line 706 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_groupby) = nullptr; }
#line 2662 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 115: /* opt_group_by_clause: GROUP BY colList  */
#line 708 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_groupby) = std::make_shared<GroupBy>((yyvsp[0].sv_cols));
    }
#line 2670 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 116: /* opt_having_clause: %empty  */
#line 714 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_having) = nullptr; }
#line 2676 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 117: /* opt_having_clause: HAVING whereClause  */
#line 716 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_having) = std::make_shared<Having>((yyvsp[0].sv_conds));
    }
#line 2684 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 118: /* opt_limit_clause: %empty  */
#line 722 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_limit) = nullptr; }
#line 2690 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 119: /* opt_limit_clause: LIMIT VALUE_INT  */
#line 724 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_limit) = std::make_shared<Limit>((yyvsp[0].sv_int));
    }
#line 2698 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 120: /* mixed_selector: col ',' agg_func  */
#line 732 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[-2].sv_col)}, std::vector<std::shared_ptr<AggFunc>>{(yyvsp[0].sv_agg_func)});
    }
#line 2706 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 121: /* mixed_selector: agg_func ',' col  */
#line 736 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)}, std::vector<std::shared_ptr<AggFunc>>{(yyvsp[-2].sv_agg_func)});
    }
#line 2714 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 122: /* mixed_selector: col ',' col  */
#line 740 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[-2].sv_col), (yyvsp[0].sv_col)}, std::vector<std::shared_ptr<AggFunc>>());
    }
#line 2722 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 123: /* mixed_selector: agg_func ',' agg_func  */
#line 744 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>(), std::vector<std::shared_ptr<AggFunc>>{(yyvsp[-2].sv_agg_func), (yyvsp[0].sv_agg_func)});
    }
#line 2730 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 124: /* mixed_selector: mixed_selector ',' col  */
#line 748 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector).first.push_back((yyvsp[0].sv_col));
    }
#line 2738 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 125: /* mixed_selector: mixed_selector ',' agg_func  */
#line 752 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector).second.push_back((yyvsp[0].sv_agg_func));
    }
#line 2746 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 129: /* table_ref: IDENTIFIER  */
#line 764 "/home/code/db2025/src/parser/yacc.y"
                 { (yyval.sv_table_ref) = ast::new_table_ref((yyvsp[0].sv_str), nullptr); }
#line 2752 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 130: /* table_ref: IDENTIFIER IDENTIFIER  */
#line 765 "/home/code/db2025/src/parser/yacc.y"
                            { (yyval.sv_table_ref) = ast::new_table_ref((yyvsp[-1].sv_str), &(yyvsp[0].sv_str)); }
#line 2758 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 131: /* table_ref: IDENTIFIER AS IDENTIFIER  */
#line 766 "/home/code/db2025/src/parser/yacc.y"
                               { (yyval.sv_table_ref) = ast::new_table_ref((yyvsp[-2].sv_str), &(yyvsp[0].sv_str)); }
#line 2764 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;


#line 2768 "/home/code/db2025/src/parser/yacc.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 769 "/home/code/db2025/src/parser/yacc.y"

