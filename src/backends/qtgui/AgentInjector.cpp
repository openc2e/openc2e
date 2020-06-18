/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "World.h"
#include "Engine.h"
#include "prayManager.h"

#include "AgentInjector.h"
#include "cobfile/c1cobfile.h"
#include "cobfile/c2cobfile.h"

#include <cassert>
#include <ghc/filesystem.hpp>

#include <QMessageBox>
#include <QPainter>

namespace fs = ghc::filesystem;

AgentInjector::AgentInjector(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);
	
	connect(ui.injectButton, SIGNAL(clicked()), this, SLOT(onInject()));
	connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(onRemove()));
	connect(ui.agentList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(onSelect(QListWidgetItem *, QListWidgetItem *)));
	connect(ui.agentList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), ui.imagePreview, SLOT(onSelect(QListWidgetItem *, QListWidgetItem *)));
	
	setAttribute(Qt::WA_QuitOnClose, false);
	
	resize(400,500);

	readAgents();
}

AgentInjector::~AgentInjector() {
	resetAgents();
}

void AgentInjector::onSelect(QListWidgetItem *current, QListWidgetItem *prev) {
	if (!current) return;

	ui.injectButton->setEnabled(true);

	if (engine.version == 2) {
		cobAgentBlock *b = (cobAgentBlock *)current->data(Qt::UserRole).value<void *>();
		assert(b);
		ui.Description->setText(b->description.c_str());
		ui.removeButton->setEnabled(b->removescript.size() != 0);
	} else {
		ui.removeButton->setEnabled(false); // TODO: support remover files!
		ui.Description->setText("");
	}
}

void AgentInjector::resetAgents() {
	if (engine.version == 2) {
		for (int i = 0; i < ui.agentList->count(); i++) {
			QListWidgetItem *item = ui.agentList->item(i);
			cobAgentBlock *b = (cobAgentBlock *)item->data(Qt::UserRole).value<void *>();
			delete b;
		}
	}
	ui.agentList->clear();
	ui.injectButton->setEnabled(false);
	ui.removeButton->setEnabled(false);

	for (auto i = cobfiles.begin(); i != cobfiles.end(); i++) {
		delete *i;
	}
}

void AgentInjector::readAgents() {
	resetAgents();

	for (unsigned int i = 0; i < world.data_directories.size(); i++) {
		fs::path p = world.data_directories[i];
		if (engine.version == 2) p = p / "Objects/";
		if (!fs::exists(p) || !fs::is_directory(p)) {
			continue;
		}

		fs::directory_iterator end_itr; // default constructor is the end
		for (fs::directory_iterator itr(p); itr != end_itr; itr++) {
			std::string cobext = itr->path().extension().string();
			std::transform(cobext.begin(), cobext.end(), cobext.begin(), (int(*)(int))tolower); // downcase
			if (cobext != ".cob") continue;

			std::string cob = itr->path().string();

			if (engine.version == 1) {
				std::ifstream cobstream(cob.c_str(), std::ios::binary);
				if (!cobstream.fail()) {
					c1cobfile cobfile = read_c1cobfile(cobstream);
					QListWidgetItem *newItem = new QListWidgetItem(cobfile.name.c_str(), ui.agentList);
					newItem->setToolTip(cob.c_str());
				}
			} else if (engine.version == 2) {
				c2cobfile *cobfile = new c2cobfile(itr->path());
				cobfiles.push_back(cobfile);

				for (std::vector<cobBlock *>::iterator i = cobfile->blocks.begin(); i != cobfile->blocks.end(); i++) {
					if ((*i)->type != "agnt") continue;
					cobAgentBlock *a = new cobAgentBlock(*i);
					QListWidgetItem *newItem = new QListWidgetItem(a->name.c_str(), ui.agentList);
					newItem->setToolTip(cob.c_str());
					newItem->setData(Qt::UserRole, QVariant::fromValue((void *)a));
				}
			}
		}
	}

	ui.agentList->sortItems();
}

void AgentInjector::onInject() {
	std::string idata;

	if (!ui.agentList->currentItem()) return;

	if (engine.version == 1) {
		QString filename = ui.agentList->currentItem()->toolTip();
		std::ifstream cobstream(filename.toLatin1(), std::ios::binary);
		if (cobstream.fail()) {
			return; // TODO: throw some kind of warning or something
		}

		c1cobfile cobfile = read_c1cobfile(cobstream);
		for (unsigned int i = 0; i < cobfile.object_scripts.size(); i++) {
			idata += cobfile.object_scripts[i] + "\n";
		}
		for (unsigned int i = 0; i < cobfile.install_scripts.size(); i++) {
			idata += "iscr," + cobfile.install_scripts[i] + "\n";
		}
	} else if (engine.version == 2) {
		cobAgentBlock *b = (cobAgentBlock *)ui.agentList->currentItem()->data(Qt::UserRole).value<void *>();
		assert(b);

		c2cobfile *cobfile = b->getParent()->getParent();
		assert(cobfile);

		// dependencies
		assert(b->deptypes.size() == b->depnames.size());
		for (unsigned int i = 0; i < b->deptypes.size(); i++) {
			unsigned short type = b->deptypes[i];
			std::string name = b->depnames[i];

			unsigned int praytype;
			switch (type) {
				case 0: praytype = 2; break;
				case 1: praytype = 1; break;
				default: QMessageBox::warning(this, tr("Couldn't inject:"), tr("Unknown dependency type")); return;
			}

			std::string directory = world.praymanager->getResourceDir(praytype);
			caos_assert(!directory.empty());

			fs::path possiblefile = fs::path(directory) / fs::path(name);
			if (!world.findFile(possiblefile.string()).empty()) continue; // TODO: update file if necessary?

			std::vector<cobBlock *>::iterator j;

			std::string encounteredfilenames;

			// check all of the available file blocks in this COB
			for (j = cobfile->blocks.begin(); j != cobfile->blocks.end(); j++) {
				if ((*j)->type != "file") continue;
				cobFileBlock *a = new cobFileBlock(*j);
				if (a->filetype == type && a->filename == name) {
					// Found dependency!
					fs::path dir = fs::path(world.getUserDataDir()) / fs::path(directory);
					if (!fs::exists(dir))
						fs::create_directory(dir);
					assert(fs::exists(dir) && fs::is_directory(dir)); // TODO: error handling

					fs::path outputfile = dir / fs::path(name);
					assert(!fs::exists(outputfile));

					std::ofstream output(outputfile.string().c_str(), std::ios::binary);
					output.write((char *)a->getFileContents(), a->filesize);

					a->getParent()->free();

					break;
				}
				encounteredfilenames += " '" + a->filename + "'";
			}

			if (j == cobfile->blocks.end()) {
				// Couldn't find dependency!
				std::string error = "Failed to find required dependency '" + name + "', possible choices were:" + encounteredfilenames;
				QMessageBox::warning(this, tr("Couldn't inject:"), error.c_str());
				return;
			}
		}

		// script injection
		for (unsigned int i = 0; i < b->scripts.size(); i++) {
			idata += b->scripts[i] + "\n";
		}
		idata += "iscr," + b->installscript + "\n";
	}

	idata += "rscr\n";

	std::string result = engine.executeNetwork(idata);
	if (result.size())
		QMessageBox::warning(this, tr("Injection returned data (error?):"), result.c_str());
}

void AgentInjector::onRemove() {
	std::string idata;

	if (!ui.agentList->currentItem()) return;

	if (engine.version == 2) {
		cobAgentBlock *b = (cobAgentBlock *)ui.agentList->currentItem()->data(Qt::UserRole).value<void *>();
		assert(b);

		idata += b->removescript += "\n";
	}

	idata += "rscr\n";

	std::string result = engine.executeNetwork(idata);
	if (result.size())
		QMessageBox::warning(this, tr("Removal returned data (error?):"), result.c_str());
}

