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

#include "Creature.h"
#include "CreatureGrapher.h"
#include "qtopenc2e.h"
#include "Engine.h"

CreatureGrapher::CreatureGrapher(QtOpenc2e *p) : QWidget(p), parent(p) {
	graph = new GraphWidget(this);
	if (engine.version < 3) {
		graph->setVerticalScale(0,255);
	}

	selector = new ChemicalSelector(this);
	connect(selector, SIGNAL(onSelectionChange(unsigned int)), this, SLOT(onChemSelectionChange(unsigned int)));
	
	resize(minimumSizeHint());

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(graph, 2);
	layout->addWidget(selector, 1);
	setLayout(layout);

	for (unsigned int i = 1; i < 255; i++) {
		graph->addDataSet(i);
	}
}

CreatureGrapher::~CreatureGrapher() {
}

void CreatureGrapher::onCreatureChange() {
	graph->wipeGraph();
}

void CreatureGrapher::onCreatureTick() {
	Creature *c = parent->getSelectedCreature();
	if (!c) return; // TODO: assert

	// TODO: we should only update on biochem ticks..

	c2eCreature *cc = dynamic_cast<c2eCreature *>(c);
	if (cc) {
		for (unsigned int i = 1; i < 255; i++)
			graph->addDataPoint(i, cc->getChemical(i));
	}
	oldCreature *oc = dynamic_cast<oldCreature *>(c);
	if (oc) {
		for (unsigned int i = 1; i < 255; i++)
			graph->addDataPoint(i, oc->getChemical(i));
	}

	graph->update();
}

void CreatureGrapher::onChemSelectionChange(unsigned int i) {
	graph->setDataSetVisible(i, selector->chemSelected(i));

	graph->update();
}

QSize CreatureGrapher::minimumSizeHint() const {
	return QSize(350, 400);
}

