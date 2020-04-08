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
#include "Room.h"
#include "Camera.h"
#include "AgentHelpers.h"

#include "Hatchery.h"
#include "AgentInjector.h"
#include "BrainViewer.h"
#include "CreatureGrapher.h"

#include "creatures/c2eCreature.h"
#include "creatures/oldCreature.h"
#include "creatures/SkeletalCreature.h"
#include "PointerAgent.h"

#include "version.h"

#include "peFile.h"

QPixmap imageFromExeResource(unsigned int resourceid, bool mask = true) {
	assert(engine.getExeFile());

	resourceInfo *r = engine.getExeFile()->getResource(PE_RESOURCETYPE_BITMAP, HORRID_LANG_ENGLISH, resourceid);
	if (!r) r = engine.getExeFile()->getResource(PE_RESOURCETYPE_BITMAP, 0x400, resourceid);
	if (!r) return QPixmap();

	unsigned int size = r->getSize() + 14;
	char *bmpdata = (char *)malloc(size);

	// fake a BITMAPFILEHEADER
	bmpdata[0] = 'B'; bmpdata[1] = 'M';
	memcpy(bmpdata + 2, &size, 4);
	memset(bmpdata + 6, 0, 8);

	memcpy(bmpdata + 14, r->getData(), r->getSize());

	QPixmap i;
	i.loadFromData((const uchar *)bmpdata, (int)size);
	if (mask) i.setMask(i.createHeuristicMask());

	free(bmpdata);

	return i;
}

QIcon iconFromImageList(QPixmap l, unsigned int n) {
	unsigned int x = n * (l.height() + 1);
	QPixmap img = l.copy(x, 0, l.height() + 1, l.height());
	return img;
}

// Constructor which creates the main window.
QtOpenc2e::QtOpenc2e(std::shared_ptr<QtBackend> backend) {
	viewport = new openc2eView(this, backend);
	setCentralWidget(viewport);

	connect(this, SIGNAL(creatureChanged()), this, SLOT(onCreatureChange()));

	// idle timer
	// TODO: should prbly have a background thread to do this?
	ourTimer = new QTimer(this);
	connect(ourTimer, SIGNAL(timeout()), this, SLOT(tick()));
	ourTimer->start();

	(void)statusBar();
	std::string titlebar = engine.getGameName() + " - openc2e ";
#ifdef DEV_BUILD
	titlebar += "(development build)";
#endif
	setWindowTitle(titlebar.c_str());
	resize(800, 600);

	hatchery = new Hatchery(this);
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

	constructMenus();

	if (engine.version == 2 && engine.getExeFile()) {
		loadC2Images();
		createC2Toolbars();
		setupC2Statusbar();
	}

	onCreatureChange();
}

void QtOpenc2e::constructMenus() {
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
	
	newEggAct = new QAction(tr("Create a random &egg"), this);
	if (engine.version > 2) newEggAct->setEnabled(false); // TODO
	connect(newEggAct, SIGNAL(triggered()), this, SLOT(newEgg()));
	debugMenu->addAction(newEggAct);

	/* Tools menu */

	toolsMenu = menuBar()->addMenu(tr("&Tools"));

	hatcheryAct = new QAction(tr("&Hatchery"), this);
	connect(hatcheryAct, SIGNAL(triggered()), this, SLOT(showHatchery()));
	toolsMenu->addAction(hatcheryAct);
	if (engine.version > 2) hatcheryAct->setEnabled(false);

	agentInjectorAct = new QAction(tr("&Agent Injector"), this);
	connect(agentInjectorAct, SIGNAL(triggered()), this, SLOT(showAgentInjector()));
	toolsMenu->addAction(agentInjectorAct);

	brainViewerAct = new QAction(tr("&Brain Viewer"), this);
	connect(brainViewerAct, SIGNAL(triggered()), this, SLOT(showBrainViewer()));
	toolsMenu->addAction(brainViewerAct);

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

/*
 * We obviously can't distribute the Creatures 2 icons with openc2e,
 * so we just use the ones in the Creatures2.exe file.
 */
void QtOpenc2e::loadC2Images() {
	// we always use the 21x21 icon set
	standardicons = imageFromExeResource(0xe3);
	handicons = imageFromExeResource(0xe4);
	favtoolbaricons = imageFromExeResource(0xe5);
	appleticons = imageFromExeResource(0xe6);

	for (unsigned int i = 0; i < 4; i++)
		seasonicon[i] = imageFromExeResource(0x98 + i, false);

	timeofdayicon[0] = imageFromExeResource(0xa3, false);
	timeofdayicon[1] = imageFromExeResource(0xc4, false);
	timeofdayicon[2] = imageFromExeResource(0xc2, false);
	timeofdayicon[3] = imageFromExeResource(0xc3, false);
	timeofdayicon[4] = imageFromExeResource(0xc5, false);

	for (unsigned int i = 0; i < 5; i++)
		temperatureicon[i] = imageFromExeResource(0xa4 + i, false);

	healthicon[0] = imageFromExeResource(0xe8); // disabled (gray)
	healthicon[1] = imageFromExeResource(0xc7); // 0/4
	healthicon[2] = imageFromExeResource(0xac); // 1/4
	healthicon[3] = imageFromExeResource(0xad); // 2/4
	healthicon[4] = imageFromExeResource(0xab); // 3/4
	healthicon[5] = imageFromExeResource(0xaa); // 4/4

	hearticon[0] = imageFromExeResource(0xe9); // disabled (gray)
	hearticon[1] = imageFromExeResource(0xae); // large
	hearticon[2] = imageFromExeResource(0xaf); // medium
	hearticon[3] = imageFromExeResource(0xb0); // small
	hearticon[4] = imageFromExeResource(0xe7); // dead (blue)
}

void QtOpenc2e::createC2Toolbars() {
	QToolBar *stdtoolbar = new QToolBar("Standard", this);
	stdtoolbar->setIconSize(QSize(22, 21));

	toolbarnextaction = stdtoolbar->addAction(iconFromImageList(standardicons, 0), "Next");
	toolbareyeviewaction = stdtoolbar->addAction(iconFromImageList(standardicons, 1), "Eye View");
	toolbareyeviewaction->setCheckable(true);
	toolbartrackaction = stdtoolbar->addAction(iconFromImageList(standardicons, 2), "Track");
	toolbartrackaction->setCheckable(true);
	toolbarhaloaction = stdtoolbar->addAction(iconFromImageList(standardicons, 3), "Halo");
	toolbarhaloaction->setCheckable(true);

	toolbarplayaction = stdtoolbar->addAction(iconFromImageList(standardicons, 4), "Play");
	stdtoolbar->insertSeparator(toolbarplayaction);
	toolbarpauseaction = stdtoolbar->addAction(iconFromImageList(standardicons, 5), "Pause");
	QActionGroup *playpausegroup = new QActionGroup(this);
	playpausegroup->addAction(toolbarplayaction);
	playpausegroup->addAction(toolbarpauseaction);
	toolbarplayaction->setCheckable(true);
	toolbarpauseaction->setCheckable(true);
	toolbarplayaction->setChecked(true);

	toolbarhelpaction = stdtoolbar->addAction(iconFromImageList(standardicons, 6), "Help");
	stdtoolbar->insertSeparator(toolbarhelpaction);
	toolbarwebaction = stdtoolbar->addAction(iconFromImageList(standardicons, 7), "Web");

	addToolBar(stdtoolbar);

	QToolBar *handtoolbar = new QToolBar("Pointer", this);
	handtoolbar->setIconSize(QSize(22, 21));

	toolbarinvisibleaction = handtoolbar->addAction(iconFromImageList(handicons, 0), "Invisible");
	toolbarinvisibleaction->setCheckable(true);
	toolbarteachaction = handtoolbar->addAction(iconFromImageList(handicons, 1), "Teach");
	handtoolbar->insertSeparator(toolbarteachaction);
	toolbarpushaction = handtoolbar->addAction(iconFromImageList(handicons, 2), "Push");
	QActionGroup *teachpushgroup = new QActionGroup(this);
	teachpushgroup->addAction(toolbarteachaction);
	teachpushgroup->addAction(toolbarpushaction);
	toolbarteachaction->setCheckable(true);
	toolbarpushaction->setCheckable(true);
	toolbarteachaction->setChecked(true);
	addToolBar(handtoolbar);

	QToolBar *applettoolbar = new QToolBar("Applets", this);
	applettoolbar->setIconSize(QSize(22, 21));

	toolbarhatcheryaction = applettoolbar->addAction(iconFromImageList(appleticons, 0), "Hatchery");
	toolbarhatcheryaction->setCheckable(true);
	connect(toolbarhatcheryaction, SIGNAL(triggered()), this, SLOT(toggleHatchery()));

	QAction *toolbarownersaction = applettoolbar->addAction(iconFromImageList(appleticons, 2), "Owner's Kit");
	toolbarownersaction->setCheckable(true);
	toolbarownersaction->setEnabled(false);

	QAction *toolbarhealthaction = applettoolbar->addAction(iconFromImageList(appleticons, 3), "Health Kit");
	toolbarhealthaction->setCheckable(true);
	toolbarhealthaction->setEnabled(false);

	QAction *toolbargraveyardaction = applettoolbar->addAction(iconFromImageList(appleticons, 9), "Graveyard");
	toolbargraveyardaction->setCheckable(true);
	toolbargraveyardaction->setEnabled(false);

	applettoolbar->addSeparator();

	QAction *tempaction;

	tempaction = applettoolbar->addAction(iconFromImageList(appleticons, 5), "Unknown Applet"); // breeder's kit?
	tempaction->setCheckable(true); tempaction->setEnabled(false);

	tempaction = applettoolbar->addAction(iconFromImageList(appleticons, 4), "Unknown Applet"); // science kit?
	tempaction->setCheckable(true); tempaction->setEnabled(false);

	tempaction = applettoolbar->addAction(iconFromImageList(appleticons, 12), "Unknown Applet"); // neuroscience kit?
	tempaction->setCheckable(true); tempaction->setEnabled(false);

	tempaction = applettoolbar->addAction(iconFromImageList(appleticons, 6), "Unknown Applet"); // observation kit?
	tempaction->setCheckable(true); tempaction->setEnabled(false);

	applettoolbar->addSeparator();

	toolbaragentaction = applettoolbar->addAction(iconFromImageList(appleticons, 7), "Agent Injector");
	toolbaragentaction->setCheckable(true);
	connect(toolbaragentaction, SIGNAL(triggered()), this, SLOT(toggleAgentInjector()));

	tempaction = applettoolbar->addAction(iconFromImageList(appleticons, 8), "Unknown Applet"); // history kit?
	tempaction->setCheckable(true); tempaction->setEnabled(false);

	tempaction = applettoolbar->addAction(iconFromImageList(appleticons, 1), "Unknown Applet"); // ecology kit?
	tempaction->setCheckable(true); tempaction->setEnabled(false);

	addToolBar(applettoolbar);

	addToolBarBreak();

	QToolBar *favtoolbar = new QToolBar("Favourites", this);
	favtoolbar->setIconSize(QSize(22, 21));

	QLabel *speechlabel = new QLabel("Speech History:", this);
	favtoolbar->addWidget(speechlabel);
	speechcombo = new QComboBox(this);
	speechcombo->setMinimumContentsLength(15);
	speechcombo->setEditable(true);
	favtoolbar->addWidget(speechcombo);
	QAction *sayaction = favtoolbar->addAction(iconFromImageList(favtoolbaricons, 0), "Say");

	QLabel *placelabel = new QLabel("Places:", this);
	favtoolbar->insertSeparator(favtoolbar->addWidget(placelabel));
	placecombo = new QComboBox(this);
	placecombo->setMinimumContentsLength(15);
	favtoolbar->addWidget(placecombo);
	QAction *goaction = favtoolbar->addAction(iconFromImageList(favtoolbaricons, 1), "Go");
	
	placecombo->addItem("The Incubator"); // TODO

	addToolBar(favtoolbar);

	QMenu *toolbarsMenu = viewMenu->addMenu(tr("&Toolbars"));
	toolbarsMenu->addAction(stdtoolbar->toggleViewAction());
	toolbarsMenu->addAction(handtoolbar->toggleViewAction());
	toolbarsMenu->addAction(applettoolbar->toggleViewAction());
	toolbarsMenu->addAction(favtoolbar->toggleViewAction());
}

void QtOpenc2e::setupC2Statusbar() {
	QLabel *w;

	seasontext = new QLabel(this);
	seasontext->setText("Spring");
	statusBar()->addWidget(seasontext);

	seasonimage = new QLabel(this);
	seasonimage->setPixmap(seasonicon[0]);
	statusBar()->addWidget(seasonimage);

	yeartext = new QLabel(this);
	yeartext->setText("Year: 000");
	statusBar()->addWidget(yeartext);

	timeofdayimage = new QLabel(this);
	timeofdayimage->setPixmap(timeofdayicon[0]);
	statusBar()->addWidget(timeofdayimage);

	// TODO: textual temperature
	temperatureimage = new QLabel(this);
	temperatureimage->setPixmap(temperatureicon[0]);
	statusBar()->addWidget(temperatureimage);

	w = new QLabel(this);
	w->setText("Health:");
	statusBar()->addPermanentWidget(w);

	healthimage = new QLabel(this);
	healthimage->setPixmap(healthicon[0]);
	statusBar()->addPermanentWidget(healthimage);

	heartimage = new QLabel(this);
	heartimage->setPixmap(hearticon[0]);
	statusBar()->addPermanentWidget(heartimage);

	QPixmap blank = imageFromExeResource(0xc6);
	iconimage = new QLabel(this);
	iconimage->setPixmap(blank);
	statusBar()->addPermanentWidget(iconimage);
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

	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> p = *i;
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

	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> p = *i;
		if (!p) continue; // grr, but needed

		CreatureAgent *a = dynamic_cast<CreatureAgent *>(p.get());
		if (!a) continue;
			
		if (p->genus != 1) continue; // TODO: check ettin/grendel status

		// TODO: add breed?
		std::string creaturename = creatureNameFor(p);
		if (creaturename.empty()) creaturename = "<Unnamed>";
		creaturename += std::string(" (") + (a->getCreature()->isFemale() ? "Female" : "Male") + ")";

		// create a new action with menu as parent, so it'll be destroyed on clear()
		QAction *creatureSelectAct = new QAction(creaturename.c_str(), creaturesMenu);
		creatureSelectAct->setData(QVariant::fromValue((void *)p.get()));

		creatureSelectAct->setCheckable(true);
		if (world.selectedcreature == p) creatureSelectAct->setChecked(true);
		connect(creatureSelectAct, SIGNAL(triggered()), this, SLOT(selectCreature()));

		if (monikerDataFor(p).getStatus() != borncreature)
			creatureSelectAct->setDisabled(true);

		creaturesMenu->addAction(creatureSelectAct);
	}

	if (creaturesMenu->isEmpty()) {
		QAction *dummyAct = new QAction("<none available>", creaturesMenu);
		dummyAct->setEnabled(false);
		creaturesMenu->addAction(dummyAct);
	}
}

void QtOpenc2e::onCreatureChange() {
	std::string titlebar = engine.getGameName() + " - openc2e ";
#ifdef DEV_BUILD
	titlebar += "(development build)";
#endif
	if (world.selectedcreature) {
		oldcreaturename = creatureNameFor(world.selectedcreature);

		if (oldcreaturename.empty())
			titlebar += " - <Unnamed>";
		else
			titlebar += " - " + oldcreaturename;
	}

	setWindowTitle(titlebar.c_str());	

	if (engine.version == 2 && engine.getExeFile()) {
		bool hc = world.selectedcreature;

		toolbareyeviewaction->setEnabled(hc);
		toolbartrackaction->setEnabled(hc);
		toolbarhaloaction->setEnabled(hc);

		// TODO: applets
	}
}

void QtOpenc2e::tick() {
	// set refreshdisplay occasionally, for updates when dorendering is false
	if (world.worldtickcount % world.ticktime == 0) // every 10 in-world seconds, with default times
		engine.refreshdisplay = true;

	bool didtick = engine.tick();

	int y = world.camera->getY();
	int x = world.camera->getX();
	viewport->tick();
	viewport->horizontalScrollBar()->setValue(x - world.camera->getMetaRoom()->x());
	viewport->verticalScrollBar()->setValue(y - world.camera->getMetaRoom()->y());
	
	if (engine.done) close();

	if (didtick) {
		if (engine.version == 2 && engine.getExeFile()) {
			// TODO: using cacheKey here is not so nice
			if (world.season < 4 && seasonimage->pixmap()->cacheKey() != seasonicon[world.season].cacheKey()) {
				seasonimage->setPixmap(seasonicon[world.season]);
				switch (world.season) {
					case 0: seasontext->setText(tr("Spring")); break;
					case 1: seasontext->setText(tr("Summer")); break;
					case 2: seasontext->setText(tr("Autumn")); break;
					case 3: seasontext->setText(tr("Winter")); break;
				}
			}
			if (world.timeofday < 5 && timeofdayimage->pixmap()->cacheKey() != timeofdayicon[world.timeofday].cacheKey()) {
				timeofdayimage->setPixmap(timeofdayicon[world.timeofday]);
			}
			yeartext->setText(boost::str(boost::format("Year: %03i") % (int)world.year).c_str());
		
			shared_ptr<Room> room_for_tempcheck;
			if (world.selectedcreature) { // prefer the room the selected creature is in
				room_for_tempcheck = roomContainingAgent(world.selectedcreature);
			}
			if (!room_for_tempcheck) // then try the room at the centre of the camera
				room_for_tempcheck = world.camera->getMetaRoom()->roomAt(world.camera->getXCentre(), world.camera->getYCentre());
			// TODO: c2 seems to try closest room as a last resort?
			if (room_for_tempcheck) {
				unsigned char temp = room_for_tempcheck->temp.getInt();
				unsigned int tempiconid = 4;
				if (temp < 255) tempiconid = temp / 64; // boundaries at 64, 128, 192 and 255
				if (temperatureimage->pixmap()->cacheKey() != temperatureicon[tempiconid].cacheKey()) {
					temperatureimage->setPixmap(temperatureicon[tempiconid]);
				}
				unsigned int temp_celsius = ((temp * 80) / 255) - 15;
				// TODO: fahrenheit calculation + textual version of temperature
				// (0 = -15C = 5F, 48 = 0C = 32F, 255 = 65C = 149F)
			}

			// TODO: norn health/drive/heartbeat, the below is a hack which changes the pixmap on every tick(!)
			if (!world.selectedcreature) {
				healthimage->setPixmap(healthicon[0]);
				heartimage->setPixmap(hearticon[0]);
			} else {
				CreatureAgent *ca = dynamic_cast<CreatureAgent *>(world.selectedcreature.get());
				if (ca) {
					oldCreature *c = dynamic_cast<oldCreature *>(ca->getCreature());
					assert(c); // this is c2, after all..
					if (c->isAlive()) {
						// this life force indicator shows the level of Glycogen
						// TODO: this does not update instantly w/Glycogen in real
						// c2, so perhaps it's actually another (related) data point?
						unsigned int x = c->getChemical(72) / 64;
						healthimage->setPixmap(healthicon[x + 2]); // 2-5 (1-4 bars)
						// TODO: flash indicator when norn is dying

						// TODO: heartbeat animation
						heartimage->setPixmap(hearticon[1]); // 1-3 = beating heart, large to small
					} else {
						// dead
						healthimage->setPixmap(healthicon[1]);
						heartimage->setPixmap(hearticon[4]); // blue heart
					}
					// TODO: drive
				}
			}
		}

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

	unsigned int i = engine.msUntilTick();
	if (i != 0 || ourTimer->interval() != 0) // TODO: things get a bit annoyingly unresponsive without this
		ourTimer->setInterval(i);

	updateAppletStatus(); // TODO: hack
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

void QtOpenc2e::updateAppletStatus() {
	if (engine.version == 2 && engine.getExeFile()) {
		// TODO: this really doesn't belong here
		if (world.paused) {
			if (toolbarplayaction->isChecked()) world.paused = false;
		} else {
			if (toolbarpauseaction->isChecked()) world.paused = true;
		}

		// disable/enable buttons if we moved between paused and unpaused
		if (world.paused && toolbarinvisibleaction->isEnabled()) {
			toolbarinvisibleaction->setEnabled(false);
			toolbarteachaction->setEnabled(false);
			toolbarpushaction->setEnabled(false);
		} else if (!world.paused && !toolbarinvisibleaction->isEnabled()) {
			toolbarinvisibleaction->setEnabled(true);
			toolbarteachaction->setEnabled(true);
			toolbarpushaction->setEnabled(true);
		}

		// update applet buttons based on whether the applet is visible
		toolbarhatcheryaction->setChecked(hatchery->isVisible());
		toolbaragentaction->setChecked(agentInjector->isVisible());

		// update 'next creature' button depending on whether there's any creatures we can select
		bool are_there_creatures_present = false;
		for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
			std::shared_ptr<Agent> p = *i;
			if (!p) continue; // grr, but needed

			CreatureAgent *a = dynamic_cast<CreatureAgent *>(p.get());
			if (!a) continue;

			if (p->genus != 1) continue; // TODO: check ettin/grendel status

			are_there_creatures_present = true;
			break;
		}
		toolbarnextaction->setEnabled(are_there_creatures_present);
	}
}

void QtOpenc2e::about() {
	std::string abouttxt = "An open-source game engine to run the Creatures "
		"series of games.\nVersion: ";
#ifdef DEV_BUILD
	abouttxt += "development build";
#else
	abouttxt += RELEASE_VERSION;
#endif
	abouttxt += " (built " __DATE__ ")";
	QMessageBox::about(this, tr("openc2e"), abouttxt.c_str());
}

void QtOpenc2e::showHatchery() {
	hatchery->show();
	hatchery->activateWindow();
	updateAppletStatus();
}

void QtOpenc2e::toggleHatchery() {
	if (hatchery->isVisible()) {
		hatchery->hide();
	} else {
		hatchery->show();
		hatchery->activateWindow();
	}
}

void QtOpenc2e::showAgentInjector() {
	agentInjector->show();
	agentInjector->activateWindow();
}

void QtOpenc2e::toggleAgentInjector() {
	if (agentInjector->isVisible()) {
		agentInjector->hide();
	} else {
		agentInjector->show();
		agentInjector->activateWindow();
	}
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

	a->genome_slots[0] = genome;
	world.newMoniker(genome, genomefile, a);
	world.history.getMoniker(world.history.findMoniker(genome)).moveToCreature(a);

	// TODO: set it dreaming
	
	c->born();

	world.hand()->addCarried(a);
}

void QtOpenc2e::newEgg() {
	makeNewEgg();
}

void QtOpenc2e::makeNewEgg() {
	std::string eggscript;
	/* create the egg obj */
	eggscript = boost::str(boost::format("new: simp eggs 8 %d 2000 0\n") % ((rand() % 6) * 8));
	/* set the pose */
	eggscript += "pose 3\n";	
	/* set the correct class/attributes */
	if (engine.version == 1)
		eggscript += "setv clas 33882624\nsetv attr 67\n";
	else if (engine.version == 2)
		eggscript += "setv cls2 2 5 2\nsetv attr 195\n";
	/* create the genome */
	if (engine.version == 1)
		eggscript += boost::str(boost::format("new: gene tokn dad%d tokn mum%d obv0\n") % (1 + rand() % 6) % (1 + rand() % 6));
	else if (engine.version == 2)
		eggscript += "new: gene tokn norn tokn norn obv0\n";
	/* set the gender */
	eggscript += "setv obv1 0\n";
	/* start the clock */
	eggscript += "tick 2400\n";
	
	/* move it into place */
	/* TODO: good positions? */
	if (engine.version == 1)
		eggscript += boost::str(boost::format("mvto %d 870\n") % (2600 + rand() % 200));
	else if (engine.version == 2)
		eggscript += boost::str(boost::format("mvto %d 750\n") % (4900 + rand() % 350));

	/* c2: enable gravity */
	if (engine.version == 2)
		eggscript += "setv grav 1\n";

	std::string err = engine.executeNetwork(eggscript);
	if (err.size())
		QMessageBox::warning(this, tr("Couldn't create egg:"), err.c_str());
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

