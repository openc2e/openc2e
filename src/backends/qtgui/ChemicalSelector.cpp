#include "World.h"
#include "ChemicalSelector.h"
#include "CreatureGrapher.h"
#include <cstring>
#include <fstream>
#include "Engine.h"
#include <QtGui>

ChemicalSelector::ChemicalSelector(CreatureGrapher *p): QWidget(p), parent(p) {
	// read in interesting chemical info from game files
	if (engine.version < 3) {
		// c1/c2 are easy; c2e wants scary catalogue stuff.
		std::ifstream ifs(world.findFile("allchemicals.str").c_str());
		if (!ifs.good()) {
			ifs.close();
			throw creaturesException("Couldn't find allchemicals.str");
		}
		for (int i = 0; ifs.good(); i++) {
			int len = ifs.get();
			char *name = new char[len+1];
			memset(name, 0, len+1);
			ifs.read(name, len);
			name[len] = 0;
			chemnames[i] = name;
		}
		ifs.close();
	}
	if (engine.version == 2) {
		// c2 has a ChemGroups file with useful data.
		std::ifstream ifs(world.findFile("Applet Data/ChemGroups").c_str());
		if (!ifs.good()) {
			ifs.close();
			throw creaturesException("Couldn't find ChemGroups");
		}
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

	// add the implicit "All" group.
	for (std::map<unsigned int, std::string>::iterator i = chemnames.begin(); i != chemnames.end(); i++) {
		chemgroups["All"].push_back(i->first);
	}

	QHBoxLayout *layout = new QHBoxLayout(this);
	grouplist = new QListWidget(this);
	connect(grouplist, SIGNAL(itemSelectionChanged()), this, SLOT(onGroupChange()));
	for (std::map<std::string, std::vector<unsigned int> >::iterator i = chemgroups.begin();
	     i != chemgroups.end(); i++) { // i hate c++
	  new QListWidgetItem(i->first.c_str(), grouplist);
	}

	chemlist = new QListWidget(this);

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
	  item->setCheckState(Qt::Unchecked);
	}
}
