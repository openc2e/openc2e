#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
0,47,4,10,0,0,0,0,4,0,0,28,0,61,0,1,
47,0,47,0,0,34,47,28,47,47,0,0,47,32,61,4,
4,42,4,0,10,28,0,0,0,47,1,0,61,60,17,42,
28,42,66,32,20,0,0,79,88,17,0,106,85,47,43,99,
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
4,32,0,1,1,20,28,37,47,0,47,47,1,0,1,26,
47,47,0,0,4,0,0,32,0,88,0,12,37,4,0,28,
0,51,47,88,101,28,1,0,0,0,55,37,1,34,0,0,
0,28,85,23,17,60,0,17,85,0,10,5,0,90,98,0,
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
0,50,7,19,0,16,0,49,0,36,46,26,46,0,41,0,
29,9,9,0,9,35,7,51,0,11,16,0,40,0,1,9,
};

/* The hash function */
unsigned int phash_func(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xd90f5d4b;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = val & 0x1f;
  a = (val + (val << 2)) >> 27;
  rsl = (a^tab_func[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_funcle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_func[] = {
9,10,0,51,49,46,0,0,49,57,0,50,59,34,11,29,
57,19,0,28,59,0,56,11,50,5,0,16,22,0,0,0,
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
