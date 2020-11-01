// vim: set ft=cpp noet ts=4 sw=4 :

// make sure we have the header imports that bison's horrible .h file needs
#include <string>
#include <list>

#include "Catalogue.h"
#include "catalogparser.h"
#include <cctype>
#include <cassert>

const char *Catalogue::catalogue_parse_p = NULL;
int Catalogue::yylineno = -1;

void Catalogue::yyinit(const char *buf) {
	catalogue_parse_p = buf;
	yylineno = 1;
}

int Catalogue::catalex() {
	const char *basep;
	const char *YYMARKER;
	assert(catalogue_parse_p);
start:
	basep = catalogue_parse_p;
	
	/*!re2c
re2c:define:YYCTYPE = "unsigned char";
re2c:define:YYCURSOR = catalogue_parse_p;
re2c:yyfill:enable = 0;
re2c:yych:conversion = 1;
re2c:indent:top = 1;

any			= [\001-\377];
eoi			= [\000];
comment		= [#*][^\r\n\000]*;
whitespace  = ( comment | [ \t\r] ) *;
newline		= [\n];
tag			= "TAG";
array		= "ARRAY";
override	= "OVERRIDE";
number		= [0-9]+;
quote		= ["];
backslash   = [\\];

eoi { goto eoi; }
whitespace { goto start; }
newline { yylineno++; goto start; }
tag { return CTOK_TAG; }
array { return CTOK_ARRAY; }
override { return CTOK_OVERRIDE; }
number { catalval.number = atoi(basep); return CTOK_INT; }
quote { goto str; }
any { catalogueParseError("Unexpected character"); }
*/
str:
	{
		std::string temp;
strloop:
		basep = catalogue_parse_p;

/*!re2c
	backslash any { temp += catalogue_descape(basep[1]); goto strloop; }
	quote { catalval.string = temp; return CTOK_STR; }
	backslash ? eoi { catalogueParseError("premature end of input"); }
	newline { temp += basep[0]; yylineno++; goto strloop; }
	any { temp += basep[0]; goto strloop; }
*/
	}
eoi:
	{
		catalogue_parse_p = NULL;
		return 0;
	}
}
