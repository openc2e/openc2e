/*
 *  PathResolver.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan
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

#include "PathResolver.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <set>
#include <map>
#include <cctype>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>

using std::map;
using std::set;
using std::string;
using namespace boost::filesystem;
namespace x = boost::xpressive;

static set<string> dircache;
static map<string, string> cache;

static bool checkDirCache(path &dir);
static bool doCacheDir(path &dir);

/* C++ is far too verbose for its own good */
static string toLowerCase(string in) {
	transform(in.begin(), in.end(), in.begin(), (int(*)(int))tolower);
	return in;
}

static path lcpath(path &orig) {
	return path(toLowerCase(orig.string()), native);
}

static path lcleaf(path &orig) {
	path br, leaf;
	br = orig.branch_path();
	leaf = path(toLowerCase(orig.leaf()), native);
	return br / leaf;
}

bool resolveFile(path &p) {
	string s = p.string();
	if (!resolveFile(s)) {
		// Maybe something changed underneath us; reset the cache and try again
		cache.clear();
		dircache.clear();
		if (!resolveFile(s))
			return false;
	}
	p = path(s, native);
	return true;
}

bool resolveFile_(string &srcPath) {
	path orig(srcPath, native);
	if (exists(orig))
		return true;
	
	orig.normalize();
	path dir = orig.branch_path();
	path leaf = path(orig.leaf(), native);

	if (!checkDirCache(dir))
		return false;

	orig = dir / lcpath(leaf);
	string fn = orig.string();

	if (exists(orig)) {
		srcPath = fn;
		return true;
	}

	map<string, string>::iterator i = cache.find(fn);
	if (i == cache.end()) {
		assert(!exists(orig));
		return false;
	}
	srcPath = cache[fn];
	return true;
}

bool resolveFile(std::string &path) {
	std::string orig = path;
	bool res = resolveFile_(path);
#if 0
	std::cerr << orig << " -> ";
	if (!res)
		std::cerr << "(nil)";
	else
		std::cerr << path;
	std::cerr << std::endl;
#endif
	return res;
}


/* If dir is cached, do nothing.
 * If dir exists, cache it.
 * If dir does not exist, see if there's one with different capitalization.
 *
 * If we find a dir, return true. Else, false.
 */
bool checkDirCache(path &dir) {
	if (dir == path())
		dir = path(".");
//	std::cerr << "checkDirCache: " << dir.string() << std::endl;
	if (dircache.end() != dircache.find(dir.string())) {
		return true;
	}
	if (exists(dir))
		return doCacheDir(dir);
	if (dir.empty())
		return false;
	bool res = resolveFile(dir);
	if (!res)
		return false;
	return checkDirCache(dir);
}

/* Cache a dir. Return true for success.
 */
bool doCacheDir(path &dir) {
//	std::cerr << "cacheing: " << dir.string() << std::endl;
	directory_iterator it(dir);
	directory_iterator fsend;
	while (it != fsend) {
		path cur = *it++;
		string key, val;
		key = cur.string();
		val = lcleaf(cur).string();
//		std::cerr << "Cache put: " << val << " -> " << key << std::endl;
		cache[val] = key;
	}
	dircache.insert(dir.string());
	return true;
}

static x::sregex constructSearchPattern(const std::string &wild) {
	x::sregex scanner;
	scanner = *(x::alnum | x::blank) | (x::s1= x::_);
	std::ostringstream matchstr;
	x::sregex_iterator cur(wild.begin(), wild.end(), scanner);
	x::sregex_iterator end;
	matchstr << "^";
	for (; cur != end; cur++) {
		x::smatch const &what_m = *cur;
		const std::string what = what_m[0];
		if (what == "*")
			matchstr << ".*";
		else if (what == "?")
			matchstr << ".";
		else if (what_m[1] != "")
			matchstr << "[" << (std::string)what_m[1] << "]";
		else
			matchstr << (std::string)what_m[0];
	}
	matchstr << "$";
	return x::sregex::compile(matchstr.str());
}

static bool checkSearchPattern(const std::string &match, const x::sregex &l) {
	return x::regex_match(match, l);
}

std::vector<std::string> findByWildcard(std::string dir, std::string wild) {
	cache.clear();
	dircache.clear();

	wild = toLowerCase(wild);

	path dirp(dir, native);
	dirp.normalize();
	if (!resolveFile(dirp))
		return std::vector<std::string>();
	dir = dirp.string();

	if (!doCacheDir(dirp))
		return std::vector<std::string>();
	std::vector<std::string> results;
	x::sregex l = constructSearchPattern(wild);

	std::string lcdir = toLowerCase(dir);
	std::map<string, string>::iterator skey = cache.lower_bound(dir);
	for (; skey != cache.end(); skey++) {
		if (skey->first.length() < lcdir.length())
			break;
		std::string dirpart, filepart; // XXX: we should use boost fops, maybe
		dirpart = skey->first.substr(0, lcdir.length());
		if (dirpart != dir)
			break;
		if (skey->first.length() < lcdir.length() + 2)
			continue;
		filepart = toLowerCase(skey->first.substr(lcdir.length() + 1));
		if (!x::regex_match(filepart, l))
			continue;
		results.push_back(skey->second);
	}
	return results;
}

/* vim: set noet: */
