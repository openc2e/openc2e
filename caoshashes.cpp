#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
4,76,10,0,0,5,0,0,4,76,61,4,119,34,16,0,
0,47,32,1,0,10,57,0,88,57,79,26,123,105,42,47,
1,0,83,119,76,119,0,85,76,16,123,0,92,0,0,0,
116,119,0,84,70,43,76,1,0,106,86,0,74,1,0,87,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xaa191421;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 11) & 0x3f;
  a = (val + (val << 3)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
101,0,4,0,0,0,0,76,1,0,70,1,20,4,61,1,
0,123,37,4,0,70,0,108,57,76,16,0,76,76,1,16,
70,0,38,47,76,16,79,0,60,0,0,61,76,34,108,61,
0,66,38,24,0,0,110,76,119,85,100,79,34,42,113,67,
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
0,0,0,47,4,0,61,0,34,0,28,0,47,0,0,32,
0,4,0,1,61,47,34,34,0,0,0,76,61,47,4,47,
119,61,108,10,34,123,4,0,61,16,0,119,0,123,123,108,
106,0,0,108,0,36,0,0,28,37,110,76,0,99,0,114,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x95188cc5;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 5) & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
37,0,76,0,0,61,0,0,101,0,61,28,0,0,4,26,
0,36,12,37,0,0,106,0,47,0,108,0,4,20,0,0,
61,101,19,0,10,108,0,0,0,101,0,109,0,61,0,53,
108,36,34,0,27,83,100,0,8,32,28,0,101,0,112,4,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xe7538f83;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 1) & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
