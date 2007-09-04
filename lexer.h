#ifndef LEXER_H
#define LEXER_H 1

#include <boost/xpressive/xpressive.hpp>
#include <map>
#include <string>

/* Warning! lexer.cpp and lexer.cpp are VERY heavy files (~350mb to build).
 * Please avoid adding any unnecessary dependencies to either, to avoid
 * needless rebuilds on memory-starved platforms.
 */

extern boost::xpressive::sregex c2caos_re, cecaos_re;
class token decodeToken(const boost::xpressive::smatch &mdata); /* actually in lexutil.cpp */
void init_caos_re();
extern std::map<std::string, std::string> condmap;

#endif

