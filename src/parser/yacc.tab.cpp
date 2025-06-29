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

#include "ast.h"
#include "yacc.tab.h"
#include <iostream>
#include <memory>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

using namespace ast;

#line 86 "/home/code/db2025/src/parser/yacc.tab.cpp"

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
  YYSYMBOL_IDENTIFIER = 50,                /* IDENTIFIER  */
  YYSYMBOL_VALUE_STRING = 51,              /* VALUE_STRING  */
  YYSYMBOL_VALUE_INT = 52,                 /* VALUE_INT  */
  YYSYMBOL_VALUE_FLOAT = 53,               /* VALUE_FLOAT  */
  YYSYMBOL_VALUE_BOOL = 54,                /* VALUE_BOOL  */
  YYSYMBOL_55_ = 55,                       /* ';'  */
  YYSYMBOL_56_ = 56,                       /* '('  */
  YYSYMBOL_57_ = 57,                       /* ')'  */
  YYSYMBOL_58_ = 58,                       /* '='  */
  YYSYMBOL_59_ = 59,                       /* ','  */
  YYSYMBOL_60_ = 60,                       /* '.'  */
  YYSYMBOL_61_ = 61,                       /* '<'  */
  YYSYMBOL_62_ = 62,                       /* '>'  */
  YYSYMBOL_63_ = 63,                       /* '*'  */
  YYSYMBOL_YYACCEPT = 64,                  /* $accept  */
  YYSYMBOL_start = 65,                     /* start  */
  YYSYMBOL_stmt = 66,                      /* stmt  */
  YYSYMBOL_txnStmt = 67,                   /* txnStmt  */
  YYSYMBOL_dbStmt = 68,                    /* dbStmt  */
  YYSYMBOL_setStmt = 69,                   /* setStmt  */
  YYSYMBOL_ddl = 70,                       /* ddl  */
  YYSYMBOL_dml = 71,                       /* dml  */
  YYSYMBOL_fieldList = 72,                 /* fieldList  */
  YYSYMBOL_colNameList = 73,               /* colNameList  */
  YYSYMBOL_field = 74,                     /* field  */
  YYSYMBOL_type = 75,                      /* type  */
  YYSYMBOL_valueList = 76,                 /* valueList  */
  YYSYMBOL_value = 77,                     /* value  */
  YYSYMBOL_condition = 78,                 /* condition  */
  YYSYMBOL_optWhereClause = 79,            /* optWhereClause  */
  YYSYMBOL_whereClause = 80,               /* whereClause  */
  YYSYMBOL_col = 81,                       /* col  */
  YYSYMBOL_colList = 82,                   /* colList  */
  YYSYMBOL_op = 83,                        /* op  */
  YYSYMBOL_expr = 84,                      /* expr  */
  YYSYMBOL_setClauses = 85,                /* setClauses  */
  YYSYMBOL_setClause = 86,                 /* setClause  */
  YYSYMBOL_selector = 87,                  /* selector  */
  YYSYMBOL_tableList = 88,                 /* tableList  */
  YYSYMBOL_tableListWithJoin = 89,         /* tableListWithJoin  */
  YYSYMBOL_join_expr_list = 90,            /* join_expr_list  */
  YYSYMBOL_join_clause = 91,               /* join_clause  */
  YYSYMBOL_opt_order_clause = 92,          /* opt_order_clause  */
  YYSYMBOL_order_clause = 93,              /* order_clause  */
  YYSYMBOL_order_col_list = 94,            /* order_col_list  */
  YYSYMBOL_order_col_with_dir = 95,        /* order_col_with_dir  */
  YYSYMBOL_opt_asc_desc = 96,              /* opt_asc_desc  */
  YYSYMBOL_set_knob_type = 97,             /* set_knob_type  */
  YYSYMBOL_agg_func_list = 98,             /* agg_func_list  */
  YYSYMBOL_agg_func = 99,                  /* agg_func  */
  YYSYMBOL_agg_func_type = 100,            /* agg_func_type  */
  YYSYMBOL_opt_group_by_clause = 101,      /* opt_group_by_clause  */
  YYSYMBOL_opt_having_clause = 102,        /* opt_having_clause  */
  YYSYMBOL_opt_limit_clause = 103,         /* opt_limit_clause  */
  YYSYMBOL_mixed_selector = 104,           /* mixed_selector  */
  YYSYMBOL_tbName = 105,                   /* tbName  */
  YYSYMBOL_colName = 106,                  /* colName  */
  YYSYMBOL_indexName = 107                 /* indexName  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYFINAL  54
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   227

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  119
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  232

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   309


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
      56,    57,    63,     2,    59,     2,    60,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    55,
      61,    58,    62,     2,     2,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   130,   130,   135,   140,   145,   153,   157,   161,   165,
     169,   176,   180,   184,   188,   195,   199,   203,   207,   211,
     215,   219,   226,   233,   237,   241,   245,   249,   253,   260,
     264,   268,   272,   276,   280,   287,   291,   298,   302,   309,
     316,   320,   324,   331,   335,   342,   346,   350,   354,   361,
     365,   374,   375,   382,   386,   393,   397,   401,   407,   416,
     420,   427,   431,   435,   439,   443,   447,   454,   458,   462,
     469,   473,   480,   487,   491,   495,   499,   506,   514,   517,
     525,   529,   536,   540,   544,   558,   562,   569,   576,   577,
     578,   582,   583,   588,   592,   599,   603,   607,   611,   615,
     619,   626,   627,   628,   629,   633,   634,   641,   642,   649,
     650,   658,   662,   666,   670,   674,   678,   684,   686,   688
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
  "T_EOF", "IDENTIFIER", "VALUE_STRING", "VALUE_INT", "VALUE_FLOAT",
  "VALUE_BOOL", "';'", "'('", "')'", "'='", "','", "'.'", "'<'", "'>'",
  "'*'", "$accept", "start", "stmt", "txnStmt", "dbStmt", "setStmt", "ddl",
  "dml", "fieldList", "colNameList", "field", "type", "valueList", "value",
  "condition", "optWhereClause", "whereClause", "col", "colList", "op",
  "expr", "setClauses", "setClause", "selector", "tableList",
  "tableListWithJoin", "join_expr_list", "join_clause", "opt_order_clause",
  "order_clause", "order_col_list", "order_col_with_dir", "opt_asc_desc",
  "set_knob_type", "agg_func_list", "agg_func", "agg_func_type",
  "opt_group_by_clause", "opt_having_clause", "opt_limit_clause",
  "mixed_selector", "tbName", "colName", "indexName", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-153)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-120)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      78,     6,    11,    12,   -28,    54,    55,   -28,    10,    65,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,    93,    23,  -153,
    -153,  -153,  -153,  -153,  -153,    -1,   -28,    42,   -28,    42,
    -153,  -153,   -28,   -28,   113,  -153,  -153,    73,    81,  -153,
    -153,  -153,  -153,   100,  -153,   108,   109,   136,    -6,   110,
     114,    -4,   111,   130,  -153,  -153,   -28,  -153,   117,   129,
     119,   131,  -153,   121,   133,   168,   165,   132,   127,   -34,
      76,   134,   -28,   -28,    87,    76,   134,   -28,    76,   132,
     135,  -153,   132,   132,   -28,   132,   -28,   137,    76,  -153,
    -153,    -7,  -153,   125,  -153,   138,   139,  -153,  -153,  -153,
     128,  -153,  -153,  -153,  -153,  -153,  -153,   140,  -153,  -153,
    -153,   144,  -153,   -20,  -153,    38,    88,  -153,   142,    89,
    -153,    90,  -153,   166,    92,    92,   132,  -153,    90,   148,
     149,   -28,    26,    26,   152,    26,   150,  -153,   132,  -153,
     143,  -153,  -153,  -153,   132,   132,  -153,  -153,  -153,  -153,
    -153,    98,  -153,    76,  -153,  -153,  -153,  -153,  -153,  -153,
      82,    82,  -153,  -153,   151,   153,  -153,   -28,   178,   167,
    -153,   167,   154,   167,  -153,  -153,   155,  -153,    99,  -153,
      90,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,   160,
     -28,   169,   170,   170,  -153,   170,   157,  -153,  -153,    76,
     164,   134,    76,   172,   172,   172,  -153,  -153,    76,  -153,
     109,   166,   173,   171,   171,   171,  -153,   134,   159,  -153,
    -153,  -153,     7,  -153,   161,  -153,  -153,  -153,  -153,  -153,
     134,  -153
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     3,    11,    12,    13,    14,     5,     0,     0,     9,
       6,    10,     7,     8,    15,     0,     0,     0,     0,     0,
     117,    18,     0,     0,     0,    91,    92,     0,     0,   101,
     102,   103,   104,   118,    73,    59,    74,     0,     0,    93,
       0,     0,     0,    56,     1,     2,     0,    21,     0,   117,
       0,     0,    17,     0,     0,     0,    51,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,     0,     0,     0,     0,     0,     0,     0,    30,
     118,    51,    70,     0,    22,     0,     0,   113,   111,    60,
      77,    78,    75,    78,    94,   112,   114,     0,    78,   115,
     116,    55,    58,     0,    35,     0,     0,    37,     0,     0,
      20,     0,    53,    52,     0,     0,     0,    31,     0,    95,
      97,     0,    51,    51,    99,    51,     0,    16,     0,    40,
       0,    42,    39,    26,     0,     0,    27,    47,    45,    46,
      48,     0,    43,     0,    65,    64,    66,    61,    62,    63,
       0,     0,    71,    72,     0,     0,    76,     0,     0,   105,
      79,   105,     0,   105,    57,    36,     0,    38,     0,    29,
       0,    54,    67,    68,    49,    69,    50,    96,    98,     0,
       0,     0,   107,   107,   100,   107,     0,    19,    44,     0,
       0,     0,     0,    83,    83,    83,    41,    80,     0,    59,
     106,   108,     0,   109,   109,   109,    81,     0,     0,    32,
      33,    34,    90,    82,    84,    85,   110,    89,    88,    87,
       0,    86
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,   -82,
      80,  -153,  -153,  -115,  -152,   -87,    14,    -9,    18,    97,
      62,  -153,   101,  -153,  -153,   -33,   -89,  -153,  -131,  -153,
    -153,    -5,  -153,  -153,  -153,     2,  -153,   -12,   -29,  -126,
    -153,    -2,   -47,   195
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    17,    18,    19,    20,    21,    22,    23,   113,   116,
     114,   142,   151,   182,   122,    89,   123,   124,    46,   160,
     184,    91,    92,    47,   100,   101,   132,   170,   213,   223,
     224,   225,   229,    37,    48,   125,    50,   192,   203,   219,
      51,    52,    53,    61
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      45,   181,    31,   119,   127,    34,   152,    73,    88,    77,
      24,    49,    56,   163,   133,   227,    43,    26,    28,   135,
      93,   228,    30,    57,    58,    60,    62,    63,    25,    95,
      65,    66,   111,    27,    29,   115,   117,   137,   117,   138,
     103,    88,    35,    36,   108,   169,   171,   207,   173,    30,
     167,   168,   126,    74,    81,    78,   216,   139,   140,   141,
      96,    97,    99,   178,    32,   198,   105,   107,    33,   109,
     102,   102,    98,   214,   215,   102,   104,   106,    55,    93,
     110,     1,   118,     2,   120,     3,     4,     5,   220,   221,
       6,   115,    59,    54,     7,     8,     9,   177,   117,    38,
      39,    40,    41,    42,    10,    11,    12,    13,    14,    15,
      38,    39,    40,    41,    42,    43,    38,    39,    40,    41,
      42,    38,    39,    40,    41,    42,    43,    16,    44,   166,
      67,    68,    43,   147,   148,   149,   150,    69,   154,   155,
     156,   147,   148,   149,   150,   143,   146,   144,   144,    72,
     157,   183,   183,   158,   159,   179,   197,   180,   144,   193,
    -117,   195,   185,   185,   204,   189,   205,    70,    71,    75,
      76,    79,    80,    82,  -119,    83,    84,    85,    86,    87,
      88,    94,    90,   128,    43,   112,   136,   131,   200,   153,
     164,   165,   209,   121,   172,   129,   130,   134,   145,   176,
     174,   187,   190,   188,   194,   199,   191,   196,   222,   208,
     202,   226,   218,   201,   206,   212,   211,   217,   175,   210,
     230,   222,   161,   186,    64,   231,     0,   162
};

static const yytype_int16 yycheck[] =
{
       9,   153,     4,    85,    91,     7,   121,    13,    15,    13,
       4,     9,    13,   128,   103,     8,    50,     6,     6,   108,
      67,    14,    50,    25,    26,    27,    28,    29,    22,    63,
      32,    33,    79,    22,    22,    82,    83,    57,    85,    59,
      73,    15,    32,    33,    77,   132,   133,   199,   135,    50,
      24,    25,    59,    59,    56,    59,   208,    19,    20,    21,
      69,    70,    71,   145,    10,   180,    75,    76,    13,    78,
      72,    73,    70,   204,   205,    77,    74,    75,    55,   126,
      78,     3,    84,     5,    86,     7,     8,     9,   214,   215,
      12,   138,    50,     0,    16,    17,    18,   144,   145,    34,
      35,    36,    37,    38,    26,    27,    28,    29,    30,    31,
      34,    35,    36,    37,    38,    50,    34,    35,    36,    37,
      38,    34,    35,    36,    37,    38,    50,    49,    63,   131,
      17,    58,    50,    51,    52,    53,    54,    56,    46,    47,
      48,    51,    52,    53,    54,    57,    57,    59,    59,    13,
      58,   160,   161,    61,    62,    57,    57,    59,    59,   171,
      60,   173,   160,   161,   193,   167,   195,    59,    59,    59,
      56,    60,    42,    56,    45,    56,    45,    56,    45,    11,
      15,    54,    50,    58,    50,    50,    42,    59,   190,    23,
      42,    42,   201,    56,    42,    57,    57,    57,    56,    56,
      50,    50,    24,    50,    50,    45,    39,    52,   217,    45,
      40,    52,    41,    44,    57,    43,   202,    44,   138,   201,
      59,   230,   125,   161,    29,   230,    -1,   126
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     7,     8,     9,    12,    16,    17,    18,
      26,    27,    28,    29,    30,    31,    49,    65,    66,    67,
      68,    69,    70,    71,     4,    22,     6,    22,     6,    22,
      50,   105,    10,    13,   105,    32,    33,    97,    34,    35,
      36,    37,    38,    50,    63,    81,    82,    87,    98,    99,
     100,   104,   105,   106,     0,    55,    13,   105,   105,    50,
     105,   107,   105,   105,   107,   105,   105,    17,    58,    56,
      59,    59,    13,    13,    59,    59,    56,    13,    59,    60,
      42,   105,    56,    56,    45,    56,    45,    11,    15,    79,
      50,    85,    86,   106,    54,    63,    81,    81,    99,    81,
      88,    89,   105,    89,    99,    81,    99,    81,    89,    81,
      99,   106,    50,    72,    74,   106,    73,   106,   105,    73,
     105,    56,    78,    80,    81,    99,    59,    79,    58,    57,
      57,    59,    90,    90,    57,    90,    42,    57,    59,    19,
      20,    21,    75,    57,    59,    56,    57,    51,    52,    53,
      54,    76,    77,    23,    46,    47,    48,    58,    61,    62,
      83,    83,    86,    77,    42,    42,   105,    24,    25,    79,
      91,    79,    42,    79,    50,    74,    56,   106,    73,    57,
      59,    78,    77,    81,    84,    99,    84,    50,    50,   105,
      24,    39,   101,   101,    50,   101,    52,    57,    77,    45,
     105,    44,    40,   102,   102,   102,    57,    78,    45,    81,
      82,    80,    43,    92,    92,    92,    78,    44,    41,   103,
     103,   103,    81,    93,    94,    95,    52,     8,    14,    96,
      59,    95
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    64,    65,    65,    65,    65,    66,    66,    66,    66,
      66,    67,    67,    67,    67,    68,    68,    68,    68,    68,
      68,    68,    69,    70,    70,    70,    70,    70,    70,    71,
      71,    71,    71,    71,    71,    72,    72,    73,    73,    74,
      75,    75,    75,    76,    76,    77,    77,    77,    77,    78,
      78,    79,    79,    80,    80,    81,    81,    81,    81,    82,
      82,    83,    83,    83,    83,    83,    83,    84,    84,    84,
      85,    85,    86,    87,    87,    88,    88,    89,    90,    90,
      91,    91,    92,    92,    93,    94,    94,    95,    96,    96,
      96,    97,    97,    98,    98,    99,    99,    99,    99,    99,
      99,   100,   100,   100,   100,   101,   101,   102,   102,   103,
     103,   104,   104,   104,   104,   104,   104,   105,   106,   107
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     6,     3,     2,     8,
       5,     3,     4,     6,     3,     2,     6,     6,     4,     7,
       4,     5,    10,    10,    10,     1,     3,     1,     3,     2,
       1,     4,     1,     1,     3,     1,     1,     1,     1,     3,
       3,     0,     2,     1,     3,     3,     1,     5,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     3,     1,     0,     2,
       4,     5,     3,     0,     1,     1,     3,     2,     1,     1,
       0,     1,     1,     1,     3,     4,     6,     4,     6,     4,
       6,     1,     1,     1,     1,     0,     3,     0,     2,     0,
       2,     3,     3,     3,     3,     3,     3,     1,     1,     1
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
#line 131 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = (yyvsp[-1].sv_node);
        YYACCEPT;
    }
#line 1739 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 3: /* start: HELP  */
#line 136 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
#line 1748 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 4: /* start: EXIT  */
#line 141 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1757 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 5: /* start: T_EOF  */
#line 146 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1766 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 6: /* stmt: dbStmt  */
#line 154 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1774 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 7: /* stmt: ddl  */
#line 158 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1782 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 8: /* stmt: dml  */
#line 162 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1790 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 9: /* stmt: txnStmt  */
#line 166 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1798 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 10: /* stmt: setStmt  */
#line 170 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1806 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 11: /* txnStmt: TXN_BEGIN  */
#line 177 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnBegin>();
    }
#line 1814 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 12: /* txnStmt: TXN_COMMIT  */
#line 181 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnCommit>();
    }
#line 1822 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 13: /* txnStmt: TXN_ABORT  */
#line 185 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnAbort>();
    }
#line 1830 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 14: /* txnStmt: TXN_ROLLBACK  */
#line 189 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnRollback>();
    }
#line 1838 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 15: /* dbStmt: SHOW TABLES  */
#line 196 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowTables>();
    }
#line 1846 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 16: /* dbStmt: CREATE TABLE tbName '(' fieldList ')'  */
#line 200 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1854 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 17: /* dbStmt: DROP TABLE tbName  */
#line 204 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1862 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 18: /* dbStmt: DESC tbName  */
#line 208 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1870 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 19: /* dbStmt: CREATE INDEX indexName ON tbName '(' colNameList ')'  */
#line 212 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1878 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 20: /* dbStmt: DROP INDEX indexName ON tbName  */
#line 216 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[0].sv_str), std::vector<std::string>{});
    }
#line 1886 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 21: /* dbStmt: SHOW INDEX tbName  */
#line 220 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 1894 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 22: /* setStmt: SET set_knob_type '=' VALUE_BOOL  */
#line 227 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetStmt>((yyvsp[-2].sv_setKnobType), (yyvsp[0].sv_bool));
    }
#line 1902 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 23: /* ddl: CREATE TABLE tbName '(' fieldList ')'  */
#line 234 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1910 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 24: /* ddl: DROP TABLE tbName  */
#line 238 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1918 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 25: /* ddl: DESC tbName  */
#line 242 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1926 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 26: /* ddl: CREATE INDEX tbName '(' colNameList ')'  */
#line 246 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1934 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 27: /* ddl: DROP INDEX tbName '(' colNameList ')'  */
#line 250 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1942 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 28: /* ddl: SHOW INDEX FROM tbName  */
#line 254 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 1950 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 29: /* dml: INSERT INTO tbName VALUES '(' valueList ')'  */
#line 261 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<InsertStmt>((yyvsp[-4].sv_str), (yyvsp[-1].sv_vals));
    }
#line 1958 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 30: /* dml: DELETE FROM tbName optWhereClause  */
#line 265 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DeleteStmt>((yyvsp[-1].sv_str), (yyvsp[0].sv_conds));
    }
#line 1966 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 31: /* dml: UPDATE tbName SET setClauses optWhereClause  */
#line 269 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<UpdateStmt>((yyvsp[-3].sv_str), (yyvsp[-1].sv_set_clauses), (yyvsp[0].sv_conds));
    }
#line 1974 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 32: /* dml: SELECT selector FROM tableListWithJoin join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 273 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>((yyvsp[-8].sv_cols), std::vector<std::shared_ptr<AggFunc>>(), (yyvsp[-6].sv_strs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit), (yyvsp[-5].sv_joinexprs));
    }
#line 1982 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 33: /* dml: SELECT agg_func_list FROM tableListWithJoin join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 277 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>(std::vector<std::shared_ptr<Col>>(), (yyvsp[-8].sv_agg_funcs), (yyvsp[-6].sv_strs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit), (yyvsp[-5].sv_joinexprs));
    }
#line 1990 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 34: /* dml: SELECT mixed_selector FROM tableListWithJoin join_expr_list optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 281 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>((yyvsp[-8].sv_mixed_selector).first, (yyvsp[-8].sv_mixed_selector).second, (yyvsp[-6].sv_strs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit), (yyvsp[-5].sv_joinexprs));
    }
#line 1998 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 35: /* fieldList: field  */
#line 288 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_fields) = std::vector<std::shared_ptr<Field>>{(yyvsp[0].sv_field)};
    }
#line 2006 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 36: /* fieldList: fieldList ',' field  */
#line 292 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_fields).push_back((yyvsp[0].sv_field));
    }
#line 2014 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 37: /* colNameList: colName  */
#line 299 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 2022 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 38: /* colNameList: colNameList ',' colName  */
#line 303 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 2030 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 39: /* field: colName type  */
#line 310 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_field) = std::make_shared<ColDef>((yyvsp[-1].sv_str), (yyvsp[0].sv_type_len));
    }
#line 2038 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 40: /* type: INT  */
#line 317 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
#line 2046 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 41: /* type: CHAR '(' VALUE_INT ')'  */
#line 321 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_STRING, (yyvsp[-1].sv_int));
    }
#line 2054 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 42: /* type: FLOAT  */
#line 325 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
#line 2062 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 43: /* valueList: value  */
#line 332 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_vals) = std::vector<std::shared_ptr<Value>>{(yyvsp[0].sv_val)};
    }
#line 2070 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 44: /* valueList: valueList ',' value  */
#line 336 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_vals).push_back((yyvsp[0].sv_val));
    }
#line 2078 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 45: /* value: VALUE_INT  */
#line 343 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>((yyvsp[0].sv_int));
    }
#line 2086 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 46: /* value: VALUE_FLOAT  */
#line 347 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>((yyvsp[0].sv_float));
    }
#line 2094 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 47: /* value: VALUE_STRING  */
#line 351 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<StringLit>((yyvsp[0].sv_str));
    }
#line 2102 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 48: /* value: VALUE_BOOL  */
#line 355 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<BoolLit>((yyvsp[0].sv_bool));
    }
#line 2110 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 49: /* condition: col op expr  */
#line 362 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_col), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2118 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 50: /* condition: agg_func op expr  */
#line 366 "/home/code/db2025/src/parser/yacc.y"
    {
        // 创建一个临时的 Col 对象来存储聚合函数信息
        // 这里需要修改 BinaryExpr 结构以支持聚合函数作为左操作数
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_agg_func), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2128 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 51: /* optWhereClause: %empty  */
#line 374 "/home/code/db2025/src/parser/yacc.y"
                      { (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>(); }
#line 2134 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 52: /* optWhereClause: WHERE whereClause  */
#line 376 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds) = (yyvsp[0].sv_conds);
    }
#line 2142 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 53: /* whereClause: condition  */
#line 383 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2150 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 54: /* whereClause: whereClause AND condition  */
#line 387 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2158 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 55: /* col: tbName '.' colName  */
#line 394 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2166 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 56: /* col: colName  */
#line 398 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[0].sv_str));
    }
#line 2174 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 57: /* col: tbName '.' colName AS IDENTIFIER  */
#line 402 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-4].sv_str), (yyvsp[-2].sv_str));
        // 注意：这里我们暂时忽略别名，因为Col结构中没有alias字段
        // 在实际实现中，需要扩展Col结构以支持别名
    }
#line 2184 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 58: /* col: colName AS IDENTIFIER  */
#line 408 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[-2].sv_str));
        // 注意：这里我们暂时忽略别名，因为Col结构中没有alias字段
        // 在实际实现中，需要扩展Col结构以支持别名
    }
#line 2194 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 59: /* colList: col  */
#line 417 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)};
    }
#line 2202 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 60: /* colList: colList ',' col  */
#line 421 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols).push_back((yyvsp[0].sv_col));
    }
#line 2210 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 61: /* op: '='  */
#line 428 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_EQ;
    }
#line 2218 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 62: /* op: '<'  */
#line 432 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LT;
    }
#line 2226 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 63: /* op: '>'  */
#line 436 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GT;
    }
#line 2234 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 64: /* op: NEQ  */
#line 440 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_NE;
    }
#line 2242 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 65: /* op: LEQ  */
#line 444 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LE;
    }
#line 2250 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 66: /* op: GEQ  */
#line 448 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GE;
    }
#line 2258 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 67: /* expr: value  */
#line 455 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_val));
    }
#line 2266 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 68: /* expr: col  */
#line 459 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_col));
    }
#line 2274 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 69: /* expr: agg_func  */
#line 463 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_func));
    }
#line 2282 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 70: /* setClauses: setClause  */
#line 470 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses) = std::vector<std::shared_ptr<SetClause>>{(yyvsp[0].sv_set_clause)};
    }
#line 2290 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 71: /* setClauses: setClauses ',' setClause  */
#line 474 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses).push_back((yyvsp[0].sv_set_clause));
    }
#line 2298 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 72: /* setClause: colName '=' value  */
#line 481 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>((yyvsp[-2].sv_str), (yyvsp[0].sv_val));
    }
#line 2306 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 73: /* selector: '*'  */
#line 488 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>();
    }
#line 2314 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 75: /* tableList: tbName  */
#line 496 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 2322 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 76: /* tableList: tableList ',' tbName  */
#line 500 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 2330 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 77: /* tableListWithJoin: tableList  */
#line 507 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs) = (yyvsp[0].sv_strs);
    }
#line 2338 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 78: /* join_expr_list: %empty  */
#line 514 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexprs) = std::vector<std::shared_ptr<JoinExpr>>();
    }
#line 2346 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 79: /* join_expr_list: join_expr_list join_clause  */
#line 518 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexprs) = (yyvsp[-1].sv_joinexprs);
        (yyval.sv_joinexprs).push_back((yyvsp[0].sv_joinexpr));
    }
#line 2355 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 80: /* join_clause: JOIN tbName ON condition  */
#line 526 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexpr) = std::make_shared<JoinExpr>("", (yyvsp[-2].sv_str), std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)}, ::INNER_JOIN);
    }
#line 2363 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 81: /* join_clause: SEMI JOIN tbName ON condition  */
#line 530 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_joinexpr) = std::make_shared<JoinExpr>("", (yyvsp[-2].sv_str), std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)}, ::SEMI_JOIN);
    }
#line 2371 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 82: /* opt_order_clause: ORDER BY order_clause  */
#line 537 "/home/code/db2025/src/parser/yacc.y"
    { 
        (yyval.sv_orderby) = (yyvsp[0].sv_orderby); 
    }
#line 2379 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 83: /* opt_order_clause: %empty  */
#line 540 "/home/code/db2025/src/parser/yacc.y"
                      { (yyval.sv_orderby) = nullptr; }
#line 2385 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 84: /* order_clause: order_col_list  */
#line 545 "/home/code/db2025/src/parser/yacc.y"
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
#line 2400 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 85: /* order_col_list: order_col_with_dir  */
#line 559 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_list) = std::vector<std::pair<std::shared_ptr<Col>, OrderByDir>>{(yyvsp[0].sv_order_col_with_dir)};
    }
#line 2408 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 86: /* order_col_list: order_col_list ',' order_col_with_dir  */
#line 563 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_list).push_back((yyvsp[0].sv_order_col_with_dir));
    }
#line 2416 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 87: /* order_col_with_dir: col opt_asc_desc  */
#line 570 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_with_dir) = std::make_pair((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2424 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 88: /* opt_asc_desc: ASC  */
#line 576 "/home/code/db2025/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_ASC;     }
#line 2430 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 89: /* opt_asc_desc: DESC  */
#line 577 "/home/code/db2025/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_DESC;    }
#line 2436 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 90: /* opt_asc_desc: %empty  */
#line 578 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_orderby_dir) = OrderBy_DEFAULT; }
#line 2442 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 91: /* set_knob_type: ENABLE_NESTLOOP  */
#line 582 "/home/code/db2025/src/parser/yacc.y"
                    { (yyval.sv_setKnobType) = EnableNestLoop; }
#line 2448 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 92: /* set_knob_type: ENABLE_SORTMERGE  */
#line 583 "/home/code/db2025/src/parser/yacc.y"
                         { (yyval.sv_setKnobType) = EnableSortMerge; }
#line 2454 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 93: /* agg_func_list: agg_func  */
#line 589 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_funcs) = std::vector<std::shared_ptr<AggFunc>>{(yyvsp[0].sv_agg_func)};
    }
#line 2462 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 94: /* agg_func_list: agg_func_list ',' agg_func  */
#line 593 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_funcs).push_back((yyvsp[0].sv_agg_func));
    }
#line 2470 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 95: /* agg_func: COUNT '(' '*' ')'  */
#line 600 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, nullptr, "");
    }
#line 2478 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 96: /* agg_func: COUNT '(' '*' ')' AS IDENTIFIER  */
#line 604 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, nullptr, (yyvsp[0].sv_str));
    }
#line 2486 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 97: /* agg_func: COUNT '(' col ')'  */
#line 608 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, (yyvsp[-1].sv_col), "");
    }
#line 2494 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 98: /* agg_func: COUNT '(' col ')' AS IDENTIFIER  */
#line 612 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, (yyvsp[-3].sv_col), (yyvsp[0].sv_str));
    }
#line 2502 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 99: /* agg_func: agg_func_type '(' col ')'  */
#line 616 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>((yyvsp[-3].sv_agg_func_type), (yyvsp[-1].sv_col), "");
    }
#line 2510 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 100: /* agg_func: agg_func_type '(' col ')' AS IDENTIFIER  */
#line 620 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>((yyvsp[-5].sv_agg_func_type), (yyvsp[-3].sv_col), (yyvsp[0].sv_str));
    }
#line 2518 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 101: /* agg_func_type: MAX  */
#line 626 "/home/code/db2025/src/parser/yacc.y"
        { (yyval.sv_agg_func_type) = AGG_MAX; }
#line 2524 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 102: /* agg_func_type: MIN  */
#line 627 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_MIN; }
#line 2530 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 103: /* agg_func_type: SUM  */
#line 628 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_SUM; }
#line 2536 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 104: /* agg_func_type: AVG  */
#line 629 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_AVG; }
#line 2542 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 105: /* opt_group_by_clause: %empty  */
#line 633 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_groupby) = nullptr; }
#line 2548 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 106: /* opt_group_by_clause: GROUP BY colList  */
#line 635 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_groupby) = std::make_shared<GroupBy>((yyvsp[0].sv_cols));
    }
#line 2556 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 107: /* opt_having_clause: %empty  */
#line 641 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_having) = nullptr; }
#line 2562 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 108: /* opt_having_clause: HAVING whereClause  */
#line 643 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_having) = std::make_shared<Having>((yyvsp[0].sv_conds));
    }
#line 2570 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 109: /* opt_limit_clause: %empty  */
#line 649 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_limit) = nullptr; }
#line 2576 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 110: /* opt_limit_clause: LIMIT VALUE_INT  */
#line 651 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_limit) = std::make_shared<Limit>((yyvsp[0].sv_int));
    }
#line 2584 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 111: /* mixed_selector: col ',' agg_func  */
#line 659 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[-2].sv_col)}, std::vector<std::shared_ptr<AggFunc>>{(yyvsp[0].sv_agg_func)});
    }
#line 2592 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 112: /* mixed_selector: agg_func ',' col  */
#line 663 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)}, std::vector<std::shared_ptr<AggFunc>>{(yyvsp[-2].sv_agg_func)});
    }
#line 2600 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 113: /* mixed_selector: col ',' col  */
#line 667 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[-2].sv_col), (yyvsp[0].sv_col)}, std::vector<std::shared_ptr<AggFunc>>());
    }
#line 2608 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 114: /* mixed_selector: agg_func ',' agg_func  */
#line 671 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>(), std::vector<std::shared_ptr<AggFunc>>{(yyvsp[-2].sv_agg_func), (yyvsp[0].sv_agg_func)});
    }
#line 2616 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 115: /* mixed_selector: mixed_selector ',' col  */
#line 675 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector).first.push_back((yyvsp[0].sv_col));
    }
#line 2624 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 116: /* mixed_selector: mixed_selector ',' agg_func  */
#line 679 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector).second.push_back((yyvsp[0].sv_agg_func));
    }
#line 2632 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;


#line 2636 "/home/code/db2025/src/parser/yacc.tab.cpp"

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

#line 689 "/home/code/db2025/src/parser/yacc.y"

