/*
 *  lexutil.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 11 Aug 2005.
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

#include "lexer.h"
#include "token.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <map>
#include <vector>
#include <string>

namespace x = boost::xpressive;
using x::smatch;
using boost::lambda::bind;
using boost::lambda::_1;

token decodeToken(const x::smatch &mdata) {
    if (mdata[1] != "") {
        std::string s1 = mdata[1];
        if (s1[0] == '\'') {
            return token(caosVar((int)s1[0]));
        } else if (s1[0] == '%') {
            int accum = 0;
            for (int i = 1; i < s1.size(); i++) {
                accum = accum << 1;
                if (s1[i] == '1')
                    accum |= 1;
            }
			return token(caosVar(accum));
        } else if (s1[0] == '[') {
            bytestring_t bs;
            smatch::nested_results_type nr = mdata.nested_results();
            for (smatch::nested_results_type::const_iterator i = nr.begin(); i != nr.end(); i++)
                bs.push_back(atoi((*i)[1].str().c_str()));
            return token(bs);
        } else if (s1[0] == '"') {
            std::ostringstream ss;
            smatch::nested_results_type nr = mdata.nested_results();
            for (smatch::nested_results_type::const_iterator i = nr.begin(); i != nr.end(); i++) {
                std::string chunk = (*i)[1];
                if (chunk.size() == 1) {
                    ss << chunk;
                } else {
                    switch(chunk[1]) {
                        case 'n': ss << '\n'; break;
                        case 'r': ss << '\r'; break;
                        case 't': ss << '\t'; break;
                        default:  ss << chunk[1]; break;
                    }
                }
            }
            return token(caosVar(ss.str()));
        } else {
            std::cerr << "Impossible: s1=" << s1 << std::endl;
            abort();
        }
    } else if (mdata[2] != "") {
        std::string s2 = mdata[2];
        if (s2[0] == '[') {
            return token(caosVar(mdata[3]));
        } else if (condmap[s2] != "") {
            return token(condmap[s2]);
        } else {
            std::string word = s2;
            std::transform(word.begin(), word.end(), word.begin(), (int(*)(int))tolower);
            return token(word);
        }
    } else if (mdata[3] != "") {
        return token(caosVar((float)atof(mdata[3].str().c_str())));
    } else if (mdata[4] != "") {
        return token(caosVar(atoi(mdata[4].str().c_str())));
	} else if (mdata[5] != "") {
		return token(token::token_nl());
    } else {
		// comment or whitespace
		return token();
    }
    assert(!"IMPOSSIBLE");
}

/* vim: set noet: */
