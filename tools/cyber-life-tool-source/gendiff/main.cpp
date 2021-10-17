#include "genome.h"
#include "Comparitor.h"

#include <iostream>


const int ver_major=0;
const int ver_minor=1;

int main( int argc, char* argv[] )
{
	if( argc < 3 )
	{
		std::cerr << "gendiff (version ";
		std::cerr << ver_major << "." << ver_minor << ")" << std::endl;
		std::cerr << "usage: gendiff file1.gen file2.gen" << std::endl;
		return 0;
	}

	try
	{
		Comparitor comp;
		Genome g1( argv[1] );
		Genome g2( argv[2] );


		comp.CompareGenomes( g1, g2, std::cout );
//		g.TestStuff();
	}
	catch( Genome::Err& e )
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	catch( ... )
	{
		std::cerr << "bugger." << std::endl;
	}



	return 0;
}

