#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,34,61,25,0,4,0,0,20,0,0,42,0,47,0,47,
1,0,47,0,88,20,70,5,76,70,0,0,61,32,28,60,
5,43,34,0,88,61,0,0,0,4,1,0,66,92,85,105,
42,88,62,17,11,0,0,66,17,66,0,108,0,67,109,99,
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
34,0,1,0,0,0,0,85,0,47,1,0,37,4,37,51,
0,16,4,4,0,76,0,98,0,76,4,0,123,0,25,26,
10,0,123,119,76,16,79,0,4,0,0,61,47,0,60,28,
0,81,123,0,0,34,120,108,108,28,55,12,108,34,98,64,
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
34,0,47,0,0,61,0,0,1,0,0,4,0,1,1,0,
0,47,61,34,0,0,0,0,0,28,0,47,34,0,61,34,
0,0,32,0,32,47,0,4,28,37,32,4,34,32,60,34,
0,0,47,37,90,0,0,0,0,0,22,1,47,0,5,4,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x7e032c28;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = val & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
0,0,16,10,0,20,0,0,60,0,16,37,0,61,0,16,
61,28,47,47,61,0,12,0,0,0,0,0,0,42,0,0,
10,47,17,0,28,0,0,0,0,0,0,20,4,1,0,0,
34,61,1,0,34,0,46,0,10,28,0,0,10,0,114,18,
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
