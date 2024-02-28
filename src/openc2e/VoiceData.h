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

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

struct VoiceEntry {
	std::string name;
	unsigned int delay_ticks;
};

class VoiceData {
  public:
	VoiceData();
	VoiceData(std::istream& in);
	VoiceData(std::string tagname);
	operator bool() const;

	std::vector<VoiceEntry> GetSyllablesFor(std::string speech);

  protected:
	std::vector<unsigned int> GetSentenceFor(std::string speech);
	bool NextSyllableFor(std::vector<unsigned int>& sentence, unsigned int& pos, VoiceEntry& syllable);

	std::vector<VoiceEntry> voices;
	std::vector<uint32_t> lookup_table;
};

#endif

/* vim: set noet: */
