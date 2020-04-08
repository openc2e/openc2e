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
#include "MetaRoom.h"
#include "Camera.h"

#include "openc2eview.h"
#include <QtGui>
#include "QtBackend.h"

#ifdef _WIN32
#include <windows.h>
#endif

/*
 * TODO:
 *
 * provide input to the game engine
 * non-X11 support
 * provide a resize event to the engine as needed
 *
 */

openc2eView::openc2eView(QWidget *parent, std::shared_ptr<QtBackend> b) : QAbstractScrollArea(parent) {
	backend = b;

	viewport()->setAttribute(Qt::WA_PaintOnScreen); // disable double-buffering
	viewport()->setAttribute(Qt::WA_OpaquePaintEvent); // no need for Qt to draw a background

	// keyboard focus. needed? better way?
	setFocusPolicy(Qt::StrongFocus);

	// we need the mouse move events
	setMouseTracking(true);

	// TODO: sane? do we really need relx/rely, anyway?
	lastmousex = mapFromGlobal(QCursor::pos()).x();
	lastmousey = mapFromGlobal(QCursor::pos()).y();

	lastMetaroom = NULL;
}

openc2eView::~openc2eView() {
}

std::shared_ptr<class Backend> openc2eView::getBackend() {
	return std::dynamic_pointer_cast<class Backend, class QtBackend>(backend);
}

void openc2eView::resizescrollbars() {
	if (world.camera->getMetaRoom()) {
		if (world.camera->getMetaRoom()->wraparound()) {
			horizontalScrollBar()->setRange(0,world.camera->getMetaRoom()->width());
		} else {
			horizontalScrollBar()->setRange(0,world.camera->getMetaRoom()->width() - viewport()->width());
		}
		verticalScrollBar()->setRange(0,world.camera->getMetaRoom()->height() - viewport()->height());
	}
}

bool firsttime = true;
int currentwidth, currentheight;

void openc2eView::resizeEvent(QResizeEvent *) {
	if (firsttime) {
		backend->setup(viewport());
		firsttime = false;
	}

	backend->resized(viewport()->width(), viewport()->height());
	currentwidth = viewport()->width();
	currentheight = viewport()->height();

	resizescrollbars();

	horizontalScrollBar()->setPageStep(width());
	verticalScrollBar()->setPageStep(height());
}

void openc2eView::paintEvent(QPaintEvent *) {
	((QApplication *)QApplication::instance())->syncX();

	if (!firsttime) {
		// TODO: mad hax
		if (currentwidth == viewport()->width() && currentheight == viewport()->height()) {
			world.drawWorld();
			backend->renderDone();
		}
	}
}

void openc2eView::mouseMoveEvent(QMouseEvent *m) {
	// add mouse move event to backend queue
	SomeEvent e;
	e.type = eventmousemove;
	e.xrel = m->x() - lastmousex;
	e.yrel = m->y() - lastmousey;
	lastmousex = e.x = m->x();
	lastmousey = e.y = m->y();
	e.button = 0;
	if (m->buttons() & Qt::LeftButton) e.button |= buttonleft;
	if (m->buttons() & Qt::MidButton) e.button |= buttonmiddle;
	if (m->buttons() & Qt::RightButton) e.button |= buttonright;
	backend->pushEvent(e);
}

// helper function
void openc2eView::mouseEvent(QMouseEvent *m, eventtype t) {
	SomeEvent e;
	e.type = t;
	e.x = m->x();
	e.y = m->y();
	switch (m->button()) {
		case Qt::LeftButton: e.button = buttonleft; break;
		case Qt::MidButton: e.button = buttonmiddle; break;
		case Qt::RightButton: e.button = buttonright; break;
		default: return;
	}
	backend->pushEvent(e);
}

void openc2eView::mousePressEvent(QMouseEvent *m) {
	mouseEvent(m, eventmousebuttondown);
}

void openc2eView::mouseReleaseEvent(QMouseEvent *m) {
	mouseEvent(m, eventmousebuttonup);
}

void openc2eView::wheelEvent(QWheelEvent *w) {
	SomeEvent e;
	e.type = eventmousebuttondown;
	e.x = w->x();
	e.y = w->y();	
	// TODO: qt combines events, we should generate one event for every 120 delta, maybe
	if (w->delta() < 0)
		e.button = buttonwheelup;
	else
		e.button = buttonwheeldown;
	backend->pushEvent(e);
}

void openc2eView::keyPressEvent(QKeyEvent *k) {
	backend->keyEvent(k, true);
}

void openc2eView::keyReleaseEvent(QKeyEvent *k) {
	backend->keyEvent(k, false);
}

void openc2eView::scrollContentsBy(int dx, int dy) {
	(void)dx;
	(void)dy;
	if (lastMetaroom)
		world.camera->moveTo(horizontalScrollBar()->value() + lastMetaroom->x(), verticalScrollBar()->value() + lastMetaroom->y());
}

void openc2eView::tick() {
	if (lastMetaroom != world.camera->getMetaRoom()) {
		lastMetaroom = world.camera->getMetaRoom();
		resizescrollbars();
	}
}

bool openc2eView::needsRender() {
	return backend->needsRender();
}

