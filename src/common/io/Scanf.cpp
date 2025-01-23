#include "Scanf.h"

#include "common/io/Reader.h"

#include <istream>
#include <sstream>
#include <string>

// cheating here to get the same results as std::istream
// todo: replace with https://en.cppreference.com/w/cpp/locale/num_get/get ?

int scan_int(Reader& r) {
	std::string buf;
	int result = 0;
	while (true) {
		if (!r.has_data_left()) {
			return result;
		}

		buf.push_back(static_cast<char>(r.peek_byte()));
		std::stringstream o(buf);
		o >> result;

		if (std::istreambuf_iterator<char>(o) == std::istreambuf_iterator<char>()) {
			uint8_t c;
			r.read(&c, 1);
			(void)c;
		} else {
			return result;
		}
	}
}

float scan_float(Reader& r) {
	std::string buf;
	float result = 0;
	while (true) {
		if (!r.has_data_left()) {
			return result;
		}

		buf.push_back(static_cast<char>(r.peek_byte()));
		std::stringstream o(buf);
		o >> result;

		if (std::istreambuf_iterator<char>(o) == std::istreambuf_iterator<char>()) {
			uint8_t c;
			r.read(&c, 1);
			(void)c;
		} else {
			return result;
		}
	}
}