#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
1,76,0,34,61,70,1,32,4,28,0,0,70,20,32,122,
76,76,61,0,0,10,0,0,76,76,0,0,76,70,4,4,
0,34,76,119,0,85,66,4,0,70,28,122,0,79,0,109,
0,0,1,1,66,0,101,0,92,107,21,123,70,60,13,16,
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
101,0,28,0,0,0,4,0,34,10,76,61,37,47,1,1,
0,12,76,76,66,70,0,20,66,76,85,0,61,25,70,122,
1,0,66,119,20,16,76,75,76,0,0,34,79,119,119,4,
0,87,88,107,0,4,120,115,79,1,76,61,25,17,123,67,
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
0,1,47,16,34,0,61,123,4,0,28,0,119,0,0,34,
0,4,0,43,123,34,34,16,0,0,0,17,0,61,36,28,
122,119,10,19,119,119,4,0,61,1,0,119,0,51,123,123,
122,76,0,116,0,123,0,0,16,51,110,83,97,124,42,122,
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
1,0,76,37,1,0,76,0,0,0,47,70,0,47,76,0,
37,0,70,0,70,25,47,85,0,0,0,66,61,0,1,79,
17,0,47,0,66,0,0,76,70,0,34,70,0,76,122,4,
0,0,4,0,10,16,76,42,0,34,0,79,55,68,51,0,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xd39c5978;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 3) & 0x3f;
  a = (val + (val << 2)) >> 26;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
