#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,34,85,108,0,4,0,0,4,0,0,42,0,47,0,47,
1,0,47,0,26,20,70,5,76,70,0,0,61,32,28,60,
5,43,34,0,75,61,0,0,0,4,1,0,66,87,85,101,
42,88,62,16,0,0,0,66,17,66,0,108,0,47,108,99,
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
34,0,1,0,0,0,0,0,0,47,1,0,37,4,37,51,
0,16,4,4,0,76,0,98,0,76,4,0,108,0,25,26,
10,0,119,123,76,16,70,0,4,0,0,61,47,0,60,28,
0,71,122,0,0,34,120,108,108,28,55,12,120,34,109,83,
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
51,51,6,0,51,26,9,49,0,11,0,9,46,0,51,49,
49,22,28,0,33,28,9,10,41,0,61,60,34,46,16,36,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xa35cbefd;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = val & 0x1f;
  a = (val + (val << 3)) >> 27;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
51,34,50,28,19,34,34,36,46,46,0,36,36,46,10,12,
18,10,0,51,6,0,25,46,0,51,19,16,49,55,0,0,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xd5b132b9;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 2) & 0x1f;
  a = (val + (val << 2)) >> 27;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
