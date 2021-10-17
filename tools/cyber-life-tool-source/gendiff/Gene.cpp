#include "Gene.h"

#include <sstream>
#include <iomanip>

Gene::Gene( const uint8* raw )
{
	myType = raw[0];
	mySubType = raw[1];
	myID = raw[2];
	myGeneration = raw[3];
	mySwitchOnTime = raw[4];
	myFlags = raw[5];
	myMutabilityWeighting = raw[6];
	myExpressionVariant = raw[7];

	myOrganID = 0;	// default (body) organ
};



void Gene::HeaderString( std::string& str )
{
	static char* switchon_names[7] =
		{ "bab", "kid", "ado", "you", "adu", "old", "sen" };

	std::ostringstream os;

	os << "[";
//	os << (int)myType;
//	os << " " << (int)mySubType;
	os << std::setw(2) << (int)myOrganID;
	os << std::setw(3) << (int)myID;
	os << std::setw(2) << (int)myGeneration;
	os << " " << switchon_names[ mySwitchOnTime ];
	os << " ";

	if( (myFlags & 16) && !(myFlags & 8) )
		os << "F";
	else if( !(myFlags & 16) && (myFlags & 8) )
		os << "M";
	else if( !(myFlags & 16) && !(myFlags & 8) )
		os << "-";
	else
		os << "?";	//suspect: M and F flags mutually exclusive
	os << ((myFlags & 4) ? "Del" : "---");
	os << ((myFlags & 2) ? "Dup" : "---");
	os << ((myFlags & 1) ? "Mut" : "---");

	os << " " << (int)myMutabilityWeighting;
	os << " " << (int)myExpressionVariant;
	os << "]";

	str = os.str();
}



// virtual
bool Gene::Compare( Gene& other )
{
	if( myOrganID == other.GetOrganID() &&
		myType == other.GetType() &&
		mySubType == other.GetSubType() &&
		myID == other.GetID() &&
		myGeneration == other.GetGeneration() &&
		mySwitchOnTime == other.GetSwitchOnTime() &&
		myFlags == other.GetFlags() &&
		myMutabilityWeighting == other.GetMutabilityWeighting() &&
		myExpressionVariant == other.GetExpressionVariant() )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool BrainOrganGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BrainOrganGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


//------------------------------------------------------------------------


//virtual
bool BrainLobeGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BrainLobeGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool BrainTractGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BrainTractGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool BiochemReceptorGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BiochemReceptorGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


//virtual
void BiochemReceptorGene::Summary( std::string& str )
{
	HeaderString( str );

	//BYTE Organ
//BYTE Tissue
//BYTE Locus
//BYTE Chemical
//BYTE Threshold
//BYTE Nominal
//BYTE Gain
//BYTE Flags


	std::ostringstream oss;

	oss << (int)myData[0] << " ";
	oss << (int)myData[1] << " ";
	oss << (int)myData[2] << " ";
	oss << "chem" << (int)myData[3] << " ";
	oss << (int)myData[4] << " ";
	oss << (int)myData[5] << " ";
	oss << (int)myData[6] << " ";
	oss << (int)myData[7];

	str += oss.str();
}



//------------------------------------------------------------------------

//virtual
bool BiochemEmitterGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BiochemEmitterGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}



//virtual
void BiochemEmitterGene::Summary( std::string& str )
{
	HeaderString( str );

	//BYTE Organ
//BYTE Tissue
//BYTE Locus
//BYTE Chemical
//BYTE Threshold
//BYTE Nominal
//BYTE Gain
//BYTE Flags


	std::ostringstream oss;

	oss << (int)myData[0] << " ";
	oss << (int)myData[1] << " ";
	oss << (int)myData[2] << " ";
	oss << "chem" << (int)myData[3] << " ";
	oss << (int)myData[4] << " ";
	oss << (int)myData[5] << " ";
	oss << (int)myData[6] << " ";
	oss << (int)myData[7];

	str += oss.str();
}


//------------------------------------------------------------------------

//virtual
bool BiochemReactionGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BiochemReactionGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


//virtual
void BiochemReactionGene::Summary( std::string& str )
{
	HeaderString( str );

	std::ostringstream oss;

	oss << " ";
	oss << (int)myData[0] << "chem" << (int)myData[1] << "+";
	oss << (int)myData[2] << "chem" << (int)myData[3] << "-->";
	oss << (int)myData[4] << "chem" << (int)myData[5] << "+";
	oss << (int)myData[6] << "chem" << (int)myData[7];
	oss << " (rate=" << (int)myData[8] << ")";

	str += oss.str();
}



//------------------------------------------------------------------------


//virtual
bool BiochemHalflifeGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BiochemHalflifeGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//virtual
void BiochemHalflifeGene::Summary( std::string& str )
{
	HeaderString( str );

	std::ostringstream oss;
	int i;
	for( i=0; i<=255; ++i )
		oss << std::setw(4) << (int)myData[i];
	str += oss.str();
}


//------------------------------------------------------------------------




//virtual
bool BiochemConcGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BiochemConcGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}
//virtual
void BiochemConcGene::Summary( std::string& str )
{
	HeaderString( str );

	std::ostringstream oss;
	oss << " chem" << (int)myData[0] << ": " << (int)myData[1];
	str += oss.str();
}


//------------------------------------------------------------------------


//virtual
bool BiochemNeuroEmitterGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((BiochemNeuroEmitterGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


//------------------------------------------------------------------------

//virtual
bool CreatureStimulusGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreatureStimulusGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


//------------------------------------------------------------------------

//virtual
bool CreatureGenusGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreatureGenusGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


//------------------------------------------------------------------------
//virtual
bool CreatureAppearanceGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreatureAppearanceGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool CreaturePoseGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreaturePoseGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool CreatureGaitGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreatureGaitGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool CreatureInstinctGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreatureInstinctGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool CreaturePigmentGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreaturePigmentGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


//------------------------------------------------------------------------
//virtual
bool CreaturePigmentBleedGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreaturePigmentBleedGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool CreatureFacialExpressionGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((CreatureFacialExpressionGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}

//------------------------------------------------------------------------

//virtual
bool OrganDefineGene::Compare( Gene& other )
{
	if( !Gene::Compare( other ) )
		return false;

	// can assume same class because of matching gene header
	// (a bit icky, really)
	if( memcmp( myData, ((OrganDefineGene&)other).myData,
		RawDataSize ) == 0 )
	{
		return true;
	}
	else
		return false;
}


