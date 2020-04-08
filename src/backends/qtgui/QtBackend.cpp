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
#include <boost/format.hpp>
#include <iostream>
#include "exceptions.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
HBITMAP screen_bmp = 0;
void *oldPixels = 0;
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

#ifdef _WIN32
	if (screen_bmp) DeleteObject(screen_bmp);
	
	SDL_Surface *surf = getMainSDLSurface();
	if (surf) surf->pixels = oldPixels;
#endif

	SDLBackend::shutdown();
}

void QtBackend::init() {
}

void QtBackend::setup(QWidget *vp) {
	viewport = vp;

#if defined(Q_WS_X11)
	// on X11, using SDL_WINDOWID works, thank goodness.
	static char windowid_str[64]; // possibly big, but who cares
	((QApplication *)QApplication::instance())->syncX();

	if (sizeof windowid_str <=
			(size_t)snprintf(windowid_str, sizeof windowid_str, "SDL_WINDOWID=0x%lx", viewport->winId())) {
		std::cerr << "windowid_str buffer was too small - how big are your longs, anyway? Panicing..." << std::endl;
		abort();
	}
	// ->winId() may have created the window for us
	((QApplication *)QApplication::instance())->syncX();
	putenv(windowid_str);
#else
	// alas, it sucks on Windows and OS X, so we use an offscreen buffer instead
	putenv("SDL_VIDEODRIVER=dummy");
#endif
	
	SDLBackend::init();

	/*
	char videodrivername[200];
	std::cout << "SDL video driver: " << SDL_VideoDriverName(videodrivername, 200) << std::endl;
	std::cout << "SDL requested colour depth: " << idealBpp() << std::endl;
	*/

	viewport->setCursor(Qt::BlankCursor);
}

int QtBackend::idealBpp() {
#if defined(Q_WS_X11)
	return SDLBackend::idealBpp();
#endif

	// TODO: handle 8bpp for C1

#ifdef _WIN32
	// TODO: how to pick up real depth on windows?
	if (viewport->depth() == 16) return 16;
	return 24;
#endif

	return 32;
}


void QtBackend::resized(int w, int h) {
	resizeNotify(w, h);

#ifdef _WIN32
	// We need to construct a DIB for blitting, based on the surface data.
	
	// Delete the old one if needed.
	if (screen_bmp) DeleteObject(screen_bmp);

	// Setup a BITMAPINFO structure for this.
	BITMAPINFO *binfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + 12); // TODO: wrong wrong wrong
	binfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo->bmiHeader.biPlanes = 1;
	binfo->bmiHeader.biClrUsed = 0;
	binfo->bmiHeader.biClrImportant = 0;

	binfo->bmiHeader.biXPelsPerMeter = 0;
	binfo->bmiHeader.biYPelsPerMeter = 0;	

	SDL_Surface *surf = getMainSDLSurface();
	assert(idealBpp() == surf->format->BitsPerPixel);

	// Set the relevant entries of the structure.
	binfo->bmiHeader.biWidth = w;
	binfo->bmiHeader.biHeight = -h;
	binfo->bmiHeader.biSizeImage = w * h * (idealBpp() / 8);
	binfo->bmiHeader.biBitCount = idealBpp();

	// Describe the format of the data and any additional information needed (eg masks/palette)
	if (idealBpp() == 16) {
		binfo->bmiHeader.biCompression = BI_BITFIELDS;
		unsigned int *masks = (unsigned int *)binfo->bmiColors;
		masks[0] = surf->format->Rmask;
		masks[1] = surf->format->Gmask;
		masks[2] = surf->format->Bmask;
	} else {
		binfo->bmiHeader.biCompression = BI_RGB;
		if (idealBpp() == 8) {
			// TODO: set binfo.bmiColors
		}
	}

	// Create the actual DIB.
	void *pixels = 0;
	HDC hdc = GetDC(viewport->winId());
	screen_bmp = CreateDIBSection(hdc, binfo, DIB_RGB_COLORS, (void **)(&pixels), NULL, 0);
	ReleaseDC(viewport->winId(), hdc);

	// Free the BITMAPINFO structure now we're done with it.
	free(binfo);

	// TODO: fall back to Qt?
	if (!screen_bmp || !pixels) {
		// Windows helpfully provides no useful error information :(
		throw creaturesException("Internal error: failed to create DIB");
	}
	
	// TODO: Observe how this helpfully stomps over surf->pixels. :-/
	oldPixels = surf->pixels; // store so we can restore before SDL shutdown
	surf->pixels = pixels;

	// So, CreateDIBSection doesn't pay a lot of attention to what we ask for.
	// Let's snaffle the information back from the DIB object.
	// TODO: Observe how this helpfully stomps over surf->w/h/pitch. :-/
	BITMAP dibsection;
	GetObject(screen_bmp, sizeof(BITMAP), &dibsection);
	surf->w = dibsection.bmWidth;
	surf->h = dibsection.bmHeight;
	surf->pitch = dibsection.bmWidthBytes;
	assert(dibsection.bmBitsPixel == surf->format->BitsPerPixel);

#endif

	// add resize window event to backend queue
	SomeEvent e;
	e.type = eventresizewindow;
	e.x = w;
	e.y = h;
	pushEvent(e);
}

void QtBackend::renderDone() {
	needsrender = false;

	// If we're not on X11, we need to copy the contents of the offscreen buffer into the window.

#if defined(_WIN32)
	// We need to blit from the DIB we made earlier. Easy!
	HDC hdc, mdc;
	SDL_Surface *surf = getMainSDLSurface(); // for width/height

	// Obtain the DC for our viewport and create a compatible one, then select the DIB into it.
	hdc = GetDC(viewport->winId());
	mdc = CreateCompatibleDC(hdc);
	SelectObject(mdc, screen_bmp);

	// Blit!
	BitBlt(hdc, 0, 0, surf->w, surf->h, mdc, 0, 0, SRCCOPY);

	// Tidy up by deleting our temporary one and releasing the viewport DC.
	DeleteDC(mdc);
	ReleaseDC(viewport->winId(), hdc);

	// TODO: call GdiFlush? SDL doesn't seem to bother.
	
#elif !defined(Q_WS_X11)
	// As a generic method, we use Qt's code.	
	// Note that we don't bother to lock because we know the dummy driver doesn't bother with locking.

	SDL_Surface *surf = getMainSDLSurface();
	QImage img((uchar *)surf->pixels, surf->w, surf->h, QImage::Format_RGB32);
	QPainter painter(viewport);
	painter.drawImage(0, 0, img);
#endif
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
