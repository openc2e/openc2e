#include "case_insensitive_filesystem.h"

#include "Ascii.h"

#include <algorithm>
#include <assert.h>
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
		s_cache[to_ascii_lowercase(entry.path())] = {entry.path()};
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
	fs::path lcpath = to_ascii_lowercase(path);

	// if file exists in cache and on filesystem, return it
	if (s_cache.count(lcpath) && fs::exists(s_cache[lcpath].realfilename)) {
		return s_cache[lcpath].realfilename;
	}
	update_directory(path.parent_path());

	if (s_cache.count(lcpath)) {
		return s_cache[lcpath].realfilename;
	}
	return "";
}

std::ofstream ofstream(const fs::path& path) {
	std::error_code err;
	auto canonpath = case_insensitive_filesystem::canonical(path, err);
	if (err) {
		std::ofstream out;
		out.setstate(std::ios_base::failbit);
		return out;
	}
	return std::ofstream(canonpath, std::ios_base::binary);
}

directory_iterator::directory_iterator(fs::path dirname) {
	// make sure dir is normal and ends with a trailing slash
	dirname = dirname.lexically_normal() / "";
	update_directory(dirname);
	lcdirname = to_ascii_lowercase(dirname);

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
