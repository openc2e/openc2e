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

#include "GraphWidget.h"

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent) {
	minvertical = -1.0f;
	maxvertical = 1.0f;
	sizehorizontal = 100;

	QPalette pal(palette());
	pal.setColor(QPalette::Background, QColor(0, 0, 0));
	setPalette(pal);
	setAutoFillBackground(true);
}

GraphWidget::~GraphWidget() {
}

void GraphWidget::resetGraph() {
	datasets.clear();
	wipeGraph();
}

void GraphWidget::wipeGraph() {
	datapoints.clear();

	update();
}

void GraphWidget::setVerticalScale(float min, float max) {
	minvertical = min;
	maxvertical = max;

	update();
}

void GraphWidget::setHorizontalSize(unsigned int size) {
	sizehorizontal = size;

	update();
}

void GraphWidget::addDataSet(unsigned int dataset) {
	datasets[dataset].colour = QColor(255, 255, 255);
	datapoints[dataset].reserve(sizehorizontal);
}

void GraphWidget::removeDataSet(unsigned int dataset) {
	std::map<unsigned int, DataSetDetails>::iterator si = datasets.find(dataset);
	if (si != datasets.end()) datasets.erase(si);
	
	std::map<unsigned int, std::vector<float> >::iterator di = datapoints.find(dataset);
	if (di != datapoints.end()) datapoints.erase(di);
	
	update();
}

void GraphWidget::setDataSetColour(unsigned int dataset, QColor colour) {
	datasets[dataset].colour = colour;
	
	update();
}

void GraphWidget::setDataSetName(unsigned int dataset, QString name) {
	datasets[dataset].name = name;
}

void GraphWidget::addDataPoint(unsigned int dataset, float data) {
	datapoints[dataset].push_back(data);
	if (datapoints[dataset].size() > sizehorizontal)
		datapoints[dataset].erase(datapoints[dataset].begin());

	update();
}

void GraphWidget::paintEvent(QPaintEvent *) {
	QPainter painter(this);

	float xmultiplier = (float)width() / sizehorizontal;
	float ymultiplier = (float)height() / (maxvertical - minvertical);

	for (std::map<unsigned int, std::vector<float> >::iterator i = datapoints.begin(); i != datapoints.end(); i++) {
		std::vector<float> &points = i->second;
		if (points.size() == 0) continue;

		painter.setPen(datasets[i->first].colour);

		unsigned int pointposition = 0;

		// drawing starts on the *right* side, so as to keep everything in sync
		if (points.size() < sizehorizontal)
			pointposition = sizehorizontal - points.size();

		float posx, posy, oldposx = 0.0f, oldposy = 0.0f;
		for (unsigned int j = 0; j < points.size(); j++) {
			posx = pointposition * xmultiplier;
			posy = height() - ((points[j] - minvertical) * ymultiplier);

			// TODO: draw point

			if (j > 0) {
				painter.drawLine(oldposx, oldposy, posx, posy);
			}

			oldposx = posx;
			oldposy = posy;
			pointposition++;
		}
	}
}

