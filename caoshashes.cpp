#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,28,0,0,0,28,0,16,32,4,10,0,1,28,61,0,
0,0,47,4,4,0,0,28,0,20,0,0,0,0,28,61,
0,61,47,1,10,16,28,0,12,88,10,38,0,10,61,42,
0,42,0,47,0,0,98,20,92,28,0,1,4,55,84,0,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xba721f9a;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 2) & 0x3f;
  a = (val + (val << 1)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
47,47,0,0,37,28,28,34,1,0,4,47,4,1,1,32,
1,47,0,0,1,0,0,10,0,32,0,61,37,0,0,4,
0,28,47,10,34,51,16,0,0,0,26,34,47,34,0,0,
0,42,61,92,12,83,0,28,61,0,47,4,0,95,100,0,
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
0,9,12,16,19,0,9,16,0,0,7,0,0,0,9,28,
0,28,0,61,61,9,18,7,10,0,0,0,38,59,0,0,
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
19,0,0,0,19,26,0,0,9,10,19,0,7,28,7,0,
0,11,0,0,32,7,20,35,28,34,4,15,22,6,0,12,
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
