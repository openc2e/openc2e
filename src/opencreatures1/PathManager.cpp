#include "PathManager.h"

#include "common/case_insensitive_filesystem.h"
#include "common/wildcard_match.h"

namespace fs = ghc::filesystem;

PathManager::PathManager(fs::path base_dir)
	: m_base_dir(base_dir) {
}

static fs::path append_path_type(fs::path base_dir, PathType type) {
	switch (type) {
		case PATH_TYPE_BASE:
			return base_dir;
		case PATH_TYPE_IMAGE:
			return base_dir / "Images";
		case PATH_TYPE_SOUND:
			return base_dir / "Sounds";
	}
}

fs::path PathManager::find_path(PathType type, const std::string& name) {
	auto dirname = append_path_type(m_base_dir, type);
	return case_insensitive_filesystem::resolve_filename(dirname / name);
}

std::vector<fs::path> PathManager::find_path_wildcard(PathType type, const std::string& pattern) {
	auto dirname = append_path_type(m_base_dir, type);

	std::vector<fs::path> results;
	for (const auto& entry : case_insensitive_filesystem::directory_iterator(dirname)) {
		if (wildcard_match(pattern, entry.lexically_relative(dirname).string())) {
			results.push_back(entry);
		}
	}

	return results;
}