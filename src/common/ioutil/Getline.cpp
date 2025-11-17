#include "Getline.h"

#include "common/io/Reader.h"

std::string getline(Reader& r) {
	std::string s;
	while (r.has_data_left()) {
		uint8_t c;
		r.read(&c, 1);
		if (c == '\n') {
			if (!s.empty() && s.back() == '\r') {
				s.pop_back();
			}
			break;
		}
		s.push_back(static_cast<char>(c));
	}
	return s;
}