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
  YYSYMBOL_EXIT = 25,                      /* EXIT  */
  YYSYMBOL_HELP = 26,                      /* HELP  */
  YYSYMBOL_TXN_BEGIN = 27,                 /* TXN_BEGIN  */
  YYSYMBOL_TXN_COMMIT = 28,                /* TXN_COMMIT  */
  YYSYMBOL_TXN_ABORT = 29,                 /* TXN_ABORT  */
  YYSYMBOL_TXN_ROLLBACK = 30,              /* TXN_ROLLBACK  */
  YYSYMBOL_ENABLE_NESTLOOP = 31,           /* ENABLE_NESTLOOP  */
  YYSYMBOL_ENABLE_SORTMERGE = 32,          /* ENABLE_SORTMERGE  */
  YYSYMBOL_COUNT = 33,                     /* COUNT  */
  YYSYMBOL_MAX = 34,                       /* MAX  */
  YYSYMBOL_MIN = 35,                       /* MIN  */
  YYSYMBOL_SUM = 36,                       /* SUM  */
  YYSYMBOL_AVG = 37,                       /* AVG  */
  YYSYMBOL_GROUP = 38,                     /* GROUP  */
  YYSYMBOL_HAVING = 39,                    /* HAVING  */
  YYSYMBOL_LIMIT = 40,                     /* LIMIT  */
  YYSYMBOL_AS = 41,                        /* AS  */
  YYSYMBOL_ORDER = 42,                     /* ORDER  */
  YYSYMBOL_BY = 43,                        /* BY  */
  YYSYMBOL_LEQ = 44,                       /* LEQ  */
  YYSYMBOL_NEQ = 45,                       /* NEQ  */
  YYSYMBOL_GEQ = 46,                       /* GEQ  */
  YYSYMBOL_T_EOF = 47,                     /* T_EOF  */
  YYSYMBOL_IDENTIFIER = 48,                /* IDENTIFIER  */
  YYSYMBOL_VALUE_STRING = 49,              /* VALUE_STRING  */
  YYSYMBOL_VALUE_INT = 50,                 /* VALUE_INT  */
  YYSYMBOL_VALUE_FLOAT = 51,               /* VALUE_FLOAT  */
  YYSYMBOL_VALUE_BOOL = 52,                /* VALUE_BOOL  */
  YYSYMBOL_53_ = 53,                       /* ';'  */
  YYSYMBOL_54_ = 54,                       /* '='  */
  YYSYMBOL_55_ = 55,                       /* '('  */
  YYSYMBOL_56_ = 56,                       /* ')'  */
  YYSYMBOL_57_ = 57,                       /* ','  */
  YYSYMBOL_58_ = 58,                       /* '.'  */
  YYSYMBOL_59_ = 59,                       /* '<'  */
  YYSYMBOL_60_ = 60,                       /* '>'  */
  YYSYMBOL_61_ = 61,                       /* '*'  */
  YYSYMBOL_YYACCEPT = 62,                  /* $accept  */
  YYSYMBOL_start = 63,                     /* start  */
  YYSYMBOL_stmt = 64,                      /* stmt  */
  YYSYMBOL_txnStmt = 65,                   /* txnStmt  */
  YYSYMBOL_dbStmt = 66,                    /* dbStmt  */
  YYSYMBOL_setStmt = 67,                   /* setStmt  */
  YYSYMBOL_ddl = 68,                       /* ddl  */
  YYSYMBOL_dml = 69,                       /* dml  */
  YYSYMBOL_fieldList = 70,                 /* fieldList  */
  YYSYMBOL_colNameList = 71,               /* colNameList  */
  YYSYMBOL_field = 72,                     /* field  */
  YYSYMBOL_type = 73,                      /* type  */
  YYSYMBOL_valueList = 74,                 /* valueList  */
  YYSYMBOL_value = 75,                     /* value  */
  YYSYMBOL_condition = 76,                 /* condition  */
  YYSYMBOL_optWhereClause = 77,            /* optWhereClause  */
  YYSYMBOL_whereClause = 78,               /* whereClause  */
  YYSYMBOL_col = 79,                       /* col  */
  YYSYMBOL_colList = 80,                   /* colList  */
  YYSYMBOL_op = 81,                        /* op  */
  YYSYMBOL_expr = 82,                      /* expr  */
  YYSYMBOL_setClauses = 83,                /* setClauses  */
  YYSYMBOL_setClause = 84,                 /* setClause  */
  YYSYMBOL_selector = 85,                  /* selector  */
  YYSYMBOL_tableList = 86,                 /* tableList  */
  YYSYMBOL_opt_order_clause = 87,          /* opt_order_clause  */
  YYSYMBOL_order_clause = 88,              /* order_clause  */
  YYSYMBOL_order_col_list = 89,            /* order_col_list  */
  YYSYMBOL_order_col_with_dir = 90,        /* order_col_with_dir  */
  YYSYMBOL_opt_asc_desc = 91,              /* opt_asc_desc  */
  YYSYMBOL_set_knob_type = 92,             /* set_knob_type  */
  YYSYMBOL_agg_func_list = 93,             /* agg_func_list  */
  YYSYMBOL_agg_func = 94,                  /* agg_func  */
  YYSYMBOL_agg_func_type = 95,             /* agg_func_type  */
  YYSYMBOL_opt_group_by_clause = 96,       /* opt_group_by_clause  */
  YYSYMBOL_opt_having_clause = 97,         /* opt_having_clause  */
  YYSYMBOL_opt_limit_clause = 98,          /* opt_limit_clause  */
  YYSYMBOL_mixed_selector = 99,            /* mixed_selector  */
  YYSYMBOL_tbName = 100,                   /* tbName  */
  YYSYMBOL_colName = 101                   /* colName  */
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
#define YYLAST   203

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  108
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  209

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   307


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
      55,    56,    61,     2,    57,     2,    58,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    53,
      59,    54,    60,     2,     2,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   119,   119,   124,   129,   134,   142,   143,   144,   145,
     146,   150,   154,   158,   162,   169,   176,   183,   187,   191,
     195,   199,   203,   210,   214,   218,   222,   226,   230,   237,
     241,   248,   252,   259,   266,   270,   274,   281,   285,   292,
     296,   300,   304,   311,   315,   324,   325,   332,   336,   343,
     347,   351,   357,   366,   370,   377,   381,   385,   389,   393,
     397,   404,   408,   412,   419,   423,   430,   437,   441,   445,
     449,   453,   460,   464,   468,   482,   486,   493,   500,   501,
     502,   506,   507,   512,   516,   523,   527,   531,   535,   539,
     543,   550,   551,   552,   553,   557,   558,   565,   566,   573,
     574,   582,   586,   590,   594,   598,   602,   608,   610
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
  "FLOAT", "INDEX", "AND", "JOIN", "EXIT", "HELP", "TXN_BEGIN",
  "TXN_COMMIT", "TXN_ABORT", "TXN_ROLLBACK", "ENABLE_NESTLOOP",
  "ENABLE_SORTMERGE", "COUNT", "MAX", "MIN", "SUM", "AVG", "GROUP",
  "HAVING", "LIMIT", "AS", "ORDER", "BY", "LEQ", "NEQ", "GEQ", "T_EOF",
  "IDENTIFIER", "VALUE_STRING", "VALUE_INT", "VALUE_FLOAT", "VALUE_BOOL",
  "';'", "'='", "'('", "')'", "','", "'.'", "'<'", "'>'", "'*'", "$accept",
  "start", "stmt", "txnStmt", "dbStmt", "setStmt", "ddl", "dml",
  "fieldList", "colNameList", "field", "type", "valueList", "value",
  "condition", "optWhereClause", "whereClause", "col", "colList", "op",
  "expr", "setClauses", "setClause", "selector", "tableList",
  "opt_order_clause", "order_clause", "order_col_list",
  "order_col_with_dir", "opt_asc_desc", "set_knob_type", "agg_func_list",
  "agg_func", "agg_func_type", "opt_group_by_clause", "opt_having_clause",
  "opt_limit_clause", "mixed_selector", "tbName", "colName", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-141)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-108)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      76,    14,    20,    21,   -42,     6,    31,   -42,    39,   -13,
    -141,  -141,  -141,  -141,  -141,  -141,  -141,    19,    23,  -141,
    -141,  -141,  -141,  -141,  -141,    74,   -42,   -42,   -42,   -42,
    -141,  -141,   -42,   -42,    83,  -141,  -141,    54,    90,  -141,
    -141,  -141,  -141,    71,  -141,    94,    95,   140,   -12,    97,
     100,   -11,    99,   115,  -141,  -141,   -42,   103,   104,  -141,
     105,   150,   147,   116,   111,   -44,    82,   117,   -42,   -42,
      98,    82,   117,   -42,    82,   116,   118,  -141,   116,   116,
     116,   112,    82,  -141,  -141,    -7,  -141,   114,  -141,   119,
     120,  -141,  -141,  -141,   -10,  -141,   -10,  -141,  -141,  -141,
     121,   -10,  -141,  -141,   129,  -141,    64,  -141,    70,    69,
    -141,    84,    87,  -141,   148,    -5,    -5,   116,  -141,    87,
     131,   132,   -42,   -42,   136,   136,   137,   136,   133,  -141,
     116,  -141,   124,  -141,  -141,  -141,   116,  -141,  -141,  -141,
    -141,  -141,    91,  -141,    82,  -141,  -141,  -141,  -141,  -141,
    -141,    62,    62,  -141,  -141,   134,   135,  -141,  -141,   141,
     149,   149,   138,   149,  -141,  -141,   130,  -141,  -141,    87,
    -141,  -141,  -141,  -141,  -141,  -141,  -141,  -141,   117,    82,
     145,   145,  -141,   145,   139,  -141,  -141,    95,   148,   146,
     151,   151,   151,  -141,   117,   142,  -141,  -141,  -141,    45,
    -141,   143,  -141,  -141,  -141,  -141,  -141,   117,  -141
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     3,    11,    12,    13,    14,     5,     0,     0,     9,
       6,    10,     7,     8,    15,     0,     0,     0,     0,     0,
     107,    19,     0,     0,     0,    81,    82,     0,     0,    91,
      92,    93,    94,   108,    67,    53,    68,     0,     0,    83,
       0,     0,     0,    50,     1,     2,     0,     0,     0,    18,
       0,     0,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
       0,     0,     0,    24,   108,    45,    64,     0,    16,     0,
       0,   103,   101,    54,    45,    69,    45,    84,   102,   104,
       0,    45,   105,   106,    49,    52,     0,    29,     0,     0,
      31,     0,     0,    47,    46,     0,     0,     0,    25,     0,
      85,    87,     0,     0,    95,    95,    89,    95,     0,    17,
       0,    34,     0,    36,    33,    20,     0,    21,    41,    39,
      40,    42,     0,    37,     0,    59,    58,    60,    55,    56,
      57,     0,     0,    65,    66,     0,     0,    71,    70,     0,
      97,    97,     0,    97,    51,    30,     0,    32,    23,     0,
      48,    61,    62,    43,    63,    44,    86,    88,     0,     0,
      73,    73,    90,    73,     0,    38,    53,    96,    98,     0,
      99,    99,    99,    35,     0,     0,    26,    27,    28,    80,
      72,    74,    75,   100,    79,    78,    77,     0,    76
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -141,  -141,  -141,  -141,  -141,  -141,  -141,  -141,  -141,   110,
      63,  -141,  -141,  -109,    50,   -19,    17,    -9,    24,    81,
      47,  -141,    86,  -141,   -58,   -74,  -141,  -141,    -6,  -141,
    -141,  -141,    -2,  -141,    -3,   -17,  -140,  -141,     5,   -50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    17,    18,    19,    20,    21,    22,    23,   106,   109,
     107,   134,   142,   171,   113,    83,   114,   115,    46,   151,
     173,    85,    86,    47,    94,   190,   200,   201,   202,   206,
      37,    48,   116,    50,   160,   180,   196,    51,    52,    53
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      45,    69,    73,   143,    43,    82,    30,    49,    82,    31,
     154,    96,    34,    87,   122,   101,    32,    89,    24,    54,
      38,    39,    40,    41,    42,   104,    26,    28,   108,   110,
     110,    57,    58,    59,    60,    43,    25,    61,    62,   145,
     146,   147,    27,    29,    33,    70,    74,   123,    44,   148,
     117,   197,   198,   204,   149,   150,    90,    91,    93,   205,
     185,    77,    98,   100,    92,   102,   118,    87,    97,    99,
      35,    36,   103,    95,    95,   124,    55,   125,    95,     1,
     108,     2,   127,     3,     4,     5,   167,    56,     6,   131,
     132,   133,     7,     8,     9,    38,    39,    40,    41,    42,
      63,    10,    11,    12,    13,    14,    15,   191,    64,   192,
      43,   138,   139,   140,   141,    38,    39,    40,    41,    42,
     129,   130,   161,    16,   163,   135,   136,   157,   158,  -107,
      43,    38,    39,    40,    41,    42,   138,   139,   140,   141,
     137,   136,   172,   172,   181,    65,   183,   168,   169,   174,
     174,    66,    67,    68,    71,    72,    76,    75,    78,    79,
      80,    81,    82,    88,    84,    43,   105,   112,   119,   186,
     128,   144,   155,   156,   159,   120,   121,   126,   162,   166,
     184,   164,   176,   177,   178,   199,   182,   189,   179,   194,
     111,   195,   203,   165,   170,   193,   188,   152,   199,   175,
     207,   208,   187,   153
};

static const yytype_uint8 yycheck[] =
{
       9,    13,    13,   112,    48,    15,    48,     9,    15,     4,
     119,    69,     7,    63,    24,    73,    10,    61,     4,     0,
      33,    34,    35,    36,    37,    75,     6,     6,    78,    79,
      80,    26,    27,    28,    29,    48,    22,    32,    33,    44,
      45,    46,    22,    22,    13,    57,    57,    57,    61,    54,
      57,   191,   192,     8,    59,    60,    65,    66,    67,    14,
     169,    56,    71,    72,    66,    74,    85,   117,    70,    71,
      31,    32,    74,    68,    69,    94,    53,    96,    73,     3,
     130,     5,   101,     7,     8,     9,   136,    13,    12,    19,
      20,    21,    16,    17,    18,    33,    34,    35,    36,    37,
      17,    25,    26,    27,    28,    29,    30,   181,    54,   183,
      48,    49,    50,    51,    52,    33,    34,    35,    36,    37,
      56,    57,   125,    47,   127,    56,    57,   122,   123,    58,
      48,    33,    34,    35,    36,    37,    49,    50,    51,    52,
      56,    57,   151,   152,   161,    55,   163,    56,    57,   151,
     152,    57,    57,    13,    57,    55,    41,    58,    55,    55,
      55,    11,    15,    52,    48,    48,    48,    55,    54,   178,
      41,    23,    41,    41,    38,    56,    56,    56,    41,    55,
      50,    48,    48,    48,    43,   194,    48,    42,    39,    43,
      80,    40,    50,   130,   144,    56,   179,   116,   207,   152,
      57,   207,   178,   117
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     7,     8,     9,    12,    16,    17,    18,
      25,    26,    27,    28,    29,    30,    47,    63,    64,    65,
      66,    67,    68,    69,     4,    22,     6,    22,     6,    22,
      48,   100,    10,    13,   100,    31,    32,    92,    33,    34,
      35,    36,    37,    48,    61,    79,    80,    85,    93,    94,
      95,    99,   100,   101,     0,    53,    13,   100,   100,   100,
     100,   100,   100,    17,    54,    55,    57,    57,    13,    13,
      57,    57,    55,    13,    57,    58,    41,   100,    55,    55,
      55,    11,    15,    77,    48,    83,    84,   101,    52,    61,
      79,    79,    94,    79,    86,   100,    86,    94,    79,    94,
      79,    86,    79,    94,   101,    48,    70,    72,   101,    71,
     101,    71,    55,    76,    78,    79,    94,    57,    77,    54,
      56,    56,    24,    57,    77,    77,    56,    77,    41,    56,
      57,    19,    20,    21,    73,    56,    57,    56,    49,    50,
      51,    52,    74,    75,    23,    44,    45,    46,    54,    59,
      60,    81,    81,    84,    75,    41,    41,   100,   100,    38,
      96,    96,    41,    96,    48,    72,    55,   101,    56,    57,
      76,    75,    79,    82,    94,    82,    48,    48,    43,    39,
      97,    97,    48,    97,    50,    75,    79,    80,    78,    42,
      87,    87,    87,    56,    43,    40,    98,    98,    98,    79,
      88,    89,    90,    50,     8,    14,    91,    57,    90
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    62,    63,    63,    63,    63,    64,    64,    64,    64,
      64,    65,    65,    65,    65,    66,    67,    68,    68,    68,
      68,    68,    68,    69,    69,    69,    69,    69,    69,    70,
      70,    71,    71,    72,    73,    73,    73,    74,    74,    75,
      75,    75,    75,    76,    76,    77,    77,    78,    78,    79,
      79,    79,    79,    80,    80,    81,    81,    81,    81,    81,
      81,    82,    82,    82,    83,    83,    84,    85,    85,    86,
      86,    86,    87,    87,    88,    89,    89,    90,    91,    91,
      91,    92,    92,    93,    93,    94,    94,    94,    94,    94,
      94,    95,    95,    95,    95,    96,    96,    97,    97,    98,
      98,    99,    99,    99,    99,    99,    99,   100,   101
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     4,     6,     3,     2,
       6,     6,     4,     7,     4,     5,     9,     9,     9,     1,
       3,     1,     3,     2,     1,     4,     1,     1,     3,     1,
       1,     1,     1,     3,     3,     0,     2,     1,     3,     3,
       1,     5,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       3,     3,     3,     0,     1,     1,     3,     2,     1,     1,
       0,     1,     1,     1,     3,     4,     6,     4,     6,     4,
       6,     1,     1,     1,     1,     0,     3,     0,     2,     0,
       2,     3,     3,     3,     3,     3,     3,     1,     1
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
#line 120 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = (yyvsp[-1].sv_node);
        YYACCEPT;
    }
#line 1714 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 3: /* start: HELP  */
#line 125 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
#line 1723 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 4: /* start: EXIT  */
#line 130 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1732 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 5: /* start: T_EOF  */
#line 135 "/home/code/db2025/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1741 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 11: /* txnStmt: TXN_BEGIN  */
#line 151 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnBegin>();
    }
#line 1749 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 12: /* txnStmt: TXN_COMMIT  */
#line 155 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnCommit>();
    }
#line 1757 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 13: /* txnStmt: TXN_ABORT  */
#line 159 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnAbort>();
    }
#line 1765 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 14: /* txnStmt: TXN_ROLLBACK  */
#line 163 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnRollback>();
    }
#line 1773 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 15: /* dbStmt: SHOW TABLES  */
#line 170 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowTables>();
    }
#line 1781 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 16: /* setStmt: SET set_knob_type '=' VALUE_BOOL  */
#line 177 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetStmt>((yyvsp[-2].sv_setKnobType), (yyvsp[0].sv_bool));
    }
#line 1789 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 17: /* ddl: CREATE TABLE tbName '(' fieldList ')'  */
#line 184 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1797 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 18: /* ddl: DROP TABLE tbName  */
#line 188 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1805 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 19: /* ddl: DESC tbName  */
#line 192 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1813 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 20: /* ddl: CREATE INDEX tbName '(' colNameList ')'  */
#line 196 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1821 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 21: /* ddl: DROP INDEX tbName '(' colNameList ')'  */
#line 200 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1829 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 22: /* ddl: SHOW INDEX FROM tbName  */
#line 204 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 1837 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 23: /* dml: INSERT INTO tbName VALUES '(' valueList ')'  */
#line 211 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<InsertStmt>((yyvsp[-4].sv_str), (yyvsp[-1].sv_vals));
    }
#line 1845 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 24: /* dml: DELETE FROM tbName optWhereClause  */
#line 215 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DeleteStmt>((yyvsp[-1].sv_str), (yyvsp[0].sv_conds));
    }
#line 1853 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 25: /* dml: UPDATE tbName SET setClauses optWhereClause  */
#line 219 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<UpdateStmt>((yyvsp[-3].sv_str), (yyvsp[-1].sv_set_clauses), (yyvsp[0].sv_conds));
    }
#line 1861 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 26: /* dml: SELECT selector FROM tableList optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 223 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>((yyvsp[-7].sv_cols), std::vector<std::shared_ptr<AggFunc>>(), (yyvsp[-5].sv_strs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit));
    }
#line 1869 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 27: /* dml: SELECT agg_func_list FROM tableList optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 227 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>(std::vector<std::shared_ptr<Col>>(), (yyvsp[-7].sv_agg_funcs), (yyvsp[-5].sv_strs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit));
    }
#line 1877 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 28: /* dml: SELECT mixed_selector FROM tableList optWhereClause opt_group_by_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 231 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SelectStmt>((yyvsp[-7].sv_mixed_selector).first, (yyvsp[-7].sv_mixed_selector).second, (yyvsp[-5].sv_strs), (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_groupby), (yyvsp[-2].sv_having), (yyvsp[0].sv_limit));
    }
#line 1885 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 29: /* fieldList: field  */
#line 238 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_fields) = std::vector<std::shared_ptr<Field>>{(yyvsp[0].sv_field)};
    }
#line 1893 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 30: /* fieldList: fieldList ',' field  */
#line 242 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_fields).push_back((yyvsp[0].sv_field));
    }
#line 1901 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 31: /* colNameList: colName  */
#line 249 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 1909 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 32: /* colNameList: colNameList ',' colName  */
#line 253 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 1917 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 33: /* field: colName type  */
#line 260 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_field) = std::make_shared<ColDef>((yyvsp[-1].sv_str), (yyvsp[0].sv_type_len));
    }
#line 1925 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 34: /* type: INT  */
#line 267 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
#line 1933 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 35: /* type: CHAR '(' VALUE_INT ')'  */
#line 271 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_STRING, (yyvsp[-1].sv_int));
    }
#line 1941 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 36: /* type: FLOAT  */
#line 275 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
#line 1949 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 37: /* valueList: value  */
#line 282 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_vals) = std::vector<std::shared_ptr<Value>>{(yyvsp[0].sv_val)};
    }
#line 1957 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 38: /* valueList: valueList ',' value  */
#line 286 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_vals).push_back((yyvsp[0].sv_val));
    }
#line 1965 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 39: /* value: VALUE_INT  */
#line 293 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>((yyvsp[0].sv_int));
    }
#line 1973 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 40: /* value: VALUE_FLOAT  */
#line 297 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>((yyvsp[0].sv_float));
    }
#line 1981 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 41: /* value: VALUE_STRING  */
#line 301 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<StringLit>((yyvsp[0].sv_str));
    }
#line 1989 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 42: /* value: VALUE_BOOL  */
#line 305 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<BoolLit>((yyvsp[0].sv_bool));
    }
#line 1997 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 43: /* condition: col op expr  */
#line 312 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_col), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2005 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 44: /* condition: agg_func op expr  */
#line 316 "/home/code/db2025/src/parser/yacc.y"
    {
        // 创建一个临时的 Col 对象来存储聚合函数信息
        // 这里需要修改 BinaryExpr 结构以支持聚合函数作为左操作数
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_agg_func), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2015 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 45: /* optWhereClause: %empty  */
#line 324 "/home/code/db2025/src/parser/yacc.y"
                      { (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>(); }
#line 2021 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 46: /* optWhereClause: WHERE whereClause  */
#line 326 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds) = (yyvsp[0].sv_conds);
    }
#line 2029 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 47: /* whereClause: condition  */
#line 333 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2037 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 48: /* whereClause: whereClause AND condition  */
#line 337 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2045 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 49: /* col: tbName '.' colName  */
#line 344 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2053 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 50: /* col: colName  */
#line 348 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[0].sv_str));
    }
#line 2061 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 51: /* col: tbName '.' colName AS IDENTIFIER  */
#line 352 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-4].sv_str), (yyvsp[-2].sv_str));
        // 注意：这里我们暂时忽略别名，因为Col结构中没有alias字段
        // 在实际实现中，需要扩展Col结构以支持别名
    }
#line 2071 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 52: /* col: colName AS IDENTIFIER  */
#line 358 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[-2].sv_str));
        // 注意：这里我们暂时忽略别名，因为Col结构中没有alias字段
        // 在实际实现中，需要扩展Col结构以支持别名
    }
#line 2081 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 53: /* colList: col  */
#line 367 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)};
    }
#line 2089 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 54: /* colList: colList ',' col  */
#line 371 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols).push_back((yyvsp[0].sv_col));
    }
#line 2097 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 55: /* op: '='  */
#line 378 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_EQ;
    }
#line 2105 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 56: /* op: '<'  */
#line 382 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LT;
    }
#line 2113 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 57: /* op: '>'  */
#line 386 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GT;
    }
#line 2121 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 58: /* op: NEQ  */
#line 390 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_NE;
    }
#line 2129 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 59: /* op: LEQ  */
#line 394 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LE;
    }
#line 2137 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 60: /* op: GEQ  */
#line 398 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GE;
    }
#line 2145 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 61: /* expr: value  */
#line 405 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_val));
    }
#line 2153 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 62: /* expr: col  */
#line 409 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_col));
    }
#line 2161 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 63: /* expr: agg_func  */
#line 413 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_func));
    }
#line 2169 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 64: /* setClauses: setClause  */
#line 420 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses) = std::vector<std::shared_ptr<SetClause>>{(yyvsp[0].sv_set_clause)};
    }
#line 2177 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 65: /* setClauses: setClauses ',' setClause  */
#line 424 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses).push_back((yyvsp[0].sv_set_clause));
    }
#line 2185 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 66: /* setClause: colName '=' value  */
#line 431 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>((yyvsp[-2].sv_str), (yyvsp[0].sv_val));
    }
#line 2193 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 67: /* selector: '*'  */
#line 438 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>();
    }
#line 2201 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 69: /* tableList: tbName  */
#line 446 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 2209 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 70: /* tableList: tableList ',' tbName  */
#line 450 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 2217 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 71: /* tableList: tableList JOIN tbName  */
#line 454 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 2225 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 72: /* opt_order_clause: ORDER BY order_clause  */
#line 461 "/home/code/db2025/src/parser/yacc.y"
    { 
        (yyval.sv_orderby) = (yyvsp[0].sv_orderby); 
    }
#line 2233 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 73: /* opt_order_clause: %empty  */
#line 464 "/home/code/db2025/src/parser/yacc.y"
                      { (yyval.sv_orderby) = nullptr; }
#line 2239 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 74: /* order_clause: order_col_list  */
#line 469 "/home/code/db2025/src/parser/yacc.y"
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
#line 2254 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 75: /* order_col_list: order_col_with_dir  */
#line 483 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_list) = std::vector<std::pair<std::shared_ptr<Col>, OrderByDir>>{(yyvsp[0].sv_order_col_with_dir)};
    }
#line 2262 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 76: /* order_col_list: order_col_list ',' order_col_with_dir  */
#line 487 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_list).push_back((yyvsp[0].sv_order_col_with_dir));
    }
#line 2270 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 77: /* order_col_with_dir: col opt_asc_desc  */
#line 494 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_order_col_with_dir) = std::make_pair((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2278 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 78: /* opt_asc_desc: ASC  */
#line 500 "/home/code/db2025/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_ASC;     }
#line 2284 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 79: /* opt_asc_desc: DESC  */
#line 501 "/home/code/db2025/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_DESC;    }
#line 2290 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 80: /* opt_asc_desc: %empty  */
#line 502 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_orderby_dir) = OrderBy_DEFAULT; }
#line 2296 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 81: /* set_knob_type: ENABLE_NESTLOOP  */
#line 506 "/home/code/db2025/src/parser/yacc.y"
                    { (yyval.sv_setKnobType) = EnableNestLoop; }
#line 2302 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 82: /* set_knob_type: ENABLE_SORTMERGE  */
#line 507 "/home/code/db2025/src/parser/yacc.y"
                         { (yyval.sv_setKnobType) = EnableSortMerge; }
#line 2308 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 83: /* agg_func_list: agg_func  */
#line 513 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_funcs) = std::vector<std::shared_ptr<AggFunc>>{(yyvsp[0].sv_agg_func)};
    }
#line 2316 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 84: /* agg_func_list: agg_func_list ',' agg_func  */
#line 517 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_funcs).push_back((yyvsp[0].sv_agg_func));
    }
#line 2324 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 85: /* agg_func: COUNT '(' '*' ')'  */
#line 524 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, nullptr, "");
    }
#line 2332 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 86: /* agg_func: COUNT '(' '*' ')' AS IDENTIFIER  */
#line 528 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, nullptr, (yyvsp[0].sv_str));
    }
#line 2340 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 87: /* agg_func: COUNT '(' col ')'  */
#line 532 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, (yyvsp[-1].sv_col), "");
    }
#line 2348 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 88: /* agg_func: COUNT '(' col ')' AS IDENTIFIER  */
#line 536 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>(AGG_COUNT, (yyvsp[-3].sv_col), (yyvsp[0].sv_str));
    }
#line 2356 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 89: /* agg_func: agg_func_type '(' col ')'  */
#line 540 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>((yyvsp[-3].sv_agg_func_type), (yyvsp[-1].sv_col), "");
    }
#line 2364 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 90: /* agg_func: agg_func_type '(' col ')' AS IDENTIFIER  */
#line 544 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_agg_func) = std::make_shared<AggFunc>((yyvsp[-5].sv_agg_func_type), (yyvsp[-3].sv_col), (yyvsp[0].sv_str));
    }
#line 2372 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 91: /* agg_func_type: MAX  */
#line 550 "/home/code/db2025/src/parser/yacc.y"
        { (yyval.sv_agg_func_type) = AGG_MAX; }
#line 2378 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 92: /* agg_func_type: MIN  */
#line 551 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_MIN; }
#line 2384 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 93: /* agg_func_type: SUM  */
#line 552 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_SUM; }
#line 2390 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 94: /* agg_func_type: AVG  */
#line 553 "/home/code/db2025/src/parser/yacc.y"
            { (yyval.sv_agg_func_type) = AGG_AVG; }
#line 2396 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 95: /* opt_group_by_clause: %empty  */
#line 557 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_groupby) = nullptr; }
#line 2402 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 96: /* opt_group_by_clause: GROUP BY colList  */
#line 559 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_groupby) = std::make_shared<GroupBy>((yyvsp[0].sv_cols));
    }
#line 2410 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 97: /* opt_having_clause: %empty  */
#line 565 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_having) = nullptr; }
#line 2416 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 98: /* opt_having_clause: HAVING whereClause  */
#line 567 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_having) = std::make_shared<Having>((yyvsp[0].sv_conds));
    }
#line 2424 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 99: /* opt_limit_clause: %empty  */
#line 573 "/home/code/db2025/src/parser/yacc.y"
                  { (yyval.sv_limit) = nullptr; }
#line 2430 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 100: /* opt_limit_clause: LIMIT VALUE_INT  */
#line 575 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_limit) = std::make_shared<Limit>((yyvsp[0].sv_int));
    }
#line 2438 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 101: /* mixed_selector: col ',' agg_func  */
#line 583 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[-2].sv_col)}, std::vector<std::shared_ptr<AggFunc>>{(yyvsp[0].sv_agg_func)});
    }
#line 2446 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 102: /* mixed_selector: agg_func ',' col  */
#line 587 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)}, std::vector<std::shared_ptr<AggFunc>>{(yyvsp[-2].sv_agg_func)});
    }
#line 2454 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 103: /* mixed_selector: col ',' col  */
#line 591 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>{(yyvsp[-2].sv_col), (yyvsp[0].sv_col)}, std::vector<std::shared_ptr<AggFunc>>());
    }
#line 2462 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 104: /* mixed_selector: agg_func ',' agg_func  */
#line 595 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector) = std::make_pair(std::vector<std::shared_ptr<Col>>(), std::vector<std::shared_ptr<AggFunc>>{(yyvsp[-2].sv_agg_func), (yyvsp[0].sv_agg_func)});
    }
#line 2470 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 105: /* mixed_selector: mixed_selector ',' col  */
#line 599 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector).first.push_back((yyvsp[0].sv_col));
    }
#line 2478 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;

  case 106: /* mixed_selector: mixed_selector ',' agg_func  */
#line 603 "/home/code/db2025/src/parser/yacc.y"
    {
        (yyval.sv_mixed_selector).second.push_back((yyvsp[0].sv_agg_func));
    }
#line 2486 "/home/code/db2025/src/parser/yacc.tab.cpp"
    break;


#line 2490 "/home/code/db2025/src/parser/yacc.tab.cpp"

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

#line 611 "/home/code/db2025/src/parser/yacc.y"

