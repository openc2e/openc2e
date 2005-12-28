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

void Scriptorium::addScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event, shared_ptr<script> s) {
	std::map<unsigned short, shared_ptr<script> > &m = getScripts(calculateValue(family, genus, species));
	m[event] = s;
}

void Scriptorium::delScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	std::map<unsigned short, shared_ptr<script> > &i = getScripts(calculateValue(family, genus, species));
	i.erase(i.find(event));
	// todo: zap from the main map if there is none left of that value
}

shared_ptr<script> Scriptorium::getScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	std::map<unsigned short, shared_ptr<script> > &x = getScripts(calculateValue(family, genus, species));
	if (x.find(event) == x.end()) {
		std::map<unsigned short, shared_ptr<script> > &x = getScripts(calculateValue(family, genus, 0));
		if (x.find(event) == x.end()) {
			std::map<unsigned short, shared_ptr<script> > &x = getScripts(calculateValue(family, 0, 0));
			if (x.find(event) == x.end())
				return shared_ptr<script>();
			else
				return x[event];
		} else return x[event];
	} else return x[event];
}

/* vim: set noet: */
