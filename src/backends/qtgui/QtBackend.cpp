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
#include "keycodes.h"

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
	// skip events from SDL
	while (SDLBackend::pollEvent(e)) {};

	if (events.size() == 0)
		return false;

	e = events.front();
	events.pop_front();
	return true;
}

void QtBackend::pushEvent(SomeEvent e) {
	events.push_back(e);
}

static int translateQtKey(int qtkey) {
	switch (qtkey) {
		case Qt::Key_Backspace: return OPENC2E_KEY_BACKSPACE;
		case Qt::Key_Tab: return OPENC2E_KEY_TAB;
		case Qt::Key_Clear: return OPENC2E_KEY_CLEAR;
		case Qt::Key_Return: return OPENC2E_KEY_RETURN;
		case Qt::Key_Enter: return OPENC2E_KEY_RETURN;
		case Qt::Key_Shift: return OPENC2E_KEY_SHIFT;
		case Qt::Key_Control: return OPENC2E_KEY_CONTROL;
		case Qt::Key_Pause: return OPENC2E_KEY_PAUSE;
		case Qt::Key_CapsLock: return OPENC2E_KEY_CAPSLOCK;
		case Qt::Key_Escape: return OPENC2E_KEY_ESCAPE;
		case Qt::Key_PageUp: return OPENC2E_KEY_PAGEUP;
		case Qt::Key_PageDown: return OPENC2E_KEY_PAGEDOWN;
		case Qt::Key_End: return OPENC2E_KEY_END;
		case Qt::Key_Home: return OPENC2E_KEY_HOME;
		case Qt::Key_Left: return OPENC2E_KEY_LEFT;
		case Qt::Key_Up: return OPENC2E_KEY_UP;
		case Qt::Key_Right: return OPENC2E_KEY_RIGHT;
		case Qt::Key_Down: return OPENC2E_KEY_DOWN;
		case Qt::Key_Print: return OPENC2E_KEY_PRINTSCREEN;
		case Qt::Key_Insert: return OPENC2E_KEY_INSERT;
		case Qt::Key_Delete: return OPENC2E_KEY_DELETE;
		case Qt::Key_0: return OPENC2E_KEY_0;
		case Qt::Key_1: return OPENC2E_KEY_1;
		case Qt::Key_2: return OPENC2E_KEY_2;
		case Qt::Key_3: return OPENC2E_KEY_3;
		case Qt::Key_4: return OPENC2E_KEY_4;
		case Qt::Key_5: return OPENC2E_KEY_5;
		case Qt::Key_6: return OPENC2E_KEY_6;
		case Qt::Key_7: return OPENC2E_KEY_7;
		case Qt::Key_8: return OPENC2E_KEY_8;
		case Qt::Key_9: return OPENC2E_KEY_9;
		case Qt::Key_A: return OPENC2E_KEY_A;
		case Qt::Key_B: return OPENC2E_KEY_B;
		case Qt::Key_C: return OPENC2E_KEY_C;
		case Qt::Key_D: return OPENC2E_KEY_D;
		case Qt::Key_E: return OPENC2E_KEY_E;
		case Qt::Key_F: return OPENC2E_KEY_F;
		case Qt::Key_G: return OPENC2E_KEY_G;
		case Qt::Key_H: return OPENC2E_KEY_H;
		case Qt::Key_I: return OPENC2E_KEY_I;
		case Qt::Key_J: return OPENC2E_KEY_J;
		case Qt::Key_K: return OPENC2E_KEY_K;
		case Qt::Key_L: return OPENC2E_KEY_L;
		case Qt::Key_M: return OPENC2E_KEY_M;
		case Qt::Key_N: return OPENC2E_KEY_N;
		case Qt::Key_O: return OPENC2E_KEY_O;
		case Qt::Key_P: return OPENC2E_KEY_P;
		case Qt::Key_Q: return OPENC2E_KEY_Q;
		case Qt::Key_R: return OPENC2E_KEY_R;
		case Qt::Key_S: return OPENC2E_KEY_S;
		case Qt::Key_T: return OPENC2E_KEY_T;
		case Qt::Key_U: return OPENC2E_KEY_U;
		case Qt::Key_V: return OPENC2E_KEY_V;
		case Qt::Key_W: return OPENC2E_KEY_W;
		case Qt::Key_X: return OPENC2E_KEY_X;
		case Qt::Key_Y: return OPENC2E_KEY_Y;
		case Qt::Key_Z: return OPENC2E_KEY_Z;
		case Qt::Key_F1: return OPENC2E_KEY_F1;
		case Qt::Key_F2: return OPENC2E_KEY_F2;
		case Qt::Key_F3: return OPENC2E_KEY_F3;
		case Qt::Key_F4: return OPENC2E_KEY_F4;
		case Qt::Key_F5: return OPENC2E_KEY_F5;
		case Qt::Key_F6: return OPENC2E_KEY_F6;
		case Qt::Key_F7: return OPENC2E_KEY_F7;
		case Qt::Key_F8: return OPENC2E_KEY_F8;
		case Qt::Key_F9: return OPENC2E_KEY_F9;
		case Qt::Key_F10: return OPENC2E_KEY_F10;
		case Qt::Key_F11: return OPENC2E_KEY_F11;
		case Qt::Key_F12: return OPENC2E_KEY_F12;
		case Qt::Key_NumLock: return OPENC2E_KEY_NUMLOCK;
		default: return -1;
	}
}

void QtBackend::inputMethodEvent(QInputMethodEvent *event) {
	auto utf8 = event->commitString().toUtf8();
	if (utf8.length() == 0) {
		return;
	}
	SomeEvent e;
	e.type = eventtextinput;
	e.text = std::string(utf8.data(), utf8.data() + utf8.length());
	pushEvent(e);
}

void QtBackend::keyEvent(QKeyEvent *k, bool pressed) {
	int translatedkey = translateQtKey(k->key());
	if (translatedkey != -1) {
		downkeys[translatedkey] = pressed;

		SomeEvent e;
		e.type = pressed ? eventrawkeydown : eventrawkeyup;
		e.key = translatedkey;
		pushEvent(e);
	}

	if (!pressed) {
		return;
	}
	auto utf8 = k->text().toUtf8();
	if (utf8.length() == 0) {
		return;
	}
	SomeEvent e;
	e.type = eventtextinput;
	e.text = std::string(utf8.data(), utf8.data() + utf8.length());
	pushEvent(e);
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
