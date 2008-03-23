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
#include "qtopenc2e.h"
#include <QtGui>
#include "openc2eview.h"
#include "Engine.h"
#include "AudioBackend.h"
#include "MetaRoom.h"

#include "AgentInjector.h"
#include "BrainViewer.h"
#include "CreatureGrapher.h"

#include "Creature.h"
#include "SkeletalCreature.h"
#include "PointerAgent.h"

// Constructor which creates the main window.

QtOpenc2e::QtOpenc2e(boost::shared_ptr<QtBackend> backend) {
	viewport = new openc2eView(this, backend);
	setCentralWidget(viewport);

	connect(this, SIGNAL(creatureChanged()), this, SLOT(onCreatureChange()));

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

	muteAct = new QAction(tr("&Mute"), this);
	muteAct->setCheckable(true);
	connect(muteAct, SIGNAL(triggered()), this, SLOT(toggleMute()));
	controlMenu->addAction(muteAct);

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

	brainViewerAct = new QAction(tr("&Brain Viewer"), this);
	connect(brainViewerAct, SIGNAL(triggered()), this, SLOT(showBrainViewer()));
	toolsMenu->addAction(brainViewerAct);

	agentInjector = new AgentInjector(this);
	brainViewer = new BrainViewer(this);
	connect(this, SIGNAL(ticked()), brainViewer, SLOT(onTick()));

	creatureGrapher = new CreatureGrapher(this);
	connect(this, SIGNAL(ticked()), creatureGrapher, SLOT(onCreatureTick())); // TODO
	creatureGrapherDock = new QDockWidget(this);
	creatureGrapherDock->hide();
	creatureGrapherDock->setWidget(creatureGrapher);
	creatureGrapherDock->setFloating(true);
	creatureGrapherDock->resize(QSize(300, 300));
	creatureGrapherDock->setWindowTitle(tr("Creature Grapher"));
	toolsMenu->addAction(creatureGrapherDock->toggleViewAction());

	/* Creatures menu */

	creaturesMenu = menuBar()->addMenu(tr("&Creatures"));
	connect(creaturesMenu, SIGNAL(aboutToShow()), this, SLOT(updateCreaturesMenu()));

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

monikerData &monikerDataFor(AgentRef a) {
	shared_ptr<class genomeFile> g = a->getSlot(0);
	assert(g);
	std::string moniker = world.history.findMoniker(g);
	return world.history.getMoniker(moniker);
}

std::string creatureNameFor(AgentRef a) {
	return monikerDataFor(a).name;
}

void QtOpenc2e::selectCreature() {
	QObject *src = sender();
	QAction *srcaction = dynamic_cast<QAction *>(src);
	assert(srcaction);

	Agent *a = (Agent *)srcaction->data().value<void *>();

	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		boost::shared_ptr<Agent> p = *i;
		if (!p) continue; // grr, but needed

		if (a == p.get()) {
			world.selectCreature(p);
			return;
		}
	}

	// looks like the creature disappeared from under us..
}

void QtOpenc2e::updateCreaturesMenu() {
	creaturesMenu->clear();

	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		boost::shared_ptr<Agent> p = *i;
		if (!p) continue; // grr, but needed

		CreatureAgent *a = dynamic_cast<CreatureAgent *>(p.get());
		if (a) {
			// TODO: add breed?
			std::string creaturename = creatureNameFor(a);
			if (creaturename.empty()) creaturename = "<Unnamed>";
			creaturename += std::string(" (") + (a->getCreature()->isFemale() ? "Female" : "Male") + ")";

			// create a new action with menu as parent, so it'll be destroyed on clear()
			QAction *creatureSelectAct = new QAction(creaturename.c_str(), creaturesMenu);
			creatureSelectAct->setData(QVariant::fromValue((void *)a));

			creatureSelectAct->setCheckable(true);
			if (world.selectedcreature == p) creatureSelectAct->setChecked(true);
			connect(creatureSelectAct, SIGNAL(triggered()), this, SLOT(selectCreature()));
			
			if (monikerDataFor(a).getStatus() != borncreature)
				creatureSelectAct->setDisabled(true);

			creaturesMenu->addAction(creatureSelectAct);
		}
	}

	if (creaturesMenu->isEmpty()) {
		QAction *dummyAct = new QAction("<none available>", creaturesMenu);
		dummyAct->setEnabled(false);
		creaturesMenu->addAction(dummyAct);
	}
}

void QtOpenc2e::onCreatureChange() {
	std::string titlebar = engine.getGameName() + " - openc2e (development build)";

	if (world.selectedcreature) {
		oldcreaturename = creatureNameFor(world.selectedcreature);

		if (oldcreaturename.empty())
			titlebar += " - <Unnamed>";
		else
			titlebar += " - " + oldcreaturename;
	}

	setWindowTitle(titlebar.c_str());	
}

void QtOpenc2e::tick() {
	// set refreshdisplay occasionally, for updates when dorendering is false
	if (world.worldtickcount % world.ticktime == 0) // every 10 in-world seconds, with default times
		engine.refreshdisplay = true;

	bool didtick = engine.tick();

	int y = world.camera.getY();
	int x = world.camera.getX();
	viewport->tick();
	viewport->horizontalScrollBar()->setValue(x - world.camera.getMetaRoom()->x());
	viewport->verticalScrollBar()->setValue(y - world.camera.getMetaRoom()->y());
	
	if (engine.done) close();

	if (didtick) {
		if (world.selectedcreature != selectedcreature) {
			selectedcreature = world.selectedcreature;
			emit creatureChanged();
		} else if (world.selectedcreature) {
			// pick up name changes
			if (creatureNameFor(world.selectedcreature) != oldcreaturename)
				onCreatureChange();
		}
		
		// TODO: emit creatureTicked() if necessary

		emit ticked();
	}

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
	muteAct->setChecked(engine.audio->isMuted());
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

void QtOpenc2e::showBrainViewer() {
	brainViewer->show();
	brainViewer->activateWindow();
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

void QtOpenc2e::toggleMute() {
	engine.audio->setMute(!engine.audio->isMuted());
}

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

	SkeletalCreature *a = new SkeletalCreature(4);

	int sex = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	oldCreature *c;
	try {
		if (engine.version == 1) c = new c1Creature(genome, (sex == 2), 0, a);
		else c = new c2Creature(genome, (sex == 2), 0, a);
	} catch (creaturesException &e) {
		delete a;
		QMessageBox::warning(this, tr("Couldn't create creature:"), e.prettyPrint().c_str());
		return;
	}
	
	a->setCreature(c);
	a->finishInit();

	// if you make this work for c2e, you should probably set sane attributes here?

	a->slots[0] = genome;
	world.newMoniker(genome, genomefile, a);
	world.history.getMoniker(world.history.findMoniker(genome)).moveToCreature(a);

	// TODO: set it dreaming
	
	c->born();

	world.hand()->addCarried(a);
}

Creature *QtOpenc2e::getSelectedCreature() {
	if (world.selectedcreature) {
		CreatureAgent *a = dynamic_cast<CreatureAgent *>(world.selectedcreature.get());
		if (a) {
			return a->getCreature();
		}
	}

	return 0;
}

