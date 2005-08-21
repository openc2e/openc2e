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
/* Line 1285 of yacc.c.  */
#line 119 "mngparser.tab.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE mnglval;



