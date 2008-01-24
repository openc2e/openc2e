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

#include "../SDLBackend.h"
#include <deque>

class QtBackend : public SDLBackend {
public:
	QtBackend();
	void init();
	void setup(class QWidget *vp);
	void resized(int w, int h);
	bool pollEvent(SomeEvent &e);
	void pushEvent(SomeEvent e);
	bool keyDown(int key);
	void keyEvent(class QKeyEvent *k, bool pressed);

	bool selfRender() { return true; }
	void requestRender() { needsrender = true; }	
	bool needsRender() { return needsrender; }
	void renderDone() { needsrender = false; }

protected:
	std::deque<SomeEvent> events;
	
	bool downkeys[256]; // TODO: public data bad
	class QWidget *viewport;
	bool needsrender;
};

#endif

