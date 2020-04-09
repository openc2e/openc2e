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
#include "attFile.h"
#include <cassert>
#include <regex>

std::istream &operator >> (std::istream &i, attFile &f) {
	f.nolines = 0;

	std::string s;
	while (std::getline(i, s)) {
		if (s.size() == 0) return i;
		if (f.nolines >= 16) return i; // TODO: what the heck? wah
		assert(f.nolines < 16);

		f.noattachments[f.nolines] = 0;
		
		bool havefirst = false;
		unsigned int x = 0;
		const std::regex ws_re("\\s+");
		for (
			auto beg = std::sregex_token_iterator(s.begin(), s.end(), ws_re, -1);
			beg != std::sregex_token_iterator();
			beg++
		) {
			unsigned int val = atoi(beg->str().c_str());
			if (havefirst) {
				f.attachments[f.nolines][f.noattachments[f.nolines] * 2] = x;
				f.attachments[f.nolines][(f.noattachments[f.nolines] * 2) + 1] = val;
				havefirst = false;
				f.noattachments[f.nolines]++;
			} else {
				havefirst = true;
				x = val; 
			}
		}

		assert(!havefirst);
		f.nolines++;
	}

	return i;
}

/* vim: set noet: */
