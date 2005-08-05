#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
76,76,0,16,61,70,66,1,4,119,0,0,16,76,16,70,
1,47,122,0,0,10,1,0,47,76,0,0,79,66,66,4,
0,101,0,66,0,0,70,122,0,79,105,123,0,26,0,61,
70,0,70,0,79,0,57,0,109,47,122,123,64,61,105,16,
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
105,0,66,0,0,0,4,119,47,10,76,10,61,47,32,1,
0,123,76,76,0,76,0,61,20,76,16,0,106,70,70,16,
1,0,108,105,79,16,70,119,16,0,0,26,92,17,66,4,
0,4,106,87,0,4,107,54,79,1,76,37,123,70,119,65,
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
