#include "creatures.h"

#include <vector>
#include <string>

namespace creatures {

using namespace std;

//! base class for blocks [abstract]
class block {
protected:
  virtual void read(istream &) = 0;
  virtual void write(ostream &) const = 0;

  virtual void rawRead(uint32, uint8 *) = 0;
  virtual uint8 *rawWrite(uint32 &) const = 0;
 
  friend istream &operator >> (istream &s, block &f) { f.read(s); return s; }
  friend ostream &operator << (ostream &s, const block &f) { f.write(s); return s; }

public:
  uint8 blockid[4];

  virtual const string &name() const = 0;
  virtual ~block() { }
};

//! block containing a file [abstract]
class fileBlock : public block {
public:
  virtual const string &filename() const = 0;
  virtual uint32 length() const = 0;
  virtual uint8 *getBuffer() const = 0;
};

//! old-style agent block [abstract]
class oldAgentBlock : public block {
};

//! PRAY block [abstract]
class prayBlock : public block {
protected:
  virtual void read(istream &);
  virtual void write(ostream &) const;
  string blockname;

public:
  const string &name() const { return blockname; }
};

//! a pray chunk made up of tags, eg the AGNT block
class tagPrayBlock : public prayBlock {
protected:
  virtual void rawRead(uint32, uint8 *);
  virtual uint8 *rawWrite(uint32 &) const;
 
public:
  vector<pair<string, uint32> > intvalues;
  vector<pair<string, string> > strvalues;
};

//! an unknown pray chunk
class unknownPrayBlock : public prayBlock {
protected:
  virtual void rawRead(uint32, uint8 *);
  virtual uint8 *rawWrite(uint32 &) const;

public:
  uint32 len;
  uint8 *buf;
};

//! 'FILE' pray chunk
class filePrayBlock : public unknownPrayBlock, public fileBlock {
public:
  const string &filename() const { return name(); }
  uint32 length() const { return len; }
  uint8 *getBuffer() const { return buf; }
  const string &name() const { return unknownPrayBlock::name(); }
};

//! faked creatures1 COB block (containing an agent) [unimplemented]
class c1CobBlock : public oldAgentBlock {
protected:
  virtual void rawRead(uint32, uint8 *);
  virtual uint8 *rawWrite(uint32 &) const;
};

//! creatures2 COB block [abstract] [unimplemented]
class c2CobBlock : public block {
protected:
  virtual void read(istream &);
  virtual void write(ostream &) const;
};

//! a c2 agent block [unimplemented]
class agentCobBlock : public oldAgentBlock, public c2CobBlock {
protected:
  virtual void rawRead(uint32, uint8 *);
  virtual uint8 *rawWrite(uint32 &) const;
};

//! a c2 file block [unimplemented]
class fileCobBlock : public c2CobBlock, public fileBlock {
protected:
  virtual void rawRead(uint32, uint8 *);
  virtual uint8 *rawWrite(uint32 &) const;
};

//! a c2 authorinfo block [unimplemented]
class authorInfoCobBlock : public c2CobBlock {
protected:
  virtual void rawRead(uint32, uint8 *);
  virtual uint8 *rawWrite(uint32 &) const;
};

//! an unknown cob block [unimplemented]
class unknownCobBlock : public c2CobBlock {
protected:
  virtual void rawRead(uint32, uint8 *);
  virtual uint8 *rawWrite(uint32 &) const;

public:
  uint32 len;
  uint8 *buf;
};

// a file consisting of blocks [abstract]
class blockFile : public creaturesFile {
protected:
  virtual void read(istream &) = 0;
  virtual void write(ostream &) const = 0;

  friend istream &operator >> (istream &s, blockFile &f) { f.read(s); return s; }
  friend ostream &operator << (ostream &s, const blockFile &f) { f.write(s); return s; }

public:
  vector<block *> blocks;

  virtual ~blockFile() { }
};

// a c2e PRAY file
class prayFile : public blockFile {
protected:
  virtual void read(istream &);
  virtual void write(ostream &) const;
};

// a creatures1-style COB file [unimplemented]
class c1CobFile : public blockFile {
protected:
  virtual void read(istream &);
  virtual void write(ostream &) const;
};

// a creatures2-style COB file [unimplemented]
class c2CobFile : public blockFile {
protected:
  virtual void read(istream &);
  virtual void write(ostream &) const;
};

}