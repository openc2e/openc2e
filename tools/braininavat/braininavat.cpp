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

#include <fstream>
using namespace std;

// Constructor which creates the main window.

BrainInAVat::BrainInAVat() {
	ourCreature = 0;
	scrollArea = new QScrollArea(this);
	ourView = new BrainView(this);
	scrollArea->setWidget(ourView);
	setCentralWidget(scrollArea);

	(void)statusBar();
	setWindowTitle(tr("openc2e's Brain in a Vat"));
	resize(400, 300);

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

	viewMenu = menuBar()->addMenu(tr("&View"));

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

// code to Do Things!

void BrainInAVat::loadFile(const QString &fileName) {
	// zot any existing file
	setCurrentFile(QString());
	if (ourCreature) {
		delete ourCreature;
		ourCreature = 0;
	}
	ourView->update();

	ifstream f(fileName.toAscii());
	if (f.fail()) {
		QMessageBox::warning(this, tr("openc2e's Brain in a Vat"), tr("Cannot read file %1.").arg(fileName));
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	// read genome
	f >> noskipws;
	shared_ptr<genomeFile> gfile(new genomeFile());
	f >> *gfile;

	// TODO: dialog to pick age, gender, variant?

	// create creature
	if (gfile->getVersion() == 3) {
		ourCreature = new c2eCreature(gfile, true, 0);
	} else {
		QMessageBox::warning(this, tr("openc2e's Brain in a Vat"), tr("This genome is of version %1, which is unsupported.").arg(gfile->getVersion()));
		QApplication::restoreOverrideCursor();
		return;
	}

	QApplication::restoreOverrideCursor();

	// we're done; update title/recent files, and display a temporary status message
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

