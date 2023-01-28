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
	void renderTexture(const Texture&, Rect, RectF, RenderOptions) override {}
	void renderCreaturesImage(creaturesImage&, unsigned int, int, int, RenderOptions) override {}
	void renderLine(float, float, float, float, Color) override {}
	void blitRenderTarget(RenderTarget*, float, float, float, float) override {}
	int32_t getWidth() const override { return 800; }
	int32_t getHeight() const override { return 600; }
	void renderClear() override {}
	void setViewportOffsetTop(int) override {}
	void setViewportOffsetBottom(int) override {}
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

	void init(const std::string&, int32_t, int32_t) override {}
	void waitForNextDraw() override {}
	void drawDone() override {}
	void shutdown() override {}

	unsigned int ticks() override { return 0; }
	bool pollEvent(BackendEvent&) override { return false; }
	bool keyDown(Openc2eKeycode) override { return false; }

	std::shared_ptr<RenderTarget> getMainRenderTarget() override {
		return std::dynamic_pointer_cast<RenderTarget>(mainrendertarget);
	}
	std::shared_ptr<RenderTarget> newRenderTarget(int32_t, int32_t) override { return {}; }

	Texture createTexture(int32_t, int32_t) override { return {}; }
	void updateTexture(Texture&, Rect, const Image&) override{};
};
