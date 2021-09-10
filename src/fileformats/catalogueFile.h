#pragma once

#include "creaturesException.h"

#include <ghc/filesystem.hpp>
#include <iosfwd>
#include <string>
#include <vector>

class catalogueException : public creaturesException {
  public:
	catalogueException(const std::string& s) throw()
		: creaturesException(s) {}
};

struct CatalogueTag {
	enum CatalogueTagType {
		TYPE_UNINITIALIZED = 0,
		TYPE_TAG,
		TYPE_ARRAY
	};

	CatalogueTagType type = TYPE_UNINITIALIZED;
	bool override = false;
	std::string name;
	std::vector<std::string> values;
};

struct CatalogueFile {
	std::vector<CatalogueTag> tags;
};


CatalogueFile readCatalogueFile(ghc::filesystem::path);
CatalogueFile readCatalogueFile(std::istream& in);