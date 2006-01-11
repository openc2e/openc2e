#include "rtree.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cstdio>

struct rcmp
{
	bool operator()(const Region &r1, const Region &r2) 
	{
#define CMP(v) \
		do { if (r1.v != r2.v) return r1.v < r2.v; } while (0)
		CMP(xmin); CMP(xmax); CMP(ymin); CMP(ymax);
#undef CMP
	}
};

Region random_in(const Region &tr) {
	double rp1 = (double)rand() / (double)RAND_MAX;
	double rp2 = (double)rand() / (double)RAND_MAX;

	int pt1 = tr.xmin + rp1 * (double)(tr.xmax - tr.xmin);
	int pt2 = tr.ymin + rp1 * (double)(tr.ymax - tr.ymin);

	return Region(pt1, pt1, pt2, pt2);

}

int main() {
	std::cerr << "rbr=" << sizeof(RBranch<Region>) << "rn=" << sizeof(RNode<Region>) << "rd=" << sizeof(RData<Region>) << std::endl;
	try {
		std::vector<Region> testVec;
		srand(time(NULL));
		RTree<Region> tree;
		for (int i = 0; i < 500; i++) {
			int x1 = rand(); // % 50000;
			int x2 = x1 + rand() % 125;
			int y1 = rand(); // % 50000;
			int y2 = y1 + rand() % 125;

			if (x1 > x2) {
				int t = x1;
				x1 = x2;
				x2 = t;
			}
			
			if (y1 > y2) {
				int t = y1;
				y1 = y2;
				y2 = t;
			}
			Region r(x1, x2, y1, y2);
			
			if (tree.find_one(r)) {
				i--;
				continue;
			}
				
			testVec.push_back(Region(x1, x2, y1, y2));
		}

		bool allok = true;
		
		int ct = 0;
		for (std::vector<Region>::iterator i = testVec.begin(); i != testVec.end(); i++) {
			tree.insert(*i, *i);
			ct++;
//			if (ct % 100 == 0)
//				fprintf(stderr, "\rins %06d", ct);
		}
		std::cerr << std::endl << "size=" << tree.size() << "inner=" << tree.inner_size() << std::endl;

		ct = 0;
		for (std::vector<Region>::iterator i = testVec.begin(); i != testVec.end(); i++) {
			if (ct++ % 100 == 0)
				fprintf(stderr, "\rchk %06d", ct);
			std::cout << "Verify of " << (*i).to_s() << "... ";
			Region q = random_in(*i);
			assert(q.overlaps((*i)));
			std::cout << q.to_s() << "... ";
			bool ok = false;

			std::vector<RTree<Region>::ptr> results = tree.find(q);
			std::cout << results.size() << " results ... ";
			for (std::vector<RTree<Region>::ptr>::iterator ci = results.begin(); ci != results.end(); ci++) {
				if ((*ci).data() == *i) {
					ok = true;
					(*ci).erase();
					break;
				}
			}

			results = tree.find(q);
			
			std::cout << results.size() << " results postdel ... ";
			for (std::vector<RTree<Region>::ptr>::iterator ci = results.begin(); ci != results.end(); ci++) {
				if ((*ci).data() == *i) {
					ok = false;
					break;
				}
			}

// */
			if (ok)
				std::cout << "ok";
			else
				std::cout << "nok";
			std::cout << std::endl;
			allok = allok && ok;
		}	

		if (!allok)
			std::cout << "Not all ok";
		else
			std::cout << "All ok";
		std::cout << std::endl;
	} catch (std::exception &e) {
		std::cout << "abort: " << e.what();
	}
	std::cout << std::endl;
	fprintf(stderr, "\n");
}
