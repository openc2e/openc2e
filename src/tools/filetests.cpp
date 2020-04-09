#include "genome.h"
#include "Catalogue.h"
#include <cassert>
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

	cout << "read " << g.genes.size() << " genes\n";

  for (vector<gene *>::iterator x = g.genes.begin(); x != g.genes.end(); x++) {
    gene *t = *x;

    cout << "semi-mangled name: " << typeid(*t).name() << "\n";
    cout << "header flags: mut " << t->header.flags._mutable << " dup " << t->header.flags.dupable << " del " << t->header.flags.delable << "\n";
    cout << "male-only " << t->header.flags.maleonly << " female-only " << t->header.flags.femaleonly << "\n";
    if (typeid(*t) == typeid(organGene)) {
      for (vector<gene *>::iterator y = ((organGene *)t)->genes.begin(); y != ((organGene *)t)->genes.end(); y++) {
        gene *s = *y;

        cout << "semi-mangled name: " << typeid(*s).name() << "\n";
        cout << "header flags: mut " << s->header.flags._mutable << " dup " << s->header.flags.dupable << " del " << s->header.flags.delable << "\n";
        cout << "male-only " << s->header.flags.maleonly << " female-only " << s->header.flags.femaleonly << "\n";
      }
    }
    cout << '\n';
  }
}

void testCatalogue() {
	Catalogue c;
	c.initFrom("data/Catalogue/");

	cout << "\n*** dumping catalogue\n";

	for (std::map<std::string, std::vector<string> >::iterator i = c.data.begin(); i != c.data.end(); i++) {
		cout << endl << "TAG \"" << i->first << "\"" << endl;
		for (std::vector<std::string>::iterator j = i->second.begin(); j != i->second.end(); j++) {
			cout << "\"" << *j << "\"" << endl;
		}
	}
}

int main() {
	try {
		cout << "\n*** genome test\n\n";
		testGenetics();
//		cout << "\n*** catalogue test\n\n";
//		testCatalogue();
		cout << "\n*** done\n";
	} catch (creaturesException &t) {
		cout << "exception raised: " << t.what() << "!\n";
		return -1;
	}

	return 0;
}

