/*
 *  Scriptorium.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on 05/11/2004.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include "Scriptorium.h"
#include <iostream>

inline unsigned int Scriptorium::calculateValue(unsigned char family, unsigned char genus, unsigned short species) {
	return (family + (genus << 8) + (species << 16));
}

void Scriptorium::addScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event, script *s) {
	std::map<unsigned short, script *> &m = getScripts(calculateValue(family, genus, species));
	if (m[event])
		m[event]->release();
	std::cerr << "map script " << (int)family << " " << (int)genus << " " <<
		species << " " << event << " to " << (void *)s << " was " 
		<< (void *)m[event] << std::endl;
	m[event] = s;
	s->retain();
}

void Scriptorium::delScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	std::map<unsigned short, script *> &i = getScripts(calculateValue(family, genus, species));
	if (i[event])
		i[event]->release();
	i.erase(i.find(event));
	// todo: zap from the main map if there is none left of that value
}

script *Scriptorium::getScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	return getScripts(calculateValue(family, genus, species))[event];
}
/* vim: set noet: */
