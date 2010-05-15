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
#include "creatures/CreatureAgent.h"
#include "creatures/Creature.h"

#include "BrainViewer.h"
#include "tools/braininavat/brainview.h"

BrainViewer::BrainViewer(QWidget *parent) : QDialog(parent) {
	scrollArea = new QScrollArea(this);
	brainview = new BrainView();
	scrollArea->setWidget(brainview);

	brainview->threshold = 0.0f;

	resize(600, 400);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(scrollArea);

	setWindowTitle(tr("Brain Viewer"));
	setAttribute(Qt::WA_QuitOnClose, false);
}

BrainViewer::~BrainViewer() {
	delete brainview;
}

void BrainViewer::onTick() {
	brainview->setCreature(0);

	if (world.selectedcreature) {
		CreatureAgent *a = dynamic_cast<CreatureAgent *>(world.selectedcreature.get());
		if (a) {
			brainview->setCreature(a->getCreature());
		}
	}

	brainview->resize(brainview->minimumSize());
	brainview->update();
}

