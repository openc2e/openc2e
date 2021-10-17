// WatchStreamer.h: interface for the WatchStreamer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef WATCH_STREAMER_H
#define WATCH_STREAMER_H

#pragma once

#include <string>
#include <vector>

class WatchStreamer  
{
public:
	WatchStreamer(const std::string& extention);
	virtual ~WatchStreamer();
	
	void StreamIn(FGSU fgsu, const std::string& non_fgsu_file);
	void StreamIn(const std::string& gamename, FGSU fgsu, const std::string& non_fgsu_file);
	void StreamIn(const std::string& gamename, const std::string& filename);

	std::vector<std::string> myNames;
	std::vector<std::string> myCAOS;

	std::string myExtention;

private:
	static std::string GetModulePath();

};

#endif // WATCH_STREAMER_H

