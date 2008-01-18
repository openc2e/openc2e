#include "../World.h"
#include "../Engine.h"

#include "AgentInjector.h"
#include "c1cobfile.h"
#include "../cobFile.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <QMessageBox>
#include <QPainter>

namespace fs = boost::filesystem;

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
	assert(current);

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
	ui.agentList->clear();
	ui.injectButton->setEnabled(false);
	ui.removeButton->setEnabled(false);

	for (std::vector<class cobFile *>::iterator i = cobfiles.begin(); i != cobfiles.end(); i++) {
		delete *i;
	}
}

void AgentInjector::readAgents() {
	resetAgents();

	for (unsigned int i = 0; i < world.data_directories.size(); i++) {
		fs::path p = world.data_directories[i];
		if (engine.version == 2) p = p / "/Objects/";
		if (!fs::exists(p) || !fs::is_directory(p)) {
			continue;
		}

		fs::directory_iterator end_itr; // default constructor is the end
		for (fs::directory_iterator itr(p); itr != end_itr; itr++) {
			std::string cobext = fs::extension(itr->path());
			std::transform(cobext.begin(), cobext.end(), cobext.begin(), (int(*)(int))tolower); // downcase
			if (cobext != ".cob") continue;
			
			std::string cob = itr->path().native_file_string();
			
			if (engine.version == 1) {
				std::ifstream cobstream(cob.c_str(), std::ios::binary);
				if (!cobstream.fail()) {
					c1cobfile cobfile(cobstream);
					QListWidgetItem *newItem = new QListWidgetItem(cobfile.name.c_str(), ui.agentList);
					newItem->setToolTip(cob.c_str());
				}
			} else if (engine.version == 2) {
				cobFile *cobfile = new cobFile(itr->path());
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
}

void AgentInjector::onInject() {
	std::string idata;

	if (!ui.agentList->currentItem()) return;

	if (engine.version == 1) {
		QString filename = ui.agentList->currentItem()->toolTip();
		std::ifstream cobstream(filename.toAscii(), std::ios::binary);
		if (cobstream.fail()) {
			return; // TODO: throw some kind of warning or something
		}

		c1cobfile cobfile(cobstream);
		for (unsigned int i = 0; i < cobfile.scripts.size(); i++) {
			idata += cobfile.scripts[i] + "\n";
		}
		for (unsigned int i = 0; i < cobfile.imports.size(); i++) {
			idata += "iscr," + cobfile.imports[i] + "\n";
		}
	} else if (engine.version == 2) {
		cobAgentBlock *b = (cobAgentBlock *)ui.agentList->currentItem()->data(Qt::UserRole).value<void *>();
		assert(b);

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

