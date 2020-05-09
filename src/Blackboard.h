/*
 *  Blackboard.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jan 12 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
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

#include "CompoundAgent.h"

#ifndef _C2E_BLACKBOARD_H
#define _C2E_BLACKBOARD_H

class Blackboard : public CompoundAgent {
	friend class BlackboardPart;

protected:
	class BlackboardPart *ourPart;
	
	// configuration
	unsigned int textx, texty;
	unsigned int backgroundcolour, chalkcolour, aliascolour;

	// state
	std::vector<std::pair<unsigned int, std::string> > strings;
	std::string currenttext;
	bool editing;
	unsigned int editingindex;

	void renderText(class RenderTarget *renderer, int xoffset, int yoffset);
	std::string getText();

	void startEditing();
	void stopEditing(bool losingfocus);

public:
	Blackboard(std::string spritefile, unsigned int firstimage, unsigned int imagecount, 
		unsigned int tx, unsigned int ty, unsigned int bgcolour, unsigned int ckcolour,
		unsigned int alcolour);

	void addBlackboardString(unsigned int n, unsigned int id, std::string text);
	void showText(bool show);

	void addPart(CompoundPart *); // override
	class BlackboardPart *getBlackboardPart() { return ourPart; }

	void broadcast(bool audible);
};

class BlackboardPart : public CompoundPart {
	friend class Blackboard;

protected:
	BlackboardPart(Blackboard *p, unsigned int _id);

public:
	void partRender(class RenderTarget *renderer, int xoffset, int yoffset);
	unsigned int getWidth() { return 0; }
	unsigned int getHeight() { return 0; }

	bool canGainFocus() { return true; }
	void gainFocus();
	void loseFocus();
	void handleTranslatedChar(unsigned char c);
	void handleRawKey(uint8_t key) override;
};

#endif
/* vim: set noet: */
