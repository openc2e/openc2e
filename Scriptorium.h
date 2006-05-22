/*
 *  Scriptorium.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 10 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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

#ifndef _SCRIPTORIUM_H
#define _SCRIPTORIUM_H

#include "caosScript.h"
#include "openc2e.h"
#include <map>

class Scriptorium {
protected:
	FRIEND_SERIALIZE(Scriptorium);
	// unsigned int = combined family/genus/species
	// unsigned short = event id
	std::map<unsigned int, std::map<unsigned short, shared_ptr<script> > > scripts;
	
	std::map<unsigned short, shared_ptr<script> > &getScripts(unsigned int value) { return scripts[value]; }
	unsigned int calculateValue(unsigned char family, unsigned char genus, unsigned short species);

public:
	void addScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event, shared_ptr<script> s);
	void delScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event);
	shared_ptr<script> getScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event);
};

#endif
/* vim: set noet: */
