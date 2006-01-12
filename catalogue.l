%option noyywrap
%option nodefault
%option yylineno
%x CAT_STRING
    #include "Catalogue.h"
    #include <cstring>
    #include "catalogue.tab.hpp"
    #include "openc2e.h"
    static std::string cat_str;

    extern void catalogueParseError(int lineno);
%%
\#[^\n\r]*[\n\r]*   // comment
\*[^\n\r]*[\n\r]*   // comment
TAG                 return CTOK_TAG;
ARRAY               return CTOK_ARRAY;
OVERRIDE            return CTOK_OVERRIDE;
[0-9]+              { catalval.number = atoi(yytext); return CTOK_INT; }
\"                  cat_str = ""; BEGIN(CAT_STRING);
[ \t\n\r]           // yum, whitespace

<CAT_STRING>\\.     cat_str += catalogue_descape(yytext[1]);
<CAT_STRING>\"      BEGIN(INITIAL); catalval.string = new std::string(cat_str); return CTOK_STR;
<CAT_STRING>.       cat_str += yytext[0];

.                   catalogueParseError(yylineno);
