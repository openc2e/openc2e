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

#ifndef _DEBUGKIT_H
#define _DEBUGKIT_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QListWidget>
#include <QTcpSocket>

class DebugKit : public QMainWindow {
	Q_OBJECT

private slots:
	void socketError();
	void connectAttempt();
	void injectAttempt();

	void connectButton();

	void selectedAgentChanged(int);
	void injectButton();

private:
	QTcpSocket *socket;
	QString injectdata;
	
	// top-level UI pages
	QTabWidget *tabwidget;
	QWidget *infopage, *injectorpage, *debugpage;

	// info page
	QGridLayout *infolayout;
	QLabel *connectedstatus;
	QLineEdit *hostname_edit;
	QSpinBox *port_edit;
	QPushButton *connect_button;
	QListWidget *gamedatadirs;

	// injector page
	QGridLayout *injectorlayout;
	QListWidget *agentlist;
	QPushButton *inject_button;

public:
	DebugKit();
	~DebugKit();

	void tryConnect();
	void setConnectedStatus(bool);
	void setBusyStatus(bool);

	void readAgents();
};

#endif

