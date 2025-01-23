#pragma once

#include "common/Exception.h"

#include <ghc/filesystem.hpp>
#include <string>
#include <vector>

class Reader;

class catalogueException : public Exception {
  public:
	catalogueException(const std::string& s) throw()
		: Exception(s) {}
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
CatalogueFile readCatalogueFile(Reader& in);