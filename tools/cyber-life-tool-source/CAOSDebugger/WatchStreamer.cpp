// WatchStreamer.cpp: implementation of the WatchStreamer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "caosdebugger.h"
#include "WatchStreamer.h"
#include "../../common/WhichEngine.h"

#include <sstream>
#include <fstream>

#include <shlwapi.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WatchStreamer::WatchStreamer(const std::string& extention) :
	myExtention(extention)
{

}

WatchStreamer::~WatchStreamer()
{

}

void WatchStreamer::StreamIn(FGSU fgsu, const std::string& non_fgsu_file)
{
	myNames.clear();
	myCAOS.clear();

	StreamIn("", fgsu, non_fgsu_file);
	StreamIn(theWhichEngine.GameName() + "\\", fgsu, non_fgsu_file);
}

void WatchStreamer::StreamIn(const std::string& gamename, FGSU fgsu, const std::string& non_fgsu_file)
{
	// Global
	if (fgsu.f < 0 || fgsu.g < 0 || fgsu.s < 0)
	{
		StreamIn(gamename, non_fgsu_file);
		return;
	}

	// Has classifier
	std::ostringstream name_0;
	name_0 << fgsu.f << ".0.0";
	std::ostringstream name_1;
	name_1 << fgsu.f << "." << fgsu.g << ".0";
	std::ostringstream name_2;
	name_2 << fgsu.f << "." << fgsu.g << "." << fgsu.s;

	StreamIn(gamename, "0.0.0");
	StreamIn(gamename, name_0.str());
	StreamIn(gamename, name_1.str());
	StreamIn(gamename, name_2.str());
}


void WatchStreamer::StreamIn(const std::string& gamename, const std::string& filename)
{
	std::string full_filename = GetModulePath() + "\\" + gamename + filename + myExtention;

	std::ifstream in(full_filename.c_str());
	while (in.good())
	{
		char buff[4096];
		in.getline(buff, 4095);
		char buff2[4096];
		in.getline(buff2, 4095);
		myNames.push_back((strlen(buff) > 0) ? buff : buff2);
		myCAOS.push_back(buff2);
	}

	// blank line to separate files
	myNames.push_back("");
	myCAOS.push_back("");
}

// static 
std::string WatchStreamer::GetModulePath()
{
	std::string strModulePath;

	TCHAR pszModuleFilename[_MAX_PATH];
	if (GetModuleFileName(GetModuleHandle(NULL), pszModuleFilename, _MAX_PATH))
	{
		PathRemoveFileSpec(pszModuleFilename);
		strModulePath = pszModuleFilename;
	}

	return strModulePath;
}

