#include "lexer.h"
#include <map>

/* Warning! lexer.cpp and lexer.cpp are VERY heavy files (~350mb to build).
 * Please avoid adding any unnecessary dependencies to either, to avoid
 * needless rebuilds on memory-starved platforms.
 */
using namespace boost::xpressive;
sregex c2caos_re, cecaos_re;

std::map<std::string, std::string> condmap;

#define NEWLINE (s5= as_xpr('\n'))
#define COMMENT (as_xpr('*') >> -*_ >> (eos | before(NEWLINE)))
#define SEPSPACE (+(set= ' ', ',', '\r', '\t') | COMMENT | NEWLINE)
#define WORDINIT (alpha | '_')
#define WORDANY (WORDINIT | '#' | digit | ':' | '?' | '!' | '_' | '+' | '-')

#define CONDITION ( \
			as_xpr('<') >> '>' | \
			((set= '<', '>', '=') >> !as_xpr('=')) | \
			as_xpr('&') >> '&' | \
			as_xpr('|') >> '|')

#define CHARLIT (s1= (as_xpr('\'') >> _ >> '\''))
#define BINVAL (s1= (as_xpr('%') >> *(as_xpr('0') | '1')))
#define WORD (s2= (as_xpr('f') >> '*' >> '*' >> 'k') | (WORDINIT >> *WORDANY) | CONDITION)

#define FLOATVAL (s3= !(set= '-','+') >> *digit >> '.' >> *digit)
#define INTVAL (s4= !(set= '-','+') >> +digit)
#define BYTESTR ((s1= as_xpr('[')) >> *(bytestr_part | space) >> as_xpr(']'))
#define QSTRING ((s1= as_xpr('"')) >> *qstring_part >> as_xpr('"'))
#define BRSTRING ((s2= as_xpr('[')) >> (s3= *( _ >> ~after(as_xpr(']')))) >> as_xpr(']'))

#define BYTESTR_PART (s1= +digit)
#define QSTRING_PART (s1= ~(set= '"', '\\') | (as_xpr('\\') >> _))

#define COMMON_CONSTANT (CHARLIT | BINVAL | FLOATVAL | INTVAL)

#define C2CAOS (SEPSPACE | COMMON_CONSTANT | WORD | QSTRING | BRSTRING)
#define CECAOS (SEPSPACE | COMMON_CONSTANT | WORD | QSTRING | BYTESTR)

void init_caos_re() {
    sregex bytestr_part = BYTESTR_PART;
    sregex qstring_part = QSTRING_PART;

    c2caos_re = C2CAOS;
    cecaos_re = CECAOS;

    condmap["<>"] = "ne";
    condmap["<"]  = "lt";
    condmap[">"]  = "gt";
    condmap[">="] = "ge";
    condmap["<="] = "le";
    condmap["="]  = "eq";
    condmap["=="] = "eq";
    condmap["&&"] = "and";
    condmap["||"] = "or";

}

