#include "cobfile/c1cobfile.h"
#include "cobfile/cobFile.h"

#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <sstream>

namespace fs = ghc::filesystem;

std::string escape(const std::string& s) {
	std::string result;
	for (char c : s) {
		if (c == '\n') {
			result += "\\n";
		} else if (c == '\r') {
			result += "\\r";
		} else if (c == '"') {
			result += "\\\"";
		} else {
			result += c;
		}
	}
	return result;
}

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
		fmt::print("\"cob2\"\n");
		fmt::print("\n");
		cobFile cob(input_path);
		for (auto &b : cob.blocks) {
			if (b->getType() == "agnt") {
				cobAgentBlock agnt(b);

				fmt::print("group AGNT \"{}\"\n", escape(agnt.name));
				fmt::print("\"Description\" \"{}\"\n", escape(agnt.description));
				fmt::print("\"Quantity Remaining\" {}\n", agnt.quantityremaining);
				fmt::print("\"Last Usage\" {}\n", agnt.lastusage);
				fmt::print("\"Reuse Interval\" {}\n", agnt.reuseinterval);
				fmt::print("\"Expiration Day\" {}\n", agnt.usebyday);
				fmt::print("\"Expiration Month\" {}\n", agnt.usebymonth);
				fmt::print("\"Expiration Year\" {}\n", agnt.usebyyear);
				fmt::print("\"Install script\" \"{}\"\n", agnt.installscript);
				fmt::print("\"Remove script\" \"{}\"\n", agnt.removescript);
				for (size_t i = 0; i < agnt.scripts.size(); ++i) {
					fmt::print("\"Script {}\" \"{}\"\n", i + 1, agnt.scripts[i]);
				}
				for (size_t i = 0; i < std::max(agnt.deptypes.size(), agnt.depnames.size()); ++i) {
					if (i < agnt.depnames.size()) {
						fmt::print("\"Dependency {}\" \"{}\"\n", i + 1, agnt.depnames[i]);
					}
					if (i < agnt.deptypes.size()) {
						fmt::print("# deptype {} = {}\n", i + 1, agnt.deptypes[i]);
					}
				}
				fmt::print("\"Thumbnail\" @ \"{}.s16\"\n", agnt.name);
				fmt::print("# thumbnailwidth = {}\n", agnt.thumbnailwidth);
				fmt::print("# thumbnailheight = {}\n", agnt.thumbnailheight);
				fmt::print("\n");
			} else if (b->getType() == "file") {
				cobFileBlock file(b);
				fmt::print("inline FILE \"{}\" \"{}\"\n", file.filename, file.filename);
				fmt::print("# filetype = {}\n", file.filetype);
				fmt::print("# filesize = {}\n", file.filesize);
				fmt::print("\n");
			} else if (b->getType() == "auth") {
				cobAuthBlock auth(b);
				fmt::print("group AUTH \"{}\"\n", auth.authorname);
				fmt::print("\"Creation Day\" {}\n", auth.daycreated);
				fmt::print("\"Creation Month\" {}\n", auth.monthcreated);
				fmt::print("\"Creation Year\" {}\n", auth.yearcreated);
				fmt::print("\"Version\" {}\n", auth.version);
				fmt::print("\"Revision\" {}\n", auth.revision);
				fmt::print("\"Email\" \"{}\"\n", auth.authoremail);
				fmt::print("\"URL\" \"{}\"\n", auth.authorurl);
				fmt::print("\"Comments\" \"{}\"\n", escape(auth.authorcomments));
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