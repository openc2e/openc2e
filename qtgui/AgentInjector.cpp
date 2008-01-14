#include "../World.h"
#include "../Engine.h"

#include "AgentInjector.h"
#include "c1cobfile.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <QMessageBox>
#include <QPainter>

namespace fs = boost::filesystem;

void AgentInjector::readAgents() {
	ui.agentList->clear();

	for (unsigned int i = 0; i < world.data_directories.size(); i++) {
		fs::path p = world.data_directories[i];
		if (!fs::exists(p) || !fs::is_directory(p)) {
			//QMessageBox::warning(this, tr("Directory missing or non-directory"), p.native_directory_string());
			continue;
		}

		fs::directory_iterator end_itr; // default constructor is the end
		for (fs::directory_iterator itr(p); itr != end_itr; itr++) {
			std::string cobext = fs::extension(itr->path());
			std::transform(cobext.begin(), cobext.end(), cobext.begin(), (int(*)(int))tolower); // downcase
			if (cobext != ".cob") continue;
			std::string cob = itr->path().native_file_string();
			std::ifstream cobstream(cob.c_str(), std::ios::binary);
			if (!cobstream.fail()) {
				c1cobfile cobfile(cobstream);
				QListWidgetItem *newItem = new QListWidgetItem(cobfile.name.c_str(), ui.agentList);
				newItem->setToolTip(cob.c_str());
			}
		}
	}
}

void AgentInjector::onInject() {
	QString filename = ui.agentList->currentItem()->toolTip();
	std::ifstream cobstream(filename.toAscii(), std::ios::binary);
	if (cobstream.fail()) {
		return; // TODO: throw some kind of warning or something
	}

	c1cobfile cobfile(cobstream);
	std::string idata;
	for (unsigned int i = 0; i < cobfile.scripts.size(); i++) {
		idata += cobfile.scripts[i] + "\n";
	}
	for (unsigned int i = 0; i < cobfile.imports.size(); i++) {
		idata += "iscr," + cobfile.imports[i] + "\n";
	}
	idata += "rscr\n";

	std::string result = engine.executeNetwork(idata);
	if (result.size())
		std::cout << "Injection returned data (error?): " << result << std::endl;
}

AgentInjector::AgentInjector(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);
	connect(ui.injectButton, SIGNAL(clicked()), this, SLOT(onInject()));
	connect(ui.agentList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), ui.imagePreview, SLOT(onSelect(QListWidgetItem *, QListWidgetItem *)));
	setAttribute(Qt::WA_QuitOnClose, false);
	resize(400,500);
	readAgents();
}
