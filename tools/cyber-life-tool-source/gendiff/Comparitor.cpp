#include "Comparitor.h"
#include "GeneType.h"

#include <iostream>


void Comparitor::CompareGenomes( Genome& genome1, Genome& genome2, std::ostream& output )
{
	int diffcount=0;
	int numchecked=0;
	int genetypeid;



	for( genetypeid=0; genetypeid < GeneType::NumOfTypes(); ++genetypeid )
	{
		std::list< Gene* > c1;
		std::list< Gene* > c2;
		std::list< Gene* >::iterator i1;
		std::list< Gene* >::iterator i2;

		int id;
		const GeneType& gtype = GeneType::GetGeneType(genetypeid);

		output << "--------------- " << gtype.GetShortName() << " ---------------------" << std::endl;

		genome1.CollectGenes( c1, gtype.GetType(), gtype.GetSubType() );
		genome2.CollectGenes( c2, gtype.GetType(), gtype.GetSubType() );

		for( id=0; id<256; ++id )
		{
			i1 = FindID( c1, (uint8)id );
			i2 = FindID( c2, (uint8)id );

			// missing in both?
			if( i1 == c1.end() && i2 == c2.end() )
				continue;

			BiochemEmitterGene* b1 = (BiochemEmitterGene*)*i1;
			BiochemEmitterGene* b2 = (BiochemEmitterGene*)*i2;

			++numchecked;

			if( i2 == c2.end() )
			{
				// only in genome 1
//				std::string str;
//				(*i1)->Summary( str );
//				output << (*i1)->GetGeneNumber() << "< " << str << std::endl;
				output << (*i1)->GetGeneNumber() << "< only in " << genome1.GetName();
				output << std::endl;
				++diffcount;
			}
			else if( i1 == c1.end() )
			{
			// only in genome 2
//			std::string str;
//			(*i2)->Summary( str );
//			output << (*i2)->GetGeneNumber() << "> " << str << std::endl;
				output << (*i2)->GetGeneNumber() << "< only in " << genome2.GetName();
				output << std::endl;
				++diffcount;
			}
			else
			{
				// same?
				if( !(*i1)->Compare( *(*i2) ) )
				{
					// nope	- show them both
					std::string str1;
					std::string str2;
					(*i1)->Summary( str1 );
					(*i2)->Summary( str2 );
					output << (*i1)->GetGeneNumber() << "< " << str1 << std::endl;
					output << (*i2)->GetGeneNumber() << "> " << str2 << std::endl;
					++diffcount;
				}
			}
		}
	}

	output << numchecked << " unique IDs checked, " << diffcount << " differences" << std::endl;
}



std::list< Gene* >::iterator Comparitor::FindID( std::list< Gene* >& c, uint8 id )
{
	std::list< Gene* >::iterator it;
	for( it=c.begin(); it != c.end(); ++it )
	{
		if( (*it)->GetID() == id )
			return it;
	}

	return c.end();
}

