// ----------------------------------------------------------------------------
// Filename:	EnginePatch.cpp
// Class:		
// Purpose:		Reuired func
// Description:	Functions required by engine cpp's for which we dont want to 
//				include the functions cpp file 
// ----------------------------------------------------------------------------
#include "../../engine/Display/ErrorMessageHandler.h"
#include "../../engine/FilePath.h"
#include "../../engine/Creature/Brain/BrainScriptFunctions.h"
#include "../../engine/Creature/GenomeStore.h"
#include "../../engine/App.h"

class Creature;
CreaturesArchive& operator >>( CreaturesArchive& ar, Creature * & c)
		{c = NULL; return ar;}

std::string ErrorMessageHandler::Format(std::string baseTag, int offsetID, std::string source, ...)
{
	// Read from catalogue
	std::string unformatted = theCatalogue.Get(baseTag, offsetID);

	// Get variable argument list
	va_list args;
	va_start(args, source);
	char szBuffer[4096];
	int nBuf = vsprintf(szBuffer, unformatted.c_str(), args);
	ASSERT(nBuf >= 0 && nBuf < sizeof(szBuffer) / sizeof(szBuffer[0]));
	va_end(args);

	return source + std::string("\n\n") + std::string(szBuffer);
}



// Return an ascii filespec in the form xxxx.yyy, given fsp as a 4-char token (eg 'NORN')
// and a 3 character string for the suffix.
// Eg. BuildFsp(Tok('g','r','e','n'),"spr") returns "gren.spr"
// SubDir is a constant referring to an optional path to the directory, eg. 
// BuildFsp(0x30303030,"tst",BODY_DATA_DIR) might return "C:\SFC\Body Data\0000.tst"
char* BuildFsp(DWORD fsp,char const* ext, int SubDir)	//subdir =-1  
{
/*    static char SFTemplate[] = "####.###";		// template for filespecs
	static char SFPath[MAX_PATH];				// temp buffer holding path	for caller
	int i;

	char* AscFsp = (char*)&fsp;					// treat DWORD token as 4 chars
  	for (i=0; i<4; i++)							// copy chars into filespec from lo to hi
    	SFTemplate[i] = *AscFsp++;

  	for (i=0; i<3; i++)							// copy extension
		SFTemplate[5+i] = ext[i];

	if	(SubDir!=-1) 							// if a subdirectory is required
		pApp->GetDirectory(SubDir, SFPath);		// copy it to start of path
	else
		SFPath[0] = '\0';						// else path starts with nothing

	strcat(SFPath,SFTemplate);					// add filename to optional path

    // Error handling info.
//	strcpy(dderr_extra, SFPath);

	return SFPath;
*/	return NULL;
}

FilePath::FilePath( std::string name, int directory,bool checkLocallyFirst,bool forceLocal){};

bool FilePath::GetWorldDirectoryVersionOfTheFile(std::string& path, bool forcecreate) const
{return false;}


CreaturesArchive &operator>>(class CreaturesArchive & archive,class Agent * &)
{ return archive;}


std::string GenomeStore::MonikerAsString(int index)
{
	std::string ret_val;
	return ret_val;
} 

std::string GenomeStore::Filename(std::string moniker)
{
	std::string ret_val;
	return ret_val;
}

int64 GetHighPerformanceTimeStamp()
{
	return 0;
}

void __cdecl OutputFormattedDebugString(char const *,...){}

std::string ErrorMessageHandler::ErrorMessageFooter()
{
	std::string temp;
	return temp;
};

//App theApp;

App::App(){};
App::~App(){};
InputManager::InputManager(void){};

