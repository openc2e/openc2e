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

#include "QtBackend.h"
#include "qtopenc2e.h"
#include "Engine.h"
#include <QKeyEvent>
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <cassert>
#include <fmt/printf.h>
#include <iostream>
#include <memory>
#include "exceptions.h"

#ifdef _WIN32
#include <windows.h>
#endif

QtBackend::QtBackend() {
	viewport = 0;
	needsrender = false;

	for (unsigned int i = 0; i < 256; i++) {
		downkeys[i] = false;
	}
}

void QtBackend::shutdown() {
	if (!viewport) return;

	SDLBackend::shutdown();
}

void QtBackend::init() {
}

void QtBackend::setup(QWidget *vp) {
	viewport = vp;

	putenv("SDL_VIDEODRIVER=dummy");
	
	SDLBackend::init();

	viewport->setCursor(Qt::BlankCursor);
}

int QtBackend::idealBpp() {
	// TODO: handle 8bpp for C1

#ifdef _WIN32
	// TODO: how to pick up real depth on windows?
	if (viewport->depth() == 16) return 16;
	return 24;
#endif

	return 32;
}


void QtBackend::resized(int w, int h) {
	SDLBackend::resizeNotify(w, h);

	// add resize window event to backend queue
	SomeEvent e;
	e.type = eventresizewindow;
	e.x = w;
	e.y = h;
	pushEvent(e);
}

void QtBackend::renderDone() {
	needsrender = false;

	// We need to copy the contents of the offscreen buffer into the window.
	SDL_Surface *surf = getMainSDLSurface();
	assert(SDL_LockSurface(surf) == 0);

	// Qt reads the alpha channel even though we tell it not to, and SDL seems
	// to write an alpha channel even when we don't use it. So, make sure it's
	// set to fully opaque before copying to the Qt window.
	for (int row = 0; row < surf->h; row++) {
		for (int col = 0; col < surf->w; col++) {
			((uint8_t*)surf->pixels)[surf->pitch * row + col * 4 + 3] = 255;
		}
	}

	QImage img((uchar *)surf->pixels, surf->w, surf->h, QImage::Format_RGB32);
	QPainter painter(viewport);
	painter.drawImage(0, 0, img);

	SDL_UnlockSurface(surf);
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

int translateQtKey(int qtkey) {
	if (qtkey >= Qt::Key_F1 && qtkey <= Qt::Key_F12) {
		return 112 + (qtkey - Qt::Key_F1);
	}

	switch (qtkey) {
		case Qt::Key_Backspace: return 8;
		case Qt::Key_Tab: return 9;
		case Qt::Key_Clear: return 12;
		case Qt::Key_Return: return 13;
		case Qt::Key_Enter: return 13;
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

void QtBackend::keyEvent(QKeyEvent *k, bool pressed) {
	int translatedkey = translateQtKey(k->key());
	int key = translatedkey;
	if (key == -1) {
		if (k->key() >= Qt::Key_0 && k->key() <= Qt::Key_9) {
			key = k->key();
		} else if (k->key() >= Qt::Key_A && k->key() <= Qt::Key_Z) {
			key = k->key();
		}
	}
	if (key != -1) {
		SomeEvent e;
		if (pressed)
			e.type = eventspecialkeydown;
		else
			e.type = eventspecialkeyup;
		e.key = key;
		pushEvent(e);
		downkeys[key] = pressed;
		if (translatedkey != -1) return;
	}

	for (int i = 0; i < k->text().size(); i++) {
		// TODO: openc2e probably doesn't like latin1
		unsigned char x = k->text().at(i).toLatin1();
		if (x > 31) { // Qt helpfully hands us non-text chars for some crazy reason
			// We have a Latin-1 key which we can process.
			SomeEvent e;
			
			// the engine only handles eventkeydown at present
			if (pressed) {
				e.type = eventkeydown;
				e.key = x;
				pushEvent(e);
			}
		}
	}
}

bool QtBackend::keyDown(int key) {
	return downkeys[key];
}

int QtBackend::run(int argc, char **argv) {
	QApplication app(argc, argv);
	std::shared_ptr<QtBackend> qtbackend = std::dynamic_pointer_cast<class QtBackend, class Backend>(engine.backend);
	assert(qtbackend.get() == this);

	QtOpenc2e myvat(qtbackend);
	myvat.show();

	return app.exec();
}
