#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,4,4,10,0,0,0,0,4,0,0,28,0,34,0,1,
47,0,37,0,25,16,76,26,76,51,0,0,47,70,20,60,
4,108,4,0,76,61,0,0,0,47,1,0,85,87,61,108,
28,26,24,88,20,0,0,70,16,47,0,107,61,17,109,99,
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
47,20,0,1,1,85,37,88,47,0,28,47,28,0,1,25,
16,47,0,0,61,0,0,10,0,87,0,61,85,4,0,28,
0,10,4,87,101,51,16,0,0,0,90,28,47,105,0,0,
0,34,85,84,100,4,0,51,85,0,25,4,0,25,98,0,
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
0,19,7,51,0,10,36,22,0,0,0,0,0,36,51,0,
51,49,49,28,28,16,0,36,0,0,36,9,41,43,12,36,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0x1e4f7f22;
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
9,46,28,51,19,9,28,36,16,61,0,0,49,16,36,50,
51,10,0,51,33,0,59,51,0,62,19,19,61,28,0,0,
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
