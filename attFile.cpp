#include "attFile.h"
#include <boost/tokenizer.hpp>
using namespace boost;
using namespace std;

istream &operator >> (istream &i, attFile &f) {
	f.nolines = 0;

	std::string s;
	while (std::getline(i, s)) {
		if (s.size() == 0) return i;
		assert(f.nolines < 16);

		f.noattachments[f.nolines] = 0;
		
		bool havefirst = false;
		unsigned int x;
		tokenizer<> tok(s);
		for (tokenizer<>::iterator beg = tok.begin(); beg != tok.end(); beg++) {
			unsigned int val = atoi(beg->c_str());
			if (havefirst) {
				f.attachments[f.nolines][f.noattachments[f.nolines] * 2] = x;
				f.attachments[f.nolines][(f.noattachments[f.nolines] * 2) + 1] = val;
				havefirst = false;
				f.noattachments[f.nolines]++;
			} else {
				havefirst = true;
				x = val; 
			}
		}

		assert(!havefirst);
		f.nolines++;
	}

	return i;
}

