#include <string>
#include <vector>
#include "exceptions.h"

using namespace std;

class MNGFile {
	private:
		FILE * f;
		long filesize;
		char * map;
		string name;
		int numsamples, scriptoffset, scriptlength, scriptend;
		char * script;
		vector< pair< string, pair< char *, int > > > samples;
	
	public:
		 MNGFile(string);
		 void enumerateSamples();
		 ~MNGFile();
};
	
class MNGFileException : public creaturesException {
	public:
		int error;
		MNGFileException(const char * m, int e) throw() : creaturesException(m) { error = e; }
};

class MNGNode {
};

class MNGTrack : public MNGNode {
};

class MNGLayer : public MNGNode {
};

class MNGLoopLayer : public MNGLayer {
};

class MNGAleatoricLayer : public MNGLayer {
};

class MNGVariable : public MNGNode {
};

class MNGUpdate : public MNGNode {
};

/* vim: set noet: */
