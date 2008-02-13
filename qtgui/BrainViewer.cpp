#include "World.h"
#include "CreatureAgent.h"
#include "Creature.h"

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
			c2eCreature *c = dynamic_cast<c2eCreature *>(a->getCreature());
			if (c) {
				brainview->setCreature(c);
			}
		}
	}

	brainview->resize(brainview->minimumSize());
	brainview->update();
}

