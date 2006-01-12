%option noyywrap
%option nodefault
%option yylineno
	#include <list>
	#include "mngparser.tab.hpp"
%%
[\n\r ]	;
Variable	return MNG_Variable;
Effect		return MNG_Effect;
Track		return MNG_Track;
Stage		return MNG_Stage;
Pan		return MNG_Pan;
Volume		return MNG_Volume;
Delay		return MNG_Delay;
TempoDelay	return MNG_TempoDelay;
Random		return MNG_Random;
FadeIn		return MNG_FadeIn;
FadeOut		return MNG_FadeOut;
BeatLength	return MNG_BeatLength;
AleotoricLayer	return MNG_AleotoricLayer;
LoopLayer	return MNG_LoopLayer;
Update		return MNG_Update;
Add 		return MNG_Add;
Subtract	return MNG_Subtract;
Multiply 	return MNG_Multiply;
Divide		return MNG_Divide;
SineWave	return MNG_SineWave;
CosineWave	return MNG_CosineWave;
Voice		return MNG_Voice;
Interval 	return MNG_Interval;
Condition 	return MNG_Condition;
BeatSynch 	return MNG_BeatSynch;
UpdateRate 	return MNG_UpdateRate;
Wave 		return MNG_Wave;
[A-Za-z]([A-Za-z0-9])*	mnglval.string = strdup(yytext); return MNG_name;
-?[0-9]+\.[0-9]+	mnglval.number = atof(yytext); return MNG_number;
-?[0-9]+		mnglval.number = atof(yytext); return MNG_number;
[(){},=] return yytext[0];
\/\/.*	return MNG_comment;
