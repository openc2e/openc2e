#include "../World.h"
#include "../Engine.h"

#include "AgentInjector.h"
#include <iostream>
#include <string>
#include "../endianlove.h"
#include "../streamutils.h"
#include <boost/scoped_array.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <QMessageBox>

namespace fs = boost::filesystem;

std::string readpascalstring(std::istream &s) {
	uint16 size;
	uint8 a; s.read((char *)&a, 1);
	if (a == 255)
		size = read16(s);
	else
		size = a;

	boost::scoped_array<char> x(new char[size]);
	//char x[size];
	s.read(x.get(), size);
	return std::string(x.get(), size);
}

struct c1cobfile {
	uint16 no_objects;
	uint32 expire_month;
	uint32 expire_day;
	uint32 expire_year;
	std::vector<std::string> scripts;
	std::vector<std::string> imports;
	uint16 no_objects_used;
	std::string name;

	uint32 imagewidth, imageheight;

	boost::scoped_array<char> imagedata;

	c1cobfile(std::ifstream &s) {
		s >> std::noskipws;

		uint16 version = read16(s);

		// TODO: mph
		if (version != 1) {
			//QMessageBox::warning(this, tr("Failed to open"), tr("Version %1 is not supported").arg((int)version));
			return;
		}

		no_objects = read16(s);
		expire_month = read32(s);
		expire_day = read32(s);
		expire_year = read32(s);
		uint16 noscripts = read16(s);
		uint16 noimports = read16(s);
		no_objects_used = read16(s);
		uint16 reserved_zero = read16(s);
		assert(reserved_zero == 0);

		for (unsigned int i = 0; i < noscripts; i++)
			scripts.push_back(readpascalstring(s));
		for (unsigned int i = 0; i < noimports; i++)
			imports.push_back(readpascalstring(s));

		imagewidth = read32(s);
		imageheight = read32(s);
		uint16 secondimagewidth = read16(s);
		assert(imagewidth == secondimagewidth);
		imagedata.reset(new char[imagewidth * imageheight]);
		s.read(imagedata.get(), imagewidth * imageheight);
		name = readpascalstring(s);
	}
};

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

void AgentInjector::onSelect(QListWidgetItem *current) {
	QString filename = ui.agentList->currentItem()->toolTip();
	std::ifstream cobstream(filename.toAscii(), std::ios::binary);
	if (cobstream.fail()) {
		return; // TODO: throw some kind of warning or something
	}

	c1cobfile cobfile(cobstream);

	QImage image(cobfile.imagedata.get(), cobfile.imagewidth, cobfile.imageheight, QImage::Format_Indexed8);
}

AgentInjector::AgentInjector(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);
	connect(ui.injectButton, SIGNAL(clicked()), this, SLOT(onInject()));
	connect(ui.agentList, SIGNAL(currentItemChanged(cur, prev)), this, SLOT(onSelect(cur)));
	setAttribute(Qt::WA_QuitOnClose, false);
	resize(400,500);
	readAgents();
}
