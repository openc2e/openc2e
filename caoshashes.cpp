#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,4,85,42,0,4,0,0,4,0,0,42,0,47,0,47,
1,0,47,0,16,20,70,5,76,61,0,0,61,47,28,60,
108,105,34,0,76,61,0,0,0,4,1,0,60,87,85,108,
42,87,62,17,43,0,0,70,17,66,0,96,0,10,110,99,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xac2ded62;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 10) & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
119,0,1,0,0,0,0,0,0,1,35,28,10,0,4,61,
0,37,0,108,0,76,0,38,0,70,123,0,32,0,101,61,
1,0,42,119,70,21,70,0,4,0,0,47,34,0,46,4,
0,76,20,0,0,47,125,16,60,70,47,56,105,0,57,89,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xf8f5ec4d;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = val & 0x3f;
  a = (val + (val << 2)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_LITTLEENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
0,22,16,51,0,9,28,49,0,0,0,0,0,36,51,0,
51,19,49,28,9,49,0,36,0,0,36,9,41,44,10,36,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x1e4f7f22;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 5) & 0x1f;
  a = (val + (val << 1)) >> 27;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
9,46,28,49,19,9,28,36,16,41,0,0,51,16,36,46,
37,10,0,49,50,0,59,49,0,60,19,19,40,28,0,0,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xd5b132b9;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 2) & 0x1f;
  a = (val + (val << 2)) >> 27;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
