#include "genome.h"
#include <fstream>
#include <typeinfo>

using namespace std;

/*
  you'll need to change some of the source filenames here to point to valid files...
*/

void testGenetics() {
	ifstream f("/Users/fuzzie/c2e/creatures3/Genetics/norn.bengal46.gen.brain.gen", std::ios::binary);
	assert(!f.fail());
	f >> noskipws;

	ofstream o("/Users/fuzzie/test.gen", std::ios::binary);
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

int main() {
  try {
    cout << "\n*** genome test\n\n";
    testGenetics();
    cout << "\n*** done\n";
  } catch (creatures::exception &t) {
    cout << "exception raised: " << t.what() << "!\n";
    return -1;
  }

  return 0;
}
