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
#include "Hatchery.h"

/*
  C1 hatchery resources:
  hatchery.bmp and htchmask.bmp used for the background/foreground
  SCAN*.BMP and hdsk.wav used for egg disk animation
  EGG*.BMP and hegg.wav used for egg movement animation
  FAN*.BMP and hfan.wav used for the fan animation
  lightoff.bmp and hlgt.wav used for the light flickering
  GENSPIN.BMP, and hmle.wav/hfml.wav used for male/female animation (male.bmp and female.bmp also present)
 */

Hatchery::Hatchery(QWidget *parent) : QDialog(parent) {
	setWindowTitle(tr("Hatchery"));
	setAttribute(Qt::WA_QuitOnClose, false);

	/* hatchery background */
	std::string hatcherybgfile = world.findFile("hatchery/hatchery.bmp");
	if (hatcherybgfile.empty()) return;
	QPixmap hatcherybg(QString(hatcherybgfile.c_str()));

	resize(hatcherybg.width() + 6, hatcherybg.height() + 6);
	
	/* create the widgets/layout */
	graphicsScene = new QGraphicsScene();
	graphicsView = new QGraphicsView(graphicsScene, this);
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(graphicsView);
	layout->setContentsMargins(0, 0, 0, 0);
	
	graphicsScene->addPixmap(hatcherybg);

	/* mask which goes over the eggs */
	std::string hatcherymaskfile = world.findFile("hatchery/htchmask.bmp");
	if (hatcherymaskfile.size()) {
		QPixmap hatcherymask(QString(hatcherymaskfile.c_str()));
		QColor maskcolour(0xff, 0x00, 0x80);
		hatcherymask.setMask(hatcherymask.createMaskFromColor(maskcolour));
	
		QGraphicsPixmapItem *maskitem = graphicsScene->addPixmap(hatcherymask);
		maskitem->setPos(0, 168);
	}

	/* fan animation */
	for (unsigned int i = 0; i < 4; i++) {
		// TODO
	}
	
	/* 'off' state for the light */
	// TODO
	
	/* eggs */
	// TODO
	
	/* gender marker animation */
	// TODO
}

Hatchery::~Hatchery() {
}

