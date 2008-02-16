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

CreatureGrapher::CreatureGrapher(QtOpenc2e *p) : QWidget(p), parent(p) {
	graph = new GraphWidget(this);
	
	QPalette pal(palette());
	pal.setColor(QPalette::Background, QColor(0, 0, 0));
	graph->setPalette(pal);
	graph->setAutoFillBackground(true);
	
	resize(minimumSizeHint());

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(graph);
	setLayout(layout);

	graph->setDataSetColour(2, QColor(255, 0, 0)); // Pyruvate in red
	graph->setDataSetColour(3, QColor(0, 255, 0)); // Glucose in blue
	graph->setDataSetColour(4, QColor(0, 0, 255)); // Glycogen in green
}

CreatureGrapher::~CreatureGrapher() {
}

void CreatureGrapher::onCreatureChange() {
	graph->wipeGraph();
}

void CreatureGrapher::onCreatureTick() {
	Creature *c = parent->getSelectedCreature();
	if (!c) return; // TODO: assert

	// TODO: oldCreature support
	c2eCreature *cc = dynamic_cast<c2eCreature *>(c);
	if (!cc) return;

	graph->addDataPoint(2, cc->getChemical(2));
	graph->addDataPoint(3, cc->getChemical(3));
	graph->addDataPoint(4, cc->getChemical(4));
}

QSize CreatureGrapher::minimumSizeHint() const {
	return QSize(100, 50);
}

