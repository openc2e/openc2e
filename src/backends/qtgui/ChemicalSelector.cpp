#include "World.h"
#include "ChemicalSelector.h"
#include "CreatureGrapher.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include "Engine.h"
#include "Catalogue.h"
#include <QHBoxLayout>
#include <fmt/printf.h>

ChemicalSelector::ChemicalSelector(CreatureGrapher *p): QWidget(p), parent(p) {
	for (unsigned int i = 0; i < 256; i++) {
		chemselected[i] = false;
	}

	// read in interesting chemical info from game files
	if (engine.version < 3) {
		// c1/c2 are easy; c2e wants scary catalogue stuff.
		std::ifstream ifs(world.findFile("allchemicals.str").c_str());
		if (!ifs.good()) {
			std::cout << "Couldn't find allchemicals.str" << std::endl;
		} else {
			char n = ifs.get();
			char o = ifs.get();
			if (n != 0 || o != 1) {
				std::cout << "allchemicals.str is corrupt (bad magic)" << std::endl;
			} else {
				for (int i = 0; ifs.good(); i++) {
					int len = ifs.get();
					// file is only !good once you tried reading past the end
					if (!ifs.good()) break;
				
					if (len) {
						std::vector<char> name(len+1, '\0');
						ifs.read(&name[0], len);
						chemnames[i] = std::string(&name[0]);
					}
				}
			}
		}
		ifs.close();
	} else {
		if (catalogue.hasTag("chemical_names")) {
			// c2e has chemical names in a catalogue file
			// TODO: there's some really dumb issues here, like chem 90 being '90' instead of 'Wounded' in c3/ds
			// TODO: one possibility is that C3 has a short_chemical_names tag without this kind of stupidity..
			const std::vector<std::string> &t = catalogue.getTag("chemical_names");
			for (unsigned int i = 0; i < t.size(); i++) {
				chemnames[i] = t[i];
			}
		}
	}

	if (engine.version == 2) {
		// c2 has a ChemGroups file with useful data.
		std::ifstream ifs(world.findFile("Applet Data/ChemGroups").c_str());
		if (!ifs.good()) {
			ifs.close();
			std::cout << "Couldn't find ChemGroups" << std::endl;
		} else {
			while (ifs.good()) {
				std::string line;
				getline(ifs, line);
				if (line[0] == '#') {
					continue;
				}
				size_t firstbar = line.find('|');
				if (firstbar == std::string::npos) break;
				std::string groupname = line.substr(0, firstbar);
				const char *cline = strchr(line.c_str(), '|');
				int chemid;
				while (sscanf(cline, "|%d", &chemid)) {
					chemgroups[groupname].push_back(chemid);
					cline = strchr(cline+1, '|');
					if (!cline) break;
				}
			}
			ifs.close();
		}
	} else if (catalogue.hasTag("chemical graphing groups")) {
		// Creatures 3 has groups in the catalogue file
		const std::vector<std::string> &t = catalogue.getTag("chemical graphing groups");
		for (unsigned int i = 0; i < t.size(); i++) {
			std::string groupname = t[i];

			std::string tagname = fmt::sprintf("chemical graphing group %d", (int)(i + 1));
			if (catalogue.hasTag(tagname)) {
				const std::vector<std::string> &t = catalogue.getTag(tagname);
				for (unsigned int i = 0; i < t.size(); i++) {
					int chem = atoi(t[i].c_str());
					if (chem != 0)
						chemgroups[groupname].push_back(chem);
				}
			}
		}
	}

	for (int i = 1; i < 255; i++) {
		if (chemnames.find(i) == chemnames.end())
			chemnames[i] = fmt::sprintf("<%d>", i);
	}

	grouplist = new QListWidget(this);
	if (chemgroups.size() == 0) grouplist->hide();
	connect(grouplist, SIGNAL(itemSelectionChanged()), this, SLOT(onGroupChange()));
	chemlist = new QListWidget(this);
	connect(chemlist, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onChemChange(QListWidgetItem*)));
	
	// add the implicit "All" group.
	for (std::map<unsigned int, std::string>::iterator i = chemnames.begin(); i != chemnames.end(); i++) {
		if (i->first != 0)
			chemgroups["All"].push_back(i->first);
	}

	for (std::map<std::string, std::vector<unsigned int> >::iterator i = chemgroups.begin();
	     i != chemgroups.end(); i++) { // i hate c++
	  new QListWidgetItem(i->first.c_str(), grouplist);
	}
	grouplist->setCurrentRow(0);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(grouplist, 1);
	layout->addWidget(chemlist, 1);
	setLayout(layout);
}

void ChemicalSelector::onGroupChange() {
	chemlist->clear();
	std::string groupname = grouplist->currentItem()->text().toStdString();
	for (std::vector<unsigned int>::iterator i = chemgroups[groupname].begin();
	     i != chemgroups[groupname].end(); i++) {
	  QListWidgetItem *item = new QListWidgetItem(chemnames[*i].c_str(), chemlist);
	  item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
	  item->setCheckState(chemselected[*i] ? Qt::Checked : Qt::Unchecked);
	  item->setData(Qt::UserRole, *i);
	}
}

void ChemicalSelector::onChemChange(QListWidgetItem *item) {
	if (!item) return;

	unsigned int i = item->data(Qt::UserRole).toUInt();
	if (i == 0) return;
	
	bool newstate = (item->checkState() == Qt::Checked);
	if (chemselected[i] == newstate) return;
	
	chemselected[i] = newstate;
	emit onSelectionChange(i);
}

