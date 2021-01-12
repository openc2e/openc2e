#pragma once

#include "Catalogue.h"

#include <string>

enum CatalogueParserTokenType {
	CTOK_EOI = 0,
	CTOK_TAG,
	CTOK_ARRAY,
	CTOK_OVERRIDE,
	CTOK_STR,
	CTOK_INT,
};

struct CatalogueParserToken {
	std::string string;
	int number;
};

extern Catalogue* parsing_cat;
extern CatalogueParserToken catalval;
