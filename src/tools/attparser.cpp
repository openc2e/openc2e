#include "common/io/FileReader.h"
#include "common/readfile.h"
#include "fileformats/attFile.h"

#include <fmt/core.h>

int main(int argc, char** argv) {
	if (argc == 1) {
		fmt::print(stderr, "USAGE: {} FILES...\n", argv[0]);
		return 1;
	}

	for (int i = 1; i < argc; ++i) {
		std::string filename = argv[i];
		fmt::print("{}\n", filename);

		FileReader in(filename);

		auto att = ReadAttFile(in);
		for (unsigned int i = 0; i < att.nolines; ++i) {
			for (unsigned int j = 0; j < att.noattachments[i]; ++j) {
				printf("%d ", att.attachments[i][j]);
			}
			printf("\n");
		}

		fmt::print("extra data '{}'\n", std::string((char*)att.extra_data.data(), (char*)&att.extra_data[att.extra_data.size()]));
	}

	return 0;
}