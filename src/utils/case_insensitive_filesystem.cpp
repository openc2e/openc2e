#include "case_insensitive_filesystem.h"

#include "utils/ascii_tolower.h"

#include <assert.h>
#include <ghc/filesystem.hpp>
#include <regex>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace case_insensitive_filesystem {

namespace fs = ghc::filesystem;

static std::unordered_map<std::string, cacheinfo> s_cache;

static void updateDirectory(fs::path dirname) {
	assert(!dirname.empty());

	// if the directory is already cached and matches what's on disk, then do nothing
	fs::path lcdirname = ascii_tolower(dirname);
	std::error_code mtime_error;
	if (s_cache.count(lcdirname)
		// avoid a check for directory existence by using the non-throwing overload
		// of fs::last_write_time, which returns file_time_type::min() on errors
		&& fs::last_write_time(s_cache[lcdirname].realfilename, mtime_error) == s_cache[lcdirname].mtime) {
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
		dirname = resolve_filename(dirname);
		if (dirname == "") {
			s_cache[lcdirname] = {dirname, fs::file_time_type::min()}; // I guess?
			return;
		}
		iter = fs::directory_iterator(dirname);
	}

	// cache directory contents
	s_cache[lcdirname] = {dirname, fs::last_write_time(dirname)};
	for (const auto& entry : fs::directory_iterator(dirname)) {
		s_cache[ascii_tolower(entry.path())] = {entry.path()};
	}
}

fs::path resolve_filename(fs::path path) {
	if (path.empty()) {
		return "";
	}
	if (!path.is_absolute()) {
		path = fs::absolute(path);
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

void add_entry(fs::path path) {
	fs::path lcpath = ascii_tolower(path);
	if (s_cache.count(lcpath)) {
		return;
	}
	s_cache[lcpath] = {path};
}

directory_iterator::directory_iterator(fs::path dirname) {
	// make sure dir is normal and ends with a trailing slash
	dirname = dirname.lexically_normal() / "";
	updateDirectory(dirname);
	lcdirname = ascii_tolower(dirname);

	it = s_cache.begin();
	while (it != s_cache.end()) {
		// str.rfind(s, 0) == 0 is equivalent to str.startswith(s)
		if (it->first.rfind(lcdirname, 0) == 0 && it->first != lcdirname) {
			break;
		}
		it++;
	}
}

directory_iterator::directory_iterator() {
	it = s_cache.end();
}

const fs::path& directory_iterator::operator*() const {
	return it->second.realfilename;
}

directory_iterator& directory_iterator::operator++() {
	it++;
	while (it != s_cache.end()) {
		// str.rfind(s, 0) == 0 is equivalent to str.startswith(s)
		if (it->first.rfind(lcdirname, 0) == 0 && it->first != lcdirname) {
			break;
		}
		it++;
	}
	return *this;
}

bool directory_iterator::operator==(const directory_iterator& other) const {
	if (it == s_cache.end() && other.it == s_cache.end()) {
		return true;
	}
	return false;
}
bool directory_iterator::operator!=(const directory_iterator& other) const {
	return !(*this == other);
}

directory_iterator directory_iterator::begin() {
	return *this;
}

directory_iterator directory_iterator::end() const {
	return directory_iterator();
}

} // namespace case_insensitive_filesystem
