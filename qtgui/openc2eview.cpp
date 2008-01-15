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

#include "../World.h"
#include "../MetaRoom.h"
#include "openc2eview.h"
#include <QtGui>
#include <boost/format.hpp>
#include "../SDLBackend.h"

/*
 * TODO:
 *
 * provide input to the game engine
 * non-X11 support
 * provide a resize event to the engine as needed
 *
 */

openc2eView::openc2eView(QWidget *parent) : QAbstractScrollArea(parent) {
	backend = boost::shared_ptr<QtBackend>(new QtBackend());

	viewport()->setAttribute(Qt::WA_PaintOnScreen); // disable double-buffering
	viewport()->setAttribute(Qt::WA_OpaquePaintEvent); // no need for Qt to draw a background

	// keyboard focus. needed? better way?
	setFocusPolicy(Qt::StrongFocus);

	// we need the mouse move events
	setMouseTracking(true);

	for (unsigned int i = 0; i < 256; i++)
		backend->downkeys[i] = false;

	// TODO: sane? do we really need relx/rely, anyway?
	lastmousex = mapFromGlobal(QCursor::pos()).x();
	lastmousey = mapFromGlobal(QCursor::pos()).y();

	lastMetaroom = NULL;
}

openc2eView::~openc2eView() {
}

boost::shared_ptr<class Backend> openc2eView::getBackend() {
	return boost::dynamic_pointer_cast<class Backend, class QtBackend>(backend);
}

void openc2eView::resizescrollbars() {
	if (world.camera.getMetaRoom()) {
		if (world.camera.getMetaRoom()->wraparound()) {
			horizontalScrollBar()->setRange(0,world.camera.getMetaRoom()->width());
		} else {
			horizontalScrollBar()->setRange(0,world.camera.getMetaRoom()->width() - viewport()->width());
		}
		verticalScrollBar()->setRange(0,world.camera.getMetaRoom()->height() - viewport()->height());
	}
}

bool firsttime = true;
int currentwidth, currentheight;

void openc2eView::resizeEvent(QResizeEvent *) {
#if defined(Q_WS_X11) || defined(Q_WS_WIN)
	if (firsttime) {
		((QApplication *)QApplication::instance())->syncX();

		std::string windowidstr = boost::str(boost::format("SDL_WINDOWID=0x%lx") % viewport()->winId());
		putenv((char *)windowidstr.c_str());

		// TODO: make init() not resize itself?
		//SDL_QuitSubSystem(SDL_INIT_VIDEO);
		backend->SDLinit();

		firsttime = false;
	}
	backend->resized(viewport()->width(), viewport()->height());
	currentwidth = viewport()->width();
	currentheight = viewport()->height();

#else
#error No SDL rendering method for this platform yet.
	// TODO: fallback to off-screen SDL RGBA buffer + QPainter::drawImage?
#endif

	// add resize window event to backend queue
	SomeEvent e;
	e.type = eventresizewindow;
	e.x = viewport()->width();
	e.y = viewport()->height();
	backend->pushEvent(e);

	resizescrollbars();

	horizontalScrollBar()->setPageStep(width());
	verticalScrollBar()->setPageStep(height());
}

void openc2eView::paintEvent(QPaintEvent *) {
	((QApplication *)QApplication::instance())->syncX();

	if (!firsttime) {
		// TODO: mad hax
		if (currentwidth == viewport()->width() && currentheight == viewport()->height())
			world.drawWorld();
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
	if (m->buttons() && Qt::LeftButton) e.button |= buttonleft;
	if (m->buttons() && Qt::MidButton) e.button |= buttonmiddle;
	if (m->buttons() && Qt::RightButton) e.button |= buttonright;
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

// TODO: handle f keys (112-123 under windows, SDLK_F1 = 282 under sdl)
// see SDLBackend

int translateQtKey(int qtkey) {
	switch (qtkey) {
		case Qt::Key_Backspace: return 8;
		case Qt::Key_Tab: return 9;
		case Qt::Key_Clear: return 12;
		case Qt::Key_Return: return 13;
		case Qt::Key_Shift: return 16;
		case Qt::Key_Control: return 17;
		case Qt::Key_Pause: return 19;
		case Qt::Key_CapsLock: return 20;
		case Qt::Key_Escape: return 27;
		case Qt::Key_PageUp: return 33;
		case Qt::Key_PageDown: return 34;
		case Qt::Key_End: return 35;
		case Qt::Key_Home: return 36;
		case Qt::Key_Left: return 37;
		case Qt::Key_Up: return 38;
		case Qt::Key_Right: return 39;
		case Qt::Key_Down: return 40;
		case Qt::Key_Print: return 42;
		case Qt::Key_Insert: return 45;
		case Qt::Key_Delete: return 46;
		case Qt::Key_NumLock: return 144;
		default: return -1;
	}
}

void openc2eView::keyPressEvent(QKeyEvent *k) {
	for (int i = 0; i < k->text().size(); i++) {
		// TODO: openc2e probably doesn't like latin1
		char x = k->text().at(i).toLatin1();
		if (x != 0) {
			// We have a Latin-1 key which we can process.
			SomeEvent e;
			e.type = eventkeydown;
			e.key = x;
			backend->pushEvent(e);
			// TODO: change downkeys as necessary
			continue;
		}
	}
	
	int key = translateQtKey(k->key());
	if (key != -1) {
		SomeEvent e;
		e.type = eventspecialkeydown;
		e.key = key;
		backend->pushEvent(e);
		backend->downkeys[key] = true;
	}
}

void openc2eView::keyReleaseEvent(QKeyEvent *k) {
	int key = translateQtKey(k->key());
	if (key != -1) {
		SomeEvent e;
		e.type = eventspecialkeyup;
		e.key = key;
		backend->pushEvent(e);
		backend->downkeys[key] = false;
	}
}

void openc2eView::scrollContentsBy(int dx, int dy) {
	(void)dx;
	(void)dy;
	if (lastMetaroom)
		world.camera.moveTo(horizontalScrollBar()->value() + lastMetaroom->x(), verticalScrollBar()->value() + lastMetaroom->y());
}

void openc2eView::tick() {
	if (lastMetaroom != world.camera.getMetaRoom()) {
		lastMetaroom = world.camera.getMetaRoom();
		resizescrollbars();
	}
}

bool QtBackend::pollEvent(SomeEvent &e) {
	// obtain events from backend
	if (SDLBackend::pollEvent(e)) return true;

	if (events.size() == 0)
		return false;

	e = events.front();
	events.pop_front();
	return true;
}

void QtBackend::pushEvent(SomeEvent e) {
	events.push_back(e);
}

bool QtBackend::keyDown(int key) {
	return downkeys[key];
}
