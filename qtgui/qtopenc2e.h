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
#include <boost/shared_ptr.hpp>
#include "AgentRef.h"

class QtOpenc2e : public QMainWindow {
	Q_OBJECT

public:
	QtOpenc2e(boost::shared_ptr<class QtBackend>);
	~QtOpenc2e();

	class Creature *getSelectedCreature();

signals:
	void ticked();
	void creatureTicked();
	void creatureChanged();

private slots:
	void tick();

	void onCreatureChange();
	void updateCreaturesMenu();
	void selectCreature();
	void updateMenus();
	
	void about();
	
	void showAgentInjector();
	void showBrainViewer();
	
	void toggleShowMap();
	void toggleShowScrollbars();
	void toggleFastSpeed();
	void toggleDisplayUpdates();
	void toggleAutokill();
	void togglePause();
	void toggleMute();
	
	void newNorn();

private:
	QToolBar *maintoolbar;
	QMenu *fileMenu, *viewMenu, *controlMenu, *debugMenu, *toolsMenu, *creaturesMenu, *helpMenu;
	QAction *exitAct, *aboutAct, *agentInjectorAct, *brainViewerAct;
	QAction *showMapAct, *newNornAct;
	QAction *pauseAct, *muteAct, *fastSpeedAct, *displayUpdatesAct, *autokillAct, *toggleScrollbarsAct;

	class AgentInjector *agentInjector;
	class BrainViewer *brainViewer;
	class CreatureGrapher *creatureGrapher;
	class QDockWidget *creatureGrapherDock;
	
	class openc2eView *viewport;

	AgentRef selectedcreature;
	std::string oldcreaturename;
};

#endif

