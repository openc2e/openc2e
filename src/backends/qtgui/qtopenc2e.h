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
	void updateAppletStatus();
	
	void about();
	
	void showHatchery();
	void toggleHatchery();
	void showAgentInjector();
	void toggleAgentInjector();
	void showBrainViewer();
	
	void toggleShowMap();
	void toggleShowScrollbars();
	void toggleFastSpeed();
	void toggleDisplayUpdates();
	void toggleAutokill();
	void togglePause();
	void toggleMute();

	void newEgg();
	void newNorn();

private:
	QMenu *fileMenu, *viewMenu, *controlMenu, *debugMenu, *toolsMenu, *creaturesMenu, *helpMenu;
	QAction *exitAct, *aboutAct, *agentInjectorAct, *brainViewerAct, *hatcheryAct;
	QAction *showMapAct, *newNornAct, *newEggAct;
	QAction *pauseAct, *muteAct, *fastSpeedAct, *displayUpdatesAct, *autokillAct, *toggleScrollbarsAct;

	class AgentInjector *agentInjector;
	class BrainViewer *brainViewer;
	class CreatureGrapher *creatureGrapher;
	class QDockWidget *creatureGrapherDock;
	class Hatchery *hatchery;
	
	class openc2eView *viewport;

	AgentRef selectedcreature;
	std::string oldcreaturename;

	class QTimer *ourTimer;

	void constructMenus();

	/*** C2 ***/

	QPixmap standardicons, handicons, favtoolbaricons, appleticons;
	QPixmap seasonicon[4], timeofdayicon[5], temperatureicon[5];
	QPixmap healthicon[6], hearticon[4];

	// statusbar
	class QLabel *seasontext, *seasonimage, *yeartext, *timeofdayimage, *temperatureimage;
	class QLabel *healthimage, *heartimage, *iconimage;

	// Standard
	QAction *toolbarnextaction, *toolbareyeviewaction, *toolbartrackaction, *toolbarhaloaction;
	QAction *toolbarplayaction, *toolbarpauseaction;
	QAction *toolbarhelpaction, *toolbarwebaction;
	class QComboBox *speechcombo, *placecombo;

	// Pointer
	QAction *toolbarinvisibleaction, *toolbarteachaction, *toolbarpushaction;

	// Applets
	QAction *toolbarhatcheryaction, *toolbaragentaction;

	void loadC2Images();
	void createC2Toolbars();
	void setupC2Statusbar();
};

#endif

