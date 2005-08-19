#include <assert.h>
#include "endianlove.h"
#include <stdio.h>
#include "mngfile.h"
#include <sys/mman.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <SDL/SDL.h>
#include "lex.mng.h"

mngFlexLexer *mnglexer = NULL;
MNGFile *g_mngfile = NULL;
extern int mngparse(); // parser

void decryptbuf(char * buf, int len) {
	int i;
	unsigned char pad = 5;
	for(i = 0; i < len; i++) {
		buf[i] ^= pad;
		pad += 0xC1;
	}
}

MNGFile::MNGFile(string n) {
	sampleno = 0;

	name = n;
	
	f = fopen(name.c_str(), "r");
	if(!f) {
		throw MNGFileException("Can't open file", errno);
	}
	
	// Hack to obtain the filesize
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	
	// mmap the data file
	map = (char *) mmap(0, filesize, PROT_READ, MAP_PRIVATE, fileno(f), 0);
	if(map == (void *) -1) {
		throw MNGFileException("Can't mmap", errno);
	}
	
	// Read metavariables from beginning of file
	numsamples = swapEndianLong(*((int *) map));
	scriptoffset = swapEndianLong(*(((int *) map) + 1));
	scriptend = swapEndianLong(*(((int *) map) + 3));
	scriptlength = scriptend - scriptoffset;

	// read the samples
	for(int i = 0; i < numsamples; i++) {
		// Sample offsets and lengths are stored in pairs after the initial 16 bytes
		int position = swapEndianLong(*((int *) map + 3 + (2 * i)));

		// skip four bytes of the WAVE header, four of the FMT header, 
		// the FMT chunk and four of the DATA header
		position += swapEndianLong(*(int *)(map + position)) + 8;

		int size = swapEndianLong(*((int *) (map + position)));
		position += 4; // Skip the size field
		
		samples.push_back(make_pair(map + position, size));
	}

	// now we have the samples, read and decode the MNG script
	script = (char *) malloc(scriptlength * sizeof(char));
	if(! script) throw MNGFileException("malloc failed", errno);
	memcpy(script, map + scriptoffset, scriptlength);
	decryptbuf(script, scriptlength);

	std::istringstream tehscript(script);
	mngrestart(&tehscript);
	g_mngfile = this;
	mngparse();
	g_mngfile = 0;
}

void MNGFile::enumerateSamples() {
	vector< pair< char *, int > >::iterator i;
	for(i = samples.begin(); i != samples.end(); i++) {
		printf("Position: %i Length: %i\n", (unsigned int)(*i).first, (*i).second);
		// PlaySound((*i).second.first, (*i).second.second);
	}
}


MNGFile::~MNGFile() {
	free(script);
	munmap(map, filesize);
	fclose(f);
}	

void mngrestart(std::istream *is) {
	if (mnglexer)
		delete mnglexer;
	mnglexer = new mngFlexLexer();
	mnglexer->yyrestart(is);
}

void mngerror(char const *s) {
	throw s; // TODO: probably should have an MNG-specific class
}

/* vim: set noet: */
