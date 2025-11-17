#pragma once

class Reader;

// I don't know what to call this file. It provides
// functionality similar to istream::operator>>(int)
// and istream::operator>>(float).

int scan_int(Reader&);
float scan_float(Reader& r);