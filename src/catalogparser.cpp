#include "catalogparser.h"

#include "Catalogue.h"
#include "caos_assert.h"

CatalogueParserToken catalval;

int cataparse() {
	// Parses the following grammar:
	// -> cgroup*
	// cgroup -> ctag | carray
	// ctag -> CTOK_TAG CTOK_OVERRIDE? CTOK_STR CTOK_STR+
	// carray -> CTOK_ARRAY CTOK_OVERRIDE? CTOK_STR CTOK_INT CTOK_STR+

	int token = Catalogue::catalex();

	while (true) {
		const int tag_or_array_token = token;
		if (tag_or_array_token == CTOK_EOI) {
			return 0;
		}
		if (!(tag_or_array_token == CTOK_TAG || tag_or_array_token == CTOK_ARRAY)) {
			std::cout << "Expected TAG or ARRAY, got " << tag_or_array_token << std::endl;
			Catalogue::catalogueParseError("Expected TAG or ARRAY");
		}
		token = Catalogue::catalex();

		bool override = false;
		if (token == CTOK_OVERRIDE) {
			override = true;
			token = Catalogue::catalex();
		}

		if (token != CTOK_STR) {
			Catalogue::catalogueParseError("Expected string");
		}
		std::string first_string = catalval.string;
		token = Catalogue::catalex();

		int count = -1;
		if (tag_or_array_token == CTOK_ARRAY) {
			if (token != CTOK_INT) {
				Catalogue::catalogueParseError("Expected int");
			}
			count = catalval.number;
			token = Catalogue::catalex();
		}

		std::list<std::string> strings;
		while (true) {
			if (token != CTOK_STR) {
				break;
			}
			strings.push_back(catalval.string);
			token = Catalogue::catalex();
		}
		if (count != -1) {
			caos_assert((size_t)count == strings.size());
		}
		parsing_cat->addVals(first_string, override, strings);
	}
}
