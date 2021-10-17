#ifndef GENE_H
#define GENE_H


#include <cstring>	// for memcpy

#include <string>

typedef unsigned char uint8;

class Gene
{
public:

	// move these into GeneType class?

	// gene types
	enum{ typeBrain=0, typeBiochem=1, typeCreature=2, typeOrgan=3 };

	// subtypes
	enum{ brainLobe=0, brainOrgan=1, brainTract=2 };
	enum{ biochemReceptor=0, biochemEmitter=1, biochemReaction=2,
		biochemHalflife=3, biochemConc=4,
		biochemNeuroEmitter=5 };
	enum{ creatureStimulus=0, creatureGenus=1, creatureAppearance=2,
		creaturePose=3, creatureGait=4, creatureInstinct=5,
		creaturePigment=6, creaturePigmentBleed=7,
		creatureFacialExpression=8 };
	enum{ organDefine=0 };

	// number of distinct type/subtype pairs:
	enum { Gene::NumGeneTypes = 19 };

	Gene( const uint8* raw );
//	virtual ~Gene();

	void HeaderString( std::string& str );
	virtual void Summary( std::string& str )
		{ HeaderString( str ); }

	virtual bool Compare( Gene& other );

	uint8 GetType() const { return myType; }
	uint8 GetSubType() const { return mySubType; }
	uint8 GetID() const { return myID; }
	uint8 GetGeneration() const { return myGeneration; }
	uint8 GetSwitchOnTime() const { return mySwitchOnTime; }
	uint8 GetFlags() const { return myFlags; }
	uint8 GetMutabilityWeighting() const { return myMutabilityWeighting; }
	uint8 GetExpressionVariant() const { return myExpressionVariant; }


	void SetOrganID( int organid )
		{ myOrganID = organid; }
	void SetGeneNumber( int genenumber )
		{ myGeneNumber = genenumber; }
	int GetGeneNumber() const
		{ return myGeneNumber; }
	int GetOrganID() const
		{ return myOrganID; }

private:
	int myGeneNumber;		// num in genome
	int myOrganID;
	uint8 myType;
	uint8 mySubType;
	uint8 myID;
	uint8 myGeneration;
	uint8 mySwitchOnTime;
	uint8 myFlags;				// sex-dependence/mutability
	uint8 myMutabilityWeighting;
	uint8 myExpressionVariant;
};




//-------------------------------------------------------------------
//BYTE [4] Lobe ID
//BYTE [2] Update Time
//BYTE [2] x
//BYYE [2] y
//BYTE Width
//BYTE Height
//BYTE Red
//BYTE Green
//BYTE Blue
//BYTE WTA
//BYTE Tissue
//BYTE [8] spare
//BYTE [48] Initialise Neuron Rule
//BYTE [48] Update Neuron Rule

class BrainLobeGene : public Gene
{
public:
	enum { RawDataSize = 121 };
	BrainLobeGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );

	static Gene* Create( const uint8* raw )
		{ return new BrainLobeGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
//BYTE ClockRate
//BYTE RepairRate
//BYTE LifeForce
//BYTE BioTickStart
//BYTE ATPDamageCoEff
class BrainOrganGene : public Gene
{
public:
	enum { RawDataSize = 5 };
	BrainOrganGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new BrainOrganGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
//BYTE [2] Update Time
//BYTE [4] Source Lobe ID
//BYTE [2] Source Lobe Lower Bound
//BYTE [2] Source Lobe Upper Bound
//BYTE [2] No Source Connections
//BYTE [4] Destination Lobe ID
//BYTE [2] Destination Lobe Lower Bound
//BYTE [2] Destination Lobe Upper Bound
//BYTE [2] No Destination Connections
//BYTE Use Random and Migrating Dendrites
//BYTE No Of Connections Is Random
//BYTE [8] spare
//BYTE [48] Initialize Dendrite Rule
//BYTE [48] Update Dendrite Rule

class BrainTractGene : public Gene
{
public:
	enum { RawDataSize = 128 };
	BrainTractGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new BrainTractGene( raw ); }

private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
//BYTE Organ
//BYTE Tissue
//BYTE Locus
//BYTE Chemical
//BYTE Threshold
//BYTE Nominal
//BYTE Gain
//BYTE Flags
class BiochemReceptorGene : public Gene
{
public:
	enum { RawDataSize = 8 };
	BiochemReceptorGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	virtual void Summary( std::string& str );
	static Gene* Create( const uint8* raw )
		{ return new BiochemReceptorGene( raw ); }

private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
//BYTE Organ
//BYTE Tissue
//BYTE Locus
//BYTE Chemical
//BYTE Threshold
//BYTE Rate
//BYTE Gain
//BYTE Flags

class BiochemEmitterGene : public Gene
{
public:
	enum { RawDataSize = 8 };
	BiochemEmitterGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	virtual void Summary( std::string& str );
	static Gene* Create( const uint8* raw )
		{ return new BiochemEmitterGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
// quant and chems round wrong way?
//BYTE Reactant0
//BYTE Quantity0
//BYTE Reactant1
//BYTE Quantity1
//BYTE Product2
//BYTE Quantity2
//BYTE Product3
//BYTE Quantity3
//BYTE ReactionRate
class BiochemReactionGene : public Gene
{
public:
	enum { RawDataSize = 9 };
	BiochemReactionGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	virtual void Summary( std::string& str );
	static Gene* Create( const uint8* raw )
		{ return new BiochemReactionGene( raw ); }

private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
class BiochemHalflifeGene : public Gene
{
public:
	enum { RawDataSize = 256 };
	BiochemHalflifeGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	virtual void Summary( std::string& str );
	static Gene* Create( const uint8* raw )
		{ return new BiochemHalflifeGene( raw ); }

private:
	uint8 myData[ RawDataSize ];
};


//-------------------------------------------------------------------
class BiochemNeuroEmitterGene : public Gene
{
public:
	enum { RawDataSize = 15 };
	BiochemNeuroEmitterGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new BiochemNeuroEmitterGene( raw ); }

private:
	uint8 myData[ RawDataSize ];
};


//-------------------------------------------------------------------
class BiochemConcGene : public Gene
{
public:
	enum { RawDataSize = 2 };
	BiochemConcGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	virtual void Summary( std::string& str );
	static Gene* Create( const uint8* raw )
		{ return new BiochemConcGene( raw ); }

private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
class CreatureStimulusGene : public Gene
{
public:
	enum { RawDataSize = 13 };
	CreatureStimulusGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreatureStimulusGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
//BYTE Genus
//BYTE [32] MumMoniker
//BYTE [32] DadMoniker

class CreatureGenusGene : public Gene
{
public:
	enum { RawDataSize = 65 };
	CreatureGenusGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreatureGenusGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};


//-------------------------------------------------------------------
class CreatureAppearanceGene : public Gene
{
public:
	enum { RawDataSize = 3 };
	CreatureAppearanceGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreatureAppearanceGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
class CreaturePoseGene : public Gene
{
public:
	enum { RawDataSize = 17 };
	CreaturePoseGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreaturePoseGene( raw ); }

private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
class CreatureGaitGene : public Gene
{
public:
	enum { RawDataSize = 9 };
	CreatureGaitGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreatureGaitGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
class CreatureInstinctGene : public Gene
{
public:
	enum { RawDataSize = 9 };
	CreatureInstinctGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreatureInstinctGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
class CreaturePigmentGene : public Gene
{
public:
	enum { RawDataSize = 2 };
	CreaturePigmentGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreaturePigmentGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};

//-------------------------------------------------------------------
class CreaturePigmentBleedGene : public Gene
{
public:
	enum { RawDataSize = 2 };
	CreaturePigmentBleedGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreaturePigmentBleedGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};



//-------------------------------------------------------------------
class CreatureFacialExpressionGene : public Gene
{
public:
	enum { RawDataSize = 11 };
	CreatureFacialExpressionGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new CreatureFacialExpressionGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};


//-------------------------------------------------------------------
class OrganDefineGene : public Gene
{
public:
	enum { RawDataSize = 5 };
	OrganDefineGene( const uint8* raw ) : Gene(raw)
		{ memcpy( myData,raw+8,sizeof(myData) ); }
	virtual bool Compare( Gene& other );
	static Gene* Create( const uint8* raw )
		{ return new OrganDefineGene( raw ); }
private:
	uint8 myData[ RawDataSize ];
};


#endif // GENE_H

