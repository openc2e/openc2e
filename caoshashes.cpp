#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,1,0,47,0,28,0,4,10,4,61,0,10,28,61,0,
0,0,0,4,4,0,0,0,0,42,0,0,0,0,10,47,
0,61,47,1,10,61,28,0,1,42,38,38,0,16,61,42,
0,61,0,47,0,0,98,10,46,28,0,47,4,55,86,0,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xba721f9a;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 2) & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
47,47,0,0,37,28,28,34,1,0,4,47,4,0,1,32,
1,47,0,0,1,0,0,10,0,32,0,61,4,0,0,4,
0,28,47,10,34,51,16,0,0,0,26,34,47,34,0,0,
0,42,61,54,16,60,0,28,85,0,47,4,0,95,0,0,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x23c282f5;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 3) & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_LITTLEENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
0,0,1,18,13,16,24,0,14,31,9,3,7,23,30,0,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x8d12e650;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 14) & 0xf;
  a = (val + (val << 1)) >> 28;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
21,7,0,13,1,7,0,22,16,0,6,11,28,31,26,7,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x292a2a2;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 11) & 0xf;
  a = (val + (val << 1)) >> 28;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
