#include "agent.h"
#include "zlib.h"

#include <exception>

namespace creatures {

// *** prayFile

void prayFile::read(istream &s) {
  uint8 majic[4];
  s.read(majic, 4);
  if (strncmp((char *)majic, "PRAY", 4) != 0) throw exception("bad majic for a PRAY file");

  while (true) {
    char stringid[4];
    for (int i = 0; i < 4; i++) s >> stringid[i];

    if (s.fail() || s.eof()) return;

    prayBlock *b;

		/*
		  known pray blocks:
		
		  AGNT, DSAG : tag blocks
		  PHOT, GENE, FILE : various file blocks
      GLST: CreaturesArchive file blocks

		  unknown pray blocks (there are more, I don't have a copy of C3 atm =/):
		
		  CREA (CreaturesArchive file?), DSEX (tag block?)
		*/
		
    // TODO: read the taggable agent blocks from a configuration file
    if (strncmp(stringid, "AGNT", 4) == 0) // Creatures Adventures/Creatures 3 agent
      b = new tagPrayBlock();
    else if (strncmp(stringid, "DSAG", 4) == 0) // Docking Station agent
      b = new tagPrayBlock();
    else
      b = new unknownPrayBlock();

    memcpy(b->blockid, stringid, 4);
    s >> *b;
    blocks.push_back(b);
  }
}

void prayFile::write(ostream &s) const {
  s << "PRAY";

  for (vector<block *>::iterator x = ((prayFile *)this)->blocks.begin(); x != ((prayFile *)this)->blocks.end(); x++) s << **x;
}

// *** prayBlock

void prayBlock::read(istream &s) {
  uint8 x[128];
  s.read(&x, 128);

  blockname = (char *)x;

  uint32 size, usize, flags;
  s.read(&size, 4);
  s.read(&usize, 4);
  s.read(&flags, 4);

  uint8 *buf;
  buf = new uint8[size];
  s.read(buf, size);

  if ((flags & 1) != 0) {
    // decompress the block
    uint8 *dest = new uint8[usize];

    if (uncompress(dest, (uLongf *)&usize, buf, size) != Z_OK) {
      delete dest; delete buf;

      throw exception("failed to decompress a PRAY chunk");
    }

    delete buf;
    buf = dest;
  }

  rawRead(size, buf);

  delete buf;
}

void prayBlock::write(ostream &s) const {
  s.write(blockid, 4);

  uint8 x[128];
  memset(x, 0, 128);
  memcpy(x, blockname.c_str(), blockname.size());
  s.write(&x, 128);

  uint32 usize, size;
  uint8 *b = rawWrite(usize);

  bool compressed;

#ifdef NO_COMPRESS_PRAY
  compressed = false;
  size = usize;
#else
  uint8 *newbuf = new uint8[usize + 12];
  size = usize + 12;
  if (compress(newbuf, (uLongf *)&size, b, usize) != Z_OK) {
    // fallback to uncompressed data
    delete newbuf;
    size = usize;
    compressed = false;
  } else if (size >= usize) {
    // the compressed block is larger than the uncompressed block
    // fallback to uncompressed data
    delete newbuf;
    size = usize;
    compressed = false;
  } else {
    delete b; b = newbuf;
    compressed = true;
  }
#endif

  s.write(&size, 4);
  s.write(&usize, 4);

  uint8 flags = compressed ? 1 : 0;
  s.write(&flags, 4);

  s.write(b, size);
}

// *** unknownPrayBlock

void unknownPrayBlock::rawRead(uint32 v, uint8 *b) {
  buf = new uint8[v];
  memcpy(buf, b, v);
  len = v;
}
 
uint8 *unknownPrayBlock::rawWrite(uint32 &l) const {
  uint8 *b = new uint8[len];
  memcpy(b, buf, len);
  l = len;
  return b;
}

// *** tagPrayBlock

/*
  format of tag pray blocks:

  4-byte nointvalues, then the values: [string name, int value]
  4-byte nostrvalues, then the values: [string name, string value]
  (string = 4-byte length followed by actual data)
*/

char *tagStringRead(uint8 *&ptr) {
  uint32 len = *(uint32 *)ptr;
  ptr += 4;

  // TODO: fixme: rewrite this code properly
  static char b[50001];
  memcpy(b, ptr, (len < 50000) ? len : 50000);
  b[(len < 50000) ? len : 50000] = 0;
  ptr += len;
  return b;
}

void tagStringWrite(uint8 *&ptr, string &s) {
  *(uint32 *)ptr = s.size(); ptr += 4;
  memcpy(ptr, s.c_str(), s.size()); ptr += s.size();
}

void tagPrayBlock::rawRead(uint32 v, uint8 *b) {
  uint8 *ptr = b;

  uint32 nointvalues = *(uint32 *)ptr; ptr += 4;

  for (uint32 i = 0; i < nointvalues; i++) {
    pair<string, uint32> value;

    value.first = tagStringRead(ptr);
    value.second = *(uint32 *)ptr;
    ptr += 4;

    intvalues.push_back(value);
  }

  uint32 nostrvalues = *(uint32 *)ptr; ptr += 4;

  for (uint32 i = 0; i < nostrvalues; i++) {
    pair<string, string> value;

    value.first = tagStringRead(ptr);
    value.second = tagStringRead(ptr);

    strvalues.push_back(value);
  }
}

uint8 *tagPrayBlock::rawWrite(uint32 &l) const {
  l = 8;

  for (vector<pair<string, uint32> >::iterator x = ((tagPrayBlock *)this)->intvalues.begin(); x != ((tagPrayBlock *)this)->intvalues.end(); x++)
    l = l + 8 + (*x).first.size();

  for (vector<pair<string, string> >::iterator x = ((tagPrayBlock *)this)->strvalues.begin(); x != ((tagPrayBlock *)this)->strvalues.end(); x++)
    l = l + 8 + (*x).first.size() + (*x).second.size();

  uint8 *buf = new uint8[l];
  uint8 *ptr = buf;

  *(uint32 *)ptr = intvalues.size(); ptr += 4;
  for (vector<pair<string, uint32> >::iterator x = ((tagPrayBlock *)this)->intvalues.begin(); x != ((tagPrayBlock *)this)->intvalues.end(); x++) {
    tagStringWrite(ptr, (*x).first);
    *(uint32 *)ptr = (*x).second; ptr += 4;
  }

  *(uint32 *)ptr = strvalues.size(); ptr += 4;
  for (vector<pair<string, string> >::iterator x = ((tagPrayBlock *)this)->strvalues.begin(); x != ((tagPrayBlock *)this)->strvalues.end(); x++) {
    tagStringWrite(ptr, (*x).first);
    tagStringWrite(ptr, (*x).second);
  }

  return buf;
}

}