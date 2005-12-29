#ifndef BYTECODE_H
#define BYTECODE_H 1

#include "lexutil.h"
#include "cmddata.h"
//#include "caosScript.h"
#include <cstdio>
#include "serialization.h"
#include "dialect.h"
#include "caosVar.h"

class script;
class caosVM;

typedef void (caosVM::*ophandler)();
class caosOp {
	public:
		// on entry vm->nip = our position + 1
		virtual void execute(caosVM *vm);
		void setCost(int cost) {
			evalcost = cost;
		}
		virtual void relocate(const std::vector<int> &relocations) {}
		caosOp() : evalcost(1), owned(false), yyline(lex_lineno) {}
		virtual ~caosOp() {};
		virtual std::string dump() { return "UNKNOWN"; }
		int getlineno() const { return yyline; }
		int getIndex()  const { return index;  }
	protected:
		int index;
		int evalcost;
		bool owned; // if it's been threaded
		int yyline; // HORRIBLE HACK
		friend class script;
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			ar & index;
			ar & evalcost;
			ar & owned;
			ar & yyline;
		}
			
};
BOOST_CLASS_EXPORT(caosOp)

class caosNoop : public caosOp {
	public:
		caosNoop() { evalcost = 0; }
		std::string dump() { return std::string("noop"); }
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
		}
};

BOOST_CLASS_EXPORT(caosNoop)

class caosJMP : public caosOp {
	protected:
		int p;
		caosJMP() : p(INT_MIN) {} // Deserialization
	public:
		caosJMP(int p_) : p(p_) { evalcost = 0; }
		void execute(caosVM *vm);
		void relocate(const std::vector<int> &relocations) {
			if (p < 0)
				p = relocations[-p];
			assert (p > 0);
		}
		std::string dump() { 
			char buf[16];
			sprintf(buf, "JMP %08d", p);
			return std::string(buf);
		}
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & p;
		}
};

BOOST_CLASS_EXPORT(caosJMP)
class simpleCaosOp : public caosOp {
	protected:
		const cmdinfo *ci;
	public:
		simpleCaosOp() : ci(NULL) {};

		void setCmdInfo(const cmdinfo *ci_) { ci = ci_; }
		
		simpleCaosOp(ophandler h, const cmdinfo *i) : ci(i) {}
		void execute(caosVM *vm);
		std::string dump() {
			return std::string(ci->fullname);
		}
		const cmdinfo *getCmdInfo() const { return ci; }
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive &ar, unsigned long version) const {
			SER_BASE(ar, caosOp);
			std::string key(ci->key);
			std::string variant(ci->variant);
			ar & key & variant;
		}

		template<class Archive>
		void load(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			std::string key, variant;
			ar & key & variant;
			if (variants.find(variant) == variants.end())
                abort(); // XXX
            Variant *v = variants[variant];
            
            if (v->keyref.find(key) == v->keyref.end())
                abort(); // XXx
			ci = v->keyref[key];
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()

};
BOOST_CLASS_EXPORT(simpleCaosOp)

class caosREPS : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & exit;
		}
	protected:
		int exit;
		caosREPS() : exit(INT_MIN) {} // Deserialization
	public:
		caosREPS(int exit_) : exit(exit_) {}
		void relocate(const std::vector<int> &relocations) {
			if (exit < 0) {
				exit = relocations[-exit];
			}
			assert(exit > 0);
		}
		void execute(caosVM *vm);
		std::string dump() { return std::string("REPS"); }
};
		
BOOST_CLASS_EXPORT(caosREPS)

class caosGSUB : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & targ;
		}
	protected:
		int targ;
		caosGSUB() : targ(INT_MIN) { } // Deserialization
	public:
		caosGSUB(int targ_) : targ(targ_) {}
		void execute(caosVM *vm);
		void relocate(const std::vector<int> &relocations) {
			if (targ < 0) {
				targ = relocations[-targ];
			}
			assert(targ > 0);
		}

		std::string dump() { 
			char buf[16];
			sprintf(buf, "GSUB %08d", targ);
			return std::string(buf);
		}
};

BOOST_CLASS_EXPORT(caosGSUB)

// Condition classes
#define CEQ 1
#define CLT 2
#define CGT 4
#define CMASK (CEQ | CLT | CGT)
#define CLE (CEQ | CLT)
#define CGE (CEQ | CGT)
#define CNE (CLT | CGT)

extern const char *cnams[];

class caosCond : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & cond & branch;
		}
	protected:
		int cond;
		int branch;

		caosCond() : cond(0), branch(INT_MIN) {} // Deserialization
	public:
		std::string dump() { 
			char buf[64];
			const char *c = NULL;
			if (cond < CMASK && cond > 0)
				c = cnams[cond];
			if (c)
				sprintf(buf, "COND %s %08d", c, branch);
			else
				sprintf(buf, "COND BAD %d %08d", cond, branch);
			return std::string(buf);
		}
				
			
		caosCond(int condition, int br)
			: cond(condition), branch(br) {}
		void execute(caosVM *vm);

		void relocate(const std::vector<int> &relocations) {
			if (branch < 0) {
				branch = relocations[-branch];
			}
			assert(branch > 0);
		}
};
BOOST_CLASS_EXPORT(caosCond)

class caosENUM_POP : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & exit;
		}
	protected:
		int exit;

		caosENUM_POP() : exit(INT_MIN) {} // Deserialization
	public:
		caosENUM_POP(int exit_) : exit(exit_) {}
		void execute(caosVM *vm);
		void relocate(const std::vector<int> &relocations) {
			if (exit < 0) {
				exit = relocations[-exit];
			}
			assert(exit > 0);
		}
		std::string dump() {
			char buf[24];
			sprintf(buf, "ENUM_POP %08d", exit);
			return std::string(buf);
		}
};
BOOST_CLASS_EXPORT(caosENUM_POP)

class caosSTOP : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
		}
	public:
		void execute(caosVM *vm);
		std::string dump() {
			return std::string("STOP");
		}
};
BOOST_CLASS_EXPORT(caosSTOP)
	
class caosAssert : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
		}
	public:
		void execute(caosVM *vm) {
			throw caosException("DBG: ASRT failed");
		}
		std::string dump() {
			return std::string("ASSERT FAILURE");
		}
};
BOOST_CLASS_EXPORT(caosAssert)

class ConstOp : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & constVal;
		}
	protected:
		caosVar constVal;
		ConstOp() { constVal.reset(); }
	public:
		virtual void execute(caosVM *vm);

		ConstOp(const caosVar &val) {
			constVal = val;
		}

		std::string dump() {
			return std::string("CONST ") + constVal.dump();
		}
};
BOOST_CLASS_EXPORT(ConstOp)

class opVAxx : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & index;
		}
	protected:
		int index;
		opVAxx() : index(0) {}
	public:
		opVAxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
		void execute(caosVM *vm) ;
			
		std::string dump() {
			char buf[16];
			sprintf(buf, "VA%02d", index);
			return std::string(buf);
		}
};
BOOST_CLASS_EXPORT(opVAxx)

class opOVxx : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & index;
		}
	protected:
		int index;
		opOVxx() : index(0) {}
	public:
		opOVxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
		void execute(caosVM *vm);
		std::string dump() {
			char buf[16];
			sprintf(buf, "OV%02d", index);
			return std::string(buf);
		}
};
BOOST_CLASS_EXPORT(opOVxx)

class opMVxx : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & index;
		}
	protected:
		int index;
		opMVxx() : index(0) {}
	public:
		opMVxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
		void execute(caosVM *vm);
		std::string dump() {
			char buf[16];
			sprintf(buf, "MV%02d", index);
			return std::string(buf);
		}
};
BOOST_CLASS_EXPORT(opMVxx)
	
class opBytestr : public caosOp {
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar, unsigned long version) {
			SER_BASE(ar, caosOp);
			ar & bytestr;
		}
	protected:
		std::vector<unsigned int> bytestr;
		opBytestr() {}
	public:
		opBytestr(const std::vector<unsigned int> &bs) : bytestr(bs) {}
		void execute(caosVM *vm); 
		std::string dump() {
			std::ostringstream oss;
			oss << "BYTESTR [ ";
			for (unsigned int i = 0; i < bytestr.size(); i++) {
				oss << i << " ";
			}
			oss << "]";
			return oss.str();
		}
};
BOOST_CLASS_EXPORT(opBytestr)

#endif

/* vim: set noet: */
