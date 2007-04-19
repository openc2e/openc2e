#include "brainview.h"
#include <QtGui>
#include "c2eBrain.h"

BrainView::BrainView(BrainInAVat *p) {
	parent = p;

	neuron_var = 0;
	dendrite_var = 0;
	threshold = -1000.0f;

	setAutoFillBackground(true);
	QPalette pal(palette());
	pal.setColor(QPalette::Background, QColor(0, 0, 0));
	setPalette(pal);
}

QSize BrainView::minimumSize() {
	c2eCreature *c = parent->getCreature();
	if (!c) return QSize(0, 0);
	c2eBrain *b = c->getBrain();
	assert(b);

	int neededwidth = 2, neededheight = 2;
	for (std::map<std::string, c2eLobe *>::iterator i = b->lobes.begin(); i != b->lobes.end(); i++) {
		c2eBrainLobeGene *lobe = i->second->getGene();
		int this_x = lobe->x + lobe->width;
		int this_y = lobe->y + lobe->height;
		if (this_x > neededwidth)
			neededwidth = this_x;
		if (this_y > neededheight)
			neededheight = this_y;
	}

	return QSize((neededwidth + 6) * 20, (neededheight + 5) * 20);
}

void BrainView::paintEvent(QPaintEvent *) {
	c2eCreature *c = parent->getCreature();
	if (!c) return;
	c2eBrain *b = c->getBrain();
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
		painter.setBrush(oldbrush);
		int lobex = (lobe->x + 4) * 20;
		int lobey = (lobe->y + 4) * 20;
		painter.drawRect(lobex, lobey, lobe->width * 20, lobe->height * 20);
		painter.drawText(lobex, lobey - 20, 200, 200, Qt::AlignLeft, QString(i->first.c_str()));

		for (unsigned int y = 0; y < lobe->height; y++)
			for (unsigned int x = 0; x < lobe->width; x++) {
				unsigned int neuronid = x + (y * lobe->width);
				c2eNeuron *neuron = i->second->getNeuron(neuronid);

				// store the centre coordinate for drawing dendrites
				assert(neuroncoords.find(neuron) == neuroncoords.end());
				neuroncoords[neuron] = std::pair<unsigned int, unsigned int>(lobex + (x * 20) + 10, lobey + (y * 20) + 10);

				// always highlight spare neuron
				if (i->second->getSpareNeuron() == neuronid) {
					// TODO: don't hardcode these names?
					if (i->second->getId() == "attn" || i->second->getId() == "decn") {
						painter.setPen(color);
						painter.setBrush(oldbrush);
						painter.drawRect(lobex + (x * 20) + 5, lobey + (y * 20) + 5, 10, 10);
					}
				}

				// if below threshold, don't draw
				float var = neuron->variables[neuron_var];
				if (var <= threshold) continue;

				// calculate appropriate colour
				float multiplier = 0.5 + (var < 0.0f ? 0.0f : (var / 2));
				QColor color(lobe->red * multiplier, lobe->green * multiplier, lobe->blue * multiplier);
		
				// draw lobe
				painter.setPen(color);
				QBrush brush(color);
				painter.setBrush(brush);
				painter.drawRect(lobex + (x * 20) + 6, lobey + (y * 20) + 6, 8, 8);	
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
			if (var <= threshold) continue;

			float multiplier = 0.5 + (var < 0.0f ? 0.0f : (var / 2));
			QColor color(destlobe->red * multiplier, destlobe->green * multiplier, destlobe->blue * multiplier);
			painter.setPen(color);

			assert(neuroncoords.find(dend->source) != neuroncoords.end());
			assert(neuroncoords.find(dend->dest) != neuroncoords.end());
			painter.drawLine(neuroncoords[dend->source].first, neuroncoords[dend->source].second, neuroncoords[dend->dest].first, neuroncoords[dend->dest].second);
		}
	}
}

