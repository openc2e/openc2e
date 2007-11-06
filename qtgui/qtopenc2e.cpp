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

#include "qtopenc2e.h"
#include <QtGui>
#include "openc2eview.h"
#include "../Engine.h"
#include "../OpenALBackend.h"

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
	setWindowTitle(tr("openc2e"));
	resize(800, 600);

	/* File menu */

	exitAct = new QAction(tr("&Exit"), this);
	exitAct->setStatusTip(tr("Exit openc2e"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(exitAct);

	/* View menu */

	viewMenu = menuBar()->addMenu(tr("&View"));

	/* Control menu */

	controlMenu = menuBar()->addMenu(tr("C&ontrol"));

	/* Debug menu */

	debugMenu = menuBar()->addMenu(tr("&Debug"));

	/* Tools menu */

	toolsMenu = menuBar()->addMenu(tr("&Tools"));

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
	engine.tick();
	if (engine.done) close();
}

// action handlers

void QtOpenc2e::about() {
	QMessageBox::about(this, tr("openc2e"), tr("An open-source game engine to run the Creatures series of games."));
}

