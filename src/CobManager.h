#pragma once

#include "Image.h"

#include <string>
#include <vector>

class CobManager {
  public:
	struct CobFileInfo {
		CobFileInfo(std::string name_, std::string filename_)
			: name(name_), filename(filename_) {}
		std::string name;
		std::string filename;
		bool is_removable = false;
	};

	std::vector<CobFileInfo> objects;

	void update();
	Image getPicture(const CobFileInfo& info);
	void inject(const CobFileInfo& info);
	void remove(const CobFileInfo& info);
};