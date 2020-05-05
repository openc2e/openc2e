#include "cobfile/c1cobfile.h"

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
    

    // printf("done reading\n");

    
}