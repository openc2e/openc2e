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

#ifndef _OPENC2EVIEW_H
#define _OPENC2EVIEW_H

#include <QAbstractScrollArea>
#include <boost/shared_ptr.hpp>
#include "Backend.h"

class openc2eView : public QAbstractScrollArea {
	Q_OBJECT

public:
	openc2eView(QWidget *parent, boost::shared_ptr<class QtBackend>);
	~openc2eView();

protected:
	// event handlers
	void resizeEvent(QResizeEvent *);
	void paintEvent(QPaintEvent *);

	void mouseMoveEvent(QMouseEvent *m);
	
	void mouseEvent(QMouseEvent *m, eventtype t);
	void mousePressEvent(QMouseEvent *m);
	void mouseReleaseEvent(QMouseEvent *m);

	void wheelEvent(QWheelEvent *w);
	
	void keyPressEvent(QKeyEvent *k);
	void keyReleaseEvent(QKeyEvent *k);

	void scrollContentsBy(int dx, int dy);

	// variables
	boost::shared_ptr<class QtBackend> backend;
	int lastmousex, lastmousey;
	class MetaRoom *lastMetaroom;

	// helpers
	void resizescrollbars();

public:
	boost::shared_ptr<class Backend> getBackend();
	bool needsRender();
	void tick();
};

#endif

