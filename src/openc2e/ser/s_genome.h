#ifndef S_GENOME_H
#define S_GENOME_H 1

#include "genome.h"
#include "serialization.h"

#include <sstream>

SAVE(genomeFile) {
	std::ostringstream genes, notes;
	genes << obj;
	obj.writeNotes(notes);
	std::string genestr = genes.str();
	std::string notestr = notes.str();
	ar& genestr& notestr;
}

LOAD(genomeFile) {
	std::string genes, notes;
	ar& genes& notes;
	std::istringstream genestream(genes), notestream(notes);
	genestream >> std::noskipws >> obj;
	//    obj.readNotes(notestream);
}

#endif
