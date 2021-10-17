#include "Genome.h"
#include "GeneType.h"

#include <fstream>
#include <iostream>
#include <ios>


Genome::Genome( const std::string& genfile ) : myFilename( genfile )
{
	uint8 buf[4];
	std::ifstream ins( genfile.c_str(), std::ios::in|std::ios::binary );
	int genenumber=0;
	int currentorganid=0;	// start from 0?

	if( !ins )
		throw Err( "Couldn't open file" );

	ins.read( (char*)buf, 4 );
	if( !( buf[0] == 'd' &&
		buf[1] == 'n' &&
		buf[2] == 'a' &&
		buf[3] == '3' ) )
	{
		throw Err( "Not a genfile ('dna3' cookie missing)" );
	}

	// step through genes
	while( true )
	{
//		std::cout << "[" << (int)ins.tellg() << "] ";

		// end-of-genome?
		ins.read( (char*)buf, 4 );
		if( buf[0] == 'g' &&
			buf[1] == 'e' &&
			buf[2] == 'n' &&
			buf[3] == 'd' )
		{
			break;
		}
		// sanity check
		if( !( buf[0] == 'g' &&
			buf[1] == 'e' &&
			buf[2] == 'n' &&
			buf[3] == 'e' ) )
		{
			throw Err( "Error loading genome (Expected 'gene' token)" );
		}

		myGenes.push_back( SlurpGene( ins ) );
		Gene& g = *(myGenes.back());
		g.SetGeneNumber( genenumber++ );

		// new organ gene?
		if( g.GetType() == Gene::typeOrgan &&
			g.GetSubType() == Gene::organDefine )
		{
			++currentorganid;
		}

		g.SetOrganID( currentorganid );
	}

	ins.close();
}


Genome::~Genome()
{
	while( !myGenes.empty() )
	{
		delete myGenes.back();
		myGenes.pop_back();
	}
}

Gene* Genome::SlurpGene( std::istream& ins )
{
	int size;
	uint8 buf[512];
	Gene* g = NULL;
	const GeneType* gtype=NULL;

	ins.read( (char*)buf, 8 );			// read gene header


	// find type
	gtype = GeneType::Find( buf[0], buf[1] );
	if( !gtype )
		throw Err( "Error reading genfile - unknown type/subtype found" );

	size = gtype->GetRawSize();
	ins.read( (char*)(&buf[8]), size );	// read it

	return gtype->CreateFromRaw( buf );
}



void Genome::CollectGenes( std::list< Gene* >& genes, uint8 type, uint8 subtype )
{
	std::vector< Gene* >::iterator it;

	for( it=myGenes.begin(); it != myGenes.end(); ++it )
	{
		if( (*it)->GetType() == type &&
			(*it)->GetSubType() == subtype )
		{
			genes.push_back( *it );
		}
	}
}

