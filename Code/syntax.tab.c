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
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 3 "./syntax.y"

    int yycolumn = 1;
    char linetext[128];
    #define YY_USER_ACTION \
                yylloc.first_line = yylloc.last_line = yylineno; \
                yylloc.first_column = yycolumn; \
                yylloc.last_column = yycolumn + yyleng - 1; \
                yycolumn += yyleng;
    //#define YYDEBUG 1
    //int yydebug = 1;
#line 15 "./syntax.y"

    #include <stdio.h>
    #include "lex.yy.c"
    #include "data.h"
    #include <stdarg.h>

    #define TERMINALS(A,B) A=tree->Node_alloc(B,yylloc.first_line)

    void yyerror(char* s);
    Node_t * Operator(Node_t * cur,char * content,int line,int argc,...);

    extern int syntax;


#line 97 "./syntax.tab.c"

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

#include "syntax.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INT = 3,                        /* INT  */
  YYSYMBOL_FLOAT = 4,                      /* FLOAT  */
  YYSYMBOL_ID = 5,                         /* ID  */
  YYSYMBOL_SEMI = 6,                       /* SEMI  */
  YYSYMBOL_COMMA = 7,                      /* COMMA  */
  YYSYMBOL_ASSIGNOP = 8,                   /* ASSIGNOP  */
  YYSYMBOL_RELOP = 9,                      /* RELOP  */
  YYSYMBOL_PLUS = 10,                      /* PLUS  */
  YYSYMBOL_MINUS = 11,                     /* MINUS  */
  YYSYMBOL_STAR = 12,                      /* STAR  */
  YYSYMBOL_DIV = 13,                       /* DIV  */
  YYSYMBOL_AND = 14,                       /* AND  */
  YYSYMBOL_OR = 15,                        /* OR  */
  YYSYMBOL_DOT = 16,                       /* DOT  */
  YYSYMBOL_NOT = 17,                       /* NOT  */
  YYSYMBOL_TYPE = 18,                      /* TYPE  */
  YYSYMBOL_LP = 19,                        /* LP  */
  YYSYMBOL_RP = 20,                        /* RP  */
  YYSYMBOL_LB = 21,                        /* LB  */
  YYSYMBOL_RB = 22,                        /* RB  */
  YYSYMBOL_LC = 23,                        /* LC  */
  YYSYMBOL_RC = 24,                        /* RC  */
  YYSYMBOL_STRUCT = 25,                    /* STRUCT  */
  YYSYMBOL_RETURN = 26,                    /* RETURN  */
  YYSYMBOL_IF = 27,                        /* IF  */
  YYSYMBOL_ELSE = 28,                      /* ELSE  */
  YYSYMBOL_WHILE = 29,                     /* WHILE  */
  YYSYMBOL_LOWER_THAN_ELSE = 30,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_YYACCEPT = 31,                  /* $accept  */
  YYSYMBOL_Program = 32,                   /* Program  */
  YYSYMBOL_ExtDefList = 33,                /* ExtDefList  */
  YYSYMBOL_ExtDef = 34,                    /* ExtDef  */
  YYSYMBOL_ExtDecList = 35,                /* ExtDecList  */
  YYSYMBOL_Specifier = 36,                 /* Specifier  */
  YYSYMBOL_StructSpecifier = 37,           /* StructSpecifier  */
  YYSYMBOL_OptTag = 38,                    /* OptTag  */
  YYSYMBOL_Tag = 39,                       /* Tag  */
  YYSYMBOL_VarDec = 40,                    /* VarDec  */
  YYSYMBOL_FunDec = 41,                    /* FunDec  */
  YYSYMBOL_VarList = 42,                   /* VarList  */
  YYSYMBOL_ParamDec = 43,                  /* ParamDec  */
  YYSYMBOL_CompSt = 44,                    /* CompSt  */
  YYSYMBOL_StmtList = 45,                  /* StmtList  */
  YYSYMBOL_Stmt = 46,                      /* Stmt  */
  YYSYMBOL_DefList = 47,                   /* DefList  */
  YYSYMBOL_Def = 48,                       /* Def  */
  YYSYMBOL_DecList = 49,                   /* DecList  */
  YYSYMBOL_Dec = 50,                       /* Dec  */
  YYSYMBOL_Exp = 51,                       /* Exp  */
  YYSYMBOL_Args = 52                       /* Args  */
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

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
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   467

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  89
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  155

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   285


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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   110,   110,   112,   113,   115,   116,   117,   118,   119,
     120,   121,   123,   124,   129,   130,   132,   133,   135,   136,
     138,   142,   143,   144,   146,   147,   148,   149,   151,   152,
     154,   159,   160,   162,   163,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   181,   182,   184,
     185,   186,   188,   189,   191,   192,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   229,   230,   231
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INT", "FLOAT", "ID",
  "SEMI", "COMMA", "ASSIGNOP", "RELOP", "PLUS", "MINUS", "STAR", "DIV",
  "AND", "OR", "DOT", "NOT", "TYPE", "LP", "RP", "LB", "RB", "LC", "RC",
  "STRUCT", "RETURN", "IF", "ELSE", "WHILE", "LOWER_THAN_ELSE", "$accept",
  "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier",
  "StructSpecifier", "OptTag", "Tag", "VarDec", "FunDec", "VarList",
  "ParamDec", "CompSt", "StmtList", "Stmt", "DefList", "Def", "DecList",
  "Dec", "Exp", "Args", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-85)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-35)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      10,    14,   -85,    44,    79,   -85,    10,    69,   -85,   -85,
      67,    81,   -85,   -85,   -85,    70,    73,   -85,    97,     5,
      85,    -1,   -85,    -1,    -1,   -85,    28,   -85,   109,    63,
     -85,    20,    91,    -1,   109,    96,   111,    54,    99,   -85,
     108,   -85,   -85,   112,   113,   127,     6,    17,   129,   -85,
     -85,   118,   -85,    -1,    13,   -85,   -85,   122,   126,   205,
     324,   210,    15,   123,   -85,   125,    83,   145,   -85,   -85,
     -85,   -85,   -85,   324,   146,   -85,   109,   -85,   -85,   -85,
     180,   -85,    80,   -85,    80,   366,   157,   161,    94,   324,
     324,   -85,   158,   -85,   -85,   -85,   227,   232,   249,   254,
     271,   276,   293,   298,    51,   315,   408,   -85,   -85,   185,
     -85,   337,   148,   -85,   -85,   -85,   -85,   -85,   380,   394,
     -85,   408,   -85,   182,   -85,   440,   -85,    80,   -85,   446,
     -85,    80,   -85,   434,   -85,   421,   -85,   -85,   143,   351,
     320,   -85,   -85,   320,   -85,    94,    94,   -85,   -85,   -85,
     150,   -85,   121,   158,   -85
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,    14,    19,     0,     2,     0,     0,    15,     8,
      20,     0,    17,     1,     3,    10,    21,     6,     0,    12,
       0,    48,     9,     0,    48,    11,     0,     5,     0,     0,
       7,     0,     0,    48,     0,     0,    29,     0,     0,    25,
       0,    21,    13,     0,     0,     0,    54,     0,    52,    16,
      47,    30,    27,     0,     0,    72,    73,    71,     0,     0,
       0,     0,     0,     0,    36,     0,     0,     0,    26,    24,
      23,    22,    50,     0,     0,    49,     0,    28,    41,    32,
       0,    82,    65,    83,    66,     0,     0,     0,     0,     0,
       0,    31,     0,    33,    42,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    55,    51,    53,     0,
      68,    88,     0,    64,    45,    46,    37,    43,     0,     0,
      74,    56,    75,    59,    76,    60,    77,    61,    78,    62,
      79,    63,    80,    57,    81,    58,    86,    70,     0,     0,
       0,    84,    89,     0,    67,     0,     0,    85,    69,    87,
      38,    40,     0,    44,    39
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -85,   -85,   173,   -85,   152,     7,   -85,   -85,   -85,   -26,
     -85,   -17,   -85,    52,   135,   -84,    30,   -85,   131,   -85,
     -58,   -74
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     4,     5,     6,    18,    31,     8,    11,    12,    19,
      20,    35,    36,    64,    65,    66,    32,    33,    47,    48,
      67,   142
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      82,    84,    85,    87,   117,    46,   112,     7,    51,    40,
      -4,     1,    28,     7,    73,   106,    88,     2,    74,    78,
       9,    45,   111,    75,     3,    41,    29,    29,     2,    38,
      34,   118,   119,    34,    89,     3,    77,    79,   121,   123,
     125,   127,   129,   131,   133,   135,     2,   139,    39,    10,
      46,   111,   136,     3,    37,    54,   137,    55,    56,    57,
      34,   150,   151,    50,    43,    58,    44,    25,   154,   149,
      15,    59,    30,    60,    16,    17,    22,    24,   -34,    13,
      61,    62,   111,    63,    92,   111,    55,    56,    57,    23,
     -18,    99,    26,    24,    58,    92,   104,    55,    56,    57,
      59,   105,    60,    27,    21,    58,    24,   -34,    24,    61,
      62,    59,    63,    60,    41,    49,    52,    24,    53,    68,
      61,    62,   153,    63,    55,    56,    57,    81,    69,    55,
      56,    57,    58,    72,    70,    71,    76,    58,    59,    29,
      60,    80,    90,    59,    24,    60,    94,    61,    62,    91,
      63,    95,   107,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   115,   114,    78,   147,   105,   116,   144,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   152,    14,
      42,   109,   105,    55,    56,    57,   140,     0,    55,    56,
      57,    58,    98,    99,   100,   101,    58,    59,   104,    60,
     110,    93,    59,   105,    60,   141,    83,   108,    55,    56,
      57,    86,     0,    55,    56,    57,    58,     0,     0,     0,
       0,    58,    59,     0,    60,     0,     0,    59,   120,    60,
      55,    56,    57,   122,     0,    55,    56,    57,    58,     0,
       0,     0,     0,    58,    59,     0,    60,     0,     0,    59,
     124,    60,    55,    56,    57,   126,     0,    55,    56,    57,
      58,     0,     0,     0,     0,    58,    59,     0,    60,     0,
       0,    59,   128,    60,    55,    56,    57,   130,     0,    55,
      56,    57,    58,     0,     0,     0,     0,    58,    59,     0,
      60,     0,     0,    59,   132,    60,    55,    56,    57,   134,
       0,    55,    56,    57,    58,     0,     0,     0,     0,    58,
      59,     0,    60,     0,     0,    59,   138,    60,    55,    56,
      57,   140,     0,    55,    56,    57,    58,    55,    56,    57,
       0,    58,    59,     0,    60,    58,     0,    59,     0,    60,
       0,    59,     0,    60,   143,    96,    97,    98,    99,   100,
     101,   102,   103,   104,     0,     0,     0,     0,   105,    96,
      97,    98,    99,   100,   101,   102,   103,   104,     0,     0,
       0,     0,   105,   148,    96,    97,    98,    99,   100,   101,
     102,   103,   104,     0,     0,     0,   113,   105,    96,    97,
      98,    99,   100,   101,   102,   103,   104,     0,     0,     0,
     145,   105,    96,    97,    98,    99,   100,   101,   102,   103,
     104,     0,     0,     0,   146,   105,    96,    97,    98,    99,
     100,   101,   102,   103,   104,     0,     0,     0,     0,   105,
      97,    98,    99,   100,   101,   102,     0,   104,     0,     0,
       0,     0,   105,    97,    98,    99,   100,   101,     0,     0,
     104,    99,   100,   101,     0,   105,   104,    99,     0,   101,
       0,   105,   104,     0,     0,     0,     0,   105
};

static const yytype_int16 yycheck[] =
{
      58,    59,    60,    61,    88,    31,    80,     0,    34,    26,
       0,     1,     7,     6,     8,    73,     1,    18,     1,     6,
       6,     1,    80,     6,    25,     5,    21,    21,    18,     1,
      23,    89,    90,    26,    19,    25,    53,    24,    96,    97,
      98,    99,   100,   101,   102,   103,    18,   105,    20,     5,
      76,   109,     1,    25,    24,     1,     5,     3,     4,     5,
      53,   145,   146,    33,     1,    11,     3,    15,   152,   143,
       1,    17,    20,    19,     5,     6,     6,    23,    24,     0,
      26,    27,   140,    29,     1,   143,     3,     4,     5,    19,
      23,    11,    19,    23,    11,     1,    16,     3,     4,     5,
      17,    21,    19,     6,    23,    11,    23,    24,    23,    26,
      27,    17,    29,    19,     5,    24,    20,    23,     7,    20,
      26,    27,     1,    29,     3,     4,     5,     1,    20,     3,
       4,     5,    11,     6,    22,    22,     7,    11,    17,    21,
      19,    19,    19,    17,    23,    19,     1,    26,    27,    24,
      29,     6,     6,     8,     9,    10,    11,    12,    13,    14,
      15,    16,     1,     6,     6,    22,    21,     6,    20,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    28,     6,
      28,     1,    21,     3,     4,     5,     1,    -1,     3,     4,
       5,    11,    10,    11,    12,    13,    11,    17,    16,    19,
      20,    66,    17,    21,    19,    20,     1,    76,     3,     4,
       5,     1,    -1,     3,     4,     5,    11,    -1,    -1,    -1,
      -1,    11,    17,    -1,    19,    -1,    -1,    17,     1,    19,
       3,     4,     5,     1,    -1,     3,     4,     5,    11,    -1,
      -1,    -1,    -1,    11,    17,    -1,    19,    -1,    -1,    17,
       1,    19,     3,     4,     5,     1,    -1,     3,     4,     5,
      11,    -1,    -1,    -1,    -1,    11,    17,    -1,    19,    -1,
      -1,    17,     1,    19,     3,     4,     5,     1,    -1,     3,
       4,     5,    11,    -1,    -1,    -1,    -1,    11,    17,    -1,
      19,    -1,    -1,    17,     1,    19,     3,     4,     5,     1,
      -1,     3,     4,     5,    11,    -1,    -1,    -1,    -1,    11,
      17,    -1,    19,    -1,    -1,    17,     1,    19,     3,     4,
       5,     1,    -1,     3,     4,     5,    11,     3,     4,     5,
      -1,    11,    17,    -1,    19,    11,    -1,    17,    -1,    19,
      -1,    17,    -1,    19,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    -1,    -1,    -1,    -1,    21,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
      -1,    -1,    21,    22,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    -1,    -1,    -1,    20,    21,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    -1,    -1,    -1,
      20,    21,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    -1,    -1,    -1,    20,    21,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    21,
       9,    10,    11,    12,    13,    14,    -1,    16,    -1,    -1,
      -1,    -1,    21,     9,    10,    11,    12,    13,    -1,    -1,
      16,    11,    12,    13,    -1,    21,    16,    11,    -1,    13,
      -1,    21,    16,    -1,    -1,    -1,    -1,    21
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,    18,    25,    32,    33,    34,    36,    37,     6,
       5,    38,    39,     0,    33,     1,     5,     6,    35,    40,
      41,    23,     6,    19,    23,    44,    19,     6,     7,    21,
      44,    36,    47,    48,    36,    42,    43,    47,     1,    20,
      42,     5,    35,     1,     3,     1,    40,    49,    50,    24,
      47,    40,    20,     7,     1,     3,     4,     5,    11,    17,
      19,    26,    27,    29,    44,    45,    46,    51,    20,    20,
      22,    22,     6,     8,     1,     6,     7,    42,     6,    24,
      19,     1,    51,     1,    51,    51,     1,    51,     1,    19,
      19,    24,     1,    45,     1,     6,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    21,    51,     6,    49,     1,
      20,    51,    52,    20,     6,     1,     6,    46,    51,    51,
       1,    51,     1,    51,     1,    51,     1,    51,     1,    51,
       1,    51,     1,    51,     1,    51,     1,     5,     1,    51,
       1,    20,    52,     7,    20,    20,    20,    22,    22,    52,
      46,    46,    28,     1,    46
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    31,    32,    33,    33,    34,    34,    34,    34,    34,
      34,    34,    35,    35,    36,    36,    37,    37,    38,    38,
      39,    40,    40,    40,    41,    41,    41,    41,    42,    42,
      43,    44,    44,    45,    45,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    47,    47,    48,
      48,    48,    49,    49,    50,    50,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    52,    52,    52
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     3,     2,     3,     2,     3,
       2,     3,     1,     3,     1,     1,     5,     2,     1,     0,
       1,     1,     4,     4,     4,     3,     4,     4,     3,     1,
       2,     4,     4,     2,     0,     2,     1,     3,     5,     7,
       5,     2,     2,     3,     7,     3,     3,     2,     0,     3,
       3,     4,     1,     3,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     4,     3,     4,
       3,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     4,     4,     3,     3,     1,     2
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
        yyerror (YY_("syntax error: cannot back up")); \
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


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
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
      yychar = yylex ();
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
  case 2: /* Program: ExtDefList  */
#line 110 "./syntax.y"
                                    { tree->root = (yyval.node) = Operator((yyval.node),"Program",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1412 "./syntax.tab.c"
    break;

  case 3: /* ExtDefList: ExtDef ExtDefList  */
#line 112 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ExtDefList",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1418 "./syntax.tab.c"
    break;

  case 4: /* ExtDefList: %empty  */
#line 113 "./syntax.y"
                                    { (yyval.node) = NULL; }
#line 1424 "./syntax.tab.c"
    break;

  case 5: /* ExtDef: Specifier ExtDecList SEMI  */
#line 115 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ExtDef",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1430 "./syntax.tab.c"
    break;

  case 6: /* ExtDef: Specifier SEMI  */
#line 116 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ExtDef",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1436 "./syntax.tab.c"
    break;

  case 7: /* ExtDef: Specifier FunDec CompSt  */
#line 117 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ExtDef",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1442 "./syntax.tab.c"
    break;

  case 8: /* ExtDef: error SEMI  */
#line 118 "./syntax.y"
                                    { (yyval.node) = NULL;yyerror("ExtDef"); }
#line 1448 "./syntax.tab.c"
    break;

  case 9: /* ExtDef: Specifier error SEMI  */
#line 119 "./syntax.y"
                                    { (yyval.node) = NULL;yyerror("ExtDef"); }
#line 1454 "./syntax.tab.c"
    break;

  case 10: /* ExtDef: Specifier error  */
#line 120 "./syntax.y"
                                    { (yyval.node) = NULL;yyerror("ExtDef,missing ;"); }
#line 1460 "./syntax.tab.c"
    break;

  case 11: /* ExtDef: Specifier error CompSt  */
#line 121 "./syntax.y"
                                    { (yyval.node) = NULL;yyerror("ExtDef"); }
#line 1466 "./syntax.tab.c"
    break;

  case 12: /* ExtDecList: VarDec  */
#line 123 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ExtDecList",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1472 "./syntax.tab.c"
    break;

  case 13: /* ExtDecList: VarDec COMMA ExtDecList  */
#line 124 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ExtDecList",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1478 "./syntax.tab.c"
    break;

  case 14: /* Specifier: TYPE  */
#line 129 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Specifier",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1484 "./syntax.tab.c"
    break;

  case 15: /* Specifier: StructSpecifier  */
#line 130 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Specifier",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1490 "./syntax.tab.c"
    break;

  case 16: /* StructSpecifier: STRUCT OptTag LC DefList RC  */
#line 132 "./syntax.y"
                                                { (yyval.node) = Operator((yyval.node),"StructSpecifier",(yyloc).first_line,5,(yyvsp[-4].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1496 "./syntax.tab.c"
    break;

  case 17: /* StructSpecifier: STRUCT Tag  */
#line 133 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"StructSpecifier",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1502 "./syntax.tab.c"
    break;

  case 18: /* OptTag: ID  */
#line 135 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"OptTag",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1508 "./syntax.tab.c"
    break;

  case 19: /* OptTag: %empty  */
#line 136 "./syntax.y"
                                    { (yyval.node) = NULL; }
#line 1514 "./syntax.tab.c"
    break;

  case 20: /* Tag: ID  */
#line 138 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Tag",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1520 "./syntax.tab.c"
    break;

  case 21: /* VarDec: ID  */
#line 142 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"VarDec",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1526 "./syntax.tab.c"
    break;

  case 22: /* VarDec: VarDec LB INT RB  */
#line 143 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"VarDec",(yyloc).first_line,4,(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1532 "./syntax.tab.c"
    break;

  case 23: /* VarDec: VarDec LB error RB  */
#line 144 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("VarDec"); }
#line 1538 "./syntax.tab.c"
    break;

  case 24: /* FunDec: ID LP VarList RP  */
#line 146 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"FunDec",(yyloc).first_line,4,(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1544 "./syntax.tab.c"
    break;

  case 25: /* FunDec: ID LP RP  */
#line 147 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"FunDec",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1550 "./syntax.tab.c"
    break;

  case 26: /* FunDec: ID LP error RP  */
#line 148 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("FunDec"); }
#line 1556 "./syntax.tab.c"
    break;

  case 27: /* FunDec: error LP VarList RP  */
#line 149 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("FunDec"); }
#line 1562 "./syntax.tab.c"
    break;

  case 28: /* VarList: ParamDec COMMA VarList  */
#line 151 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"VarList",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1568 "./syntax.tab.c"
    break;

  case 29: /* VarList: ParamDec  */
#line 152 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ParamDec",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1574 "./syntax.tab.c"
    break;

  case 30: /* ParamDec: Specifier VarDec  */
#line 154 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"ParamDec",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1580 "./syntax.tab.c"
    break;

  case 31: /* CompSt: LC DefList StmtList RC  */
#line 159 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"CompSt",(yyloc).first_line,4,(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1586 "./syntax.tab.c"
    break;

  case 32: /* CompSt: LC DefList error RC  */
#line 160 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("CompSt"); }
#line 1592 "./syntax.tab.c"
    break;

  case 33: /* StmtList: Stmt StmtList  */
#line 162 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"StmtList",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1598 "./syntax.tab.c"
    break;

  case 34: /* StmtList: %empty  */
#line 163 "./syntax.y"
                                    { (yyval.node) = NULL; }
#line 1604 "./syntax.tab.c"
    break;

  case 35: /* Stmt: Exp SEMI  */
#line 165 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Stmt",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1610 "./syntax.tab.c"
    break;

  case 36: /* Stmt: CompSt  */
#line 166 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Stmt",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1616 "./syntax.tab.c"
    break;

  case 37: /* Stmt: RETURN Exp SEMI  */
#line 167 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Stmt",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1622 "./syntax.tab.c"
    break;

  case 38: /* Stmt: IF LP Exp RP Stmt  */
#line 168 "./syntax.y"
                                                { (yyval.node) = Operator((yyval.node),"Stmt",(yyloc).first_line,5,(yyvsp[-4].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1628 "./syntax.tab.c"
    break;

  case 39: /* Stmt: IF LP Exp RP Stmt ELSE Stmt  */
#line 169 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Stmt",(yyloc).first_line,7,(yyvsp[-6].node),(yyvsp[-5].node),(yyvsp[-4].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1634 "./syntax.tab.c"
    break;

  case 40: /* Stmt: WHILE LP Exp RP Stmt  */
#line 170 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Stmt",(yyloc).first_line,5,(yyvsp[-4].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-2].node),(yyvsp[-1].node)); }
#line 1640 "./syntax.tab.c"
    break;

  case 41: /* Stmt: error SEMI  */
#line 171 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Stmt"); }
#line 1646 "./syntax.tab.c"
    break;

  case 42: /* Stmt: Exp error  */
#line 172 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Stmt"); }
#line 1652 "./syntax.tab.c"
    break;

  case 43: /* Stmt: IF error Stmt  */
#line 173 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Stmt"); }
#line 1658 "./syntax.tab.c"
    break;

  case 44: /* Stmt: IF LP Exp RP Stmt ELSE error  */
#line 174 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Stmt"); }
#line 1664 "./syntax.tab.c"
    break;

  case 45: /* Stmt: RETURN error SEMI  */
#line 175 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Stmt"); }
#line 1670 "./syntax.tab.c"
    break;

  case 46: /* Stmt: RETURN Exp error  */
#line 176 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Stmt"); }
#line 1676 "./syntax.tab.c"
    break;

  case 47: /* DefList: Def DefList  */
#line 181 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"DefList",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1682 "./syntax.tab.c"
    break;

  case 48: /* DefList: %empty  */
#line 182 "./syntax.y"
                                    { (yyval.node) = NULL; }
#line 1688 "./syntax.tab.c"
    break;

  case 49: /* Def: Specifier DecList SEMI  */
#line 184 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Def",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1694 "./syntax.tab.c"
    break;

  case 50: /* Def: Specifier error SEMI  */
#line 185 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Def,missing ;"); }
#line 1700 "./syntax.tab.c"
    break;

  case 51: /* Def: Specifier DecList error SEMI  */
#line 186 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Def,missing ; 3"); }
#line 1706 "./syntax.tab.c"
    break;

  case 52: /* DecList: Dec  */
#line 188 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"DecList",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1712 "./syntax.tab.c"
    break;

  case 53: /* DecList: Dec COMMA DecList  */
#line 189 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"DecList",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1718 "./syntax.tab.c"
    break;

  case 54: /* Dec: VarDec  */
#line 191 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Dec",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1724 "./syntax.tab.c"
    break;

  case 55: /* Dec: VarDec ASSIGNOP Exp  */
#line 192 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Dec",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1730 "./syntax.tab.c"
    break;

  case 56: /* Exp: Exp ASSIGNOP Exp  */
#line 197 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1736 "./syntax.tab.c"
    break;

  case 57: /* Exp: Exp AND Exp  */
#line 198 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1742 "./syntax.tab.c"
    break;

  case 58: /* Exp: Exp OR Exp  */
#line 199 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1748 "./syntax.tab.c"
    break;

  case 59: /* Exp: Exp RELOP Exp  */
#line 200 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1754 "./syntax.tab.c"
    break;

  case 60: /* Exp: Exp PLUS Exp  */
#line 201 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1760 "./syntax.tab.c"
    break;

  case 61: /* Exp: Exp MINUS Exp  */
#line 202 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1766 "./syntax.tab.c"
    break;

  case 62: /* Exp: Exp STAR Exp  */
#line 203 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1772 "./syntax.tab.c"
    break;

  case 63: /* Exp: Exp DIV Exp  */
#line 204 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1778 "./syntax.tab.c"
    break;

  case 64: /* Exp: LP Exp RP  */
#line 205 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1784 "./syntax.tab.c"
    break;

  case 65: /* Exp: MINUS Exp  */
#line 206 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1790 "./syntax.tab.c"
    break;

  case 66: /* Exp: NOT Exp  */
#line 207 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,2,(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1796 "./syntax.tab.c"
    break;

  case 67: /* Exp: ID LP Args RP  */
#line 208 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,4,(yyvsp[-3].node),(yyvsp[-3].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1802 "./syntax.tab.c"
    break;

  case 68: /* Exp: ID LP RP  */
#line 209 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1808 "./syntax.tab.c"
    break;

  case 69: /* Exp: Exp LB Exp RB  */
#line 210 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,4,(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1814 "./syntax.tab.c"
    break;

  case 70: /* Exp: Exp DOT ID  */
#line 211 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1820 "./syntax.tab.c"
    break;

  case 71: /* Exp: ID  */
#line 212 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1826 "./syntax.tab.c"
    break;

  case 72: /* Exp: INT  */
#line 213 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1832 "./syntax.tab.c"
    break;

  case 73: /* Exp: FLOAT  */
#line 214 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Exp",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1838 "./syntax.tab.c"
    break;

  case 74: /* Exp: Exp ASSIGNOP error  */
#line 215 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1844 "./syntax.tab.c"
    break;

  case 75: /* Exp: Exp RELOP error  */
#line 216 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1850 "./syntax.tab.c"
    break;

  case 76: /* Exp: Exp PLUS error  */
#line 217 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1856 "./syntax.tab.c"
    break;

  case 77: /* Exp: Exp MINUS error  */
#line 218 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1862 "./syntax.tab.c"
    break;

  case 78: /* Exp: Exp STAR error  */
#line 219 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1868 "./syntax.tab.c"
    break;

  case 79: /* Exp: Exp DIV error  */
#line 220 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1874 "./syntax.tab.c"
    break;

  case 80: /* Exp: Exp AND error  */
#line 221 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1880 "./syntax.tab.c"
    break;

  case 81: /* Exp: Exp OR error  */
#line 222 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1886 "./syntax.tab.c"
    break;

  case 82: /* Exp: MINUS error  */
#line 223 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1892 "./syntax.tab.c"
    break;

  case 83: /* Exp: NOT error  */
#line 224 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1898 "./syntax.tab.c"
    break;

  case 84: /* Exp: ID LP error RP  */
#line 225 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1904 "./syntax.tab.c"
    break;

  case 85: /* Exp: Exp LB error RB  */
#line 226 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1910 "./syntax.tab.c"
    break;

  case 86: /* Exp: Exp DOT error  */
#line 227 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Exp"); }
#line 1916 "./syntax.tab.c"
    break;

  case 87: /* Args: Exp COMMA Args  */
#line 229 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Args",(yyloc).first_line,3,(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)); }
#line 1922 "./syntax.tab.c"
    break;

  case 88: /* Args: Exp  */
#line 230 "./syntax.y"
                                    { (yyval.node) = Operator((yyval.node),"Args",(yyloc).first_line,1,(yyvsp[0].node)); }
#line 1928 "./syntax.tab.c"
    break;

  case 89: /* Args: error Args  */
#line 231 "./syntax.y"
                                    { (yyval.node) = NULL; yyerror("Wrong Args"); }
#line 1934 "./syntax.tab.c"
    break;


#line 1938 "./syntax.tab.c"

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
      yyerror (YY_("syntax error"));
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
  yyerror (YY_("memory exhausted"));
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

  return yyresult;
}

#line 234 "./syntax.y"


