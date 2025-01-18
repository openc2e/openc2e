#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>
#include <fstream>
#include <sstream>
#define CV 1

#include "caosScript.h"
#include "caosValue.h"
#include "ser/s_caosScript.h"
#include "ser/s_caosValue.h"
#include "ser/s_genome.h"


class foo {
  public:
	int i;
	foo()
		: i(0) {}
	foo(int j)
		: i(j) {}

  private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& BOOST_SERIALIZATION_NVP(i);
	}
};

genomeFile* tryLoadGenome() {
	genomeFile* f = NULL;
	try {
		f = new genomeFile();
		std::ifstream genestream("data/Genetics/norn.bengal46.gen.brain.gen", std::ios::binary | std::ios::in);
		//        std::ifstream notestream("data/Genetics/norn.bengal46.gen.brain.gno", std::ios::binary | std::ios::in);
		genestream >> std::noskipws >> *f;
		//        f->readNotes(notestream);

		fmt::print("pre-save, loaded {} genes.\n", f->genes.size());
	} catch (std::exception& e) {
		fmt::print(stderr, "Warning, genome load failed, skipping that test.\n");
		fmt::print(stderr, "Exception was: {}\n", e.what());
		if (f)
			delete f;
		f = NULL;
	}
	return f;
}

int main(int argc, char** argv) {
	registerDelegates();

	std::ofstream ofs("sertest.dat");
	caosValue null, str, intv, floatv;
	caosScript scr("c3", "<test input>");

	null.reset();
	str.setString("Hello, world!");
	intv.setInt(42);
	floatv.setFloat(0.5);

	std::istringstream ss("inst sets va00 \"hello world\\n\" outv 42 outs \"\\n\" outs va00 slow stop rscr");

	if (argc != 2)
		scr.parse(ss);
	else {
		std::ifstream ifs(argv[1]);
		scr.parse(ifs);
	}

	{
		const foo v(42);
		boost::archive::text_oarchive oa(ofs);
		oa << v;
		oa << (const caosValue&)null << (const caosValue&)str << (const caosValue&)intv << (const caosValue&)floatv;
		oa << (const caosScript&)scr;
		genomeFile* f = tryLoadGenome();
		oa << (const genomeFile* const)f;
		delete f;
	}
	ofs.close();

	caosValue nnull, nstr, nintv, nfloatv;
	caosScript si;
	{
		foo v;
		std::ifstream ifs("sertest.dat", std::ios::binary);
		boost::archive::text_iarchive ia(ifs);
		ia >> v;
		fmt::print("v.i = {}\n", v.i);
		ia >> nnull >> nstr >> nintv >> nfloatv;
		ia >> si;

		genomeFile* f;
		ia >> f;
		if (!f)
			fmt::print(stderr, "Warning, no genome class found in output. Load failed maybe?\n");
		else
			fmt::print("f->genes.size() = {}\n", f->genes.size());
		delete f;
	}

#if CV
#define D(x) fmt::print(#x " = {}\n", x.dump());
#else
#define D(x) \
	do { \
	} while (0)
#endif
	D(nnull);
	D(nstr);
	D(nintv);
	D(nfloatv);
	fmt::print("{}\n", si.installer->dump());
	return 0;
}
