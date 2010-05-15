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

#ifndef _BRAININAVAT_H
#define _BRAININAVAT_H

#include "creatures/Creature.h"

#include <QMainWindow>

class BrainInAVat : public QMainWindow {
	Q_OBJECT

private:
	c2eCreature *ourCreature;
	class BrainView *ourView;
	class QScrollArea *scrollArea;

public:
	BrainInAVat();
	~BrainInAVat();
	c2eCreature *getCreature() { return ourCreature; }

private slots:
	void open();
	void openRecentFile();
	void about();
	void tick();
	void setSomeVar();
	void setNoThreshold();
	void setNonZeroThreshold();
	void setShowNone();
	void toggleSleep();

private:
	void loadFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	void updateRecentFileActions();
	QString strippedName(const QString &fullFileName);

	QString curFile;

	QMenu *fileMenu, *recentFilesMenu, *viewMenu, *helpMenu, *controlMenu;
	QAction *openAct, *exitAct, *aboutAct, *separatorAct, *tickAct, *neuronActs[8], *dendriteActs[8], *noThresholdAct, *nonZeroThresholdAct, *showNoneAct, *sleepToggleAct;
	QToolBar *controlToolbar;
	class QActionGroup *neuronActGroup, *dendriteActGroup, *thresholdActGroup;

	enum { MaxRecentFiles = 5 };
	QAction *recentFileActs[MaxRecentFiles];
};

#endif

