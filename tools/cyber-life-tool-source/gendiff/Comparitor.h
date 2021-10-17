#ifndef COMPARITOR_H
#define COMPARITOR_H

#include "Genome.h"
#include <ostream>


class Comparitor
{
public:
	void CompareGenomes( Genome& genome1, Genome& genome2, std::ostream& output );
private:

	std::list< Gene* >::iterator FindID( std::list< Gene* >& c, uint8 id );

};

#endif // COMPARITOR_H

