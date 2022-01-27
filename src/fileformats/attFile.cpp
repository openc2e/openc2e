#include "attFile.h"

#include "common/readfile.h"

#include <fmt/core.h>
#include <iostream>

std::string _debugrepr_at(std::vector<uint8_t>& bytes, size_t p) {
	if (p >= bytes.size()) {
		return "end of input";
	}
	auto b = bytes[p];
	if (b == '\r') {
		return "'\\r'";
	} else if (b == '\n') {
		return "'\\n'";
	} else if (b == '\t') {
		return "'\\t'";
	} else if (isprint(b)) {
		return fmt::format("'{:c}'", b);
	} else {
		return fmt::format("'\\x{:02x}'", b);
	}
}

bool _is_space_at(const std::vector<uint8_t>& bytes, size_t p) {
	if (p >= bytes.size()) {
		return false;
	}
	auto b = bytes[p];
	return b == ' ' || b == '\t';
}
bool _is_space_or_newline_at(std::vector<uint8_t>& bytes, size_t p) {
	if (p >= bytes.size()) {
		return false;
	}
	auto b = bytes[p];
	return b == ' ' || b == '\t' || b == '\r' || b == '\n';
}
bool _isdigit_at(const std::vector<uint8_t>& bytes, size_t p) {
	if (p >= bytes.size()) {
		return false;
	}
	auto b = bytes[p];
	return b == '0' || b == '1' || b == '2' || b == '3' || b == '4' || b == '5' || b == '6' || b == '7' || b == '8' || b == '9';
}

int _atoi_at(const std::vector<uint8_t>& bytes, size_t* p) {
	int value = 0;
	while (_isdigit_at(bytes, *p)) {
		value = value * 10 + (bytes[*p] - '0');
		(*p)++;
	}
	return value;
}
bool _is_eoi_at(const std::vector<uint8_t>& bytes, size_t p) {
	return p >= bytes.size();
}
bool _skip_newline_at(const std::vector<uint8_t>& bytes, size_t p) {
	if (p < bytes.size() && bytes[p] == '\n') {
		p++;
		return true;
	}
	if (p < bytes.size() && bytes[p] == '\r') {
		p++;
		if (p + 1 < bytes.size() && bytes[p + 1] == '\n') {
			p++;
		}
		return true;
	}
	return false;
}

attFile ReadAttFile(std::istream& in) {
	std::vector<uint8_t> bytes = readfilebinary(in);

	attFile att;
	size_t p = 0;

	// parse lines
	while (att.nolines < att.attachments.size()) {
		while (_is_space_or_newline_at(bytes, p)) {
			p++;
		}
		if (!_isdigit_at(bytes, p)) {
			break;
		}

		// parse integers
		while (true) {
			while (_is_space_at(bytes, p)) {
				p++;
			}
			if (_is_eoi_at(bytes, p)) {
				break;
			}
			if (_skip_newline_at(bytes, p)) {
				break;
			}
			if (!_isdigit_at(bytes, p)) {
				throw std::runtime_error(fmt::format("Expected digit, found {}\n", _debugrepr_at(bytes, p)));
			}
			if (att.noattachments[att.nolines] >= att.attachments[0].size()) {
				throw std::runtime_error(fmt::format("Too many integers on one line, found {}", att.noattachments[att.nolines]));
			}

			int x = _atoi_at(bytes, &p);
			att.attachments[att.nolines][att.noattachments[att.nolines]] = x;
			att.noattachments[att.nolines]++;
		}

		// increment the line we're on, unless it was blank
		if (att.noattachments[att.nolines] > 0) {
			if (att.noattachments[att.nolines] % 2 != 0) {
				throw std::runtime_error(fmt::format("Expected even number of integers, but got {}", att.noattachments[att.nolines]));
			}
			if (att.nolines > 0 && att.noattachments[att.nolines] != att.noattachments[0]) {
				throw std::runtime_error(fmt::format("Expected same number of integers as first line {}, but got {}", att.noattachments[0], att.noattachments[att.nolines]));
			}

			att.nolines++;
		}
	}

	// parse the weird comments at the end.
	// only known to exist in Creatures Village files i04a, i05a, i20a, i50a
	if (p < bytes.size()) {
		while (_is_space_or_newline_at(bytes, p)) {
			p++;
		}
		size_t start = p;
		size_t end = p;
		while (!_is_eoi_at(bytes, p)) {
			if (_is_space_or_newline_at(bytes, p)) {
				p++;
			} else {
				p++;
				end = p;
			}
		}
		// TODO: should this be CP1252-encoded text?
		att.extra_data.insert(att.extra_data.end(), &bytes[start], &bytes[end]);
	}

	return att;
}
