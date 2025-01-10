#include "common/Exception.h"
#include "common/Ranges.h"
#include "common/endianlove.h"
#include "fileformats/c1cobfile.h"
#include "fileformats/c2cobfile.h"
#include "fileformats/caoslexer.h"
#include "fileformats/pngImage.h"
#include "openc2e/caosparser.h"
#include "openc2e/dialect.h"

#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>

namespace fs = ghc::filesystem;

void caos1_format_visitor(CAOSNodePtr node, std::string& out) {
	if (CAOSCommandNode* ccn = dynamic_cast<CAOSCommandNode*>(node.get())) {
		out += ccn->name;
		for (auto a : ccn->args) {
			out += " ";
			caos1_format_visitor(a, out);
		}
	} else if (CAOSConditionNode* ccn = dynamic_cast<CAOSConditionNode*>(node.get())) {
		for (size_t i = 0; i < ccn->args.size(); ++i) {
			if (i > 0)
				out += " ";
			caos1_format_visitor(ccn->args[i], out);
		}
	} else if (CAOSLiteralValueNode* clvn = dynamic_cast<CAOSLiteralValueNode*>(node.get())) {
		out += fmt::format("{}", clvn->token.data);
	} else if (CAOSLiteralWordNode* clwn = dynamic_cast<CAOSLiteralWordNode*>(node.get())) {
		out += clwn->word;
	} else {
		abort();
	}
}

std::string caos1_format(const std::string& text) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, text.c_str());
	auto toplevel = parse(tokens, getDialectByName("c1"));

	std::string out;
	int indent = 0;
	for (auto c : toplevel) {
		auto ccn = (CAOSCommandNode*)c.get();
		if (contains({"elif", "else", "endi", "ever", "next", "repe", "retn", "untl"}, ccn->name)) {
			indent = std::max(indent - 1, 0);
		} else if (contains({"endm"}, ccn->name)) {
			indent = 0;
		} else if (contains({"iscr", "rscr", "scrp"}, ccn->name)) {
			indent = 0;
			if (out.size() > 0) {
				out += "\n";
			}
		}
		for (int i = 0; i < indent; ++i) {
			out += "    ";
		}
		caos1_format_visitor(c, out);
		if (contains({"doif", "elif", "else", "enum", "epas", "esee", "etch", "iscr", "loop", "reps", "scrp", "subr"}, ccn->name)) {
			indent++;
		}
		out += "\n";
		if (contains({"wait"}, ccn->name)) {
			out += "\n";
		}
	}
	return out;
}

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

int main(int argc, char** argv) {
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

	std::ifstream in(input_path, std::ios::binary);

	unsigned char magic[4];
	in.read((char*)magic, 4);
	in.seekg(0);

	if (memcmp(magic, "cob2", 4) == 0) {
		fmt::print("\"cob2\"\n");
		fmt::print("\n");
		c2cobfile cob(input_path);
		for (auto& b : cob.blocks) {
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
				fmt::print("# thumbnailwidth = {}\n", agnt.thumbnail.width);
				fmt::print("# thumbnailheight = {}\n", agnt.thumbnail.height);
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
			} else {
				fmt::print("unknown block type '{}'\n", b->getType());
			}
		}
	} else if (memcmp(magic, "\x01\x00", 2) == 0) {
		c1cobfile cob = read_c1cobfile(in);
		std::string sprite_filename = stem.string() + ".png";
		fmt::print("*# COB-Name \"{}\"\n", cob.name);
		if (cob.picture) {
			fmt::print("*# Picture \"{}\"\n", sprite_filename);
		}
		fmt::print("*# Quantity available = {}\n", cob.quantity_available);
		if (cob.quantity_used != 0) {
			fmt::print("*# Quantity used = {}\n", cob.quantity_used);
		}
		if (cob.expiration_month != 0 || cob.expiration_day != 0 || cob.expiration_year != 0) {
			fmt::print("*# Expiration month = {}\n", cob.expiration_month);
			fmt::print("*# Expiration day = {}\n", cob.expiration_day);
			fmt::print("*# Expiration year = {}\n", cob.expiration_year);
		}
		fmt::print("\n");

		std::string script;
		for (auto& install_script : cob.install_scripts) {
			script += "iscr," + install_script + "\n";
			// TODO: if only one, don't add iscr, and remove endm
		}
		for (auto& object_script : cob.object_scripts) {
			script += object_script + "\n";
			// TODO: make sure it ends with endm
		}
		try {
			fmt::print(caos1_format(script));
		} catch (const Exception& e) {
			fmt::print("* exception formatting CAOS: {}\n", e.what());
			fmt::print(script);
		}

		if (cob.picture) {
			WritePngFile(cob.picture, sprite_filename);
		}

	} else {
		fmt::print(stderr, "Not a recognized COB format: bad magic 0x{:02x}{:02x}{:02x}{:02x}\n", magic[0], magic[1], magic[2], magic[3]);
		exit(1);
	}
}