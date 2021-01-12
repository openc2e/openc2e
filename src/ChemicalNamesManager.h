#pragma once

#include <string>
#include <utility>
#include <vector>

class ChemicalNamesManager {
  public:
	ChemicalNamesManager();
	~ChemicalNamesManager();
	const std::vector<std::string>& getChemicalNames();
	const std::vector<std::pair<std::string, std::vector<int>>>& getChemicalGroups();

  private:
	bool chemical_names_loaded = false;
	std::vector<std::string> chemical_names;
	bool chemical_groups_loaded = false;
	std::vector<std::pair<std::string, std::vector<int>>> chemical_groups;
};