#include "caoshashes.h"

#ifdef __C2E_LITTLEENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
4,1,10,0,0,5,0,0,0,1,61,76,10,70,16,0,
0,71,70,1,0,57,16,0,85,79,79,26,34,6,42,47,
1,0,76,119,76,108,0,85,61,76,108,0,87,79,0,108,
60,76,0,79,70,85,76,1,0,1,72,0,14,1,0,87,
};

/* The hash function */
unsigned int phash_cmd(unsigned int val)
{
  unsigned int a, b, rsl;
  val += 0xaa191421;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 11) & 0x3f;
  a = (val + (val << 3)) >> 26;
  rsl = (a^tab_cmd[b]);
  return rsl;
}

#endif
#ifdef __C2E_BIGENDIAN
/* tab_cmdle for the mapping for the perfect hash */

/* small adjustments to _a_ to make values distinct */
unsigned char tab_cmd[] = {
47,0,1,0,0,0,4,76,37,16,79,26,108,4,61,1,
0,32,70,119,0,76,0,37,87,70,47,0,76,10,32,17,
16,0,42,105,92,76,79,0,37,0,0,0,70,28,61,4,
0,81,106,66,0,4,120,79,108,87,70,61,98,71,127,55,
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
0,0,0,119,0,0,28,0,47,0,61,0,47,0,0,37,
47,28,0,4,119,10,34,1,0,0,0,76,16,47,47,47,
26,122,101,37,32,119,4,0,61,119,0,37,0,54,34,108,
106,76,0,108,0,50,0,0,37,34,108,77,0,111,28,114,
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
37,0,76,101,0,61,0,0,105,0,32,28,0,0,4,55,
0,36,0,37,0,0,106,0,47,0,108,0,4,20,0,0,
61,101,19,0,10,108,0,0,0,101,0,106,0,61,0,53,
108,57,34,0,27,83,109,98,8,32,28,0,101,0,114,4,
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
