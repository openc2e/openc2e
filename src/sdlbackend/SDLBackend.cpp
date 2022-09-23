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

#include "SDLBackend.h"

#include "common/Exception.h"
#include "common/backend/Keycodes.h"
#include "common/creaturesImage.h"
#include "imgui_sdl.h"

#include <array>
#include <cassert>
#include <imgui.h>
#include <memory>

// reasonable defaults
constexpr int OPENC2E_DEFAULT_WIDTH = 800;
constexpr int OPENC2E_DEFAULT_HEIGHT = 600;

SDLBackend::SDLBackend()
	: mainrendertarget(this) {
	mainrendertarget.texture = nullptr;
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

static void ImGuiInit(SDL_Window* window) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = nullptr; // don't save settings
	if (!ImGuiSDL_Init(window)) {
		throw Exception("Couldn't initialize ImGui - are you using SDL's OpenGL backend?");
	}
}

void SDLBackend::init(const std::string& name) {
	int init = SDL_INIT_VIDEO;

	if (SDL_Init(init) < 0)
		throw Exception(std::string("SDL error during initialization: ") + SDL_GetError());

	window = SDL_CreateWindow(name.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		OPENC2E_DEFAULT_WIDTH, OPENC2E_DEFAULT_HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window) {
		throw Exception(std::string("SDL error creating window: ") + SDL_GetError());
	}

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // for imgui_sdl
	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		throw Exception(std::string("SDL error creating renderer: ") + SDL_GetError());
	}

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

	ImGuiInit(window);
}

void SDLBackend::shutdown() {
	SDL_Quit();
}

static bool ImGuiConsumeEvent(const SDL_Event& event) {
	ImGuiSDL_ProcessEvent(&event);
	if (ImGui::GetIO().WantCaptureMouse && (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL)) {
		return true;
	}
	if (ImGui::GetIO().WantCaptureKeyboard && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_TEXTINPUT)) {
		return true;
	}

	return false;
}

bool SDLBackend::pollEvent(BackendEvent& e) {
	SDL_Event event;
retry:
	if (!SDL_PollEvent(&event))
		return false;

	if (ImGuiConsumeEvent(event)) {
		goto retry;
	}

	switch (event.type) {
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					resizeNotify(event.window.data1, event.window.data2);
					e.window_id = event.window.windowID;
					e.type = eventresizewindow;
					e.x = event.window.data1;
					e.y = event.window.data2;
					return true;
				default:
					goto retry;
			}

		case SDL_MOUSEMOTION:
			e.window_id = event.motion.windowID;
			e.type = eventmousemove;
			e.x = event.motion.x / userscale;
			e.y = event.motion.y / userscale - mainrendertarget.viewport_offset_top;
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
			e.window_id = event.button.windowID;
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
			e.y = event.button.y / userscale - mainrendertarget.viewport_offset_top;
			break;

		case SDL_MOUSEWHEEL:
			e.window_id = event.wheel.windowID;
			e.type = eventmousebuttondown;
			if (event.wheel.y > 0) {
				e.button = buttonwheeldown;
				break;
			} else if (event.wheel.y < 0) {
				e.button = buttonwheelup;
				break;
			} else {
				goto retry;
			}
			break;

		case SDL_TEXTINPUT:
			e.window_id = event.text.windowID;
			e.type = eventtextinput;
			e.text = event.text.text;
			break;

		case SDL_KEYUP: {
			e.window_id = event.key.windowID;
			int key = translateScancode(event.key.keysym.scancode);
			if (key != -1) {
				e.type = eventrawkeyup;
				e.key = key;
				return true;
			}
			goto retry;
		}

		case SDL_KEYDOWN: {
			e.window_id = event.key.windowID;
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
	Uint8 b = (color >> 8) & 0xff;
	Uint8 a = (color >> 0) & 0xff;
	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_SetRenderDrawColor(parent->renderer, r, g, b, a);
	SDL_RenderDrawLine(parent->renderer, x1, y1 + viewport_offset_top, x2, y2 + viewport_offset_top);
}

Texture SDLBackend::createTexture(const Image& image) {
	return createTextureWithTransparentColor(image, Color{});
}

Texture SDLBackend::createTextureWithTransparentColor(const Image& image, Color transparent_color) {
	assert(image.data);
	assert(image.width > 0);
	assert(image.height > 0);
	if (image.format != if_index8) {
		assert(image.palette.data() == nullptr);
	}

	// create surface
	Uint32 sdlformat = SDL_PIXELFORMAT_UNKNOWN;
	switch (image.format) {
		case if_index8:
			sdlformat = SDL_PIXELFORMAT_INDEX8;
			break;
		case if_rgb555:
			sdlformat = SDL_PIXELFORMAT_RGB555;
			break;
		case if_rgb565:
			sdlformat = SDL_PIXELFORMAT_RGB565;
			break;
		case if_bgr24:
			sdlformat = SDL_PIXELFORMAT_BGR24;
			break;
		case if_rgb24:
			sdlformat = SDL_PIXELFORMAT_RGB24;
			break;
	}

	SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormatFrom(
		const_cast<uint8_t*>(image.data.data()),
		image.width,
		image.height,
		SDL_BITSPERPIXEL(sdlformat), // depth
		image.width * SDL_BYTESPERPIXEL(sdlformat), // pitch
		sdlformat);
	assert(surf);

	if (image.format == if_index8) {
		if (image.palette.data()) {
			shared_array<SDL_Color> palette(image.palette.size());
			for (size_t i = 0; i < image.palette.size(); ++i) {
				palette[i].r = image.palette[i].r;
				palette[i].g = image.palette[i].g;
				palette[i].b = image.palette[i].b;
				palette[i].a = image.palette[i].a;
			}
			SDL_SetPaletteColors(surf->format->palette, palette.data(), 0, palette.size());
		} else {
			SDL_SetPaletteColors(surf->format->palette, default_palette.data(), 0, default_palette.size());
		}
	}

	// set colour-keying
	if (transparent_color.a > 0) {
		if (transparent_color.a != 255) {
			throw Exception("Expected alpha value of transparent color to be 255");
		}
		Uint32 sdlcolorkey = SDL_MapRGB(
			surf->format,
			transparent_color.r,
			transparent_color.g,
			transparent_color.b);
		SDL_SetColorKey(surf, SDL_TRUE, sdlcolorkey);
	}

	// create texture
	Texture tex(
		SDL_CreateTextureFromSurface(renderer, surf),
		image.width,
		image.height,
		&SDL_DestroyTexture);
	assert(tex);
	return tex;
}

unsigned int SDLRenderTarget::getWidth() const {
	return drawablewidth / scale;
}
unsigned int SDLRenderTarget::getHeight() const {
	return drawableheight / scale - viewport_offset_top - viewport_offset_bottom;
}

void SDLRenderTarget::setViewportOffsetTop(int offset_top) {
	viewport_offset_top = offset_top;
}

void SDLRenderTarget::setViewportOffsetBottom(int offset_bottom) {
	viewport_offset_bottom = offset_bottom;
}

void SDLRenderTarget::renderCreaturesImage(creaturesImage& img, unsigned int frame, int x, int y, RenderOptions options) {
	if (x + (int)img.width(frame) <= 0 || x >= (int)getWidth() || y + (int)img.height(frame) <= 0 || y >= (int)getHeight()) {
		return;
	}

	if (!img.getTextureForFrame(frame)) {
		img.getTextureForFrame(frame) = parent->createTextureWithTransparentColor(img.getImageForFrame(frame), Color{0, 0, 0, 0xff});
	}

	SDL_Texture* tex = img.getTextureForFrame(frame).as<SDL_Texture>();
	assert(tex);

	SDL_SetTextureAlphaMod(tex, options.alpha);
	SDL_RendererFlip flip = options.mirror ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	SDL_Rect srcrect;
	srcrect.x = img.getXOffsetForFrame(frame);
	srcrect.y = img.getYOffsetForFrame(frame);
	srcrect.w = img.width(frame);
	srcrect.h = img.height(frame);

	SDL_Rect destrect;
	destrect.x = x;
	destrect.y = y + viewport_offset_top;
	destrect.w = (options.override_drawsize ? options.overridden_drawwidth : srcrect.w) * options.scale;
	destrect.h = (options.override_drawsize ? options.overridden_drawheight : srcrect.h) * options.scale;

	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_RenderCopyEx(parent->renderer, tex, &srcrect, &destrect, 0, nullptr, flip);
}

void SDLRenderTarget::renderClear() {
	SDL_SetRenderDrawColor(parent->renderer, 0, 0, 0, 255);
	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_RenderClear(parent->renderer);
}

void SDLRenderTarget::renderDone() {
	if (this != &parent->mainrendertarget) {
		return;
	}
}

void SDLRenderTarget::blitRenderTarget(RenderTarget* s, int x, int y, int w, int h) {
	SDLRenderTarget* src = dynamic_cast<SDLRenderTarget*>(s);
	assert(src);

	SDL_Rect r;
	r.x = x;
	r.y = y + viewport_offset_top;
	r.w = w;
	r.h = h;
	SDL_SetRenderTarget(parent->renderer, texture);
	SDL_RenderCopy(parent->renderer, src->texture, nullptr, &r);
}

RenderTarget* SDLBackend::newRenderTarget(unsigned int w, unsigned int h) {
	SDL_Texture* texture = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, w, h);
	assert(texture);

	SDLRenderTarget* newtarget = new SDLRenderTarget(this);
	newtarget->texture = texture;
	newtarget->drawablewidth = w;
	newtarget->drawableheight = h;
	return newtarget;
}

void SDLBackend::freeRenderTarget(RenderTarget* s) {
	SDLRenderTarget* target = dynamic_cast<SDLRenderTarget*>(s);
	assert(target);

	SDL_DestroyTexture(target->texture);
	delete target;
}

// left out: menu, select, execute, snapshot, numeric keypad
struct _keytrans {
	int sdl, openc2e;
};
static const std::array<_keytrans, 72> keytrans = {{
	{SDL_SCANCODE_BACKSPACE, OPENC2E_KEY_BACKSPACE},
	{SDL_SCANCODE_TAB, OPENC2E_KEY_TAB},
	{SDL_SCANCODE_CLEAR, OPENC2E_KEY_CLEAR},
	{SDL_SCANCODE_RETURN, OPENC2E_KEY_RETURN},
	{SDL_SCANCODE_RSHIFT, OPENC2E_KEY_SHIFT},
	{SDL_SCANCODE_LSHIFT, OPENC2E_KEY_SHIFT},
	{SDL_SCANCODE_RCTRL, OPENC2E_KEY_CONTROL},
	{SDL_SCANCODE_LCTRL, OPENC2E_KEY_CONTROL},
	{SDL_SCANCODE_PAUSE, OPENC2E_KEY_PAUSE},
	{SDL_SCANCODE_CAPSLOCK, OPENC2E_KEY_CAPSLOCK},
	{SDL_SCANCODE_ESCAPE, OPENC2E_KEY_ESCAPE},
	{SDL_SCANCODE_SPACE, OPENC2E_KEY_SPACE},
	{SDL_SCANCODE_PAGEUP, OPENC2E_KEY_PAGEUP},
	{SDL_SCANCODE_PAGEDOWN, OPENC2E_KEY_PAGEDOWN},
	{SDL_SCANCODE_END, OPENC2E_KEY_END},
	{SDL_SCANCODE_HOME, OPENC2E_KEY_HOME},
	{SDL_SCANCODE_LEFT, OPENC2E_KEY_LEFT},
	{SDL_SCANCODE_UP, OPENC2E_KEY_UP},
	{SDL_SCANCODE_RIGHT, OPENC2E_KEY_RIGHT},
	{SDL_SCANCODE_DOWN, OPENC2E_KEY_DOWN},
	{SDL_SCANCODE_PRINTSCREEN, OPENC2E_KEY_PRINTSCREEN},
	{SDL_SCANCODE_INSERT, OPENC2E_KEY_INSERT},
	{SDL_SCANCODE_DELETE, OPENC2E_KEY_DELETE},
	{SDL_SCANCODE_0, OPENC2E_KEY_0},
	{SDL_SCANCODE_1, OPENC2E_KEY_1},
	{SDL_SCANCODE_2, OPENC2E_KEY_2},
	{SDL_SCANCODE_3, OPENC2E_KEY_3},
	{SDL_SCANCODE_4, OPENC2E_KEY_4},
	{SDL_SCANCODE_5, OPENC2E_KEY_5},
	{SDL_SCANCODE_6, OPENC2E_KEY_6},
	{SDL_SCANCODE_7, OPENC2E_KEY_7},
	{SDL_SCANCODE_8, OPENC2E_KEY_8},
	{SDL_SCANCODE_9, OPENC2E_KEY_9},
	{SDL_SCANCODE_A, OPENC2E_KEY_A},
	{SDL_SCANCODE_B, OPENC2E_KEY_B},
	{SDL_SCANCODE_C, OPENC2E_KEY_C},
	{SDL_SCANCODE_D, OPENC2E_KEY_D},
	{SDL_SCANCODE_E, OPENC2E_KEY_E},
	{SDL_SCANCODE_F, OPENC2E_KEY_F},
	{SDL_SCANCODE_G, OPENC2E_KEY_G},
	{SDL_SCANCODE_H, OPENC2E_KEY_H},
	{SDL_SCANCODE_I, OPENC2E_KEY_I},
	{SDL_SCANCODE_J, OPENC2E_KEY_J},
	{SDL_SCANCODE_K, OPENC2E_KEY_K},
	{SDL_SCANCODE_L, OPENC2E_KEY_L},
	{SDL_SCANCODE_M, OPENC2E_KEY_M},
	{SDL_SCANCODE_N, OPENC2E_KEY_N},
	{SDL_SCANCODE_O, OPENC2E_KEY_O},
	{SDL_SCANCODE_P, OPENC2E_KEY_P},
	{SDL_SCANCODE_Q, OPENC2E_KEY_Q},
	{SDL_SCANCODE_R, OPENC2E_KEY_R},
	{SDL_SCANCODE_S, OPENC2E_KEY_S},
	{SDL_SCANCODE_T, OPENC2E_KEY_T},
	{SDL_SCANCODE_U, OPENC2E_KEY_U},
	{SDL_SCANCODE_V, OPENC2E_KEY_V},
	{SDL_SCANCODE_W, OPENC2E_KEY_W},
	{SDL_SCANCODE_X, OPENC2E_KEY_X},
	{SDL_SCANCODE_Y, OPENC2E_KEY_Y},
	{SDL_SCANCODE_Z, OPENC2E_KEY_Z},
	{SDL_SCANCODE_F1, OPENC2E_KEY_F1},
	{SDL_SCANCODE_F2, OPENC2E_KEY_F2},
	{SDL_SCANCODE_F3, OPENC2E_KEY_F3},
	{SDL_SCANCODE_F4, OPENC2E_KEY_F4},
	{SDL_SCANCODE_F5, OPENC2E_KEY_F5},
	{SDL_SCANCODE_F6, OPENC2E_KEY_F6},
	{SDL_SCANCODE_F7, OPENC2E_KEY_F7},
	{SDL_SCANCODE_F8, OPENC2E_KEY_F8},
	{SDL_SCANCODE_F9, OPENC2E_KEY_F9},
	{SDL_SCANCODE_F10, OPENC2E_KEY_F10},
	{SDL_SCANCODE_F11, OPENC2E_KEY_F11},
	{SDL_SCANCODE_F12, OPENC2E_KEY_F12},
	{SDL_SCANCODE_NUMLOCKCLEAR, OPENC2E_KEY_NUMLOCK},
}};

int SDLBackend::translateScancode(int key) {
	for (auto keytran : keytrans) {
		if (keytran.sdl == key)
			return keytran.openc2e;
	}

	return -1;
}

// TODO: this is possibly not a great idea, we should maybe maintain our own state table
bool SDLBackend::keyDown(int key) {
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);

	for (auto keytran : keytrans) {
		if (keytran.openc2e == key)
			if (keystate[keytran.sdl])
				return true;
	}

	return false;
}

void SDLBackend::setDefaultPalette(span<Color> palette) {
	if (palette.size() != 256) {
		throw Exception("Default palette must have 256 colors");
	}
	for (unsigned int i = 0; i < 256; i++) {
		default_palette[i].r = palette[i].r;
		default_palette[i].g = palette[i].g;
		default_palette[i].b = palette[i].b;
		default_palette[i].a = palette[i].a;
	}
}

static constexpr int OPENC2E_MAX_FPS = 60;
static constexpr int OPENC2E_MIN_FPS = 20;

void SDLBackend::waitForNextDraw() {
	// TODO: calculate scale etc here instead of in resizeNotify
	// TODO: we have to calculate renderer sizes when the backend is initialized,
	// otherwise side panels get in weird locations. related to issue with panels
	// when resizing in general?

	bool focused = SDL_GetWindowFlags(window) & (SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
	Uint32 desired_ticks_per_frame = 1000 / (focused ? OPENC2E_MAX_FPS : OPENC2E_MIN_FPS);
	Uint32 frame_end = SDL_GetTicks();
	if (frame_end - last_frame_end < desired_ticks_per_frame) {
		SDL_Delay(desired_ticks_per_frame - (frame_end - last_frame_end));
	}
	last_frame_end = frame_end;

	ImGuiSDL_NewFrame(window);
	ImGui::NewFrame();
}

void SDLBackend::drawDone() {
	ImGui::Render();
	ImGuiSDL_RenderDrawData(ImGui::GetDrawData());

	SDL_RenderPresent(renderer);
}