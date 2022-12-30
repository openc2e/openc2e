/*
 *  NullBackend.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Dec 15 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#pragma once

#include "Backend.h"

#include <memory>

class NullRenderTarget : public RenderTarget {
  public:
	virtual void renderTexture(const Texture&, Rect, RectF, RenderOptions) {}
	virtual void renderCreaturesImage(creaturesImage&, unsigned int, int, int, RenderOptions) {}
	virtual void renderLine(float, float, float, float, unsigned int) {}
	virtual void blitRenderTarget(RenderTarget*, int, int, int, int) {}
	virtual unsigned int getWidth() const { return 800; }
	virtual unsigned int getHeight() const { return 600; }
	virtual void renderClear() {}
	virtual void setViewportOffsetTop(int) {}
	virtual void setViewportOffsetBottom(int) {}
};

class NullBackend : public Backend {
  protected:
	std::shared_ptr<NullRenderTarget> mainrendertarget{std::make_shared<NullRenderTarget>()};
	NullBackend() {}
	NullBackend(const NullBackend&) = delete;
	NullBackend(NullBackend&&) = delete;
	NullBackend& operator=(const NullBackend&) = delete;
	NullBackend& operator=(NullBackend&&) = delete;

  public:
	static Backend* get_instance();

	virtual void init(const std::string&, int, int) {}
	virtual void waitForNextDraw() {}
	virtual void drawDone() {}
	virtual void shutdown() {}

	virtual void resize(int, int) {}

	virtual unsigned int ticks() { return 0; }
	virtual bool pollEvent(BackendEvent&) { return false; }
	virtual bool keyDown(int) { return false; }

	virtual bool selfRender() { return false; }
	virtual void requestRender() {}

	virtual std::shared_ptr<RenderTarget> getMainRenderTarget() {
		return std::dynamic_pointer_cast<RenderTarget>(mainrendertarget);
	}
	virtual std::shared_ptr<RenderTarget> newRenderTarget(unsigned int, unsigned int) { return {}; }

	virtual Texture createTexture(const Image&) { return {}; }
	virtual Texture createTextureWithTransparentColor(const Image&, Color) { return {}; }

	virtual unsigned int textWidth(std::string) { return 0; }
};
