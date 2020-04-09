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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H 1

#include <QWidget>

struct DataSetDetails {
	bool visible;
	QColor colour;
	QString name;
};

class GraphWidget : public QWidget {
	Q_OBJECT

protected:
	std::map<unsigned int, DataSetDetails> datasets;
	std::map<unsigned int, std::vector<float> > datapoints;

	float minvertical, maxvertical;
	unsigned int sizehorizontal;

	void paintEvent(QPaintEvent *);

public:
	GraphWidget(QWidget *parent);
	~GraphWidget();

	void resetGraph();
	void wipeGraph();

	void setVerticalScale(float min, float max);
	void setHorizontalSize(unsigned int size);

	void addDataSet(unsigned int dataset);
	void removeDataSet(unsigned int dataset);
	void setDataSetColour(unsigned int dataset, QColor colour);
	void setDataSetName(unsigned int dataset, QString name);
	void setDataSetVisible(unsigned int dataset, bool vis);

	void addDataPoint(unsigned int dataset, float data);
};

#endif /* GRAPHWIDGET_H */

