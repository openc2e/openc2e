#include "case_insensitive_filesystem.h"

#include "Ascii.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace case_insensitive_filesystem {

namespace fs = ghc::filesystem;

struct path_hash {
	std::size_t operator()(const fs::path& path) const {
		return hash_value(path);
	}
};

static std::unordered_map<fs::path, cacheinfo, path_hash> s_cache;

static bool path_startswith(const fs::path& path, fs::path prefix) {
	auto end = prefix.end();
	if (!std::prev(end)->has_filename()) {
		end--;
	}
	return std::equal(prefix.begin(), end, path.begin());
}

static void update_directory(fs::path dirname) {
	assert(!dirname.empty());

	// if the directory is already cached and matches what's on disk, then do nothing
	fs::path lcdirname = to_ascii_lowercase(dirname);
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
		if (path_startswith(it->first, lcdirname)) {
			it = s_cache.erase(it);
		} else {
			it++;
		}
	}

	// optimistically try the passed-in directory name, otherwise use resolveFile
	std::error_code directory_iterator_error;
	auto iter = fs::directory_iterator(dirname, directory_iterator_error);
	if (directory_iterator_error) {
		std::error_code err;
		dirname = case_insensitive_filesystem::canonical(dirname, err);
		if (err) {
			s_cache[lcdirname] = {dirname, fs::file_time_type::min()}; // I guess?
			return;
		}
		iter = fs::directory_iterator(dirname);
	}

	// cache directory contents
	s_cache[lcdirname] = {dirname, fs::last_write_time(dirname)};
	for (const auto& entry : iter) {
		s_cache[to_ascii_lowercase(entry.path())] = {entry.path()};
	}
}

fs::path canonical(const fs::path& path, std::error_code& ec) {
	if (path.empty()) {
		ec = std::make_error_code(std::errc::invalid_argument);
		return {};
	}
	// make the path absolute, normalized, and no trailing slash.
	fs::path abspath = fs::absolute(path).lexically_normal();
	if (abspath.filename().empty()) {
		abspath = abspath.parent_path();
	}
	// for cache comparisons we need the whole path lowercased.
	fs::path lcpath = to_ascii_lowercase(abspath);

	// if file exists in cache and on filesystem, return it
	if (s_cache.count(lcpath) && fs::exists(s_cache[lcpath].realfilename)) {
		return s_cache[lcpath].realfilename;
	}

	// otherwise, update directory cache and then check if we found it
	update_directory(abspath.parent_path());
	if (s_cache.count(lcpath)) {
		return s_cache[lcpath].realfilename;
	}
	ec = std::make_error_code(std::errc::no_such_file_or_directory);
	return {};
}

bool exists(const fs::path& path) {
	std::error_code err;
	auto canonpath = case_insensitive_filesystem::canonical(path, err);
	return !err;
}

std::ofstream ofstream(const fs::path& path) {
	std::error_code err;
	auto canonpath = case_insensitive_filesystem::canonical(path, err);
	if (err) {
		return std::ofstream(path, std::ios_base::binary);
	}
	return std::ofstream(canonpath, std::ios_base::binary);
}

directory_iterator::directory_iterator(const fs::path& dirname) {
	// make the path absolute, normalized, and ends with a trailing slash.
	fs::path absdirname = fs::absolute(dirname / "").lexically_normal();
	// for cache comparisons we need the whole path lowercased.
	lcdirname = to_ascii_lowercase(absdirname);

	// TODO: don't update cached directory contents if they're already fresh enough
	update_directory(absdirname);

	// iterate results from the cache
	it = s_cache.begin();
	while (it != s_cache.end()) {
		if (path_startswith(it->first, lcdirname) && it->first != lcdirname) {
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
		if (path_startswith(it->first, lcdirname) && it->first != lcdirname) {
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
