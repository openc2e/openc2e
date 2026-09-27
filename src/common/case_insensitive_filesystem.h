#pragma once

#include <filesystem>
#include <system_error>
#include <unordered_map>

class FileWriter;

namespace case_insensitive_filesystem {

namespace fs = std::filesystem;

namespace detail {
struct cacheinfo {
	fs::path realfilename;
	fs::file_time_type mtime = fs::file_time_type::min();
};

struct path_hash {
	std::size_t operator()(const fs::path& path) const {
		return hash_value(path);
	}
};

} // namespace detail

fs::path canonical(const fs::path&, std::error_code&);
bool exists(const fs::path&);
FileWriter create_file(const fs::path&);

class directory_iterator {
  public:
	directory_iterator(const fs::path& dirname);
	const fs::path& operator*() const;
	directory_iterator& operator++();
	bool operator==(const directory_iterator&) const;
	bool operator!=(const directory_iterator&) const;
	directory_iterator begin();
	directory_iterator end() const;

  private:
	directory_iterator();
	fs::path lcdirname;
	std::unordered_map<fs::path, detail::cacheinfo, detail::path_hash>::iterator it;
};

} // namespace case_insensitive_filesystem