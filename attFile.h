#include <fstream>

class attFile {
public:
	unsigned int attachments[16][20];
	unsigned int noattachments[16];
	unsigned int nolines;
	friend std::istream &operator >> (std::istream &, attFile &);
};

