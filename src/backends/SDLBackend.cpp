/*
 *  SDLBackend.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Oct 24 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include <array>
#include <cassert>
#include <memory>

#include "fileformats/creaturesImage.h"
#include "keycodes.h"
#include "Engine.h"
#include "openc2e.h"
#include "SDLBackend.h"
#include "World.h"

#if defined(SDL_VIDEO_DRIVER_X11)
// Workaround for https://bugzilla.libsdl.org/show_bug.cgi?id=5289
#include "SDL_config.h"
#undef SDL_VIDEO_DRIVER_DIRECTFB // pulls in directfb.h otherwise
#include "SDL_syswm.h"
#include <X11/Xlib.h>
#endif

SDLBackend *g_backend;

// reasonable defaults
constexpr int OPENC2E_DEFAULT_WIDTH = 800;
constexpr int OPENC2E_DEFAULT_HEIGHT = 600;

SDLBackend::SDLBackend() : mainrendertarget(this) {
	networkingup = false;

	mainrendertarget.texture = nullptr;
}

int SDLBackend::idealBpp() {
	// shadow surfaces seem to generally be faster (presumably due to overdraw), so get SDL to create one for us
	if (engine.version == 1) return 0;
	else return 16;
}

void SDLBackend::resizeNotify(int _w, int _h) {
	windowwidth = _w;
	windowheight = _h;

	SDL_GetRendererOutputSize(renderer, &mainrendertarget.drawablewidth, &mainrendertarget.drawableheight);
	assert(mainrendertarget.drawablewidth / windowwidth == mainrendertarget.drawableheight / windowheight);
	float oldscale = mainrendertarget.scale;
	float newscale = mainrendertarget.drawablewidth / windowwidth * userscale;
	if (abs(newscale) > 0.01 && abs(oldscale - newscale) > 0.01) {
		printf("* SDL setting scale to %.2fx\n", newscale);	
		mainrendertarget.scale = newscale;
		SDL_RenderSetScale(renderer, mainrendertarget.scale, mainrendertarget.scale);
	}
}

void SDLBackend::setUserScale(float userscale_) {
	userscale = userscale_;
	resizeNotify(windowwidth, windowheight);
}

void SDLBackend::init() {
	int init = SDL_INIT_VIDEO;

	if (SDL_Init(init) < 0)
		throw creaturesException(std::string("SDL error during initialization: ") + SDL_GetError());

	std::string windowtitle;
	if (engine.getGameName().size()) windowtitle = engine.getGameName() + " - ";
	windowtitle += "openc2e";
	std::string titlebar = windowtitle + " (development build)";

	window = SDL_CreateWindow(titlebar.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		OPENC2E_DEFAULT_WIDTH, OPENC2E_DEFAULT_HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);
	assert(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	assert(renderer);

	{
		SDL_RendererInfo info;
		info.name = nullptr;
		SDL_GetRendererInfo(renderer, &info);
		printf("* SDL Renderer: %s\n", info.name);
	}

	SDL_GetWindowSize(window, &windowwidth, &windowheight);
	resizeNotify(windowwidth, windowheight);

	SDL_ShowCursor(false);
	SDL_StartTextInput();
}

void SDLBackend::initFrom(void *window_id) {
	int init = SDL_INIT_VIDEO;

	if (SDL_Init(init) < 0)
		throw creaturesException(std::string("SDL error during initialization: ") + SDL_GetError());

	window = SDL_CreateWindowFrom(window_id);
	assert(window);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	assert(renderer);

	{
		SDL_RendererInfo info;
		info.name = nullptr;
		SDL_GetRendererInfo(renderer, &info);
		printf("* SDL Renderer: %s\n", info.name);
	}

#if defined(SDL_VIDEO_DRIVER_X11)
	// Workaround for https://bugzilla.libsdl.org/show_bug.cgi?id=5289
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);
	if (info.subsystem == SDL_SYSWM_X11) {
		XSelectInput(info.info.x11.display, info.info.x11.window,
		             (FocusChangeMask | EnterWindowMask | LeaveWindowMask |
		              ExposureMask | ButtonReleaseMask | PointerMotionMask |
		              KeyPressMask | KeyReleaseMask | PropertyChangeMask |
		              StructureNotifyMask | KeymapStateMask));
	}
#endif
	SDL_ShowCursor(false);
	SDL_StartTextInput();
}

int SDLBackend::networkInit() {
	if (SDLNet_Init() < 0)
		throw creaturesException(std::string("SDL_net error during initialization: ") + SDLNet_GetError());
	networkingup = true;

	listensocket = 0;
	int listenport = 20000;
	while ((!listensocket) && (listenport < 20050)) {
		listenport++;
		IPaddress ip;

		SDLNet_ResolveHost(&ip, 0, listenport);
		listensocket = SDLNet_TCP_Open(&ip);
	}
	
	if (!listensocket)
		throw creaturesException(std::string("Failed to open a port to listen on."));

	return listenport;
}

void SDLBackend::shutdown() {
	if (networkingup && listensocket)
		SDLNet_TCP_Close(listensocket);
	SDLNet_Quit();
	SDL_Quit();
}

void SDLBackend::handleEvents() {
	if (networkingup)
		handleNetworking();
}

void SDLBackend::handleNetworking() {
	// handle incoming network connections
	while (TCPsocket connection = SDLNet_TCP_Accept(listensocket)) {
		// check this connection is coming from localhost
		IPaddress *remote_ip = SDLNet_TCP_GetPeerAddress(connection);
		unsigned char *rip = (unsigned char *)&remote_ip->host;
		if ((rip[0] != 127) || (rip[1] != 0) || (rip[2] != 0) || (rip[3] != 1)) {
			std::cout << "Someone tried connecting via non-localhost address! IP: " << (int)rip[0] << "." << (int)rip[1] << "." << (int)rip[2] << "." << (int)rip[3] << std::endl;
			SDLNet_TCP_Close(connection);
			continue;
		}
			
		// read the data from the socket
		std::string data;
		bool done = false;
		while (!done) {
			char buffer;
			int i = SDLNet_TCP_Recv(connection, &buffer, 1);
			if (i == 1) {
				data = data + buffer;
				// TODO: maybe we should check for rscr\n like c2e seems to
				if ((data.size() > 3) && (data.find("rscr\n", data.size() - 5) != data.npos)) done = true;
			} else done = true;
		}

		// pass the data onto the engine, and send back our response
		std::string tosend = engine.executeNetwork(data);
		SDLNet_TCP_Send(connection, (void *)tosend.c_str(), tosend.size());
		
		// and finally, close the connection
		SDLNet_TCP_Close(connection);
	}
}

bool SDLBackend::pollEvent(BackendEvent &e) {
	SDL_Event event;
retry:
	if (!SDL_PollEvent(&event)) return false;

	switch (event.type) {
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					resizeNotify(event.window.data1, event.window.data2);
					e.type = eventresizewindow;
					e.x = event.window.data1;
					e.y = event.window.data2;
					return true;
				default:
					goto retry;
			}

		case SDL_MOUSEMOTION:
			e.type = eventmousemove;
			e.x = event.motion.x / userscale;
			e.y = event.motion.y / userscale;
			e.xrel = event.motion.xrel / userscale;
			e.yrel = event.motion.yrel / userscale;
			e.button = 0;
			if (event.motion.state & SDL_BUTTON(1))
				e.button |= buttonleft;
			if (event.motion.state & SDL_BUTTON(2))
				e.button |= buttonmiddle;
			if (event.motion.state & SDL_BUTTON(3))
				e.button |= buttonright;
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (event.type == SDL_MOUSEBUTTONDOWN)
				e.type = eventmousebuttondown;
			else
				e.type = eventmousebuttonup;
			switch (event.button.button) {
				case SDL_BUTTON_LEFT: e.button = buttonleft; break;
				case SDL_BUTTON_RIGHT: e.button = buttonright; break;
				case SDL_BUTTON_MIDDLE: e.button = buttonmiddle; break;
				default: goto retry;
			}
			e.x = event.button.x / userscale;
			e.y = event.button.y / userscale;
			break;

		case SDL_MOUSEWHEEL:
			e.type = eventmousebuttondown;
			if (event.wheel.y > 0) {
				e.button = buttonwheeldown; break;
			} else if (event.wheel.y < 0) {
				e.button = buttonwheelup; break;
			} else {
				goto retry;
			}
			break;

		case SDL_TEXTINPUT:
			e.type = eventtextinput;
			e.text = event.text.text;
			break;

        case SDL_KEYUP:
            {
                int key = translateScancode(event.key.keysym.scancode);
                if (key != -1) {
                    e.type = eventrawkeyup;
                    e.key = key;
                    return true;
                }
                goto retry;
            }

		case SDL_KEYDOWN:
			{
				int key = translateScancode(event.key.keysym.scancode);
				if (key != -1) {
					e.type = eventrawkeydown;
					e.key = key;
					return true;
				}
				goto retry;
			}

		case SDL_QUIT:
			e.type = eventquit;
			break;

		default:
			goto retry;
	}
	
	return true;
}

void SDLRenderTarget::renderLine(int x1, int y1, int x2, int y2, unsigned int color) {
	Uint8 r = (color >> 24) & 0xff;
	Uint8 g = (color >> 16) & 0xff;
	Uint8 b = (color >> 8)  & 0xff;
	Uint8 a = (color >> 0)  & 0xff;
	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_SetRenderDrawColor(parent->renderer, r, g, b, a);
	SDL_RenderDrawLine(parent->renderer, x1, y1, x2, y2);
}

SDL_Texture* SDLBackend::createTexture(void *data, unsigned int width, unsigned int height, imageformat format, bool black_is_transparent, uint8_t* custom_palette) {
	assert(data);
	assert(width > 0);
	assert(height > 0);
	if (format != if_paletted) {
		assert(custom_palette == nullptr);
	}	
	
	// create surface
	Uint32 sdlformat = SDL_PIXELFORMAT_UNKNOWN;
	switch (format) {
		case if_paletted:
			sdlformat = SDL_PIXELFORMAT_INDEX8;
			break;
		case if_16bit_555:
			sdlformat = SDL_PIXELFORMAT_RGB565;
			break;
		case if_16bit_565:
			sdlformat = SDL_PIXELFORMAT_RGB565;
			break;
		case if_24bit:
			sdlformat = SDL_PIXELFORMAT_RGB888;
			break;
	}
	
	SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormatFrom(
		data,
		width,
		height,
		SDL_BITSPERPIXEL(sdlformat), // depth
		width * SDL_BYTESPERPIXEL(sdlformat), // pitch
		sdlformat
	);
	assert(surf);

	if (format == if_paletted) {
		SDL_SetPaletteColors(
			surf->format->palette,
			// TODO: custom_palette not guaranteed to fit SDL_Color ?
			custom_palette ? (SDL_Color *)custom_palette : palette,
			0,
			256);
	}

	// set colour-keying
	if (black_is_transparent) {
		SDL_SetColorKey(surf, SDL_TRUE, 0);
	}
	
	// create texture
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
	assert(tex);
	SDL_FreeSurface(surf);
	return tex;
}

class SDLTextureAtlas : public TextureAtlas {
public:
	~SDLTextureAtlas() {
		for (auto* t : textures) {
			SDL_DestroyTexture(t);
		}
	}
	std::vector<SDL_Texture*> textures;
};

TextureAtlasHandle SDLBackend::createTextureAtlasFromCreaturesImage(const std::shared_ptr<creaturesImage>& image) {
	assert(image);
	
	auto atlas = std::make_shared<SDLTextureAtlas>();
	atlas->textures.resize(image->numframes());
	for (size_t i = 0; i < image->numframes(); ++i) {
		atlas->textures[i] = createTexture(
			image->data(i),
			image->width(i),
			image->height(i),
			image->format(),
			true,
			image->hasCustomPalette() ? image->getCustomPalette() : nullptr
		);
	}
	return std::dynamic_pointer_cast<TextureAtlas>(atlas);
}

unsigned int SDLRenderTarget::getWidth() const {
	return drawablewidth / scale;
}
unsigned int SDLRenderTarget::getHeight() const {
	return drawableheight / scale;
}

void SDLRenderTarget::renderTexture(const TextureAtlasHandle& atlas, size_t i, int x, int y, uint8_t transparency, bool mirror) {
	auto sdl_atlas = std::dynamic_pointer_cast<SDLTextureAtlas>(atlas);
	assert(i < sdl_atlas->textures.size());
	
	SDL_Texture *tex = sdl_atlas->textures[i];
	
	// do actual render
	SDL_SetTextureAlphaMod(tex, 255 - transparency);
	SDL_RendererFlip flip = mirror ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	
	SDL_Rect destrect;
	destrect.x = x;
	destrect.y = y;
	SDL_QueryTexture(tex, nullptr, nullptr, &destrect.w, &destrect.h);
	
	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_RenderCopyEx(parent->renderer, tex, nullptr, &destrect, 0, nullptr, flip);
}

void SDLRenderTarget::renderClear() {
	SDL_SetRenderDrawColor(parent->renderer, 0, 0, 0, 255);
	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_RenderClear(parent->renderer);
}

void SDLRenderTarget::renderDone() {
	if (this == &parent->mainrendertarget) {
		SDL_SetRenderTarget(parent->renderer, texture);
		SDL_RenderPresent(parent->renderer);
	}
}

void SDLRenderTarget::blitRenderTarget(RenderTarget *s, int x, int y, int w, int h) {
	SDLRenderTarget *src = dynamic_cast<SDLRenderTarget *>(s);
	assert(src);

	SDL_Rect r; r.x = x; r.y = y; r.w = w; r.h = h;
	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_RenderCopy(parent->renderer, src->texture, nullptr, &r);
}

RenderTarget *SDLBackend::newRenderTarget(unsigned int w, unsigned int h) {
	SDL_Texture *texture = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, w, h);
	assert(texture);

	SDLRenderTarget *newtarget = new SDLRenderTarget(this);
	newtarget->texture = texture;
	newtarget->drawablewidth = w;
	newtarget->drawableheight = h;
	return newtarget;
}

void SDLBackend::freeRenderTarget(RenderTarget *s) {
	SDLRenderTarget *target = dynamic_cast<SDLRenderTarget *>(s);
	assert(target);

	SDL_DestroyTexture(target->texture);
	delete target;
}

// left out: menu, select, execute, snapshot, numeric keypad
struct _keytrans { int sdl, openc2e; };
static const std::array<_keytrans, 72> keytrans = {{
	{ SDL_SCANCODE_BACKSPACE, OPENC2E_KEY_BACKSPACE },
	{ SDL_SCANCODE_TAB, OPENC2E_KEY_TAB },
	{ SDL_SCANCODE_CLEAR, OPENC2E_KEY_CLEAR },
	{ SDL_SCANCODE_RETURN, OPENC2E_KEY_RETURN },
	{ SDL_SCANCODE_RSHIFT, OPENC2E_KEY_SHIFT },
	{ SDL_SCANCODE_LSHIFT, OPENC2E_KEY_SHIFT },
	{ SDL_SCANCODE_RCTRL, OPENC2E_KEY_CONTROL },
	{ SDL_SCANCODE_LCTRL, OPENC2E_KEY_CONTROL },
	{ SDL_SCANCODE_PAUSE, OPENC2E_KEY_PAUSE },
	{ SDL_SCANCODE_CAPSLOCK, OPENC2E_KEY_CAPSLOCK },
	{ SDL_SCANCODE_ESCAPE, OPENC2E_KEY_ESCAPE },
	{ SDL_SCANCODE_SPACE, OPENC2E_KEY_SPACE },
	{ SDL_SCANCODE_PAGEUP, OPENC2E_KEY_PAGEUP },
	{ SDL_SCANCODE_PAGEDOWN, OPENC2E_KEY_PAGEDOWN },
	{ SDL_SCANCODE_END, OPENC2E_KEY_END },
	{ SDL_SCANCODE_HOME, OPENC2E_KEY_HOME },
	{ SDL_SCANCODE_LEFT, OPENC2E_KEY_LEFT },
	{ SDL_SCANCODE_UP, OPENC2E_KEY_UP },
	{ SDL_SCANCODE_RIGHT, OPENC2E_KEY_RIGHT },
	{ SDL_SCANCODE_DOWN, OPENC2E_KEY_DOWN },
	{ SDL_SCANCODE_PRINTSCREEN, OPENC2E_KEY_PRINTSCREEN },
	{ SDL_SCANCODE_INSERT, OPENC2E_KEY_INSERT },
	{ SDL_SCANCODE_DELETE, OPENC2E_KEY_DELETE },
	{ SDL_SCANCODE_0, OPENC2E_KEY_0 },
	{ SDL_SCANCODE_1, OPENC2E_KEY_1 },
	{ SDL_SCANCODE_2, OPENC2E_KEY_2 },
	{ SDL_SCANCODE_3, OPENC2E_KEY_3 },
	{ SDL_SCANCODE_4, OPENC2E_KEY_4 },
	{ SDL_SCANCODE_5, OPENC2E_KEY_5 },
	{ SDL_SCANCODE_6, OPENC2E_KEY_6 },
	{ SDL_SCANCODE_7, OPENC2E_KEY_7 },
	{ SDL_SCANCODE_8, OPENC2E_KEY_8 },
	{ SDL_SCANCODE_9, OPENC2E_KEY_9 },
	{ SDL_SCANCODE_A, OPENC2E_KEY_A },
	{ SDL_SCANCODE_B, OPENC2E_KEY_B },
	{ SDL_SCANCODE_C, OPENC2E_KEY_C },
	{ SDL_SCANCODE_D, OPENC2E_KEY_D },
	{ SDL_SCANCODE_E, OPENC2E_KEY_E },
	{ SDL_SCANCODE_F, OPENC2E_KEY_F },
	{ SDL_SCANCODE_G, OPENC2E_KEY_G },
	{ SDL_SCANCODE_H, OPENC2E_KEY_H },
	{ SDL_SCANCODE_I, OPENC2E_KEY_I },
	{ SDL_SCANCODE_J, OPENC2E_KEY_J },
	{ SDL_SCANCODE_K, OPENC2E_KEY_K },
	{ SDL_SCANCODE_L, OPENC2E_KEY_L },
	{ SDL_SCANCODE_M, OPENC2E_KEY_M },
	{ SDL_SCANCODE_N, OPENC2E_KEY_N },
	{ SDL_SCANCODE_O, OPENC2E_KEY_O },
	{ SDL_SCANCODE_P, OPENC2E_KEY_P },
	{ SDL_SCANCODE_Q, OPENC2E_KEY_Q },
	{ SDL_SCANCODE_R, OPENC2E_KEY_R },
	{ SDL_SCANCODE_S, OPENC2E_KEY_S },
	{ SDL_SCANCODE_T, OPENC2E_KEY_T },
	{ SDL_SCANCODE_U, OPENC2E_KEY_U },
	{ SDL_SCANCODE_V, OPENC2E_KEY_V },
	{ SDL_SCANCODE_W, OPENC2E_KEY_W },
	{ SDL_SCANCODE_X, OPENC2E_KEY_X },
	{ SDL_SCANCODE_Y, OPENC2E_KEY_Y },
	{ SDL_SCANCODE_Z, OPENC2E_KEY_Z },
	{ SDL_SCANCODE_F1, OPENC2E_KEY_F1 },
	{ SDL_SCANCODE_F2, OPENC2E_KEY_F2 },
	{ SDL_SCANCODE_F3, OPENC2E_KEY_F3 },
	{ SDL_SCANCODE_F4, OPENC2E_KEY_F4 },
	{ SDL_SCANCODE_F5, OPENC2E_KEY_F5 },
	{ SDL_SCANCODE_F6, OPENC2E_KEY_F6 },
	{ SDL_SCANCODE_F7, OPENC2E_KEY_F7 },
	{ SDL_SCANCODE_F8, OPENC2E_KEY_F8 },
	{ SDL_SCANCODE_F9, OPENC2E_KEY_F9 },
	{ SDL_SCANCODE_F10, OPENC2E_KEY_F10 },
	{ SDL_SCANCODE_F11, OPENC2E_KEY_F11 },
	{ SDL_SCANCODE_F12, OPENC2E_KEY_F12 },
	{ SDL_SCANCODE_NUMLOCKCLEAR, OPENC2E_KEY_NUMLOCK },
}};

int SDLBackend::translateScancode(int key) {
	for (unsigned int i = 0; i < keytrans.size(); i++) {
		if (keytrans[i].sdl == key)
			return keytrans[i].openc2e;
	}

	return -1;
}

// TODO: this is possibly not a great idea, we should maybe maintain our own state table
bool SDLBackend::keyDown(int key) {
	const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
	
	for (unsigned int i = 0; i < keytrans.size(); i++) {
		if (keytrans[i].openc2e == key)
			if (keystate[keytrans[i].sdl])
				return true;
	}

	return false;
}

void SDLBackend::setPalette(uint8_t *data) {
	for (unsigned int i = 0; i < 256; i++) {
		palette[i].r = data[i * 3];
		palette[i].g = data[(i * 3) + 1];
		palette[i].b = data[(i * 3) + 2];
		palette[i].a = 255;
	}
}

void SDLBackend::delay(int msec) {
	SDL_Delay(msec);
}

int SDLBackend::run() {
	resize(800, 600);
	
	// do a first-pass draw of the world. TODO: correct?
	world.drawWorld();

	const int OPENC2E_MAX_FPS = 60;

	while (!engine.done) {

		Uint32 frame_start = SDL_GetTicks();

		engine.tick();
		world.drawWorld();

		bool focused = SDL_GetWindowFlags(window) & (SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
		Uint32 desired_ticks_per_frame = focused ? 1000 / OPENC2E_MAX_FPS : world.ticktime;
		Uint32 frame_end = SDL_GetTicks();
		if (frame_end - frame_start < desired_ticks_per_frame) {
			SDL_Delay(desired_ticks_per_frame - (frame_end - frame_start));
		}
	}

	return 0;
}
