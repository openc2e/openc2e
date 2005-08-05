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
0,34,0,16,119,0,61,0,119,0,28,0,47,0,0,34,
119,4,47,1,119,34,34,16,0,0,0,70,0,61,47,28,
42,119,10,37,119,119,4,0,61,119,0,37,0,123,101,122,
110,0,0,109,0,17,0,0,60,51,98,76,121,124,119,47,
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
37,119,76,8,0,1,0,0,119,0,5,61,20,85,4,26,
0,54,46,66,46,0,37,0,47,0,1,0,79,88,0,0,
34,105,36,0,51,110,0,79,0,28,0,106,0,28,0,108,
101,57,34,0,8,70,109,108,97,25,37,0,100,0,112,5,
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
