#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,1,0,28,119,1,32,119,119,85,0,0,79,76,20,76,
51,60,34,0,0,10,1,0,87,70,76,0,100,76,34,4,
0,110,0,34,0,87,119,4,0,70,92,101,0,104,1,105,
47,0,108,0,70,0,10,0,98,119,123,60,76,107,100,47,
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
0,122,70,76,0,70,0,20,81,61,123,0,61,4,76,109,
1,0,108,105,76,16,104,0,16,0,0,34,88,51,106,4,
0,71,110,24,0,4,96,76,122,1,109,70,79,4,123,89,
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
1,4,61,1,119,34,119,16,0,0,0,76,0,47,4,28,
42,119,101,37,32,119,4,0,61,16,0,119,0,37,50,110,
46,76,0,109,0,123,0,0,60,51,108,83,123,124,119,114,
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
85,28,76,0,0,32,0,0,119,0,61,28,61,85,4,21,
0,25,5,66,24,0,101,0,47,0,60,0,4,88,0,0,
10,25,17,0,10,108,0,85,0,101,0,101,0,61,0,106,
108,61,34,0,16,70,106,25,44,32,27,0,97,0,114,4,
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
