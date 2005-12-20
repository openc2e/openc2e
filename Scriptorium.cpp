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
	if (m.find(event) != m.end())
		m[event]->release();
	m[event] = s;
	s->retain();
}

void Scriptorium::delScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	std::map<unsigned short, script *> &i = getScripts(calculateValue(family, genus, species));
	if (i.find(event) != i.end())
		i[event]->release();
	i.erase(i.find(event));
	// todo: zap from the main map if there is none left of that value
}

script *Scriptorium::getScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	std::map<unsigned short, script *> &x = getScripts(calculateValue(family, genus, species));
	if (x.find(event) == x.end()) {
		std::map<unsigned short, script *> &x = getScripts(calculateValue(family, genus, 0));
		if (x.find(event) == x.end()) {
			std::map<unsigned short, script *> &x = getScripts(calculateValue(family, 0, 0));
			if (x.find(event) == x.end())
				return 0;
			else
				return x[event];
		} else return x[event];
	} else return x[event];
}

/* vim: set noet: */
