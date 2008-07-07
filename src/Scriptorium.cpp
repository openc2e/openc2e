/*
 *  Scriptorium.cpp
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
	// Retrieve the list of scripts for the classifier, return if there aren't any.
	std::map<unsigned int, std::map<unsigned short, shared_ptr<script> > >::iterator x = scripts.find(calculateValue(family, genus, species));
	if (x == scripts.end())
		return;

	// Retrieve the script, return if it doesn't exist.
	std::map<unsigned short, shared_ptr<script> >::iterator j = x->second.find(event);
	if (j == x->second.end())
		return;

	// Erase the script.
	x->second.erase(j);

	// If there are no scripts left, erase the whole list of scripts for this classifier.
	if (x->second.size() == 0)
		scripts.erase(x);
}

shared_ptr<script> Scriptorium::getScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short event) {
	std::map<unsigned short, shared_ptr<script> > &x = getScripts(calculateValue(family, genus, species));
	if (x.find(event) == x.end()) {
		std::map<unsigned short, shared_ptr<script> > &x = getScripts(calculateValue(family, genus, 0));
		if (x.find(event) == x.end()) {
			std::map<unsigned short, shared_ptr<script> > &x = getScripts(calculateValue(family, 0, 0));
			if (x.find(event) == x.end()) {
				std::map<unsigned short, shared_ptr<script> > &x = getScripts(calculateValue(0, 0, 0));
				if (x.find(event) == x.end()) {
					return shared_ptr<script>();
				} else return x[event];
			} else return x[event];
		} else return x[event];
	} else return x[event];
}

/* vim: set noet: */
