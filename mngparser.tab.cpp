/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse mngparse
#define yylex   mnglex
#define yyerror mngerror
#define yylval  mnglval
#define yychar  mngchar
#define yydebug mngdebug
#define yynerrs mngnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MNG_Variable = 258,
     MNG_Effect = 259,
     MNG_Track = 260,
     MNG_Stage = 261,
     MNG_Pan = 262,
     MNG_Volume = 263,
     MNG_Delay = 264,
     MNG_TempoDelay = 265,
     MNG_Random = 266,
     MNG_FadeIn = 267,
     MNG_FadeOut = 268,
     MNG_BeatLength = 269,
     MNG_AleotoricLayer = 270,
     MNG_LoopLayer = 271,
     MNG_Update = 272,
     MNG_Add = 273,
     MNG_Subtract = 274,
     MNG_Multiply = 275,
     MNG_Divide = 276,
     MNG_SineWave = 277,
     MNG_CosineWave = 278,
     MNG_Voice = 279,
     MNG_Interval = 280,
     MNG_Condition = 281,
     MNG_BeatSynch = 282,
     MNG_UpdateRate = 283,
     MNG_Wave = 284,
     MNG_number = 285,
     MNG_name = 286,
     MNG_comment = 287
   };
#endif
#define MNG_Variable 258
#define MNG_Effect 259
#define MNG_Track 260
#define MNG_Stage 261
#define MNG_Pan 262
#define MNG_Volume 263
#define MNG_Delay 264
#define MNG_TempoDelay 265
#define MNG_Random 266
#define MNG_FadeIn 267
#define MNG_FadeOut 268
#define MNG_BeatLength 269
#define MNG_AleotoricLayer 270
#define MNG_LoopLayer 271
#define MNG_Update 272
#define MNG_Add 273
#define MNG_Subtract 274
#define MNG_Multiply 275
#define MNG_Divide 276
#define MNG_SineWave 277
#define MNG_CosineWave 278
#define MNG_Voice 279
#define MNG_Interval 280
#define MNG_Condition 281
#define MNG_BeatSynch 282
#define MNG_UpdateRate 283
#define MNG_Wave 284
#define MNG_number 285
#define MNG_name 286
#define MNG_comment 287




/* Copy the first part of user declarations.  */
#line 1 "mngparser.ypp"

	extern char *mngtext;
	extern void mngerror(char const *);
	#include "lex.mng.h"
	extern mngFlexLexer *mnglexer;
	int mnglex(void) { return mnglexer->yylex(); }
	#include <stdio.h>
	#include "mngfile.h"
	#include <list>


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 13 "mngparser.ypp"
typedef union YYSTYPE {
	float number;
	char *string;
	class MNGNode *node;
	class MNGExpression *expr;
	class MNGExpressionContainer *container;
	class MNGVariableNode *variable;
	class MNGVariableDecNode *variabledec;
	class MNGTrackDecNode *trackdec;
	class MNGEffectDecNode *effectdec;
	class MNGAssignmentNode *assignment;
	class MNGStageNode *stage;
	class MNGLayer *layer;
	std::list<class MNGAssignmentNode *> *assignmentarray;
	std::list<class MNGNode *> *array;
	std::list<class MNGStageNode *> *stagearray;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 178 "mngparser.tab.cpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 190 "mngparser.tab.cpp"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   255

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  47
/* YYNRULES -- Number of rules. */
#define YYNRULES  99
/* YYNRULES -- Number of states. */
#define YYNSTATES  222

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   287

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      33,    35,     2,     2,    34,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    38,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,     2,    37,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    16,    23,
      31,    33,    36,    41,    43,    45,    48,    50,    52,    54,
      56,    58,    63,    68,    73,    78,    83,    90,    98,   100,
     103,   105,   107,   109,   111,   113,   115,   117,   122,   127,
     132,   140,   148,   150,   153,   155,   158,   160,   162,   164,
     166,   168,   170,   172,   174,   176,   178,   180,   182,   184,
     186,   188,   190,   195,   200,   202,   205,   209,   211,   213,
     215,   217,   219,   221,   223,   225,   227,   229,   231,   233,
     235,   237,   244,   251,   258,   265,   272,   279,   284,   286,
     289,   291,   293,   295,   297,   299,   304,   313,   318,   323
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      40,     0,    -1,    41,    -1,    40,    41,    -1,    43,    -1,
      54,    -1,    42,    -1,    32,    -1,     3,    33,    31,    34,
      71,    35,    -1,     4,    33,    31,    35,    36,    44,    37,
      -1,    45,    -1,    44,    45,    -1,     6,    36,    46,    37,
      -1,    32,    -1,    47,    -1,    46,    47,    -1,    48,    -1,
      49,    -1,    51,    -1,    52,    -1,    32,    -1,     7,    33,
      71,    35,    -1,     8,    33,    71,    35,    -1,     8,    33,
      71,    35,    -1,     9,    33,    71,    35,    -1,    10,    33,
      71,    35,    -1,    11,    33,    71,    34,    71,    35,    -1,
       5,    33,    31,    35,    36,    55,    37,    -1,    56,    -1,
      55,    56,    -1,    60,    -1,    61,    -1,    57,    -1,    58,
      -1,    59,    -1,    50,    -1,    32,    -1,    12,    33,    71,
      35,    -1,    13,    33,    71,    35,    -1,    14,    33,    71,
      35,    -1,    15,    33,    31,    35,    36,    62,    37,    -1,
      16,    33,    31,    35,    36,    63,    37,    -1,    64,    -1,
      62,    64,    -1,    65,    -1,    63,    65,    -1,    66,    -1,
      32,    -1,    50,    -1,    42,    -1,    67,    -1,    78,    -1,
      83,    -1,    84,    -1,    81,    -1,    32,    -1,    50,    -1,
      42,    -1,    67,    -1,    83,    -1,    85,    -1,    81,    -1,
       4,    33,    31,    35,    -1,    17,    36,    68,    37,    -1,
      69,    -1,    68,    69,    -1,    70,    38,    71,    -1,    32,
      -1,    31,    -1,    25,    -1,     8,    -1,     7,    -1,    72,
      -1,    73,    -1,    74,    -1,    75,    -1,    76,    -1,    77,
      -1,    53,    -1,    70,    -1,    30,    -1,    18,    33,    71,
      34,    71,    35,    -1,    19,    33,    71,    34,    71,    35,
      -1,    20,    33,    71,    34,    71,    35,    -1,    21,    33,
      71,    34,    71,    35,    -1,    22,    33,    71,    34,    71,
      35,    -1,    23,    33,    71,    34,    71,    35,    -1,    24,
      36,    79,    37,    -1,    80,    -1,    79,    80,    -1,    85,
      -1,    81,    -1,    66,    -1,    82,    -1,    67,    -1,    25,
      33,    71,    35,    -1,    26,    33,    70,    34,    30,    34,
      30,    35,    -1,    27,    33,    71,    35,    -1,    28,    33,
      71,    35,    -1,    29,    33,    31,    35,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    77,    77,    78,    81,    82,    83,    84,    87,    90,
      93,    94,    97,    98,   101,   102,   105,   106,   107,   108,
     109,   112,   115,   118,   121,   124,   127,   130,   133,   134,
     137,   138,   139,   140,   141,   142,   143,   146,   149,   152,
     155,   158,   161,   162,   165,   166,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   180,   181,   182,   183,   184,
     185,   186,   189,   192,   195,   196,   199,   200,   203,   204,
     205,   206,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   220,   223,   226,   229,   232,   235,   238,   241,   242,
     245,   246,   247,   248,   249,   252,   255,   258,   261,   264
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MNG_Variable", "MNG_Effect",
  "MNG_Track", "MNG_Stage", "MNG_Pan", "MNG_Volume", "MNG_Delay",
  "MNG_TempoDelay", "MNG_Random", "MNG_FadeIn", "MNG_FadeOut",
  "MNG_BeatLength", "MNG_AleotoricLayer", "MNG_LoopLayer", "MNG_Update",
  "MNG_Add", "MNG_Subtract", "MNG_Multiply", "MNG_Divide", "MNG_SineWave",
  "MNG_CosineWave", "MNG_Voice", "MNG_Interval", "MNG_Condition",
  "MNG_BeatSynch", "MNG_UpdateRate", "MNG_Wave", "MNG_number", "MNG_name",
  "MNG_comment", "'('", "','", "')'", "'{'", "'}'", "'='", "$accept",
  "statementlist", "statement", "variabledec", "effectdec", "stagelist",
  "stage", "stagesettinglist", "stagesetting", "pan", "effectvolume",
  "layervolume", "delay", "tempodelay", "random", "trackdec", "track",
  "tracksetting", "fadein", "fadeout", "beatlength", "aleotoriclayerdec",
  "looplayerdec", "aleotoriclayer", "looplayer", "aleotoriclayercommand",
  "looplayercommand", "effect", "updateblock", "assignmentlist",
  "assignment", "variable", "expression", "add", "subtract", "multiply",
  "divide", "sinewave", "cosinewave", "voiceblock", "voicecommands",
  "voicecommand", "interval", "condition", "beatsynch", "updaterate",
  "wave", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,    40,    44,    41,   123,   125,    61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    39,    40,    40,    41,    41,    41,    41,    42,    43,
      44,    44,    45,    45,    46,    46,    47,    47,    47,    47,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    55,
      56,    56,    56,    56,    56,    56,    56,    57,    58,    59,
      60,    61,    62,    62,    63,    63,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    65,    65,    65,    65,    65,
      65,    65,    66,    67,    68,    68,    69,    69,    70,    70,
      70,    70,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    79,
      80,    80,    80,    80,    80,    81,    82,    83,    84,    85
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     6,     7,
       1,     2,     4,     1,     1,     2,     1,     1,     1,     1,
       1,     4,     4,     4,     4,     4,     6,     7,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     4,     4,     4,
       7,     7,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     1,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     6,     6,     6,     6,     6,     4,     1,     2,
       1,     1,     1,     1,     1,     4,     8,     4,     4,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,     7,     0,     2,     6,     4,     5,
       0,     0,     0,     1,     3,     0,     0,     0,     0,     0,
       0,    71,    70,     0,     0,     0,     0,     0,     0,     0,
      69,    80,    68,    78,    79,     0,    72,    73,    74,    75,
      76,    77,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     8,     0,    13,     0,    10,     0,     0,     0,     0,
       0,     0,    36,    35,     0,    28,    32,    33,    34,    30,
      31,     0,     0,     0,     0,     0,     0,     0,     0,     9,
      11,     0,     0,     0,     0,     0,     0,    27,    29,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      20,     0,    14,    16,    17,    18,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    12,    15,    23,    37,    38,    39,
       0,     0,    26,    81,    82,    83,    84,    85,    86,     0,
       0,     0,     0,     0,     0,    21,    22,    24,    25,     0,
       0,     0,     0,     0,     0,    47,    49,    48,     0,    42,
      46,    50,    51,    54,    52,    53,     0,    55,    57,    56,
       0,    44,    58,    61,    59,    60,     0,     0,     0,     0,
       0,     0,    40,    43,     0,    41,    45,     0,    67,     0,
      64,     0,     0,    92,    94,     0,    88,    91,    93,    90,
       0,     0,     0,     0,    62,    63,    65,     0,     0,    87,
      89,    95,    97,    98,    99,    66,     0,     0,     0,     0,
       0,    96
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     5,     6,     7,     8,    54,    55,   101,   102,   103,
     104,    63,   105,   106,    33,     9,    64,    65,    66,    67,
      68,    69,    70,   158,   170,   159,   171,   160,   161,   189,
     190,    34,    35,    36,    37,    38,    39,    40,    41,   162,
     195,   196,   163,   198,   164,   165,   175
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -166
static const short int yypact[] =
{
      24,   -24,   -22,   -19,  -166,    12,  -166,  -166,  -166,  -166,
     -13,   -12,   -10,  -166,  -166,   -14,    19,    27,   165,    28,
      32,  -166,  -166,    -8,    30,    36,    39,    40,    42,    50,
    -166,  -166,  -166,  -166,  -166,    56,  -166,  -166,  -166,  -166,
    -166,  -166,    61,   185,   165,   165,   165,   165,   165,   165,
     165,  -166,    67,  -166,    20,  -166,    87,   111,   115,   116,
     117,   119,  -166,  -166,    93,  -166,  -166,  -166,  -166,  -166,
    -166,    78,   120,   127,   128,   134,   135,   136,   157,  -166,
    -166,   165,   165,   165,   165,    -9,   129,  -166,  -166,   165,
     165,   165,   165,   165,   165,   165,   138,   142,   144,   145,
    -166,    89,  -166,  -166,  -166,  -166,  -166,   147,   159,   167,
     168,   169,   170,   171,   172,   173,   175,   176,   177,   178,
     165,   165,   165,   165,  -166,  -166,  -166,  -166,  -166,  -166,
     143,   180,  -166,  -166,  -166,  -166,  -166,  -166,  -166,   179,
     183,   186,   187,   114,   126,  -166,  -166,  -166,  -166,   190,
     188,   189,   193,   194,   195,  -166,  -166,  -166,    57,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,   196,  -166,  -166,  -166,
      63,  -166,  -166,  -166,  -166,  -166,   199,   149,    98,   165,
     165,   165,  -166,  -166,   200,  -166,  -166,   197,  -166,    79,
    -166,   181,   201,  -166,  -166,     6,  -166,  -166,  -166,  -166,
     198,   202,   203,   204,  -166,  -166,  -166,   165,   184,  -166,
    -166,  -166,  -166,  -166,  -166,  -166,   206,   205,   207,   212,
     208,  -166
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -166,  -166,   215,   -30,  -166,  -166,   182,  -166,   146,  -166,
    -166,   -11,  -166,  -166,  -166,  -166,  -166,   191,  -166,  -166,
    -166,  -166,  -166,  -166,  -166,    86,    75,  -165,  -137,  -166,
      59,  -153,   -44,  -166,  -166,  -166,  -166,  -166,  -166,  -166,
    -166,    51,  -136,  -166,   -25,  -166,  -125
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      71,    72,    73,    74,    75,    76,    77,   172,   173,    10,
     149,    11,    13,   193,    12,     1,     2,     3,    15,    16,
      18,    17,   111,   150,   191,    44,    52,     1,     2,     3,
     193,   152,   192,   172,   173,   166,   191,   107,   108,   109,
     110,   194,   197,   209,     4,   113,   114,   115,   116,   117,
     118,   119,    53,   199,    19,   216,     4,    79,   194,   197,
       1,   149,    20,    45,    42,    56,     1,    52,    43,    46,
     199,    56,    47,    48,   150,    49,   139,   140,   141,   142,
     150,   151,   152,    50,   153,   154,    21,    22,   152,   155,
     153,    51,   166,    53,   182,   167,    96,    97,    98,    99,
     185,    56,   149,    78,    30,    57,    58,    59,    60,    61,
      32,   188,    89,   156,   168,   150,   205,     1,   149,   174,
      81,   100,    56,   152,   192,    62,   124,   166,   156,     1,
      87,   150,   157,   169,    56,   200,   201,   202,   151,   152,
     168,   153,   154,   150,    82,   174,   155,   157,    83,    84,
      85,   152,    86,   153,    90,   166,    21,    22,   167,   169,
     112,    91,    92,   215,    96,    97,    98,    99,    93,    94,
      95,   120,    21,    22,    30,   121,    23,   122,   123,   143,
      32,   188,   126,    24,    25,    26,    27,    28,    29,   100,
      30,    21,    22,    56,   127,    31,    32,    57,    58,    59,
      60,    61,   128,   129,   130,   131,   132,   133,   134,    30,
     135,   136,   137,   138,   145,    32,   144,    62,   146,   207,
      14,   147,   148,   176,   177,   178,   179,   180,   181,   184,
     187,   203,   204,   211,   208,   218,    80,   212,   213,   214,
     217,   219,   220,   221,   183,   186,   210,   125,   206,     0,
       0,     0,     0,     0,     0,    88
};

static const short int yycheck[] =
{
      44,    45,    46,    47,    48,    49,    50,   144,   144,    33,
       4,    33,     0,   178,    33,     3,     4,     5,    31,    31,
      34,    31,    31,    17,   177,    33,     6,     3,     4,     5,
     195,    25,    26,   170,   170,    29,   189,    81,    82,    83,
      84,   178,   178,    37,    32,    89,    90,    91,    92,    93,
      94,    95,    32,   178,    35,   208,    32,    37,   195,   195,
       3,     4,    35,    33,    36,     8,     3,     6,    36,    33,
     195,     8,    33,    33,    17,    33,   120,   121,   122,   123,
      17,    24,    25,    33,    27,    28,     7,     8,    25,    32,
      27,    35,    29,    32,    37,    32,     7,     8,     9,    10,
      37,     8,     4,    36,    25,    12,    13,    14,    15,    16,
      31,    32,    34,   143,   144,    17,    37,     3,     4,   144,
      33,    32,     8,    25,    26,    32,    37,    29,   158,     3,
      37,    17,   143,   144,     8,   179,   180,   181,    24,    25,
     170,    27,    28,    17,    33,   170,    32,   158,    33,    33,
      33,    25,    33,    27,    34,    29,     7,     8,    32,   170,
      31,    34,    34,   207,     7,     8,     9,    10,    34,    34,
      34,    33,     7,     8,    25,    33,    11,    33,    33,    36,
      31,    32,    35,    18,    19,    20,    21,    22,    23,    32,
      25,     7,     8,     8,    35,    30,    31,    12,    13,    14,
      15,    16,    35,    35,    35,    35,    35,    35,    35,    25,
      35,    35,    35,    35,    35,    31,    36,    32,    35,    38,
       5,    35,    35,    33,    36,    36,    33,    33,    33,    33,
      31,    31,    35,    35,    33,    30,    54,    35,    35,    35,
      34,    34,    30,    35,   158,   170,   195,   101,   189,    -1,
      -1,    -1,    -1,    -1,    -1,    64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,    32,    40,    41,    42,    43,    54,
      33,    33,    33,     0,    41,    31,    31,    31,    34,    35,
      35,     7,     8,    11,    18,    19,    20,    21,    22,    23,
      25,    30,    31,    53,    70,    71,    72,    73,    74,    75,
      76,    77,    36,    36,    33,    33,    33,    33,    33,    33,
      33,    35,     6,    32,    44,    45,     8,    12,    13,    14,
      15,    16,    32,    50,    55,    56,    57,    58,    59,    60,
      61,    71,    71,    71,    71,    71,    71,    71,    36,    37,
      45,    33,    33,    33,    33,    33,    33,    37,    56,    34,
      34,    34,    34,    34,    34,    34,     7,     8,     9,    10,
      32,    46,    47,    48,    49,    51,    52,    71,    71,    71,
      71,    31,    31,    71,    71,    71,    71,    71,    71,    71,
      33,    33,    33,    33,    37,    47,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    35,    35,    71,
      71,    71,    71,    36,    36,    35,    35,    35,    35,     4,
      17,    24,    25,    27,    28,    32,    42,    50,    62,    64,
      66,    67,    78,    81,    83,    84,    29,    32,    42,    50,
      63,    65,    67,    81,    83,    85,    33,    36,    36,    33,
      33,    33,    37,    64,    33,    37,    65,    31,    32,    68,
      69,    70,    26,    66,    67,    79,    80,    81,    82,    85,
      71,    71,    71,    31,    35,    37,    69,    38,    33,    37,
      80,    35,    35,    35,    35,    71,    70,    34,    30,    34,
      30,    35
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 77 "mngparser.ypp"
    { ;}
    break;

  case 3:
#line 78 "mngparser.ypp"
    { ;}
    break;

  case 4:
#line 81 "mngparser.ypp"
    { g_mngfile->add(yyvsp[0].effectdec); ;}
    break;

  case 5:
#line 82 "mngparser.ypp"
    { g_mngfile->add(yyvsp[0].trackdec); ;}
    break;

  case 6:
#line 83 "mngparser.ypp"
    { g_mngfile->add(yyvsp[0].variabledec); ;}
    break;

  case 7:
#line 84 "mngparser.ypp"
    { ;}
    break;

  case 8:
#line 87 "mngparser.ypp"
    { yyval.variabledec = new MNGVariableDecNode(yyvsp[-3].string, yyvsp[-1].expr); ;}
    break;

  case 9:
#line 90 "mngparser.ypp"
    { yyval.effectdec = new MNGEffectDecNode(yyvsp[-4].string); ((MNGEffectDecNode *)yyval.effectdec)->children = yyvsp[-1].stagearray; ;}
    break;

  case 10:
#line 93 "mngparser.ypp"
    { yyval.stagearray = new std::list<MNGStageNode *>; if (yyvsp[0].stage) yyval.stagearray->push_back(yyvsp[0].stage); ;}
    break;

  case 11:
#line 94 "mngparser.ypp"
    { yyval.stagearray = yyvsp[-1].stagearray; if (yyvsp[0].stage) yyval.stagearray->push_back(yyvsp[0].stage); ;}
    break;

  case 12:
#line 97 "mngparser.ypp"
    { yyval.stage = new MNGStageNode(); ((MNGStageNode *)yyval.stage)->children = yyvsp[-1].array; ;}
    break;

  case 13:
#line 98 "mngparser.ypp"
    { yyval.stage = 0; ;}
    break;

  case 14:
#line 101 "mngparser.ypp"
    { yyval.array = new std::list<MNGNode *>; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 15:
#line 102 "mngparser.ypp"
    { yyval.array = yyvsp[-1].array; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 16:
#line 105 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 17:
#line 106 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 18:
#line 107 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 19:
#line 108 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 20:
#line 109 "mngparser.ypp"
    { yyval.node = 0; ;}
    break;

  case 21:
#line 112 "mngparser.ypp"
    { yyval.container = new MNGPanNode(yyvsp[-1].expr); ;}
    break;

  case 22:
#line 115 "mngparser.ypp"
    { yyval.container = new MNGEffectVolumeNode(yyvsp[-1].expr); ;}
    break;

  case 23:
#line 118 "mngparser.ypp"
    { yyval.container = new MNGLayerVolumeNode(yyvsp[-1].expr); ;}
    break;

  case 24:
#line 121 "mngparser.ypp"
    { yyval.container = new MNGDelayNode(yyvsp[-1].expr); ;}
    break;

  case 25:
#line 124 "mngparser.ypp"
    { yyval.container = new MNGTempoDelayNode(yyvsp[-1].expr); ;}
    break;

  case 26:
#line 127 "mngparser.ypp"
    { yyval.expr = new MNGRandomNode(yyvsp[-3].expr, yyvsp[-1].expr); ;}
    break;

  case 27:
#line 130 "mngparser.ypp"
    { yyval.trackdec = new MNGTrackDecNode(yyvsp[-4].string); ((MNGTrackDecNode *)yyval.trackdec)->children = yyvsp[-1].array; ;}
    break;

  case 28:
#line 133 "mngparser.ypp"
    { yyval.array = new std::list<MNGNode *>; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 29:
#line 134 "mngparser.ypp"
    { yyval.array = yyvsp[-1].array; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 30:
#line 137 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 31:
#line 138 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 32:
#line 139 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 33:
#line 140 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 34:
#line 141 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 35:
#line 142 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 36:
#line 143 "mngparser.ypp"
    { yyval.node = 0; ;}
    break;

  case 37:
#line 146 "mngparser.ypp"
    { yyval.container = new MNGFadeInNode(yyvsp[-1].expr); ;}
    break;

  case 38:
#line 149 "mngparser.ypp"
    { yyval.container = new MNGFadeOutNode(yyvsp[-1].expr); ;}
    break;

  case 39:
#line 152 "mngparser.ypp"
    { yyval.container = new MNGBeatLengthNode(yyvsp[-1].expr); ;}
    break;

  case 40:
#line 155 "mngparser.ypp"
    { yyval.node = new MNGAleotoricLayerNode(yyvsp[-4].string); ((MNGAleotoricLayerNode *)yyval.node)->children = yyvsp[-1].array; ;}
    break;

  case 41:
#line 158 "mngparser.ypp"
    { yyval.node = new MNGLoopLayerNode(yyvsp[-4].string); ((MNGLoopLayerNode *)yyval.node)->children = yyvsp[-1].array; ;}
    break;

  case 42:
#line 161 "mngparser.ypp"
    { yyval.array = new std::list<MNGNode *>; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 43:
#line 162 "mngparser.ypp"
    { yyval.array = yyvsp[-1].array; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 44:
#line 165 "mngparser.ypp"
    { yyval.array = new std::list<MNGNode *>; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 45:
#line 166 "mngparser.ypp"
    { yyval.array = yyvsp[-1].array; if (yyvsp[0].node) yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 46:
#line 169 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 47:
#line 170 "mngparser.ypp"
    { yyval.node = 0; ;}
    break;

  case 48:
#line 171 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 49:
#line 172 "mngparser.ypp"
    { yyval.node = yyvsp[0].variabledec; ;}
    break;

  case 50:
#line 173 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 51:
#line 174 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 52:
#line 175 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 53:
#line 176 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 54:
#line 177 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 55:
#line 180 "mngparser.ypp"
    { yyval.node = 0; ;}
    break;

  case 56:
#line 181 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 57:
#line 182 "mngparser.ypp"
    { yyval.node = yyvsp[0].variabledec; ;}
    break;

  case 58:
#line 183 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 59:
#line 184 "mngparser.ypp"
    { yyval.node = yyvsp[0].container; ;}
    break;

  case 60:
#line 185 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 61:
#line 186 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 62:
#line 189 "mngparser.ypp"
    { yyval.node = new MNGEffectNode(yyvsp[-1].string); ;}
    break;

  case 63:
#line 192 "mngparser.ypp"
    { yyval.node = new MNGUpdateNode(); ((MNGUpdateNode *)yyval.node)->children = yyvsp[-1].assignmentarray; ;}
    break;

  case 64:
#line 195 "mngparser.ypp"
    { yyval.assignmentarray = new std::list<MNGAssignmentNode *>; if (yyvsp[0].assignment) yyval.assignmentarray->push_back(yyvsp[0].assignment); ;}
    break;

  case 65:
#line 196 "mngparser.ypp"
    { yyval.assignmentarray = yyvsp[-1].assignmentarray; if (yyvsp[0].assignment) yyval.assignmentarray->push_back(yyvsp[0].assignment); ;}
    break;

  case 66:
#line 199 "mngparser.ypp"
    { yyval.assignment = new MNGAssignmentNode(yyvsp[-2].variable, yyvsp[0].expr); ;}
    break;

  case 67:
#line 200 "mngparser.ypp"
    { yyval.assignment = 0; ;}
    break;

  case 68:
#line 203 "mngparser.ypp"
    { yyval.variable = new MNGVariableNode(yyvsp[0].string); ;}
    break;

  case 69:
#line 204 "mngparser.ypp"
    { yyval.variable = new MNGVariableNode(INTERVAL); ;}
    break;

  case 70:
#line 205 "mngparser.ypp"
    { yyval.variable = new MNGVariableNode(VOLUME); ;}
    break;

  case 71:
#line 206 "mngparser.ypp"
    { yyval.variable = new MNGVariableNode(PAN); ;}
    break;

  case 72:
#line 209 "mngparser.ypp"
    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 73:
#line 210 "mngparser.ypp"
    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 74:
#line 211 "mngparser.ypp"
    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 75:
#line 212 "mngparser.ypp"
    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 76:
#line 213 "mngparser.ypp"
    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 77:
#line 214 "mngparser.ypp"
    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 78:
#line 215 "mngparser.ypp"
    { yyval.expr = yyvsp[0].expr; ;}
    break;

  case 79:
#line 216 "mngparser.ypp"
    { yyval.expr = yyvsp[0].variable; ;}
    break;

  case 80:
#line 217 "mngparser.ypp"
    { yyval.expr = new MNGConstantNode(yyvsp[0].number); ;}
    break;

  case 81:
#line 220 "mngparser.ypp"
    { yyval.expr = new MNGAddNode(yyvsp[-3].expr, yyvsp[-1].expr); ;}
    break;

  case 82:
#line 223 "mngparser.ypp"
    { yyval.expr = new MNGSubtractNode(yyvsp[-3].expr, yyvsp[-1].expr); ;}
    break;

  case 83:
#line 226 "mngparser.ypp"
    { yyval.expr = new MNGMultiplyNode(yyvsp[-3].expr, yyvsp[-1].expr); ;}
    break;

  case 84:
#line 229 "mngparser.ypp"
    { yyval.expr = new MNGDivideNode(yyvsp[-3].expr, yyvsp[-1].expr); ;}
    break;

  case 85:
#line 232 "mngparser.ypp"
    { yyval.expr = new MNGSineWaveNode(yyvsp[-3].expr, yyvsp[-1].expr); ;}
    break;

  case 86:
#line 235 "mngparser.ypp"
    { yyval.expr = new MNGCosineWaveNode(yyvsp[-3].expr, yyvsp[-1].expr); ;}
    break;

  case 87:
#line 238 "mngparser.ypp"
    { yyval.node = new MNGVoiceNode(); ((MNGVoiceNode *)yyval.node)->children = yyvsp[-1].array; ;}
    break;

  case 88:
#line 241 "mngparser.ypp"
    { yyval.array = new std::list<MNGNode *>; yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 89:
#line 242 "mngparser.ypp"
    { yyval.array = yyvsp[-1].array; yyval.array->push_back(yyvsp[0].node); ;}
    break;

  case 90:
#line 245 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 91:
#line 246 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 92:
#line 247 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 93:
#line 248 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 94:
#line 249 "mngparser.ypp"
    { yyval.node = yyvsp[0].node; ;}
    break;

  case 95:
#line 252 "mngparser.ypp"
    { yyval.node = new MNGIntervalNode(yyvsp[-1].expr); ;}
    break;

  case 96:
#line 255 "mngparser.ypp"
    { yyval.node = new MNGConditionNode(yyvsp[-5].variable, yyvsp[-3].number, yyvsp[-1].number); ;}
    break;

  case 97:
#line 258 "mngparser.ypp"
    { yyval.container = new MNGUpdateRateNode(yyvsp[-1].expr); ;}
    break;

  case 98:
#line 261 "mngparser.ypp"
    { yyval.container = new MNGUpdateRateNode(yyvsp[-1].expr); ;}
    break;

  case 99:
#line 264 "mngparser.ypp"
    { yyval.node = new MNGWaveNode(yyvsp[-1].string); ;}
    break;


    }

/* Line 1010 of yacc.c.  */
#line 1763 "mngparser.tab.cpp"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
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

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}



