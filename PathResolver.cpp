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
#include <set>
#include <map>
#include <cctype>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace boost::filesystem;

static set<string> dircache;
static map<string, string> cache;

static bool checkDirCache(path &dir);
static bool doCacheDir(path &dir);

/* C++ is far too verbose for its own good */
string toLowerCase(string in) {
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
    if (!resolveFile(s))
        return false;
    p = path(s);
    return true;
}

bool resolveFile(string &srcPath) {
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
    
    map<string, string>::iterator i = cache.find(fn);
    if (i == cache.end())
        return false;
    srcPath = cache[fn];
    return true;
}

/* If dir is cached, do nothing.
 * If dir exists, cache it.
 * If dir does not exist, see if there's one with different capitalization.
 *
 * If we find a dir, return true. Else, false.
 */
bool checkDirCache(path &dir) {
    if (dircache.end() != dircache.find(dir.string()))
        return true;
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
    directory_iterator it(dir);
    directory_iterator fsend;
    while (it != fsend) {
        path cur = *it++;
        string key, val;
        key = cur.string();
        val = lcleaf(cur).string();
        cache[val] = key;
    }
    dircache.insert(dir.string());
    return true;
}
