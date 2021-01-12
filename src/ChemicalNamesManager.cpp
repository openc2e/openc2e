#include "ChemicalNamesManager.h"

#include "Catalogue.h"
#include "Engine.h"
#include "World.h"
#include "fileformats/strFile.h"
#include "fileformats/verticalbarFile.h"
#include "utils/enumerate.h"
#include "utils/range.h"
#include "utils/to_vector.h"
#include "utils/trim.h"

#include <fmt/core.h>

ChemicalNamesManager::ChemicalNamesManager() = default;
ChemicalNamesManager::~ChemicalNamesManager() = default;

const std::vector<std::string>& ChemicalNamesManager::getChemicalNames() {
	if (!chemical_names_loaded) {
		if (engine.version < 3) {
			// c1/c2 are easy; c2e wants scary catalogue stuff.
			chemical_names = ReadStrFile(world.findFile("allchemicals.str"));
		} else {
			if (catalogue.hasTag("chemical_names")) {
				// c2e has chemical names in a catalogue file
				// TODO: there's some really dumb issues here, like chem 90 being '90' instead of 'Wounded' in c3/ds
				// TODO: one possibility is that C3 has a short_chemical_names tag without this kind of stupidity..
				chemical_names = catalogue.getTag("chemical_names");
			}
		}
		chemical_names_loaded = true;
	}
	if (chemical_names.size() < 256) {
		chemical_names.resize(256);
	}
	for (auto i : range(256)) {
		chemical_names[i] = trim(chemical_names[i]);
		if (chemical_names[i].empty()) {
			chemical_names[i] = fmt::format("<{}>", i);
		}
	}
	return chemical_names;
}

const std::vector<std::pair<std::string, std::vector<int>>>& ChemicalNamesManager::getChemicalGroups() {
	if (!chemical_groups_loaded) {
		chemical_groups.emplace_back("All", to_vector(range(256)));

		if (engine.version == 2) {
			// c2 has a ChemGroups file with useful data.
			auto lines = ReadVerticalBarSeparatedValuesFile(world.findFile("Applet Data/ChemGroups"));

			std::vector<std::pair<std::string, std::vector<int>>> chemgroups;
			for (auto l : lines) {
				std::string groupname = l[0];
				std::vector<int> chemids;
				for (auto i = l.begin() + 1; i != l.end(); ++i) {
					chemids.push_back(std::stoi(*i));
				}
				chemical_groups.emplace_back(groupname, chemids);
			}

		} else if (catalogue.hasTag("chemical graphing groups")) {
			// Creatures 3 has groups in the catalogue file
			const auto& groupnames = catalogue.getTag("chemical graphing groups");
			for (auto groupname : enumerate(groupnames)) {
				std::vector<int> chemids;
				std::string tagname = fmt::format("chemical graphing group {}", groupname.i + 1);
				if (catalogue.hasTag(tagname)) {
					const auto& chemids_tag = catalogue.getTag(tagname);
					for (auto c : chemids_tag) {
						int chem = std::stoi(c);
						if (chem != 0) {
							chemids.push_back(chem);
						}
					}
				}
				chemical_groups.emplace_back(groupname.value(), chemids);
			}
		}

		chemical_groups_loaded = true;
	}
	return chemical_groups;
}