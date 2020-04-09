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

/*
 * TODO:
 *
 * handle timeouts
 * notice if the socket was closed on us unexpectedly!
 *
 */

#include "debugkit.h"
#include <QtGui>

#include <assert.h>
#include <fstream>

using namespace std;

// Constructor which creates the main window.

DebugKit::DebugKit() {
	setWindowTitle(tr("openc2e's Debug Kit"));
	
	tabwidget = new QTabWidget(this);
	setCentralWidget(tabwidget);

	infopage = new QWidget();
	infolayout = new QGridLayout(infopage);
	connectedstatus = new QLabel(infopage);
	connectedstatus->setText(tr("Not connected"));
	infolayout->addWidget(connectedstatus, 0, 0, 1, 3);
	hostname_edit = new QLineEdit(infopage);
	hostname_edit->setText("localhost");
	infolayout->addWidget(hostname_edit, 1, 0);
	port_edit = new QSpinBox(infopage);
	port_edit->setMinimum(1); port_edit->setMaximum(65535);
	port_edit->setValue(20001); // TODO: read from dotfile
	infolayout->addWidget(port_edit, 1, 1);
	connect_button = new QPushButton(tr("Connect"), infopage);
	connect_button->connect(connect_button, SIGNAL(clicked()), this, SLOT(connectButton()));
	infolayout->addWidget(connect_button, 1, 2);
	QLabel *gamedatalabel = new QLabel(tr("Game data directories:"), this);
	infolayout->addWidget(gamedatalabel, 2, 0, 1, 3);
	gamedatadirs = new QListWidget(infopage);
	infolayout->addWidget(gamedatadirs, 3, 0, 1, 3);
	tabwidget->addTab(infopage, tr("Info"));
	
	injectorpage = new QWidget();
	injectorlayout = new QGridLayout(injectorpage);
	agentlist = new QListWidget(injectorpage);
	agentlist->setSortingEnabled(true);
	agentlist->connect(agentlist, SIGNAL(currentRowChanged(int)), this, SLOT(selectedAgentChanged(int)));
	injectorlayout->addWidget(agentlist, 0, 0);
	inject_button = new QPushButton(tr("Inject"), injectorpage);
	inject_button->setDisabled(true);
	inject_button->connect(inject_button, SIGNAL(clicked()), this, SLOT(injectButton()));
	injectorlayout->addWidget(inject_button, 1, 0);
	tabwidget->addTab(injectorpage, tr("Agent Injector"));
	
	debugpage = new QWidget();
	tabwidget->addTab(debugpage, tr("Debug"));

	resize(600, 400);

	socket = new QTcpSocket();
	socket->connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));

	setConnectedStatus(false);

	tryConnect();
}

DebugKit::~DebugKit() {
	delete socket;
}

void DebugKit::connectButton() {
	if (connect_button->text() == tr("Disconnect")) {
		setConnectedStatus(false);
		connectedstatus->setText(tr("Not connected"));
	} else {
		tryConnect();
	}
}

void DebugKit::setConnectedStatus(bool s) {
	tabwidget->setTabEnabled(1, s);
	tabwidget->setTabEnabled(2, s);
	hostname_edit->setDisabled(s);
	port_edit->setDisabled(s);
	gamedatadirs->setDisabled(!s);
	if (s) {
		connect_button->setText(tr("Disconnect"));
	} else {
		gamedatadirs->clear();
		connect_button->setText(tr("Connect"));
	}
}

void DebugKit::setBusyStatus(bool s) {
	connect_button->setDisabled(s);
	if (agentlist->currentRow() != -1)
		inject_button->setDisabled(s);
}

void DebugKit::tryConnect() {
	assert(socket->state() == QAbstractSocket::UnconnectedState);
	setBusyStatus(true);
	socket->connect(socket, SIGNAL(connected()), this, SLOT(connectAttempt()));
	socket->connectToHost(hostname_edit->text(), port_edit->value());
}

void DebugKit::socketError() {
	socket->disconnect(socket, SIGNAL(connected()), 0, 0);
	
	//if (socket->error() == QAbstractSocket::RemoteHostClosedError)
	//	return;

	setBusyStatus(false);
	setConnectedStatus(false);

	QString err = tr("Not connected") + " (" + socket->errorString() + ")";
	if (socket->error() == QAbstractSocket::ConnectionRefusedError)
		err = err + " - " + tr("Is openc2e running?");

	connectedstatus->setText(err);
}

void DebugKit::connectAttempt() {
	socket->disconnect(socket, SIGNAL(connected()), this, SLOT(connectAttempt()));
	setBusyStatus(false);

	// obtain the data we need: outs "hi\n", outs gnam, outs "\n", outs oc2e ddir
	socket->write("outs \"hi\\n\"\nouts gnam\nouts \"\\n\"\nouts oc2e ddir\nrscr\n");	
	socket->waitForReadyRead(200); // wait for 200ms at most
	QString result = socket->readLine().data();
	if (result == "hi\n") {
		setConnectedStatus(true);
		QString gnam = QString(socket->readLine().data()).trimmed();
		connectedstatus->setText(tr("Connected to game") + " \"" + gnam + "\"");

		QString l = socket->readLine().data();
		while (l.size() > 0) {
			gamedatadirs->addItem(l.trimmed());
			l = socket->readLine().data();
		}

		readAgents();
	} else {
		setConnectedStatus(false);
		connectedstatus->setText(tr("Not connected") + " (" + tr("bad handshake \"") + result + "\"" + tr("; are you sure openc2e is running?") + ")");
	}
	socket->close();
}

void DebugKit::selectedAgentChanged(int i) {
	if (i != -1)
		inject_button->setDisabled(false);
}

#include <iostream>
#include <string>
#include "../../endianlove.h"
#include "../../streamutils.h"

std::string readpascalstring(std::istream &s) {
	uint16 size;
	uint8 a; s.read((char *)&a, 1);
	if (a == 255)
		size = read16(s);
	else
		size = a;

	std::vector<char> x(size);
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

		uint32 imagewidth = read32(s);
		uint32 imageheight = read32(s);
		uint16 secondimagewidth = read16(s);
		assert(imagewidth == secondimagewidth);
		std::vector<char> imagedata(imagewidth * imageheight);
		s.read(imagedata.data(), imagewidth * imageheight);
		name = readpascalstring(s);
	}
};

void DebugKit::readAgents() {
	inject_button->setDisabled(true);
	agentlist->clear();

	for (unsigned int i = 0; i < gamedatadirs->count(); i++) {
		QString dirname = gamedatadirs->item(i)->text();
		QDir dir(dirname);
		if (!dir.exists()) {
			QMessageBox msgBox(QMessageBox::Warning, tr("Directory missing"), dirname, 0, this);
			continue;
		}
		
		QStringList filters;
		filters << "*.cob";
		dir.setNameFilters(filters);

		QStringList cobfiles = dir.entryList();
		for (unsigned int j = 0; j < cobfiles.size(); j++) {
			QString file = dirname + '/' + cobfiles[j];
			std::ifstream cobstream(file.toLatin1(), std::ios::binary);
			if (!cobstream.fail()) {
				c1cobfile cobfile(cobstream);
				QListWidgetItem *newItem = new QListWidgetItem(cobfile.name.c_str(), agentlist);
				newItem->setToolTip(file);
			}
		}
	}
}

void DebugKit::injectButton() {
	QString filename = agentlist->currentItem()->toolTip();
	std::ifstream cobstream(filename.toLatin1(), std::ios::binary);
	if (cobstream.fail()) {
		QMessageBox::warning(this, tr("Failed to open"), filename);
		return;
	}

	c1cobfile cobfile(cobstream);

	std::string idata;
	// this works around a stupid qt issue where it drops the socket if there's no lines returned
	// TODO: surely this is just fuzzie being dumb
	idata += "outs \"\\n\"\n";
	for (unsigned int i = 0; i < cobfile.scripts.size(); i++) {
		idata += cobfile.scripts[i] + "\n";
	}
	for (unsigned int i = 0; i < cobfile.imports.size(); i++) {
		idata += "iscr," + cobfile.imports[i] + "\n";
	}
	idata += "rscr\n";

	injectdata = idata.c_str();
	
	setBusyStatus(true);
	socket->connect(socket, SIGNAL(connected()), this, SLOT(injectAttempt()));
	socket->connectToHost(hostname_edit->text(), port_edit->value());
}

void DebugKit::injectAttempt() {
	socket->disconnect(socket, SIGNAL(connected()), this, SLOT(injectAttempt()));
	setBusyStatus(false);

	socket->write(injectdata.toLatin1());
	socket->waitForReadyRead(200); // wait for 200ms at most
	QString result = QString(socket->readAll().data()).trimmed();
	std::cout << (char *)result.toLatin1().data() << std::endl;
	socket->close();

	if (result.size())
		QMessageBox::warning(this, tr("Injection returned data (error?)"), result);
}

