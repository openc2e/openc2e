//	SVRule captions are stored at the begining to the captions(.gno) file 
//  The file structure is as follows:
//	file header = <int ?><int numsvrules>
//	gene header = <int genetype><int genesubtype><int gene id><int rule number><int array end marker?>
//	<SVRule::length no of strings><int array end marker>
//  <note string>
//
//	a string has the following format = <int string length>[string length no of characters]
//
//	After SVcaptions come the normal gene captions which are not read here
//  For integers the least significat bit is stored first.

#include "SVCaptions.h"

SVCaptions::SVCaptions(const std::string filename, Genome &genome, Lobes &lobes, Tracts &tracts, int finalGeneAge):
myNoCaptions(0), myFilesize(0), myCaptions(NULL), myLobes(lobes), myTracts(tracts)
{

	for(int geneAge = 0; geneAge <= finalGeneAge; geneAge++)
	{

		genome.SetAge(geneAge);
		genome.Reset();

		// get unique lobe ids from genome
		while (genome.GetGeneType(BRAINGENE,G_LOBE,NUMBRAINSUBTYPES)) 
		{
			try {
				genome.AdjustGenePointerBy(-6);	// back up to unique id cos GetgeneType skips it
				myLobeCaptions.push_back( Caption() );
				myLobeCaptions.back().uniqueId = genome.GetCodon(0, 255);
				genome.AdjustGenePointerBy(+5);
				{
					Lobe lobe(genome);	// try to create - if fails this gene was ignored
				}
			} catch(GenomeInitFailedException) {}
		}

		// get unique tract ids from genome
		genome.Reset();
		while (genome.GetGeneType(BRAINGENE,G_TRACT,NUMBRAINSUBTYPES)) 
		{
			try {
				genome.AdjustGenePointerBy(-6);	// back up to unique id cos GetgeneType skips it
				myTractCaptions.push_back(Caption());
				myTractCaptions.back().uniqueId = genome.GetCodon(0, 255);
				genome.AdjustGenePointerBy(+5);
				{
					Tract tract(genome, myLobes);	// try to create tact - if fails this gene was ignored
				}
			} catch(GenomeInitFailedException) {}
		}
	}

	// load captions 
	LoadCaptions(filename);
}

SVCaptions::~SVCaptions()
{
}

void SVCaptions::LoadCaptions(std::string captionsFilename)
{
	int length = captionsFilename.find_last_of(".");

	if(length != std::string::npos) captionsFilename.resize(length);
	// add extension in correct place
	captionsFilename += ".gno";

	// load whole file
	OFSTRUCT ofstruct;
	DWORD bytesRead;
	HFILE hFile = OpenFile(captionsFilename.begin(), &ofstruct, OF_READ);
	int iSize = GetFileSize((HANDLE)hFile, NULL);
	
	if(iSize > 0)
	{
		//load file is present and set sizes
		myCaptions = new unsigned char[iSize];
		ReadFile((HANDLE)hFile, myCaptions, iSize, &bytesRead, NULL) ; 
		_lclose(hFile);
		myCaptions += 2;	//skip first integer
		myNoCaptions = *myCaptions | (*++myCaptions << 8);	// read nosvrules
		myCaptions++;
		myFilesize = iSize - 4;
	}

	for(int l = 0; l != myLobes.size(); l++)
	{
		GetCaption(BRAINGENE, G_LOBE, myLobeCaptions[l].uniqueId, 0, myLobeCaptions[l].comment[0], &myLobeCaptions[l].notes[0]);
		GetCaption(BRAINGENE, G_LOBE, myLobeCaptions[l].uniqueId, 1, myLobeCaptions[l].comment[1], &myLobeCaptions[l].notes[1]);
	}

	for(int t = 0; t != myTracts.size(); t++)
	{
		GetCaption(BRAINGENE, G_TRACT, myTractCaptions[t].uniqueId, 0, myTractCaptions[t].comment[0], &myTractCaptions[t].notes[0]);
		GetCaption(BRAINGENE, G_TRACT, myTractCaptions[t].uniqueId, 1, myTractCaptions[t].comment[1], &myTractCaptions[t].notes[1]);
	}
	
	if(myFilesize != 0)
		delete [](myCaptions-4);

}





void SVCaptions::GetCaption(int geneType, int geneSubType, int geneID, int ruleNo, std::string comments[], std::string *notes)
{	
	int gT=-1, gST=-1, gID=-1, rNo=-1;
	int c=0;
	unsigned char *reader = myCaptions;

	while(c != myNoCaptions)
	{
		// read caption header
		gT = *reader | (*++reader << 8);
		gST = *++reader | (*++reader << 8);
		gID = *++reader | (*++reader << 8);
		rNo = *++reader | (*++reader << 8);
		reader+=3;		// move off rNo and skip array end marker
		
		if(geneType == gT && geneSubType == gST && geneID == gID && ruleNo == rNo)
			break;	// found caption
			
		if(!(reader = FindNextCaption(reader)))
			break;	// error end of file reached
		c++;
	}

	if(geneType == gT && geneSubType == gST && geneID == gID && ruleNo == rNo)
		CopyCaption(reader, comments, notes);
	else
		CopyBlankCaption(comments, notes);
}


unsigned char * SVCaptions::FindNextCaption(unsigned char *reader) 
{
	int stringLength, i;
	for(int s = 0; s != SVRule::length; s++)	// for number of strings
	{
		stringLength = *reader | (*++reader << 8);
		if(stringLength)
		{
			i = 0;
			while(i++ != stringLength)	// find string terminator
			{
				if(reader-myCaptions == myFilesize) return NULL;	//error EOF
				reader++;
			}
		}
		reader++;	//move on to next stringLength
	}
	reader+=2;	// end of array marker

	stringLength = *reader | (*++reader << 8);
	reader++;
	if(stringLength)
	{
		i = 0;
		while(i++ != stringLength)	// find string terminator
		{
			if(reader-myCaptions == myFilesize) return NULL;	//error EOF
			reader++;
		}
	}
	return reader;
}


void SVCaptions::CopyCaption(unsigned char *startPos, std::string comments[], std::string *notes)
{
	unsigned char *reader = startPos;
	int stringLength;
	int s;
	for(s = 0; s != SVRule::length; s++)	// for number of strings
	{
		stringLength = *reader | (*++reader << 8);
		reader++;
		if(stringLength && (reader+stringLength)-myCaptions != myFilesize)
		{
			// read string
			char *temp = new char[stringLength+1];
			int c;
			for(c=0; c != stringLength; c++) 
				temp[c] = *reader++;
			temp[c] = 0;	
			comments[s] = temp;
			delete temp;
		}
		else
		{
			// comment of no length or error
			comments[s] = "No Comment";
		}
	}
	reader+=2;

	// read general notes string
	stringLength = *reader | (*++reader << 8);
	reader++;
	if(stringLength && (reader+stringLength)-myCaptions != myFilesize)
	{
		// read string
		char *temp = new char[stringLength+1];
		int c;
		for(c=0; c != stringLength; c++) 
			temp[c] = *reader++;
		temp[c] = 0;
		*notes = temp;
	}
	else
	{
		// comment of no length or error
		*notes = "No Comment";
	}

}	


void SVCaptions::CopyBlankCaption(std::string comments[], std::string *notes)
{
	for(int s = 0; s != SVRule::length; s++)	// for number of strings
	{
		comments[s] = "No Comment";
	}
	*notes = "No Comment";
	return;
}	

