#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,1,0,28,119,1,32,66,119,85,0,0,79,76,20,119,
51,60,34,0,0,10,1,0,87,70,76,0,109,76,34,4,
0,110,0,34,0,0,119,4,0,76,110,108,0,104,1,105,
47,0,108,0,70,0,10,0,98,119,123,60,70,98,100,47,
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
101,0,0,0,0,0,4,70,47,4,76,10,61,47,32,1,
0,123,76,76,0,76,0,61,20,61,123,0,16,70,70,120,
1,0,106,119,110,16,107,0,16,0,0,26,85,119,108,4,
0,70,92,66,0,4,97,69,61,1,70,37,108,4,123,67,
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
1,4,0,1,34,34,119,16,0,0,0,76,0,47,4,28,
119,119,51,37,32,119,4,0,61,16,0,119,0,37,50,110,
46,76,0,109,0,123,0,0,60,51,108,72,123,96,119,104,
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
37,32,76,119,0,61,0,0,101,0,32,28,70,0,4,55,
0,36,0,37,0,0,105,0,47,0,123,0,4,20,0,0,
61,101,19,0,10,108,0,51,0,101,0,106,0,61,0,106,
108,57,34,0,27,81,109,101,97,32,28,0,101,0,112,4,
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
