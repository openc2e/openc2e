#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
1,76,0,119,0,0,1,0,32,76,61,0,119,1,119,0,
0,51,12,76,1,70,92,0,60,20,76,25,61,37,88,47,
32,0,76,108,70,0,0,47,16,72,123,0,12,88,0,0,
13,79,0,68,88,20,70,0,0,116,32,0,66,1,0,47,
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
32,0,61,0,0,0,0,76,47,47,70,1,123,4,61,10,
0,119,25,4,0,61,0,47,5,76,37,0,32,76,0,16,
10,0,123,123,76,76,79,0,4,0,0,123,70,119,28,61,
0,79,108,66,0,0,105,37,123,85,20,79,108,50,46,83,
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
0,0,47,47,4,0,34,0,61,0,61,0,47,0,0,42,
0,4,0,1,32,47,1,0,0,0,0,70,28,0,47,47,
119,61,16,10,34,51,4,0,61,37,0,47,0,32,119,108,
106,70,0,47,0,1,0,0,10,28,110,64,0,59,0,114,
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
61,0,1,101,0,1,0,0,110,0,61,88,0,32,4,34,
4,4,12,16,0,0,100,0,47,0,0,0,0,17,0,0,
28,51,57,0,26,0,0,0,0,0,0,50,0,28,0,98,
26,6,1,0,34,81,100,0,8,34,61,0,101,0,119,4,
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
