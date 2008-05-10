/*
 *  renderable.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Thu Dec 29 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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

#ifndef _RENDERABLE_H
#define _RENDERABLE_H

#include <set>

struct renderablezorder {
	bool operator()(const class renderable *s1, const class renderable *s2) const;
};

class renderable {
protected:
	bool added;
	std::multiset<renderable *, renderablezorder>::iterator renders_iter;
		
public:
	renderable() { added = false; }
	virtual void render(class Surface *renderer, int xoffset, int yoffset) = 0;
	virtual bool showOnRemoteCameras() { return true; }
	virtual unsigned int getZOrder() const = 0;
	virtual void zapZOrder();
	virtual void addZOrder();
	virtual ~renderable();
};

#endif
/* vim: set noet: */
