#include "Catalogue.h"
#include "genome.h"

#include <cassert>
#include <fmt/core.h>
#include <fstream>
#include <typeinfo>

using namespace std;

/*
  you'll need to change some of the source filenames here to point to valid files...
*/

void testGenetics() {
	ifstream f("/home/fuzzie/creatures3/Genetics/norn.bengal46.gen.brain.gen", std::ios::binary);
	assert(!f.fail());
	f >> noskipws;

	ofstream o("/home/fuzzie/test.gen", std::ios::binary);
	assert(!o.fail());
	genomeFile g;
	f >> g;
	o << g;

	fmt::print("read {} genes\n", g.genes.size());

	for (vector<gene*>::iterator x = g.genes.begin(); x != g.genes.end(); x++) {
		gene* t = *x;

		fmt::print("semi-mangled name: {}\n", typeid(*t).name());
		fmt::print("header flags: mut {} dup {} del {}\n", t->header.flags._mutable, t->header.flags.dupable, t->header.flags.delable);
		fmt::print("male-only {} female-only {}\n", t->header.flags.maleonly, t->header.flags.femaleonly);
		if (typeid(*t) == typeid(organGene)) {
			for (vector<gene*>::iterator y = ((organGene*)t)->genes.begin(); y != ((organGene*)t)->genes.end(); y++) {
				gene* s = *y;

				fmt::print("semi-mangled name: {}\n", typeid(*s).name());
				fmt::print("header flags: mut {} dup {} del {}\n", s->header.flags._mutable, s->header.flags.dupable, s->header.flags.delable);
				fmt::print("male-only {} female-only {}\n", s->header.flags.maleonly, s->header.flags.femaleonly);
			}
		}
		fmt::print("\n");
	}
}

void testCatalogue() {
	Catalogue c;
	c.initFrom("data/Catalogue/");

	fmt::print("\n*** dumping catalogue\n");

	for (std::map<std::string, std::vector<string> >::iterator i = c.data.begin(); i != c.data.end(); i++) {
		fmt::print("\nTAG \"{}\n", i->first);
		for (std::vector<std::string>::iterator j = i->second.begin(); j != i->second.end(); j++) {
			fmt::print("\"{}\"\n", *j);
		}
	}
}

int main() {
	try {
		fmt::print("\n*** genome test\n\n");
		testGenetics();
		//		fmt::print("\n*** catalogue test\n\n");
		//		testCatalogue();
		fmt::print("\n*** done\n");
	} catch (Exception& t) {
		fmt::print("exception raised: {}!\n", t.what());
		return -1;
	}

	return 0;
}
