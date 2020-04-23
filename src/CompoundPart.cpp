/*
 *  CompoundPart.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
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

#include "CompoundPart.h"
#include "CameraPart.h"
#include "Camera.h"
#include "CompoundAgent.h"
#include "World.h"
#include "Engine.h"
#include "creaturesImage.h"
#include "Backend.h"
#include "Agent.h"
#include "imageManager.h"
#include <cassert>
#include <memory>

shared_ptr<creaturesImage> TextEntryPart::caretsprite;

void CompoundPart::render(RenderTarget *renderer, int xoffset, int yoffset) {
	if (parent->visible) {
		partRender(renderer, xoffset + (int)parent->x, yoffset + (int)parent->y);
		if (parent->displaycore /*&& (id == 0)*/) {
			// TODO: tsk, this should be drawn along with the other craziness on the line plane, i expect
			int xoff = xoffset + (int)parent->x + x;
			int yoff = yoffset + (int)parent->y + y;
			renderer->renderLine(xoff + (getWidth() / 2), yoff, xoff + getWidth(), yoff + (getHeight() / 2), 0xFF0000CC);
			renderer->renderLine(xoff + getWidth(), yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff + getHeight(), 0xFF0000CC);
			renderer->renderLine(xoff + (getWidth() / 2), yoff + getHeight(), xoff, yoff + (getHeight() / 2), 0xFF0000CC);
			renderer->renderLine(xoff, yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff, 0xFF0000CC);
		}	
	}
}

bool CompoundPart::showOnRemoteCameras() {
	return !parent->camerashy();
}

bool CompoundPart::canClick() {
	return parent->activateable();
}

void SpritePart::partRender(RenderTarget *renderer, int xoffset, int yoffset) {
	// TODO: we need a nicer way to handle such errors
	if (getCurrentSprite() >= getSprite()->numframes()) {
		if (engine.version == 2) {
			// hack for invalid poses - use the last sprite in the file (as real C2 does)
			spriteno = getSprite()->numframes() - 1;
		} else {
			std::string err = fmt::sprintf("pose to be rendered %d (firstimg %d, base %d) was past end of sprite file '%s' (%d sprites)",
			        pose, firstimg, base, getSprite()->getName(), getSprite()->numframes());
			parent->unhandledException(err, false);
			return;
		}
	}
	assert(getCurrentSprite() < getSprite()->numframes());
	renderer->render(getSprite(), getCurrentSprite(), xoffset + x, yoffset + y, has_alpha, alpha, draw_mirrored);
}

void SpritePart::setFrameNo(unsigned int f) {
	assert(f < animation.size());
	frameno = f;
}

void SpritePart::setPose(unsigned int p) {
	if (firstimg + base + p >= getSprite()->numframes()) {
		if (engine.version == 2) {
			// hack for invalid poses - use the last sprite in the file (as real C2 does)
			spriteno = getSprite()->numframes() - 1;
		} else {
			// TODO: mention anim frame if animation is non-empty
			std::string err = fmt::sprintf("new pose %d (firstimg %d, base %d) was past end of sprite file '%s' (%d sprites)",
			        p, firstimg, base, getSprite()->getName(), getSprite()->numframes());
			parent->unhandledException(err, false);
			return;
		}
	} else {
		spriteno = firstimg + base + p;
	}

	pose = p;
}

void SpritePart::setBase(unsigned int b) {
	base = b;
}

bool SpritePart::transparentAt(unsigned int x, unsigned int y) {
	return getSprite()->transparentAt(getCurrentSprite(), x, y);
}

void CompoundPart::gainFocus() {
	assert(false);
}

void CompoundPart::loseFocus() {
	throw creaturesException("impossible loseFocus() call");
}

void CompoundPart::handleKey(char c) {
	throw creaturesException("impossible handleKey() call");
}

void CompoundPart::handleSpecialKey(char c) {
	throw creaturesException("impossible handleSpecialKey() call");
}

int CompoundPart::handleClick(float clickx, float clicky) {
	return parent->handleClick(clickx + x + parent->x, clicky + y + parent->y);
}

CompoundPart::CompoundPart(Agent *p, unsigned int _id, int _x, int _y, int _z) : parent(p), zorder(_z), id(_id) {
	auto compound_parent = dynamic_cast<CompoundAgent*>(parent);
	part_sequence_number = compound_parent ? compound_parent->nextPartSequenceNumber() : 0;

	addZOrder();	
	x = _x;
	y = _y;
	
	has_alpha = false;
}

CompoundPart::~CompoundPart() {
	world.zorder.erase(zorder_iter);
}

SpritePart::SpritePart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg,
						int _x, int _y, unsigned int _z) : AnimatablePart(p, _id, _x, _y, _z) {
	origsprite = sprite = world.gallery->getImage(spritefile);
	firstimg = fimg;
	caos_assert(sprite);
	
	pose = 0;
	base = 0;
	spriteno = firstimg;
	is_transparent = (engine.version > 2);
	framerate = 1;
	framedelay = 0;
	draw_mirrored = false;
	
	if (sprite->numframes() <= firstimg) {
		if (world.gametype == "cv") {
			// Creatures Village allows you to create sprites with crazy invalid data, do the same as it does
			// (obviously the firstimg data is invalid so all attempts to change the pose/etc will fail, same as in CV)
			spriteno = 0;
		} else if (engine.bmprenderer) {
			// BLCK hasn't been called yet, so we can't check validity yet
		} else {
			throw caosException(fmt::sprintf("Failed to create sprite part: first sprite %d is beyond %d sprite(s) in file", firstimg, sprite->numframes()));
		}
	}
}

SpritePart::~SpritePart() {
}

#include "images/bmpImage.h"

void SpritePart::changeSprite(std::string spritefile, unsigned int fimg) {
	shared_ptr<creaturesImage> spr = world.gallery->getImage(spritefile);
	caos_assert(spr);
	base = 0; // TODO: should we preserve base?

	// TODO: this is a hack for the bmprenderer, is it really a good idea?
	if (engine.bmprenderer) {
		bmpImage *origimg = dynamic_cast<bmpImage *>(sprite.get());
		bmpImage *newimg = dynamic_cast<bmpImage *>(spr.get());
		if (origimg && newimg && origimg->numframes() > 0) newimg->setBlockSize(origimg->width(0), origimg->height(0));
	}

	caos_assert(spr->numframes() > fimg);
	firstimg = fimg;
	// TODO: should we preserve tint?

	origsprite = sprite = spr;

	setPose(pose); // TODO: we need to preserve pose, but shouldn't we do some sanity checking?
}

void SpritePart::changeSprite(shared_ptr<creaturesImage> spr) {
	caos_assert(spr);
	// TODO: should we preserve tint?
	base = 0; // TODO: should we preserve base?

	origsprite = sprite = spr;

	setPose(pose); // TODO: we need to preserve pose, but shouldn't we do some sanity checking?
}

unsigned int SpritePart::getWidth() {
	return sprite->width(getCurrentSprite());
}

unsigned int SpritePart::getHeight() {
	return sprite->height(getCurrentSprite());
}

unsigned int CompoundPart::getZOrder() const {
	return parent->getZOrder() + zorder;
}

void CompoundPart::zapZOrder() {
	renderable::zapZOrder();
	world.zorder.erase(zorder_iter);
}

void CompoundPart::addZOrder() {
	renderable::addZOrder();
	zorder_iter = world.zorder.insert(this);	
}

void SpritePart::tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap) {
	sprite = world.gallery->tint(origsprite, r, g, b, rotation, swap);
}

DullPart::DullPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			 unsigned int _z) : SpritePart(p, _id, spritefile, fimg, _x, _y, _z) {
}

ButtonPart::ButtonPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
	unsigned int _z, const bytestring_t &animhover, int msgid, int option) : SpritePart(p, _id, spritefile, fimg, _x, _y, _z) {
	messageid = msgid;
	hitopaquepixelsonly = (option == 1);
	hoveranimation = animhover;
}

unsigned int calculateScriptId(unsigned int message_id); // from caosVM_agent.cpp, TODO: move into shared file

int ButtonPart::handleClick(float x, float y) {
	return calculateScriptId(messageid);
}

void ButtonPart::mouseIn() {
	// TODO: what if ANIM is called during mouse hover?
	if (hoveranimation.size()) {
		oldanim = animation;
		animation = hoveranimation;
		setFrameNo(0);
	}
}

void ButtonPart::mouseOut() {
	// TODO: what if ANIM is called during mouse hover?
	if (hoveranimation.size()) {
		animation = oldanim;
		if (animation.size())
			setFrameNo(0);
	}
}

TextPart::TextPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z, std::string fontsprite)
	                : SpritePart(p, _id, spritefile, fimg, _x, _y, _z) {
	textsprite = world.gallery->getImage(fontsprite);
	caos_assert(textsprite);
	caos_assert(textsprite->numframes() == 224);
	leftmargin = 8; topmargin = 8; rightmargin = 8; bottommargin = 8;
	linespacing = 0; charspacing = 0;
	horz_align = leftalign; vert_align = top;
	currpage = 0;
	recalculateData(); // ie, insert a blank first page
}

TextPart::~TextPart() {
}

void TextPart::addTint(std::string tintinfo) {
	// add a tint, starting at text.size(), using the data in tintinfo
	// TODO: there's some caching to be done here, but tinting is rather rare, so..
	
	unsigned short r = 128, g = 128, b = 128, rot = 128, swap = 128;
	int where = 0;
	std::string cur;
	for (unsigned int i = 0; i <= tintinfo.size(); i++) {
		if (i == tintinfo.size() || tintinfo[i] == ' ') {
			unsigned short val = atoi(cur.c_str());
			if (val <= 256) {
				switch (where) {
					case 0: r = val; break;
					case 1: g = val; break;
					case 2: b = val; break;
					case 3: rot = val; break;
					case 4: swap = val; break;
				}
			} // TODO: else explode();
			where++;
			cur = "";
			if (where > 4) break;
		} else cur += tintinfo[i];
	}

	texttintinfo t;
	t.offset = text.size();

	if (!(r == 128 && g == 128 &&  b == 128 && rot == 128 && swap == 128)) {
		t.sprite = world.gallery->tint(textsprite, r, g, b, rot, swap);
	} else t.sprite = textsprite;

	tints.push_back(t);
}

void TextPart::setText(std::string t) {
	text.clear();
	tints.clear();

	// parse and remove the <tint> tagging
	for (unsigned int i = 0; i < t.size(); i++) {
		if ((t[i] == '<') && (t.size() > i+4))
			if ((t[i + 1] == 't') && (t[i + 2] == 'i') && (t[i + 3] == 'n') && (t[i + 4] == 't')) {
				i += 5;
				std::string tintinfo;
				if (t[i] == ' ') i++; // skip initial space, if any
				for (; i < t.size(); i++) {
					if (t[i] == '>') 
						break;
					tintinfo += t[i];
				}
				addTint(tintinfo);
				continue;
			}
		text += t[i];
	}
	
	recalculateData();
}

void TextEntryPart::setText(std::string t) {
	TextPart::setText(t);

	// place caret at the end of the text
	caretpos = text.size();
}

unsigned int calculateScriptId(unsigned int message_id); // from caosVM_agent.cpp, TODO: move into shared file

int TextEntryPart::handleClick(float clickx, float clicky) {
	world.setFocus(this);

	return -1; // TODO: this shouldn't be passed onto the parent agent?
}

void TextEntryPart::handleKey(char c) {
	text.insert(caretpos, 1, c);
	caretpos++;
	recalculateData();
}

void TextEntryPart::handleSpecialKey(char c) {
	switch (c) {
		case 8: // backspace
			if (caretpos == 0) return;
			text.erase(text.begin() + (caretpos - 1));
			caretpos--;
			break;

		case 13: // return
			// TODO: check if we should do this or a newline
			parent->queueScript(calculateScriptId(messageid), 0); // TODO: is a null FROM correct?
			return;

		case 37: // left
			if (caretpos == 0) return;
			caretpos--;
			return;

		case 39: // right
			if (caretpos == text.size()) return;
			caretpos++;
			return;

		case 38: // up
		case 40: // down
			return;

		case 46: // delete
			if ((text.size() == 0) || (caretpos >= text.size()))
				return;
			text.erase(text.begin() + caretpos);
			break;

		default:
			return;
	}

	assert(caretpos <= text.size());

	recalculateData();
}

void TextPart::setFormat(int left, int top, int right, int bottom, int line, int _char, horizontalalign horza, verticalalign verta, bool lastpagescroll) {
	leftmargin = left;
	topmargin = top;
	rightmargin = right;
	bottommargin = bottom;
	linespacing = line;
	charspacing = _char;
	horz_align = horza;
	vert_align = verta;
	last_page_scroll = lastpagescroll;
	recalculateData();
}

unsigned int TextPart::calculateWordWidth(std::string word) {
	unsigned int x = 0;
	for (unsigned int i = 0; i < word.size(); i++) {
		if (word[i] < 32) continue; // TODO: replace with space or similar?
		int spriteid = word[i] - 32;	

		x += textsprite->width(spriteid);
		if (i != 0) x += charspacing;
	}
	return x;
}

/*
 * Recalculate the data used for rendering the text part.
 */
void TextPart::recalculateData() {
	linedata currentdata;

	lines.clear();
	pages.clear();
	pageheights.clear();
	pages.push_back(0);
	if (text.size() == 0) {
		pageheights.push_back(0);
		lines.push_back(currentdata); // blank line, so caret is rendered in TextEntryParts
		return;
	}

	unsigned int textwidth = getWidth() - leftmargin - rightmargin;
	unsigned int textheight = getHeight() - topmargin - bottommargin;

	unsigned int currenty = 0, usedheight = 0;
	unsigned int i = 0;
	while (i < text.size()) {
		bool newline = false;
		unsigned int startoffset = i;
		// first, retrieve a word from the text
		std::string word;
		for (; i < text.size(); i++) {
			if ((text[i] == ' ') || (text[i] == '\n')) {
				if (text[i] == '\n') newline = true;
				i++;
				break;
			}
			word += text[i];
		}

		// next, work out whether it fits
		unsigned int wordlen = calculateWordWidth(word);
		unsigned int spacelen = textsprite->width(0) + charspacing;
		unsigned int possiblelen = wordlen;
		if (currentdata.text.size() > 0)
			possiblelen = wordlen + spacelen;
		// TODO: set usedheight as appropriate/needed
		usedheight = textsprite->height(0);
		if (currentdata.width + possiblelen <= textwidth) {
			// the rest of the word fits on the current line, so that's okay.
			// add a space if we're not the first word on this line
			if (currentdata.text.size() > 0) word = std::string(" ") + word;
			currentdata.text += word;
			currentdata.width += possiblelen;
		} else if (wordlen <= textwidth) {
			// the rest of the word doesn't fit on the current line, but does on the next line.
			if (currenty + usedheight > textheight) {
				pageheights.push_back(currenty);
				pages.push_back(lines.size());
				currenty = 0;
			} else currenty += usedheight + linespacing + 1;
			currentdata.text += " "; // TODO: HACK THINK ABOUT THIS
			lines.push_back(currentdata);
			currentdata.reset(startoffset);

			currentdata.text += word;
			currentdata.width += wordlen;
		} else {
			// TODO: word is too wide to fit on a single line
			// we should output as much as possible and then go backwards
		}

		// we force a newline here if necessary (ie, if the last char is '\n', except not in the last case)
		if ((i < text.size()) && (newline)) {
			if (currenty + usedheight > textheight) {
				pageheights.push_back(currenty);
				pages.push_back(lines.size());
				currenty = 0;
			} else currenty += usedheight + linespacing + 1;
			lines.push_back(currentdata);
			currentdata.reset(i);
		}
	}

	if (currentdata.text.size() > 0) {
		currenty += usedheight;
		if (text[text.size() - 1] == ' ') currentdata.text += " "; // TODO: HACK THINK ABOUT THIS
		lines.push_back(currentdata);
	}
	
	pageheights.push_back(currenty);
}

void TextPart::partRender(RenderTarget *renderer, int xoffset, int yoffset, TextEntryPart *caretdata) {
	SpritePart::partRender(renderer, xoffset, yoffset);
	
	unsigned int xoff = xoffset + x + leftmargin;
	unsigned int yoff = yoffset + y + topmargin;
	unsigned int textwidth = getWidth() - leftmargin - rightmargin;
	unsigned int textheight = getHeight() - topmargin - bottommargin;

	unsigned int currenty = 0, usedheight = 0;
	if (vert_align == bottom)
		currenty = textheight - pageheights[currpage];
	else if (vert_align == middle)
		currenty = (textheight - pageheights[currpage]) / 2;
	unsigned int startline = pages[currpage];
	unsigned int endline = (currpage + 1 < pages.size() ? pages[currpage + 1] : lines.size());
	shared_ptr<creaturesImage> sprite = textsprite; unsigned int currtint = 0;
	for (unsigned int i = startline; i < endline; i++) {	
		unsigned int currentx = 0, somex = xoff;
		if (horz_align == rightalign)
			somex = somex + (textwidth - lines[i].width);
		else if (horz_align == centeralign)
			somex = somex + ((textwidth - lines[i].width) / 2);

		for (unsigned int x = 0; x < lines[i].text.size(); x++) {
			if (currtint < tints.size() && tints[currtint].offset == lines[i].offset + x) {
				sprite = tints[currtint].sprite;
				currtint++;
			}
		
			if (lines[i].text[x] < 32) continue; // TODO: replace with space or similar?
			int spriteid = lines[i].text[x] - 32;
			renderer->render(sprite, spriteid, somex + currentx, yoff + currenty, has_alpha, alpha);
			if ((caretdata) && (caretdata->caretpos == lines[i].offset + x))
				caretdata->renderCaret(renderer, somex + currentx, yoff + currenty);
			currentx += textsprite->width(spriteid) + charspacing;
		}
		if ((caretdata) && (caretdata->caretpos == lines[i].offset + lines[i].text.size()))
			caretdata->renderCaret(renderer, somex + currentx, yoff + currenty);		
		currenty += textsprite->height(0) + linespacing + 1;
	}
}

FixedTextPart::FixedTextPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		                                  unsigned int _z, std::string fontsprite) : TextPart(p, _id, spritefile, fimg, _x, _y, _z, fontsprite) {
	// nothing, hopefully.. :)
}

TextEntryPart::TextEntryPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		                                  unsigned int _z, unsigned int msgid, std::string fontsprite) : TextPart(p, _id, spritefile, fimg, _x, _y, _z, fontsprite) {
	// TODO: hm, this never gets freed..
	if (!caretsprite) { caretsprite = world.gallery->getImage("cursor"); caos_assert(caretsprite); }

	caretpose = 0;
	caretpos = 0;
	focused = false;
	messageid = msgid;
}

void TextEntryPart::partRender(RenderTarget *renderer, int xoffset, int yoffset) {
	TextPart::partRender(renderer, xoffset, yoffset, (focused ? this : 0));
}

void TextEntryPart::renderCaret(RenderTarget *renderer, int xoffset, int yoffset) {
	// TODO: fudge xoffset/yoffset as required
	renderer->render(caretsprite, caretpose, xoffset, yoffset, has_alpha, alpha);
}

void TextEntryPart::tick() {
	SpritePart::tick();

	if (focused) {
		caretpose++;
		if (caretpose == caretsprite->numframes())
			caretpose = 0;
	}
}

void SpritePart::tick() {
	if (!animation.empty()) {
		if (framerate > 1) {
			framedelay++;
			if (framedelay == (unsigned int)framerate + 1)
				framedelay = 0;
		}
	}
		
	if (framedelay == 0)
		updateAnimation();
}

void AnimatablePart::updateAnimation() {
	if (animation.empty()) return;

	if (frameno == animation.size()) return;
	assert(frameno < animation.size());

	if (animation[frameno] == 255) {
		if (frameno == (animation.size() - 1)) {
			frameno = 0;
		} else {
			// if we're not at the end, we ought to have at least one more item
			assert(frameno + 1 < animation.size());

			frameno = animation[frameno + 1];

			if (frameno >= animation.size()) {
				// this is an internal error because it should have been validated at pose-setting time
				std::string err = fmt::sprintf("internal animation error: tried looping back to frame %d but that is beyond animation size %d",
				        (int)frameno, (int)animation.size());
				parent->unhandledException(err, false);
				animation.clear();
				return;
			}

		}
	}

	setPose(animation[frameno]);
	frameno++;
}

CameraPart::CameraPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		unsigned int _z, unsigned int view_width, unsigned int view_height, unsigned int camera_width, unsigned int camera_height)
		: SpritePart(p, _id, spritefile, fimg, _x, _y, _z) {
	viewwidth = view_width;
	viewheight = view_height;
	camerawidth = camera_width;
	cameraheight = camera_height;
	camera = shared_ptr<Camera>(new PartCamera(this));
}

void CameraPart::partRender(RenderTarget *renderer, int xoffset, int yoffset) {
	// TODO: hack to stop us rendering cameras inside cameras. better way?
	if (renderer == engine.backend->getMainRenderTarget()) {
		// make sure we're onscreen before bothering to do any work..
		if (xoffset + x + (int)camerawidth >= 0 && yoffset + y + (int)cameraheight >= 0 &&
			xoffset + x < (int)renderer->getWidth() && yoffset + y < (int)renderer->getHeight()) {
			RenderTarget *surface = engine.backend->newRenderTarget(viewwidth, viewheight);
			assert(surface); // TODO: good behaviour?
			world.drawWorld(camera.get(), surface);
			renderer->blitRenderTarget(surface, xoffset + x, yoffset + y, camerawidth, cameraheight);
			engine.backend->freeRenderTarget(surface);
		}
	}
	
	SpritePart::partRender(renderer, xoffset, yoffset);
}

void CameraPart::tick() {
	SpritePart::tick();

	camera->tick();
}

GraphPart::GraphPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		unsigned int _z, unsigned int novalues) : SpritePart(p, _id, spritefile, fimg, _x, _y, _z) {
	// TODO: store novalues
}

/* vim: set noet: */

