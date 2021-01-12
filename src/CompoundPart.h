/*
 *  CompoundPart.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

#pragma once

#include "partzorder.h"
#include "renderable.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class Agent;
class creaturesImage;

class CompoundPart : public renderable {
  protected:
	std::multiset<CompoundPart*, partzorder>::iterator zorder_iter;
	Agent* parent;

	CompoundPart(Agent* p, unsigned int _id, int _x, int _y, int _z);

  public:
	int x, y;
	unsigned int zorder, id;
	unsigned int part_sequence_number;

	bool has_alpha;
	unsigned char alpha;

	virtual void render(class RenderTarget* renderer, int xoffset, int yoffset);
	virtual void partRender(class RenderTarget* renderer, int xoffset, int yoffset) = 0;
	virtual void tick() {}

	virtual void mouseIn() {}
	virtual void mouseOut() {}

	virtual bool canGainFocus() { return false; }
	virtual void gainFocus();
	virtual void loseFocus();
	virtual int handleClick(float, float);
	virtual void handleTranslatedChar(unsigned char c);
	virtual void handleRawKey(uint8_t c);

	virtual unsigned int getWidth() = 0;
	virtual unsigned int getHeight() = 0;

	virtual bool showOnRemoteCameras();

	virtual bool canClick();

	Agent* getParent() const { return parent; }
	unsigned int getZOrder() const;
	void zapZOrder();
	void addZOrder();

	bool operator<(const CompoundPart& b) const {
		return zorder < b.zorder;
	}

	virtual ~CompoundPart();
};

/* vim: set noet: */
