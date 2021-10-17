#include <string>
#include "LobeNames.h"
#include "../../common/Catalogue.h"
#include "../../common/GameInterface.h"
#include "../../common/WhichEngine.h"
#include "../../engine/Creature/Brain/BrainScriptFunctions.h"


extern GameInterface theGameInterface;

LobeNames::LobeNames(Lobes lobes)
{

	myLobes = lobes;

	myLobeNames = new char *[myLobes.size()];
	myNeuronNames = new char **[myLobes.size()];
	for(int l = 0; l != myLobes.size(); l++)
	{
		myLobeNames[l] = NULL;
		myNeuronNames[l] = new char *[GetLobeSize(*myLobes[l])];
	}
	int ln = 0;
	try
	{
		for(; ln != myLobes.size(); ln++)
		{
			// find correct theCatalogue id using quad char array
			const char *lobeQuad = GetLobeName(*myLobes[ln]);
			int offset = theCatalogue.GetArrayCountForTag("Brain Lobe Quads")-1;
			while(offset != -1)
			{
				if(!strcmp(theCatalogue.Get( "Brain Lobe Quads", offset), lobeQuad))
					break;
				offset--;
			}



			if(offset == -1)
				HandleNoLobeDefinition(ln);
			else
			{


				// get proper name
				const char *lobeName = theCatalogue.Get("Brain Lobes", offset);
				myLobeNames[ln] = new char[strlen(lobeName)+1];
				strcpy(myLobeNames[ln], lobeName);
			

				// get lobe details
				const char *lobeNeuronName = theCatalogue.Get("Brain Lobe Neuron Names", offset);
				int defaultNoOfNeurons = theCatalogue.GetArrayCountForTag(lobeNeuronName);

				// set up neuron names
				for(int n = 0; n != GetLobeSize(*myLobes[ln]); n++)
				{
					const char *neuronName;
					if(n < defaultNoOfNeurons)
					{
						std::string decnquad = "decn";
						std::string verbquad = "verb";
						if(decnquad == lobeQuad || verbquad == lobeQuad)
							neuronName = theCatalogue.Get(lobeNeuronName, GetScriptOffsetFromNeuronId(n));
						else
							neuronName = theCatalogue.Get(lobeNeuronName, n);
						myNeuronNames[ln][n] = new char[strlen(neuronName)+1];
						strcpy(myNeuronNames[ln][n], neuronName);
					}
					else
					{
						myNeuronNames[ln][n] = new char[9+20];		
						sprintf(myNeuronNames[ln][n], "Neuron %d", n);
					}
				}
			}
		}

	}
	catch(Catalogue::Err e)
	{
		MessageBox(NULL, e.what(), "LobeNames", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	
		for(; ln != myLobes.size(); ln++)
			HandleNoLobeDefinition(ln);
	}
}


LobeNames::~LobeNames()
{

	for(int l = 0; l != myLobes.size(); l++)
	{
		for(int n = 0; n != GetLobeSize(*myLobes[l]); n++)
			delete []myNeuronNames[l][n];

		delete []myLobeNames[l];
		delete []myNeuronNames[l];
	}
	delete []myLobeNames;
	delete []myNeuronNames;

}

void LobeNames::HandleNoLobeDefinition(int l)
{
	// find correct theCatalogue id using quad char array
	if(myLobeNames[l] == NULL)
	{
		myLobeNames[l] = new char[5];
		strcpy(myLobeNames[l], GetLobeName(*myLobes[l]));
	}

	// set up neuron names
	for(int n = 0; n != GetLobeSize(*myLobes[l]); n++)
	{
		myNeuronNames[l][n] = new char[9+20];		
		sprintf(myNeuronNames[l][n], "Neuron %d", n);
	}
}

