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

QtBackend::QtBackend() {
	for (unsigned int i = 0; i < 256; i++) {
		downkeys[i] = false;
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

