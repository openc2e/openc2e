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

#include "../World.h"
#include "qtopenc2e.h"
#include <QtGui>
#include "openc2eview.h"
#include "../Engine.h"
#include "../OpenALBackend.h"
#include "../MetaRoom.h"

#include "ui_agents.h"

// Constructor which creates the main window.

QtOpenc2e::QtOpenc2e() {
	viewport = new openc2eView(this);
	setCentralWidget(viewport);

	engine.addPossibleBackend("qtgui", viewport->getBackend());
	engine.addPossibleAudioBackend("openal", shared_ptr<AudioBackend>(new OpenALBackend()));

	// TODO: handle this?
	/*if (!*/ engine.initialSetup() /*) return 0 */ ;

	// idle timer
	// TODO: should prbly have an every-X-seconds timer or a background thread to do this
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
	timer->start();

	(void)statusBar();
	std::string titlebar = engine.getGameName() + " - openc2e (development build)";
	setWindowTitle(titlebar.c_str());
	resize(800, 600);

	/* File menu */

	exitAct = new QAction(tr("&Exit"), this);
	exitAct->setStatusTip(tr("Exit openc2e"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(exitAct);

	/* View menu */

	viewMenu = menuBar()->addMenu(tr("&View"));

	toggleScrollbarsAct = new QAction(tr("Show &Scrollbars"), this);
	toggleScrollbarsAct->setCheckable(true);
	connect(toggleScrollbarsAct, SIGNAL(triggered()), this, SLOT(toggleShowScrollbars()));
	viewMenu->addAction(toggleScrollbarsAct);
	
	// only enable scrollbars for c1/c2, by default
	toggleScrollbarsAct->setChecked(engine.version < 3);
	toggleShowScrollbars();

	/* Control menu */

	controlMenu = menuBar()->addMenu(tr("C&ontrol"));
	connect(controlMenu, SIGNAL(aboutToShow()), this, SLOT(updateMenus()));

	pauseAct = new QAction(tr("&Pause"), this);
	connect(pauseAct, SIGNAL(triggered()), this, SLOT(togglePause()));
	controlMenu->addAction(pauseAct);

	controlMenu->addSeparator();
	
	fastSpeedAct = new QAction(tr("&Fast speed"), this);
	fastSpeedAct->setCheckable(true);
	connect(fastSpeedAct, SIGNAL(triggered()), this, SLOT(toggleFastSpeed()));
	controlMenu->addAction(fastSpeedAct);
	
	displayUpdatesAct = new QAction(tr("Slow &display updates"), this);
	displayUpdatesAct->setCheckable(true);
	displayUpdatesAct->setEnabled(false);
	connect(displayUpdatesAct, SIGNAL(triggered()), this, SLOT(toggleDisplayUpdates()));
	controlMenu->addAction(displayUpdatesAct);

	autokillAct = new QAction(tr("&Autokill"), this);
	autokillAct->setCheckable(true);
	autokillAct->setChecked(world.autokill);
	connect(autokillAct, SIGNAL(triggered()), this, SLOT(toggleAutokill()));
	controlMenu->addAction(autokillAct);

	/* Debug menu */

	debugMenu = menuBar()->addMenu(tr("&Debug"));
	connect(debugMenu, SIGNAL(aboutToShow()), this, SLOT(updateMenus()));
	
	showMapAct = new QAction(tr("Show &Map"), this);
	showMapAct->setCheckable(true);
	connect(showMapAct, SIGNAL(triggered()), this, SLOT(toggleShowMap()));
	debugMenu->addAction(showMapAct);

	newNornAct = new QAction(tr("Create a new (debug) &Norn"), this);
	if (engine.version > 2) newNornAct->setEnabled(false); // TODO
	connect(newNornAct, SIGNAL(triggered()), this, SLOT(newNorn()));
	debugMenu->addAction(newNornAct);

	/* Tools menu */

	toolsMenu = menuBar()->addMenu(tr("&Tools"));
	
	agentInjectorAct = new QAction(tr("&Agent Injector"), this);
	connect(agentInjectorAct, SIGNAL(triggered()), this, SLOT(showAgentInjector()));
	toolsMenu->addAction(agentInjectorAct);

	agentInjector = new AgentInjector();

	/* Creatures menu */

	creaturesMenu = menuBar()->addMenu(tr("&Creatures"));

	/* Help menu */

	menuBar()->addSeparator();

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Find out about openc2e"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
}

QtOpenc2e::~QtOpenc2e() {
}

void QtOpenc2e::tick() {
	// set refreshdisplay occasionally, for updates when dorendering is false
	if (world.worldtickcount % world.ticktime == 0) // every 10 in-world seconds, with default times
		engine.refreshdisplay = true;

	engine.tick();

	int y = world.camera.getY();
	int x = world.camera.getX();
	viewport->tick();
	viewport->horizontalScrollBar()->setValue(x - world.camera.getMetaRoom()->x());
	viewport->verticalScrollBar()->setValue(y - world.camera.getMetaRoom()->y());
	
	if (engine.done) close();

	if (viewport->needsRender()) {
		viewport->viewport()->repaint();
	}
}

// action handlers

void QtOpenc2e::updateMenus() {
	showMapAct->setChecked(world.showrooms);
	fastSpeedAct->setChecked(engine.fastticks);
	displayUpdatesAct->setChecked(!engine.dorendering);
	autokillAct->setChecked(world.autokill);
	if (world.paused) pauseAct->setText("&Play");
	else pauseAct->setText("&Pause");
}

void QtOpenc2e::about() {
	QMessageBox::about(this, tr("openc2e"), tr("An open-source game engine to run the Creatures series of games."));
}

void QtOpenc2e::showAgentInjector() {
	agentInjector->show();
	agentInjector->activateWindow();
}

void QtOpenc2e::toggleShowMap() {
	world.showrooms = !world.showrooms;
}

void QtOpenc2e::toggleShowScrollbars() {
	if (toggleScrollbarsAct->isChecked()) {
		viewport->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		viewport->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	} else {
		viewport->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		viewport->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
}

void QtOpenc2e::toggleFastSpeed() {
	engine.fastticks = !engine.fastticks;
	displayUpdatesAct->setEnabled(engine.fastticks);
	if (!engine.fastticks) {
		engine.dorendering = true;
		displayUpdatesAct->setChecked(false);
	}
}

void QtOpenc2e::toggleDisplayUpdates() {
	engine.dorendering = !engine.dorendering;
}

void QtOpenc2e::toggleAutokill() {
	world.autokill = !world.autokill;
}

void QtOpenc2e::togglePause() {
	world.paused = !world.paused;
}

#include "../Creature.h"
#include "../SkeletalCreature.h"
#include "../PointerAgent.h"

#undef slots
void QtOpenc2e::newNorn() {
	if (engine.version > 2) return; // TODO: fixme

	std::string genomefile = "test";
	shared_ptr<genomeFile> genome;
	try {
		genome = world.loadGenome(genomefile);
	} catch (creaturesException &e) {
		QMessageBox::warning(this, tr("Couldn't load genome file:"), e.prettyPrint().c_str());
		return;
	}

	if (!genome) {
		//
		return;
	}

	int sex = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	oldCreature *c;
	try {
		if (engine.version == 1) c = new c1Creature(genome, (sex == 2), 0);
		else c = new c2Creature(genome, (sex == 2), 0);	
	} catch (creaturesException &e) {
		QMessageBox::warning(this, tr("Couldn't create creature:"), e.prettyPrint().c_str());
		return;
	}

	SkeletalCreature *a;
	try {
		a = new SkeletalCreature(4, c);
	} catch (creaturesException &e) {
		delete c;
		QMessageBox::warning(this, tr("Couldn't create creature agent:"), e.prettyPrint().c_str());
		return;
	}
	a->finishInit();
	
	// if you make this work for c2e, you should probably set sane attributes here?

	c->setAgent(a);

	a->slots[0] = genome;
	world.newMoniker(genome, genomefile, a);

	// TODO: set it dreaming

	world.hand()->addCarried(a);
}

