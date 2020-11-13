/*
 *  PathResolver.cpp
 *  openc2e
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
#include "utils/ascii_tolower.h"

#include <ghc/filesystem.hpp>
#include <regex>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace fs = ghc::filesystem;

struct cacheinfo {
	std::string realfilename;
	fs::file_time_type mtime = fs::file_time_type::min();
};
static std::unordered_map<std::string, cacheinfo> s_cache;

static void updateDirectory(std::string dirname) {
	// if the directory is already cached and matches what's on disk, then do nothing
	fs::path lcdirname = ascii_tolower(dirname);
	std::error_code mtime_error;
	if (s_cache.count(lcdirname)
	    // avoid a check for directory existence by using the non-throwing overload
	    // of fs::last_write_time, which returns file_time_type::min() on errors
	    && fs::last_write_time(s_cache[lcdirname].realfilename, mtime_error) == s_cache[lcdirname].mtime)
	{
		return;
	}

	// remove existing cache entries
	fs::path withtrailingslash = lcdirname / "";
	for (auto it = s_cache.begin(); it != s_cache.end();) {
		// str.rfind(s, 0) == 0 is equivalent to str.startswith(s)
		if (it->first.rfind(withtrailingslash, 0) == 0) {
			it = s_cache.erase(it);
		} else {
			it++;
		}
	}

	// optimistically try the passed-in directory name, otherwise use resolveFile
	std::error_code directory_iterator_error;
	auto iter = fs::directory_iterator(dirname, directory_iterator_error);
	if (directory_iterator_error) {
		dirname = resolveFile(dirname);
		if (dirname == "") {
			s_cache[lcdirname] = { dirname, fs::file_time_type::min() }; // I guess?
			return;
		}
		iter = fs::directory_iterator(dirname);
	}

	// cache directory contents
	s_cache[lcdirname] = { dirname, fs::last_write_time(dirname) };
	for (const auto& entry : fs::directory_iterator(dirname)) {
		s_cache[ascii_tolower(entry.path())] = { entry.path() };
	}
}

std::string resolveFile(fs::path path) {
	if (path.empty()) {
		return "";
	}
	if (path.filename().empty()) {
		path = path.parent_path();
	}
	fs::path lcpath = ascii_tolower(path);

	// if file exists in cache and on filesystem, return it
	if (s_cache.count(lcpath) && fs::exists(s_cache[lcpath].realfilename)) {
		return s_cache[lcpath].realfilename;
	}
	updateDirectory(path.parent_path());

	if (s_cache.count(lcpath)) {
		return s_cache[lcpath].realfilename;
	}
	return "";
}

std::vector<std::string> findByWildcard(std::string dirname, std::string wild) {
	// make sure dir is normal and ends with a trailing slash
	dirname = fs::path(dirname).lexically_normal() / "";
	auto lcdirname = ascii_tolower(dirname);
	updateDirectory(dirname);

	std::string search_pattern = "^";
	for (auto c : wild) {
		if (c == '*') {
			search_pattern += ".*";
		} else if (c == '?') {
			search_pattern += ".";
		} else {
			search_pattern += std::string("[") + c + "]";
		}
	}
	search_pattern += "$";
	std::regex re(search_pattern);

	std::vector<std::string> results;
	for (const auto& it : s_cache) {
		// str.rfind(s, 0) != 0 is equivalent to !str.startswith(s)
		if (it.first.rfind(lcdirname, 0) != 0) {
			continue;
		}
		if (std::regex_match(it.first.substr(lcdirname.size()), re)) {
			results.push_back(it.second.realfilename);
		}
	}

	return results;
}
