#pragma once

#include <filesystem>
#include <vector>

class FileWriter;

enum PathType {
	PATH_TYPE_MAIN,
	PATH_TYPE_IMAGE,
	PATH_TYPE_SOUND,
};

class PathManager {
  public:
	PathManager();
	void set_main_directory(std::filesystem::path main_dir);
	std::filesystem::path find_path(PathType, const std::string&);
	std::vector<std::filesystem::path> find_path_wildcard(PathType, const std::string&);
	FileWriter create_file(PathType, const std::string&);

  private:
	std::filesystem::path m_main_dir;
};