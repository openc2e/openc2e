#ifndef SVCaptions_H
#define SVCaptions_H

#include "../../engine/Creature/Brain/BrainAccess.h"
#include "../../engine/Creature/Genome.h"
#include <vector>
#include <string>

class SVCaptions : public BrainAccess
{
public:

	typedef struct 
	{
		int uniqueId;
		std::string comment[2][SVRule::length];
		std::string notes[2];
	} Caption;


	SVCaptions(const std::string filename, Genome &genome, Lobes &lobes, Tracts &tracts, int finalgeneAge);
	~SVCaptions();

	const Caption *const GetCaption(Lobe *lobe)
	{
		int l;
		if((l = LobeIdInList(*lobe)) != -1)
			return &myLobeCaptions[l%myLobes.size()];
		else
			return NULL;
	}

	const Caption *GetCaption(Tract *tract)
	{
		int t;
		if((t = TractIdInList(*tract)) != -1)
			return &myTractCaptions[t%myTracts.size()];
		else
			return NULL;
	}


private:

	Lobes &myLobes;
	Tracts &myTracts;
	std::vector<Caption>myTractCaptions;	
	std::vector<Caption>myLobeCaptions;	
	unsigned char *myCaptions;
	int myFilesize;
	int myNoCaptions;

	void LoadCaptions(std::string captionsFilename);
	void GetCaption(int geneType, int geneSubType, int geneID, int ruleNo, std::string comments[], std::string *notes);
	unsigned char *FindNextCaption(unsigned char *reader);
	void CopyCaption(unsigned char *startPos, std::string comments[], std::string *notes);
	void CopyBlankCaption(std::string comments[], std::string *notes);
};

#endif

