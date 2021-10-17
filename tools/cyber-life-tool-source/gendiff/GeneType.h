#ifndef GENETYPE_H
#define GENETYPE_H

#include "Gene.h"



class GeneType
{
public:
	GeneType( const std::string& shortname, uint8 type, uint8 subtype, int rawsize,
		Gene* (*createfunction)( const uint8* rawdata ) );

	uint8 GetType() const { return myType; }
	uint8 GetSubType() const { return mySubType; }
	int GetRawSize() const { return myRawSize; }
	const std::string& GetShortName() const { return myShortName; }
	Gene* CreateFromRaw ( const uint8* raw ) const
		{ return myCreationFunction( raw ); }

	//
	static const GeneType* Find( uint8 type, uint8 subtype );
	static int NumOfTypes() { return Gene::NumGeneTypes; }
	static const GeneType& GetGeneType(int i) { return myTypes[i]; }


private:
	static GeneType myTypes[ Gene::NumGeneTypes ];

	uint8	myType;
	uint8	mySubType;
	int		myRawSize;
	std::string	myShortName;
	Gene*	(*myCreationFunction)( const uint8* rawdata );
};

#endif // GENETYPE_H

