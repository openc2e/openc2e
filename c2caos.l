%{
#include <math.h>
#include <stdio.h>
#include <iostream>
//#include "token.h"
#include "lexutil.h"
#include "exceptions.h"

#define YY_FATAL_ERROR(str) throw parseException(str)
%}

%option noyywrap
%option nodefault
%option yylineno
%x L_BYTESTR
%x L_STRING
%x L_BRSTRING
%x L_COMMENT

STRINGBIT   \\.|[^\\]
DIGIT       [0-9]
WORDINIT    [a-zA-Z_]
WORDANY     [$#a-zA-Z0-9:?!_+-]
%%

f\*\*k	lex_lineno = yylineno; return make_word("f**k");

\*[^\r\n]*

\'.\'       lex_lineno = yylineno; return make_int(yytext[1]);
%[01]*                 lex_lineno = yylineno; return make_bin(yytext);
{WORDANY}*{WORDINIT}{WORDANY}*                lex_lineno = yylineno; return make_word(yytext);
[-+]?{DIGIT}+           lex_lineno = yylineno;  return make_int(atoi(yytext));

"<>"                     lex_lineno = yylineno;  return make_word("ne");
"<="                     lex_lineno = yylineno;  return make_word("le");
">="                     lex_lineno = yylineno;  return make_word("ge");
"<"                      lex_lineno = yylineno;  return make_word("lt");
">"                      lex_lineno = yylineno;  return make_word("gt");
"=="                     lex_lineno = yylineno;  return make_word("eq");
"="                      lex_lineno = yylineno;  return make_word("eq");
"&&"                     lex_lineno = yylineno;  return make_word("and");
"||"                     lex_lineno = yylineno;  return make_word("or");

"["          BEGIN(L_BRSTRING);
<L_BRSTRING>"]"              BEGIN(INITIAL); lex_lineno = yylineno; return make_string();
<L_BRSTRING>.               push_string_lit(yytext[0]);

\"          BEGIN(L_STRING);
<L_STRING>\\.             push_string_escape(yytext[1]);
<L_STRING>\"              BEGIN(INITIAL); lex_lineno = yylineno; return make_string();
<L_STRING>.               push_string_lit(yytext[0]);

[ ,\t\n\r]     // eat whitespace and commas (separating commands)
.                         parse_error(yytext, yylineno);
%%
