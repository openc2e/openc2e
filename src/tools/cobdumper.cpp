#include "cobfile/c1cobfile.h"
#include "cobfile/cobFile.h"

#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <sstream>

namespace fs = ghc::filesystem;

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "syntax: cobdumper filename" << std::endl;
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		std::cerr << "File " << input_path << " doesn't exist" << std::endl;
		exit(1);
	}
    
    fs::path stem = input_path.stem();
    // fs::path output_directory = stem;

    std::ifstream in(input_path, std::ios::binary);

	unsigned char magic[4];
	in.read((char*)magic, 4);
	in.seekg(0);

	if (memcmp(magic, "cob2", 4) == 0) {
		printf("cob2 file!\n");
		cobFile cob(input_path);
		for (auto &b : cob.blocks) {
			if (b->getType() == "agnt") {
				cobAgentBlock agnt(b);

				fmt::print("name = {}\n", agnt.name);
				fmt::print("description = {}\n", agnt.description);
				fmt::print("quantityremaining = {}\n", agnt.quantityremaining);
				fmt::print("lastusage = {}\n", agnt.lastusage);
				fmt::print("reuseinterval = {}\n", agnt.reuseinterval);
				fmt::print("usebyday = {}\n", agnt.usebyday);
				fmt::print("usebymonth = {}\n", agnt.usebymonth);
				fmt::print("usebyyear = {}\n", agnt.usebyyear);
				fmt::print("installscript = {}\n", agnt.installscript);
				fmt::print("removescript = {}\n", agnt.removescript);
				for (auto s : agnt.scripts) {
					fmt::print("script[] = {}\n", s);
				}
				for (auto d : agnt.deptypes) {
					fmt::print("deptype[] = {}\n", d);
				}
				for (auto d : agnt.depnames) {
					fmt::print("dep[] = {}\n", d);
				}
				fmt::print("thumbnailwidth = {}\n", agnt.thumbnailwidth);
				fmt::print("thumbnailheight = {}\n", agnt.thumbnailheight);
				fmt::print("\n");
			} else if (b->getType() == "file") {
				cobFileBlock file(b);
				fmt::print("filetype = {}\n", file.filetype);
				fmt::print("filesize = {}\n", file.filesize);
				fmt::print("filename = {}\n", file.filename);
				fmt::print("\n");
			} else if (b->getType() == "auth") {
				cobAuthBlock auth(b);
				fmt::print("daycreated = {}\n", auth.daycreated);
				fmt::print("monthcreated = {}\n", auth.monthcreated);
				fmt::print("yearcreated = {}\n", auth.yearcreated);
				fmt::print("version = {}\n", auth.version);
				fmt::print("revision = {}\n", auth.revision);
				fmt::print("authorname = {}\n", auth.authorname);
				fmt::print("authoremail = {}\n", auth.authoremail);
				fmt::print("authorurl = {}\n", auth.authorurl);
				fmt::print("authorcomments = {}\n", auth.authorcomments);
				fmt::print("\n");
			}else {
				fmt::print("unknown block type '{}'\n", b->getType());
			}
		}
	} else if (memcmp(magic, "\x01\x00", 2) == 0) {
		c1cobfile cob = read_c1cobfile(in);
		fmt::print("*# COB-Name \"{}\"\n", cob.name);
		fmt::print("*# Picture \"{}\"\n", stem.string() + ".spr");
		fmt::print("*# Quantity available = {}\n", cob.quantity_available);
		fmt::print("*# Quantity used = {}\n", cob.quantity_used);
		fmt::print("*# Expiration month = {}\n", cob.expiration_month);
		fmt::print("*# Expiration day = {}\n", cob.expiration_day);
		fmt::print("*# Expiration year = {}\n", cob.expiration_year);
		fmt::print("\n");
		for (size_t i = 0; i < cob.install_scripts.size(); ++i) {
			std::string s = cob.install_scripts[i];
			std::replace(s.begin(), s.end(), ',', '\n');
			fmt::print("iscr\n{}\n\n", s);
		}
		for (size_t i = 0; i < cob.object_scripts.size(); ++i) {
			std::string s = cob.object_scripts[i];
			std::replace(s.begin(), s.end(), ',', '\n');
			// TODO: make sure it ends with endm
			fmt::print("{}\n\n", s);
		}
	} else {
		fmt::print(stderr, "Not a recognized COB format: bad magic 0x{:02x}{:02x}{:02x}{:02x}\n", magic[0], magic[1], magic[2], magic[3]);
		exit(1);
	}
}