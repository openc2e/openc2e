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

#include "braininavat.h"
#include <QtGui>
#include "brainview.h"
#include "c2eBrain.h"

#include <fstream>
using namespace std;

// Constructor which creates the main window.

BrainInAVat::BrainInAVat() {
	ourCreature = 0;
	scrollArea = new QScrollArea(this);
	ourView = new BrainView();
	scrollArea->setWidget(ourView);
	setCentralWidget(scrollArea);

	(void)statusBar();
	setWindowTitle(tr("openc2e's Brain in a Vat"));
	resize(600, 400);

	/* File menu */

	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open a genome file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit Brain in a Vat"));
	connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	separatorAct = fileMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		fileMenu->addAction(recentFileActs[i]);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	updateRecentFileActions();

	/* View menu */

	neuronActGroup = new QActionGroup(this);
	dendriteActGroup = new QActionGroup(this);
	thresholdActGroup = new QActionGroup(this);

	// neuron/dendrite variable selection
	for (unsigned int i = 0; i < 8; i++) {
		// TODO: friendly names for neuron/dendrite vars

		neuronActs[i] = new QAction(tr("Neuron var %1").arg(i), this);
		neuronActGroup->addAction(neuronActs[i]);
		neuronActs[i]->setCheckable(true);
		connect(neuronActs[i], SIGNAL(triggered()), this, SLOT(setSomeVar()));
		dendriteActs[i] = new QAction(tr("Dendrite var %1").arg(i), this);
		dendriteActGroup->addAction(dendriteActs[i]);
		dendriteActs[i]->setCheckable(true);
		connect(dendriteActs[i], SIGNAL(triggered()), this, SLOT(setSomeVar()));
	}
	neuronActs[0]->setChecked(true);
	dendriteActs[0]->setChecked(true);

	// threshold for element visibility
	noThresholdAct = new QAction(tr("Show all elements"), this);
	thresholdActGroup->addAction(noThresholdAct);
	noThresholdAct->setCheckable(true);
	noThresholdAct->setChecked(true);
	connect(noThresholdAct, SIGNAL(triggered()), this, SLOT(setNoThreshold()));
	nonZeroThresholdAct = new QAction(tr("Show elements with non-zero values only"), this);
	thresholdActGroup->addAction(nonZeroThresholdAct);
	nonZeroThresholdAct->setCheckable(true);
	connect(nonZeroThresholdAct, SIGNAL(triggered()), this, SLOT(setNonZeroThreshold()));
	showNoneAct = new QAction(tr("Show no elements"), this);
	thresholdActGroup->addAction(showNoneAct);
	showNoneAct->setCheckable(true);
	connect(showNoneAct, SIGNAL(triggered()), this, SLOT(setShowNone()));

	viewMenu = menuBar()->addMenu(tr("&View"));
	for (unsigned int i = 0; i < 8; i++)
		viewMenu->addAction(neuronActs[i]);
	viewMenu->addSeparator();
	for (unsigned int i = 0; i < 8; i++)
		viewMenu->addAction(dendriteActs[i]);
	viewMenu->addSeparator();
	viewMenu->addAction(noThresholdAct);
	viewMenu->addAction(nonZeroThresholdAct);
	viewMenu->addAction(showNoneAct);

	/* Control menu */

	tickAct = new QAction(tr("&Tick"), this);
	tickAct->setStatusTip(tr("Run the brain through one iteration"));
	tickAct->setEnabled(false);
	connect(tickAct, SIGNAL(triggered()), this, SLOT(tick()));

	sleepToggleAct = new QAction(tr("&Sleep"), this);
	sleepToggleAct->setStatusTip(tr("Set whether the brain is asleep (and dreaming) or not"));
	sleepToggleAct->setCheckable(true);
	sleepToggleAct->setEnabled(false);
	connect(sleepToggleAct, SIGNAL(triggered()), this, SLOT(toggleSleep()));

	controlMenu = menuBar()->addMenu(tr("&Control"));
	controlMenu->addAction(tickAct);
	controlMenu->addAction(sleepToggleAct);

	controlToolbar = addToolBar(tr("Control"));
	controlToolbar->addAction(tickAct);

	/* Help menu */

	menuBar()->addSeparator();

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Find out about Brain in a Vat"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
}

BrainInAVat::~BrainInAVat() {
	if (ourCreature)
		delete ourCreature;
	delete ourView;
}

// action handlers

void BrainInAVat::open() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Pick a genome"), QString(), tr("Genomes (*.gen)"));
	if (!fileName.isEmpty())
		loadFile(fileName);
}

void BrainInAVat::openRecentFile() {
	QAction *action = qobject_cast<QAction *>(sender());

	if (action)
		loadFile(action->data().toString());
}

void BrainInAVat::about() {
	QMessageBox::about(this, tr("openc2e's Brain in a Vat"), tr("An openc2e tool to monitor and experiment upon creature brains."));
}

void BrainInAVat::tick() {
	// brain updates are every 4 ticks
	// TODO: this is icky
	for (unsigned int i = 0; i < 4; i++)
		ourCreature->tick();

	ourView->update();
}

void BrainInAVat::setSomeVar() {
	QAction *action = qobject_cast<QAction *>(sender());

	for (unsigned int i = 0; i < 8; i++) {
		if (action == neuronActs[i])
			ourView->neuron_var = i;
		else if (action == dendriteActs[i])
			ourView->dendrite_var = i;
	}

	ourView->update();
}

void BrainInAVat::setNoThreshold() {
	ourView->threshold = -1000.0f;
	ourView->update();
}

void BrainInAVat::setNonZeroThreshold() {
	ourView->threshold = 0.0f;
	ourView->update();
}

void BrainInAVat::setShowNone() {
	ourView->threshold = 1000.0f;
	ourView->update();
}

void BrainInAVat::toggleSleep() {
	ourCreature->setDreaming(!ourCreature->isDreaming());
	sleepToggleAct->setChecked(ourCreature->isDreaming());
}

// code to Do Things!

void BrainInAVat::loadFile(const QString &fileName) {
	// zot any existing file
	setCurrentFile(QString());
	if (ourCreature) {
		delete ourCreature;
		ourCreature = 0;
		ourView->setCreature(ourCreature);
	}
	tickAct->setEnabled(false);
	sleepToggleAct->setChecked(false);
	sleepToggleAct->setEnabled(false);
	ourView->update();

	ifstream f(fileName.toAscii(), std::ios::binary);
	if (f.fail()) {
		QMessageBox::warning(this, tr("openc2e's Brain in a Vat"), tr("Cannot read file %1.").arg(fileName));
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	// read genome
	f >> noskipws;
	shared_ptr<genomeFile> gfile(new genomeFile());
	try {
		f >> *gfile;
	} catch (genomeException &e) {
		QApplication::restoreOverrideCursor();
		QMessageBox::warning(this, tr("openc2e's Brain in a Vat"), tr("Failed loading the genome due to error '%1'!").arg(e.what()));
		return;
	}

	// TODO: dialog to pick age, gender, variant?

	// create creature
	if (gfile->getVersion() == 3) {
		try {
			ourCreature = new c2eCreature(gfile, true, 0);
		} catch (creaturesException &e) {
			QApplication::restoreOverrideCursor();
			QMessageBox::warning(this, tr("openc2e's Brain in a Vat"), e.what());
			return;
		}
	} else {
		QApplication::restoreOverrideCursor();
		QMessageBox::warning(this, tr("openc2e's Brain in a Vat"), tr("This genome is of version %1, which is unsupported.").arg(gfile->getVersion()));
		return;
	}

	QApplication::restoreOverrideCursor();

	ourView->setCreature(ourCreature);
	
	// we're done; update title/recent files, and display a temporary status message
	tickAct->setEnabled(true);
	sleepToggleAct->setEnabled(true);
	ourView->resize(ourView->minimumSize());
	ourView->update();
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("Loaded %1 genes").arg(gfile->genes.size()), 2000);
}

// Some Recent Files magic.

void BrainInAVat::setCurrentFile(const QString &fileName) {
	curFile = fileName;
	if (curFile.isEmpty()) {
		setWindowTitle(tr("openc2e's Brain in a Vat"));
		return; // TODO: is this correct?
	} else
		setWindowTitle(tr("%1 - %2").arg(strippedName(curFile)).arg(tr("openc2e's Brain in a Vat")));

	// read the list of recent files
	QSettings settings("ccdevnet.org", "Brain In A Vat");
	QStringList files = settings.value("recentFileList").toStringList();

	// make sure the current file is at the top!
	files.removeAll(fileName);
	files.prepend(fileName);
	// remove any which are over the size
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	// store the list of recent files
	settings.setValue("recentFileList", files);

	// update ourselves..
	updateRecentFileActions(); // obviously assumes we only have one window
}

void BrainInAVat::updateRecentFileActions() {
	// read the list of recent files
	QSettings settings("ccdevnet.org", "Brain In A Vat");
	QStringList files = settings.value("recentFileList").toStringList();

	// enable recent files actions as needed
	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}

	// make sure the rest are disabled [ie, hidden]
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		recentFileActs[j]->setVisible(false);

	// only show the separator if there are recent files
	separatorAct->setVisible(numRecentFiles > 0);
}

QString BrainInAVat::strippedName(const QString &fullFileName) {
	return QFileInfo(fullFileName).fileName();
}

