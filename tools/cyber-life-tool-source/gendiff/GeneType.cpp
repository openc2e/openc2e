
#include "GeneType.h"

GeneType::GeneType( const std::string& shortname, uint8 type, uint8 subtype,
	int rawsize, Gene* (*createfunction)( const uint8* rawdata ) ) :
	myType(type),
	mySubType(subtype),
	myRawSize(rawsize),
	myShortName(shortname),
	myCreationFunction(createfunction)
{
}

// static data
GeneType GeneType::myTypes[ Gene::NumGeneTypes ] =
{
	GeneType( "brlobe",
		Gene::typeBrain, Gene::brainLobe,
		BrainLobeGene::RawDataSize,
		BrainLobeGene::Create ),
	GeneType( "borgan",
		Gene::typeBrain, Gene::brainOrgan,
		BrainOrganGene::RawDataSize,
		BrainOrganGene::Create  ),
	GeneType( "btract",
		Gene::typeBrain, Gene::brainTract,
		BrainTractGene::RawDataSize,
		BrainTractGene::Create  ),
	GeneType( "recptr",
		Gene::typeBiochem, Gene::biochemReceptor,
		BiochemReceptorGene::RawDataSize,
		BiochemReceptorGene::Create  ),
	GeneType( "emittr",
		Gene::typeBiochem, Gene::biochemEmitter,
		BiochemEmitterGene::RawDataSize,
		BiochemEmitterGene::Create ),
	GeneType( "reactn",
		Gene::typeBiochem, Gene::biochemReaction,
		BiochemReactionGene::RawDataSize,
		BiochemReactionGene::Create ),
	GeneType( "hflife",
		Gene::typeBiochem, Gene::biochemHalflife,
		BiochemHalflifeGene::RawDataSize,
		BiochemHalflifeGene::Create ),
	GeneType( "conc  ",
		Gene::typeBiochem, Gene::biochemConc,
		BiochemConcGene::RawDataSize,
		BiochemConcGene::Create ),
	GeneType( "nuemtr",
		Gene::typeBiochem, Gene::biochemNeuroEmitter,
		BiochemNeuroEmitterGene::RawDataSize,
		BiochemNeuroEmitterGene::Create ),
	GeneType( "stimls",
		Gene::typeCreature, Gene::creatureStimulus,
		CreatureStimulusGene::RawDataSize,
		CreatureStimulusGene::Create ),
	GeneType( "genus ",
		Gene::typeCreature, Gene::creatureGenus,
		CreatureGenusGene::RawDataSize,
		CreatureGenusGene::Create ),
	GeneType( "appear",
		Gene::typeCreature, Gene::creatureAppearance,
		CreatureAppearanceGene::RawDataSize,
		CreatureAppearanceGene::Create ),
	GeneType( "pose  ",
		Gene::typeCreature, Gene::creaturePose,
		CreaturePoseGene::RawDataSize,
		CreaturePoseGene::Create ),
	GeneType( "gait  ",
		Gene::typeCreature, Gene::creatureGait,
		CreatureGaitGene::RawDataSize,
		CreatureGaitGene::Create ),
	GeneType( "insnct",
		Gene::typeCreature, Gene::creatureInstinct,
		CreatureInstinctGene::RawDataSize,
		CreatureInstinctGene::Create ),
	GeneType( "pigmnt",
		Gene::typeCreature, Gene::creaturePigment,
		CreaturePigmentGene::RawDataSize,
		CreaturePigmentGene::Create ),
	GeneType( "pbleed",
		Gene::typeCreature, Gene::creaturePigmentBleed,
		CreaturePigmentBleedGene::RawDataSize,
		CreaturePigmentBleedGene::Create ),
	GeneType( "facexp",
		Gene::typeCreature, Gene::creatureFacialExpression,
		CreatureFacialExpressionGene::RawDataSize,
		CreatureFacialExpressionGene::Create ),
	GeneType( "organ ",
		Gene::typeOrgan, Gene::organDefine,
		OrganDefineGene::RawDataSize,
		OrganDefineGene::Create )
};




const GeneType* GeneType::Find( uint8 type, uint8 subtype )
{
	int i;


	for( i=0; i<Gene::NumGeneTypes; ++i )
	{
		if( myTypes[i].GetType() == type &&
			myTypes[i].GetSubType() == subtype )
		{
			return &(myTypes[i]);
		}
	}

	return NULL;	// none found
}



