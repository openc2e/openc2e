#pragma once

#include <ghc/filesystem.hpp>
#include <vector>

enum PathType {
	PATH_TYPE_BASE,
	PATH_TYPE_IMAGE,
	PATH_TYPE_SOUND,
};

class PathManager {
  public:
	PathManager(ghc::filesystem::path base_dir);
	ghc::filesystem::path find_path(PathType, const std::string&);
	std::vector<ghc::filesystem::path> find_path_wildcard(PathType, const std::string&);

  private:
	ghc::filesystem::path m_base_dir;
};