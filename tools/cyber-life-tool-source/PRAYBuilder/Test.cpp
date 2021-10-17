#include "../../Common/PrayFiles/PrayManager.h"
#include "../../Common/PrayFiles/PrayException.h"
#include "../../Common/PrayFiles/StringIntGroup.h"

#include "stdio.h"

#include <vector>
#include <string>
#include <strstream>
#include <ios>

int main(int argc,char* argv[])
{
	PrayManager p;
	p.SetLanguage("en-gb");
	/*
	char* testString = "One fine day, in the middle of the night, two dead men got up to fight. Back to back they faced each other. Drew their swords and shot each other. Isn't Daniel a clever boy? Aaah Smelly cat, Smelly cat, what are they feeding you? Smelly cat, Smelly cat, it's not your fault!!!";
	unsigned char* testData = (unsigned char*)testString;
	p.AddChunkToFile("My Ditty","TEXT","C:\\pray\\test.pray",strlen(testString),testData,false);
	p.AddChunkToFile("My Song","TEXT","C:\\pray\\test.pray",strlen(testString),testData,true);
	*/

	StringIntGroup sig;

	sig.AddString("Greeting","Hello you person you!");
	sig.AddInt("Cost",500);

	std::ostrstream os;

	sig.SaveToStream(os);

	char* str = os.str();
	int leng = os.pcount();
	p.AddChunkToFile("My Map","AGNT","c:\\pray\\maps.pray",leng,(unsigned char*)str,true);

	p.AddDir("C:\\pray\\");
	p.RescanFolders();

	std::vector<std::string> names;
	p.GetChunks("TEXT",names);
	for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++)
		printf("%s\n",(*it).c_str());
	{
		PrayChunkPtr pcp(p.GetChunk("My Song"));
		std::string pcpstr;
		pcpstr.resize(pcp->GetSize());
		for(int i=0;i<pcp->GetSize();i++)
			pcpstr.at(i) = (char)(pcp->GetData()[i]);
		printf("Chunk \"My Song\" is %d bytes long and reads %s",pcp->GetSize(),pcpstr.c_str());
	}

	{
		PrayChunkPtr sigp(p.GetChunk("My Map"));
		std::istrstream ist((char*)sigp->GetData(),sigp->GetSize());
		StringIntGroup rsig(ist);
		std::string greeting;
		int cost;
		if (!rsig.FindString("Greeting",greeting))
			printf("Pants - Stringread failed\n");
		if (!rsig.FindInt("Cost",cost))
			printf("Pants - Intread failed\n");
		printf("%s, %d.\n",greeting.c_str(),cost);
	}
	p.GarbageCollect(false);
	return 0;
}

