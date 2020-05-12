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
#include "Engine.h"

#include "openc2eview.h"
#include "QtBackend.h"

#ifdef _WIN32
#include <memory>
#include <windows.h>
#endif

#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>

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

	setViewport(new openc2eviewport());
	viewport()->setAttribute(Qt::WA_NativeWindow);
	viewport()->setAttribute(Qt::WA_NoSystemBackground); // no need for Qt to draw a background
	viewport()->setAttribute(Qt::WA_PaintOnScreen);

	setAttribute(Qt::WA_InputMethodEnabled);

	backend->setup(viewport());

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
	if (engine.camera->getMetaRoom()) {
		if (engine.camera->getMetaRoom()->wraparound()) {
			horizontalScrollBar()->setRange(0,engine.camera->getMetaRoom()->width());
		} else {
			horizontalScrollBar()->setRange(0,engine.camera->getMetaRoom()->width() - viewport()->width());
		}
		verticalScrollBar()->setRange(0,engine.camera->getMetaRoom()->height() - viewport()->height());
	}
}

int currentwidth, currentheight;

void openc2eView::resizeEvent(QResizeEvent *) {
	backend->resized(viewport()->width(), viewport()->height());
	currentwidth = viewport()->width();
	currentheight = viewport()->height();

	resizescrollbars();

	horizontalScrollBar()->setPageStep(width());
	verticalScrollBar()->setPageStep(height());
}

void openc2eView::paintEvent(QPaintEvent *) {
	// TODO: mad hax
	if (currentwidth == viewport()->width() && currentheight == viewport()->height()) {
		world.drawWorld();
		backend->renderDone();
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

void openc2eView::inputMethodEvent(QInputMethodEvent *e) {
	backend->inputMethodEvent(e);
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
		engine.camera->moveTo(horizontalScrollBar()->value() + lastMetaroom->x(), verticalScrollBar()->value() + lastMetaroom->y());
}

void openc2eView::tick() {
	if (lastMetaroom != engine.camera->getMetaRoom()) {
		lastMetaroom = engine.camera->getMetaRoom();
		resizescrollbars();
	}
}

bool openc2eView::needsRender() {
	return backend->needsRender();
}

