#pragma once

#include <fstream>
#include <ghc/filesystem.hpp>
#include <vector>

enum PathType {
	PATH_TYPE_MAIN,
	PATH_TYPE_IMAGE,
	PATH_TYPE_SOUND,
};

class PathManager {
  public:
	PathManager();
	void set_main_directory(ghc::filesystem::path main_dir);
	ghc::filesystem::path find_path(PathType, const std::string&);
	std::vector<ghc::filesystem::path> find_path_wildcard(PathType, const std::string&);
	std::ofstream ofstream(PathType, const std::string&);

  private:
	ghc::filesystem::path m_main_dir;
};