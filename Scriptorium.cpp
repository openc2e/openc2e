/*
 *  Scriptorium.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on 05/11/2004.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include "Scriptorium.h"

unsigned int Scriptorium::calculateValue(unsigned char family, unsigned char genus, unsigned short species) {
	return (family + (genus << 8) + (species << 16));
}

void Scriptorium::addScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event, script &script) {
	getScripts(calculateValue(family, genus, species))[event] = script;
}

void Scriptorium::delScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	std::map<unsigned short, script> &i = getScripts(calculateValue(family, genus, species));
	i.erase(i.find(event));
	// todo: zap from the main map if there is none left of that value
}

script &Scriptorium::getScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	return getScripts(calculateValue(family, genus, species))[event];
}
