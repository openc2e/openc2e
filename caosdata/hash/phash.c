/* table for the mapping for the perfect hash */
#ifndef STANDARD
#include "standard.h"
#endif /* STANDARD */
#ifndef PHASH
#include "phash.h"
#endif /* PHASH */
#ifndef LOOKUPA
#include "lookupa.h"
#endif /* LOOKUPA */

/* small adjustments to _a_ to make values distinct */
ub2 tab[] = {
243,324,65,0,112,157,243,0,3,230,0,154,0,36,184,154,
0,65,243,328,162,243,6,234,7,65,6,174,243,28,157,157,
65,48,0,337,36,0,28,162,23,0,345,36,161,154,234,0,
234,155,201,0,6,162,230,324,112,3,65,112,154,0,0,155,
234,154,36,0,6,0,6,0,211,234,62,65,243,39,234,324,
26,174,0,28,154,26,161,329,0,161,249,264,155,243,341,344,
234,13,0,26,249,162,161,378,238,258,65,166,65,62,171,359,
0,167,328,184,157,174,154,154,0,28,365,159,222,111,243,369,
};

/* The hash function */
ub4 phash(val)
ub4 val;
{
  ub4 a, b, rsl;
  val += 0xacabb6c3;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 2) & 0x7f;
  a = (val + (val << 8)) >> 25;
  rsl = (a^tab[b]);
  return rsl;
}

