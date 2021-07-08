/*
 *  attFile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri 25 Feb 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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
#include "fileformats/attFile.h"

#include <cassert>
#include <cctype>
#include <string>

std::istream& operator>>(std::istream& i, attFile& f) {
	// TODO: replace this whole thing with a proper parser
	f.nolines = 0;

	std::string s;
	while (std::getline(i, s)) {
		if (s.size() == 0)
			return i;
		if (f.nolines >= 16)
			return i; // TODO: what the heck? wah
		assert(f.nolines < 16);

		f.noattachments[f.nolines] = 0;

		bool havefirst = false;
		unsigned int x = 0;
		size_t p = 0;
		while (true) {
			const char* start = s.c_str() + p;
			while (p < s.size() && std::isdigit(s[p])) {
				p++;
			}
			unsigned int val = atoi(start);
			if (havefirst) {
				f.attachments[f.nolines][f.noattachments[f.nolines] * 2] = x;
				f.attachments[f.nolines][(f.noattachments[f.nolines] * 2) + 1] = val;
				havefirst = false;
				f.noattachments[f.nolines]++;
			} else {
				havefirst = true;
				x = val;
			}
			if (p == s.size()) {
				break;
			}
			assert(s[p] == ' ' || s[p] == '\r');
			while (p < s.size() && s[p] == ' ') {
				p++;
			}
			if (p == s.size() || s[p] == '\r') {
				break;
			}
		}

		assert(!havefirst);
		f.nolines++;
	}

	return i;
}

/* vim: set noet: */
