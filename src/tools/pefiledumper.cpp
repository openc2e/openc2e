#include "fileformats/peFile.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

std::string repr(const std::string& s) {
	std::string result = "\"";
	for (char c : s) {
		if (c == '"') {
			result += "\\\"";
		} else if (c == '\n') {
			result += "\\n";
		} else if (c == '\r') {
			result += "\\r";
		} else {
			result += c;
		}
	}
	result += "\"";
	return result;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "syntax: {} filename\n", argv[0]);
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		fmt::print(stderr, "File {} doesn't exist\n", input_path.string());
		exit(1);
	}

	peFile pe(input_path);
	for (auto& resource : pe.resources) {
		fmt::print("resource: type={} lang={} name={}\n",
			peFile::resource_type_to_string(resource.type),
			peFile::language_to_string(resource.lang, resource.sublang), resource.name);
		if (resource.type == PE_RESOURCETYPE_STRING) {
			for (auto& s : pe.getResourceStrings(resource)) {
				fmt::print("{}\n", repr(s));
			}
		}
	}

	return 0;
}