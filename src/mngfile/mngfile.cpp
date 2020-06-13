/*
 *  mngfile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue 16 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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
#include <assert.h>
#include <sstream>
#include "endianlove.h"
#include "creaturesException.h"
#include "mngfile.h"
#include "mmapifstream.h"

MNGFile *g_mngfile = NULL;
extern int mngparse(); // parser

void decryptbuf(char * buf, int len) {
	int i;
	unsigned char pad = 5;
	for(i = 0; i < len; i++) {
		buf[i] ^= pad;
		pad += 0xC1;
	}
}

MNGFile::MNGFile(std::string n) {
	sampleno = 0;
	name = n;
	
	stream = new mmapifstream(n);
	if (!stream) { delete stream; throw MNGFileException("open failed"); }

	// Read metavariables from beginning of file
	numsamples = swapEndianLong(*((int *) stream->map));
	scriptoffset = swapEndianLong(*(((int *) stream->map) + 1));
	scriptend = swapEndianLong(*(((int *) stream->map) + 3));
	scriptlength = scriptend - scriptoffset;

	// read the samples
	for(int i = 0; i < numsamples; i++) {
		// Sample offsets and lengths are stored in pairs after the initial 16 bytes
		int position = swapEndianLong(*((int *) stream->map + 3 + (2 * i)));

		// skip four bytes of the WAVE header, four of the FMT header, 
		// the FMT chunk and four of the DATA header
		position += swapEndianLong(*(int *)(stream->map + position)) + 8;

		int size = swapEndianLong(*((int *) (stream->map + position)));
		position += 4; // Skip the size field
		
		samples.push_back(std::make_pair(stream->map + position, size));
	}

	// now we have the samples, read and decode the MNG script
	script = (char *) malloc(scriptlength + 1);
	script[scriptlength] = 0;
	if(! script) { delete stream; throw MNGFileException("malloc failed"); }
	memcpy(script, stream->map + scriptoffset, scriptlength);
	decryptbuf(script, scriptlength);

	yyinit(script);
	g_mngfile = this;
	try {
		mngparse();
	} catch (...) {
		delete stream;
		g_mngfile = 0;
		throw;
	}
	g_mngfile = 0;

	processState *p = new processState(this);
	for (std::map<std::string, MNGEffectDecNode *>::iterator i = effects.begin(); i != effects.end(); i++)
		(*i).second->postProcess(p);
	for (std::map<std::string, MNGTrackDecNode *>::iterator i = tracks.begin(); i != tracks.end(); i++)
		(*i).second->postProcess(p);
	// don't call postProcess on variabledec!
	delete p;
}

void MNGFile::enumerateSamples() {
	std::vector< std::pair< char *, int > >::iterator i;
	for(i = samples.begin(); i != samples.end(); i++) {
		printf("Position: \"%p\" Length: %i\n", (void *)(*i).first, (*i).second);
		// PlaySound((*i).second.first, (*i).second.second);
	}
}

unsigned int MNGFile::getSampleForName(std::string name) {
	unsigned int n = -1;

	if (samplemappings.find(name) != samplemappings.end()) {
		n = samplemappings[name];
	} else {
		n = sampleno;
		if (n >= samples.size())
			throw MNGFileException("ran out of samples"); // TODO: more info
		samplemappings[name] = n;
		sampleno++;
	}

	return n;
}

MNGFile::~MNGFile() {
	for (std::map<std::string, MNGEffectDecNode *>::iterator i = effects.begin(); i != effects.end(); i++)
		delete i->second;
	for (std::map<std::string, MNGTrackDecNode *>::iterator i = tracks.begin(); i != tracks.end(); i++)
		delete i->second;
	for (std::map<std::string, MNGVariableDecNode *>::iterator i = variables.begin(); i != variables.end(); i++)
		delete i->second;
	free(script);
	delete stream;
}	

void MNGFile::add(class MNGEffectDecNode *n) {
	effects[n->getName()] = n;
}

void MNGFile::add(class MNGTrackDecNode *n) {
	std::string trackname = n->getName();
	std::transform(trackname.begin(), trackname.end(), trackname.begin(), (int(*)(int))tolower);
	tracks[trackname] = n;
}

void MNGFile::add(class MNGVariableDecNode *n) {
	variables[n->getName()] = n;
}

std::string MNGFile::dump() {
	std::ostringstream oss;

	std::map<std::string, class MNGEffectDecNode *>::iterator ei;
	std::map<std::string, class MNGTrackDecNode *>::iterator ti;

	for (ei = effects.begin(); ei != effects.end(); ei++)
		oss << ei->first << " " << ei->second->dump() << std::endl;

	for (ti = tracks.begin(); ti != tracks.end(); ti++)
		oss << ti->first << " " << ti->second->dump() << std::endl;

	return oss.str();
}

void mngerror(char const *s) {
	throw MNGFileException(s, g_mngfile->yylineno);
}

/* vim: set noet: */
