#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,1,0,76,119,1,0,119,4,85,0,0,16,76,0,76,
76,60,34,0,0,10,1,0,4,70,79,0,4,88,119,4,
0,106,0,76,0,1,122,4,0,76,70,101,0,116,0,105,
47,0,5,0,70,0,116,0,92,47,123,123,79,61,105,119,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xb6486d18;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 4) & 0x3f;
  a = (val + (val << 5)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
12,0,10,0,0,0,4,0,61,4,79,61,20,47,1,1,
0,122,70,76,0,10,0,20,66,61,123,0,61,108,76,71,
1,0,108,105,76,16,104,87,16,0,0,34,88,51,106,4,
0,81,110,24,0,4,96,125,122,1,66,70,79,4,123,89,
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
0,34,0,47,4,0,61,0,119,0,28,0,47,0,0,34,
119,4,47,1,119,34,34,16,0,0,0,76,0,61,47,28,
42,119,10,37,119,119,4,0,61,119,0,37,0,123,120,110,
46,76,0,57,0,17,0,0,60,51,108,77,98,111,119,104,
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
85,4,76,8,0,1,0,0,119,0,61,61,61,16,4,26,
0,28,12,66,46,0,108,0,47,0,51,0,79,88,0,0,
34,25,36,0,51,108,0,85,0,101,0,106,0,28,0,106,
101,26,34,0,34,70,100,51,100,25,37,0,43,0,112,5,
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
