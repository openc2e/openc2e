/*
 *  VoiceData.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 12 May 2010.
 *  Copyright (c) 2010 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#ifndef _VOICEDATA_H
#define _VOICEDATA_H

#include "endianlove.h"
#include <vector>
#include <string>
#include <istream>

struct VoiceEntry {
	std::string name;
	unsigned int delay;
};

class VoiceData {
protected:
	std::vector<VoiceEntry> Voices;
	std::vector<uint32> LookupTable;

public:
	VoiceData(std::ifstream &file);
	VoiceData(std::string tagname);

	std::vector<unsigned int> GetSentenceFor(std::string in);
	bool NextSyllableFor(std::vector<unsigned int> &sentence, unsigned int &pos, VoiceEntry &syllable);
};

#endif

/* vim: set noet: */
