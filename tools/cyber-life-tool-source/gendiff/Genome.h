#ifndef GENOME_H
#define GENOME_H

#ifdef _MSC_VER
// turn off warning about symbols too long for debugger
#pragma warning (disable : 4786)
#endif // _MSC_VER

#include <vector>
#include <list>
#include <string>

#include "Gene.h"

class Genome
{
public:
	class Err
	{
	public:
		Err( const std::string& msg ) : myText( msg ) {}
		const char* what() { return myText.c_str(); }
	private:
		std::string myText;
	};


	Genome( const std::string& genfile );
	~Genome();

	int GetGeneCount() const { return myGenes.size(); }
	Gene& GetGene(int i) { return *(myGenes[i]); }

	void TestStuff();

	void CollectGenes( std::list< Gene* >& genes, uint8 type, uint8 subtype );

	const std::string& GetName() const
		{ return myFilename; }
private:
	int GeneDataSize( const uint8* rawheader );
	Gene* Genome::SlurpGene( std::istream& ins );

	std::vector< Gene* > myGenes;

	std::string myFilename;
};


#endif // GENOME_H

