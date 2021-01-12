#include "parseException.h"

#include "fileformats/caostoken.h"

#include <fmt/core.h>

std::string parseException::prettyPrint() const {
	std::string filename = this->filename;
	if (filename == "")
		filename = std::string("(UNKNOWN)");

	std::string buf = fmt::format(
		"Parse error at line {} in file {}: {}",
		lineno == -1 ? "(UNKNOWN)" : std::to_string(lineno),
		filename,
		what());
	if (!context)
		buf += "\n";
	else {
		buf += " near:\n";
		int toklen = -1, stlen = 0;
		for (size_t i = 0; i < context->size(); i++) {
			std::string tokstr = (*context)[i].format();
			if (i == (size_t)ctxoffset) {
				toklen = tokstr.size();
			} else if (toklen == -1) {
				stlen += tokstr.size() + 1;
			}
			buf += tokstr;
			buf += " ";
		}
		buf += "\n";
		if (toklen != -1) {
			for (int i = 0; i < stlen; i++)
				buf += " ";
			for (int i = 0; i < toklen; i++)
				buf += "^";
			buf += "\n";
		}
	}
	return buf;
}
