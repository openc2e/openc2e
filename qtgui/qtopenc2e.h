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

#ifndef _QTOPENC2E_H
#define _QTOPENC2E_H

#include <QMainWindow>
#include "AgentInjector.h"

class QtOpenc2e : public QMainWindow {
	Q_OBJECT

public:
	QtOpenc2e();
	~QtOpenc2e();

private slots:
	void tick();

	void about();

	void agents();

private:
	QToolBar *maintoolbar;
	QMenu *fileMenu, *viewMenu, *controlMenu, *debugMenu, *toolsMenu, *creaturesMenu, *helpMenu;
	QAction *exitAct, *aboutAct, *agentInjectorAct;

	AgentInjector *agentInjector;
	
	class openc2eView *viewport;
};

#endif

