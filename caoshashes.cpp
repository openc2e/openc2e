#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,0,34,0,36,49,10,22,38,24,51,0,46,19,10,46,
49,51,10,28,0,33,0,7,0,53,51,19,46,49,38,0,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x7e032c28;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 3) & 0x1f;
  a = (val + (val << 3)) >> 27;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
9,19,0,0,0,0,51,7,0,37,58,0,19,36,61,51,
9,0,36,19,49,33,43,28,36,7,51,15,50,0,26,41,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xecc69356;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 7) & 0x1f;
  a = (val + (val << 2)) >> 27;
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
