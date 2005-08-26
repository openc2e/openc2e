%{
#include <math.h>
#include <stdio.h>
#include <iostream>
//#include "token.h"
#include "lexutil.h"
%}

%option noyywrap
%option nodefault
%option yylineno
%x L_BYTESTR
%x L_STRING
%x L_COMMENT

STRINGBIT   \\.|[^\\]
DIGIT       [0-9]
WORDINIT    [a-zA-Z_]
WORDANY     [a-zA-Z0-9:?!_+-]
%%

\*[^\r\n]*

\'.\'       lex_lineno = yylineno; return make_int(yytext[1]);
%[01]*                 lex_lineno = yylineno; return make_bin(yytext);
{WORDINIT}{WORDANY}*                lex_lineno = yylineno; return make_word(yytext);
[-+]?{DIGIT}*\.{DIGIT}+ lex_lineno = yylineno;  return make_float(atof(yytext));
[-+]?{DIGIT}+           lex_lineno = yylineno;  return make_int(atoi(yytext));

"<>"                     lex_lineno = yylineno;  return make_word("ne");
"<="                     lex_lineno = yylineno;  return make_word("le");
">="                     lex_lineno = yylineno;  return make_word("ge");
"<"                      lex_lineno = yylineno;  return make_word("lt");
">"                      lex_lineno = yylineno;  return make_word("gt");
"="                      lex_lineno = yylineno;  return make_word("eq");
"&&"                     lex_lineno = yylineno;  return make_word("and");
"||"                     lex_lineno = yylineno;  return make_word("or");

"["         BEGIN(L_BYTESTR);

<L_BYTESTR>[0-9]+         push_bytestr(atoi(yytext));
<L_BYTESTR>[ \t\n\r]
<L_BYTESTR>"]"            BEGIN(INITIAL); lex_lineno = yylineno; return make_bytestr();

\"          BEGIN(L_STRING);
<L_STRING>\\.             push_string_escape(yytext[1]);
<L_STRING>\"              BEGIN(INITIAL); lex_lineno = yylineno; return make_string();
<L_STRING>.               push_string_lit(yytext[0]);

[ \t\n\r]     // eat whitespace
.                         parse_error(yytext, yylineno);
%%

