#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
1,0,28,28,0,25,0,0,10,70,61,0,122,4,119,0,
0,17,123,76,1,10,66,0,1,1,1,119,34,37,5,0,
47,0,0,0,70,105,0,76,76,70,105,0,4,1,0,0,
1,76,0,92,57,37,76,0,0,122,71,0,74,0,0,28,
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
17,0,34,0,0,0,0,4,123,0,26,0,108,4,61,32,
0,122,46,4,0,76,0,119,11,76,119,0,123,0,0,108,
10,0,16,122,17,34,70,0,4,0,0,61,79,0,119,47,
0,66,105,0,0,34,106,98,108,76,19,54,101,34,112,64,
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
0,0,0,34,4,0,34,0,0,0,61,0,47,0,0,0,
0,0,0,1,34,47,1,0,0,0,0,70,28,0,10,47,
16,37,0,0,34,32,4,0,34,47,0,32,0,37,112,32,
37,0,0,0,0,17,0,0,10,28,110,66,0,55,42,0,
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
0,0,1,60,0,10,0,0,101,0,4,10,0,61,0,1,
0,28,34,0,0,0,43,0,0,0,0,0,0,42,0,0,
10,20,16,0,16,0,0,0,0,0,0,101,4,47,0,0,
12,37,47,0,34,0,43,0,8,28,61,0,97,0,114,35,
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
