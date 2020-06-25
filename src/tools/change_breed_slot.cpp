#include "fileformats/genomeFile.h"

#include <algorithm>
#include <ctype.h>
#include <fmt/core.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "creatures/lifestage.h"
#include "spanstream.h"
#include "util.h"
#include "vectorstream.h"

namespace fs = ghc::filesystem;

std::string part_number_to_name(int part_number) {
    switch (part_number) {
        case 0: return "Head";
        case 1: return "Body";
        case 2: return "Legs";
        case 3: return "Arms";
        case 4: return "Tail";
        default: return std::to_string(part_number);
    }
}

std::string species_number_to_name(int species_number) {
    switch (species_number) {
        case 0: return "Norn";
        case 1: return "Grendel";
        case 2: return "Ettin";
        case 3: return "Geat";
        default: return std::to_string(species_number);
    }
}

int species_name_to_number(std::string name) {
    std::transform(name.begin(), name.end(), name.begin(), &tolower);
    
    if (name == "norn") {
        return 0;
    } else if (name == "grendel") {
        return 1;
    } else if (name == "ettin") {
        return 2;
    } else if (name == "geat") {
        return 3;
    } else {
        fmt::print(stderr, "error: unknown species '{}'\n", name);
        exit(1);
    }
}

std::string breed_slot_to_name(int slot_number) {
    if (slot_number < 26) {
        char x = 'A' + slot_number;
        return std::string(&x, &x + 1);
    } else {
        return std::to_string(slot_number);
    }
}

int breed_slot_name_to_number(std::string name) {
    if (name.size() != 1) {
        fmt::print(stderr, "error: unknown breed slot '{}'\n", name);
    }
    return toupper(name[0]) - 'A';
}

void check_roundtrip(const std::vector<uint8_t> &data) {
    genomeFile genome;
    spanstream(data) >> genome;

    vectorstream out;
    out << genome;
    while (static_cast<size_t>(out.tellp()) < data.size()) {
        out.write("", 1);
    }

    for (size_t i = 0; i < out.vector().size(); ++i) {
        if (out.vector()[i] != data[i]) {
            fmt::print(stderr, "error: sanity check failed: round-tripping differs at pos {}\n", i);
            exit(1);
        }
    }
}

void change_genome(std::string filename, int new_species_number, int new_slot_number) {
    auto data = readfilebinary(filename);
    check_roundtrip(data);
    
    genomeFile genome;
    spanstream(data) >> genome;
    
    for (size_t i = 0; i < genome.genes.size(); ++i) {
        gene* gene = genome.genes[i].get();
        
        if (typeid(*gene) == typeid(creatureAppearanceGene)) {
            auto *cag = (creatureAppearanceGene*)gene;
            
            std::string gender;
            if (cag->header.flags.maleonly && cag->header.flags.femaleonly) {
                gender = "Both";
            } else if (cag->header.flags.maleonly) {
                gender = "Male";
            } else if (cag->header.flags.femaleonly) {
                gender = "Female";
            } else {
                gender = "Both";
            }
            
            fmt::print(
                "appearance gene #{}: part={} species={} slot={} lifestage={} gender={} variant={}\n",
                i,
                part_number_to_name(cag->part),
                species_number_to_name(cag->species),
                breed_slot_to_name(cag->variant),
                lifestage_to_name(cag->header.switchontime),
                gender,
                cag->header.variant
            );
            
            cag->species = new_species_number;
            cag->variant = new_slot_number;
        }
    }
    
    fmt::print("writing to {}\n", filename);
    std::ofstream out(filename, std::ios_base::binary);
    out << genome;
    
    while (static_cast<size_t>(out.tellp()) < data.size()) {
        out.write("", 1);
    }
}

void change_filename(std::string filename, int new_species_number, int new_slot_number) {
    std::string directory = fs::path(filename).parent_path();
    std::string stem = fs::path(filename).stem();
    std::transform(stem.begin(), stem.end(), stem.begin(), &tolower);
    
    if (stem.size() != 4) {
        fmt::print(stderr, "error: doesn't look like an appearance file\n");
        exit(1);
    }
    
    fmt::print(
        "file {}: species={} slot={} part={} gender={} lifestage={}\n",
        fs::path(filename).filename().string(),
        species_number_to_name((stem[1] - '0') % 4),
        (char)toupper(stem[3]),
        (char)toupper(stem[0]),
        ((stem[1] - '0') < 4 ? "Male" : "Female"),
        lifestage_to_name((lifestage)(stem[2] - '0'))
    );
    
    std::string newname = fmt::format(
        "{}{}{}{}{}",
        stem[0],
        new_species_number + ((stem[1] - '0') < 4 ? 0 : 4),
        stem[2],
        (char)tolower(breed_slot_to_name(new_slot_number)[0]),
        fs::path(filename).extension().string()
    );
    
    newname = fs::path(filename).parent_path() / newname;
    if (newname != filename) {
        fmt::print("renaming to {}\n", newname);
        int ret = rename(filename.c_str(), newname.c_str());
        if (ret != 0) {
            perror("error renaming file");
            exit(1);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fmt::print(stderr, "USAGE: {} genome_file species slot\n", argv[0]);
        exit(1);
    }
    
    std::string filename = argv[1];
    if (!fs::exists(filename)) {
		fmt::print(stderr, "error: input file doesn't exist!\n");
		exit(1);
	}
    
    int new_species_number = species_name_to_number(argv[2]);
    int new_slot_number = breed_slot_name_to_number(argv[3]);
    
    std::string extension = fs::path(filename).extension();
    std::transform(extension.begin(), extension.end(), extension.begin(), &tolower);
    if (extension == ".gen") {
        change_genome(filename, new_species_number, new_slot_number);
    } else if (extension == ".c16" || extension == ".s16" || extension == ".att") {
        change_filename(filename, new_species_number, new_slot_number);
    } else {
        fmt::print(stderr, "error: unknown file type\n");
    }
}