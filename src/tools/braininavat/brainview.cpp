#include "../../Creature.h"
#include "../../c2eBrain.h"
#include "../../oldBrain.h"

#include <QtGui>
#include "brainview.h"
#include <boost/format.hpp>

BrainView::BrainView() {
	creature = 0;

	neuron_var = 0;
	dendrite_var = 0;
	threshold = -1000.0f;

	setAutoFillBackground(true);
	QPalette pal(palette());
	pal.setColor(QPalette::Background, QColor(0, 0, 0));
	setPalette(pal);
}

void BrainView::setCreature(Creature *c) {
	creature = c;
}

QSize BrainView::minimumSize() {
	if (!creature) return QSize(0, 0);
	
	int neededwidth = 2, neededheight = 2;
	
	c2eCreature *c = dynamic_cast<c2eCreature *>(creature);
	oldCreature *oc = dynamic_cast<oldCreature *>(creature);
	if (c) {
		c2eBrain *b = c->getBrain();
		assert(b);

		for (std::map<std::string, c2eLobe *>::iterator i = b->lobes.begin(); i != b->lobes.end(); i++) {
			c2eBrainLobeGene *lobe = i->second->getGene();
			int this_x = lobe->x + lobe->width;
			int this_y = lobe->y + lobe->height;
			if (this_x > neededwidth)
				neededwidth = this_x;
			if (this_y > neededheight)
				neededheight = this_y;
		}
	} else if (oc) {
		oldBrain *b = oc->getBrain();
		assert(b);
		
		for (std::vector<oldLobe *>::iterator i = b->lobes.begin(); i != b->lobes.end(); i++) {
			oldBrainLobeGene *lobe = (*i)->getGene();
			int this_x = lobe->x + (*i)->getWidth();
			int this_y = lobe->y + (*i)->getHeight();
			if (this_x > neededwidth)
				neededwidth = this_x;
			if (this_y > neededheight)
				neededheight = this_y;
		}
	} else return QSize(0, 0);

	return QSize((neededwidth + 6) * 20, (neededheight + 5) * 20);
}

void BrainView::drawLobeBoundaries(QPainter &painter, unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string text) {
	int lobex = (x + 4) * 20;
	int lobey = (y + 4) * 20;
	painter.drawRect(lobex, lobey, width * 20, height * 20);
	painter.drawText(lobex, lobey - 20, 200, 200, Qt::AlignLeft, QString(text.c_str()));
}

void BrainView::paintEvent(QPaintEvent *) {
	if (!creature) return;
	c2eCreature *c = dynamic_cast<c2eCreature *>(creature);
	if (c) {
		drawC2eBrain(c->getBrain());
	} else {
		oldCreature *oc = dynamic_cast<oldCreature *>(creature);
		if (!oc) return;
		drawOldBrain(oc->getBrain());
	}
}

std::string niceNameForOldLobe(unsigned int id) {
	switch (id) {
		case 0: return "percept";
		case 1: return "drive";
		case 2: return "stim source";
		case 3: return "verb";
		case 4: return "noun";
		case 5: return "sensory";
		case 6: return "decision";
		case 7: return "attention";
		case 8: return "concept";
	}
	return boost::str(boost::format("lobe %d") % (int)id);
}

void BrainView::drawOldBrain(oldBrain *b) {
	assert(b);
	
	QPainter painter(this);
	QColor color(255, 255, 255);
	painter.setPen(color);
	
	QBrush oldbrush = painter.brush();
	
	std::map<oldNeuron *, std::pair<unsigned int, unsigned int> > neuroncoords;

	unsigned int id = 0;
	for (std::vector< oldLobe *>::iterator i = b->lobes.begin(); i != b->lobes.end(); i++) {
		oldBrainLobeGene *lobe = (*i)->getGene();
		drawLobeBoundaries(painter, lobe->x, lobe->y, (*i)->getWidth(), (*i)->getHeight(), niceNameForOldLobe(id));
		id++;

		for (unsigned int y = 0; y < (*i)->getHeight(); y++) {
			for (unsigned int x = 0; x < (*i)->getWidth(); x++) {
				unsigned int neuronid = x + (y * (*i)->getWidth());
				oldNeuron *neuron = (*i)->getNeuron(neuronid);
				
				int lobex = (lobe->x + 4) * 20;
				int lobey = (lobe->y + 4) * 20;
	
				// store the centre coordinate for drawing dendrites
				assert(neuroncoords.find(neuron) == neuroncoords.end());
				neuroncoords[neuron] = std::pair<unsigned int, unsigned int>(lobex + (x * 20) + 10, lobey + (y * 20) + 10);
				
				// if below threshold, don't draw
				float var = neuron->output / 255.0f; // TODO: allow choosing of state too?
				// TODO: muh
				if (threshold == 0.0f) {
					if (var == threshold) continue;
				} else {
					if (var <= threshold) continue;
				}

				// calculate appropriate colour
				float multiplier = 0.5 + (var / 2.2);
				QColor color(255 * multiplier, 255 * multiplier, 255 * multiplier);
		
				// draw neuron
				painter.setPen(color);
				QBrush brush(color);
				painter.setBrush(brush);
				painter.drawRect(lobex + (x * 20) + 6, lobey + (y * 20) + 6, 8, 8);
				painter.setBrush(oldbrush);
			}
		}	
	}

	for (std::vector< oldLobe *>::iterator i = b->lobes.begin(); i != b->lobes.end(); i++) {
		for (unsigned int j = 0; j < (*i)->getNoNeurons(); j++) {
			oldNeuron *dest = (*i)->getNeuron(j);
			for (unsigned int type = 0; type < 2; type++) {
				for (std::vector<oldDendrite>::iterator d = dest->dendrites[type].begin();
					d != dest->dendrites[type].end(); d++) {
					oldNeuron *src = d->src;

					float var = d->strength / 255.0;
					if (threshold == 0.0f) {
						if (var == threshold) continue;
					} else {
						if (var <= threshold) continue;
					}

					float multiplier = 0.5 + (var / 2.2);
					QColor color(multiplier * 255, multiplier * 255, multiplier * 255);
					painter.setPen(color);

					assert(neuroncoords.find(src) != neuroncoords.end());
					assert(neuroncoords.find(dest) != neuroncoords.end());
					painter.drawLine(neuroncoords[src].first, neuroncoords[src].second, neuroncoords[dest].first, neuroncoords[dest].second);
				}
			}
		}
	}
}

void BrainView::drawC2eBrain(c2eBrain *b) {
	assert(b);

	QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing, true); <- atrociously slow

	QBrush oldbrush = painter.brush();

	std::map<c2eNeuron *, std::pair<unsigned int, unsigned int> > neuroncoords;

	// draw lobes/neurons
	for (std::map<std::string, c2eLobe *>::iterator i = b->lobes.begin(); i != b->lobes.end(); i++) {
		c2eBrainLobeGene *lobe = i->second->getGene();
		QColor color(lobe->red, lobe->green, lobe->blue);
		painter.setPen(color);
		drawLobeBoundaries(painter, lobe->x, lobe->y, lobe->width, lobe->height, i->first);

		for (unsigned int y = 0; y < lobe->height; y++) {
			for (unsigned int x = 0; x < lobe->width; x++) {
				unsigned int neuronid = x + (y * lobe->width);
				c2eNeuron *neuron = i->second->getNeuron(neuronid);

				int lobex = (lobe->x + 4) * 20;
				int lobey = (lobe->y + 4) * 20;
	
				// store the centre coordinate for drawing dendrites
				assert(neuroncoords.find(neuron) == neuroncoords.end());
				neuroncoords[neuron] = std::pair<unsigned int, unsigned int>(lobex + (x * 20) + 10, lobey + (y * 20) + 10);
				
				// always highlight spare neuron
				if (i->second->getSpareNeuron() == neuronid) {
					// TODO: don't hardcode these names?
					if (i->second->getId() == "attn" || i->second->getId() == "decn" || i->second->getId() == "comb") {
						painter.setPen(color);
						painter.drawRect(lobex + (x * 20) + 5, lobey + (y * 20) + 5, 10, 10);
					}
				}

				// if below threshold, don't draw
				float var = neuron->variables[neuron_var];
				// TODO: muh
				if (threshold == 0.0f) {
					if (var == threshold) continue;
				} else {
					if (var <= threshold) continue;
				}

				// calculate appropriate colour
				float multiplier = 0.5 + (var / 2.2);
				QColor color(lobe->red * multiplier, lobe->green * multiplier, lobe->blue * multiplier);
		
				// draw neuron
				painter.setPen(color);
				QBrush brush(color);
				painter.setBrush(brush);
				painter.drawRect(lobex + (x * 20) + 6, lobey + (y * 20) + 6, 8, 8);
				painter.setBrush(oldbrush);
			}
		}
	}

	// draw dendrites
	for (std::vector<c2eTract *>::iterator i = b->tracts.begin(); i != b->tracts.end(); i++) {
		c2eBrainTractGene *tract = (*i)->getGene();
		std::string destlobename = std::string((char *)tract->destlobe, 4);
		if (b->lobes.find(destlobename) == b->lobes.end())
			continue; // can't find the source lobe, so whu? must be a bad tract
		c2eBrainLobeGene *destlobe = b->lobes[destlobename]->getGene();
		
		for (unsigned int j = 0; j < (*i)->getNoDendrites(); j++) {
			c2eDendrite *dend = (*i)->getDendrite(j);

			float var = dend->variables[dendrite_var];
			if (threshold == 0.0f) {
				if (var == threshold) continue;
			} else {
				if (var <= threshold) continue;
			}

			float multiplier = 0.5 + (var / 2.2);
			QColor color(destlobe->red * multiplier, destlobe->green * multiplier, destlobe->blue * multiplier);
			painter.setPen(color);

			assert(neuroncoords.find(dend->source) != neuroncoords.end());
			assert(neuroncoords.find(dend->dest) != neuroncoords.end());
			painter.drawLine(neuroncoords[dend->source].first, neuroncoords[dend->source].second, neuroncoords[dend->dest].first, neuroncoords[dend->dest].second);
		}
	}
}

