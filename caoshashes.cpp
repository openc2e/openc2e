#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,1,4,42,0,0,0,0,4,0,0,4,0,61,0,1,
47,0,1,0,47,28,76,28,76,47,0,0,47,47,61,4,
4,42,28,0,1,61,0,0,0,47,1,0,61,60,0,20,
28,87,4,47,42,0,0,0,38,32,0,106,20,85,22,57,
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
47,32,0,1,16,12,34,51,4,0,47,47,1,0,1,4,
47,47,0,0,1,0,0,32,0,25,0,10,37,4,0,28,
0,51,47,88,34,36,1,0,0,0,92,42,1,101,0,0,
0,28,61,19,42,60,0,17,85,0,25,4,0,44,102,0,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x23c282f5;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 3) & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_LITTLEENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
0,9,19,10,19,0,9,16,0,0,10,0,0,0,59,28,
0,28,0,11,9,9,9,24,34,0,0,0,36,5,0,0,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xa2136dac;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 5) & 0x1f;
  a = (val + (val << 1)) >> 27;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
19,0,7,0,19,26,0,0,9,7,9,6,7,28,2,0,
28,6,0,0,11,60,32,20,12,34,4,15,36,22,0,12,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x75fd85b3;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 4) & 0x1f;
  a = val >> 27;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
