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

#ifndef _QTBACKEND_H
#define _QTBACKEND_H

#include "backends/SDLBackend.h"
#include <QApplication>
#include <deque>

class QInputMethodEvent;
class QtOpenc2e;

class QtBackend : public SDLBackend {
public:
	QtBackend();
	void shutdown();
	void init();
	void setup(class QWidget *vp);
	void resized(int w, int h);
	bool pollEvent(BackendEvent &e);
	void pushEvent(BackendEvent e);
	bool keyDown(int key);
	void inputMethodEvent(QInputMethodEvent*);
	void keyEvent(class QKeyEvent *k, bool pressed);

	int run();

protected:
	QApplication* app;
	QtOpenc2e* myvat;
	std::deque<BackendEvent> events;
	
	bool downkeys[256]; // TODO: public data bad
	class QWidget *viewport;

	int idealBpp();
};

#endif

