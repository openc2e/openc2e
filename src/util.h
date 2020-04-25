#ifndef UTIL_H
#define UTIL_H 1

#include <string>
#include <iosfwd>

std::string readfile(const std::string &filename);
std::string readfile(std::istream &i);

#endif
