#include <vector>
#include <string>
#include <istream>
#include <ostream>

using namespace std;

//! base class for blocks [abstract]
class block {
protected:
  virtual void read(istream &) = 0;
  virtual void write(ostream &) const = 0;

  virtual void rawRead(unsigned int, unsigned char *) = 0;
  virtual unsigned char *rawWrite(unsigned int &) const = 0;

  friend istream &operator >> (istream &s, block &f) { f.read(s); return s; }
  friend ostream &operator << (ostream &s, const block &f) { f.write(s); return s; }

public:
  unsigned char blockid[4];

  virtual const string &name() const = 0;
  virtual ~block() { }
};

//! block containing a file [abstract]
class fileBlock : public block {
public:
  virtual const string &filename() const = 0;
  virtual unsigned int length() const = 0;
  virtual unsigned char *getBuffer() const = 0;
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
  virtual void rawRead(unsigned int, unsigned char *);
  virtual unsigned char *rawWrite(unsigned int &) const;

public:
  vector<pair<string, unsigned int> > intvalues;
  vector<pair<string, string> > strvalues;
};

//! an unknown pray chunk
class unknownPrayBlock : public prayBlock {
protected:
  virtual void rawRead(unsigned int, unsigned char *);
  virtual unsigned char *rawWrite(unsigned int &) const;

public:
  unsigned int len;
  unsigned char *buf;
};

//! 'FILE' pray chunk
class filePrayBlock : public unknownPrayBlock, public fileBlock {
public:
  const string &filename() const { return name(); }
  unsigned int length() const { return len; }
  unsigned char *getBuffer() const { return buf; }
  const string &name() const { return unknownPrayBlock::name(); }
};

//! faked creatures1 COB block (containing an agent) [unimplemented]
class c1CobBlock : public oldAgentBlock {
protected:
  virtual void rawRead(unsigned int, unsigned char *);
  virtual unsigned char *rawWrite(unsigned int &) const;
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
  virtual void rawRead(unsigned int, unsigned char *);
  virtual unsigned char *rawWrite(unsigned int &) const;
};

//! a c2 file block [unimplemented]
class fileCobBlock : public c2CobBlock, public fileBlock {
protected:
  virtual void rawRead(unsigned int, unsigned char *);
  virtual unsigned char *rawWrite(unsigned int &) const;
};

//! a c2 authorinfo block [unimplemented]
class authorInfoCobBlock : public c2CobBlock {
protected:
  virtual void rawRead(unsigned int, unsigned char *);
  virtual unsigned char *rawWrite(unsigned int &) const;
};

//! an unknown cob block [unimplemented]
class unknownCobBlock : public c2CobBlock {
protected:
  virtual void rawRead(unsigned int, unsigned char *);
  virtual unsigned char *rawWrite(unsigned int &) const;

public:
  unsigned int len;
  unsigned char *buf;
};

// a file consisting of blocks [abstract]
class blockFile {
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
