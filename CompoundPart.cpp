/*
 *  CompoundAgent.cpp
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

#include "CompoundPart.h"
#include "openc2e.h"
#include "c16Image.h"
#include "SDLBackend.h"

creaturesImage *TextEntryPart::caretsprite = 0;

void CompoundPart::render(SDLBackend *renderer, int xoffset, int yoffset) {
	renderer->render(getSprite(), getCurrentSprite(), xoffset + x, yoffset + y, is_transparent, transparency);
}

CompoundPart::CompoundPart(unsigned int _id, std::string spritefile, unsigned int fimg,
						int _x, int _y, unsigned int _z) {
	id = _id;
	firstimg = fimg;
	x = _x;
	y = _y;
	zorder = _z;
	sprite = gallery.getImage(spritefile);
	caos_assert(sprite);
	pose = 0;
	base = 0;
	is_transparent = false;
	framerate = 1;
	framedelay = 0;
}

DullPart::DullPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			 unsigned int _z) : CompoundPart(_id, spritefile, fimg, _x, _y, _z) {
}

ButtonPart::ButtonPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
	unsigned int _z, const bytestring &animhover, int msgid, int option) : CompoundPart(_id, spritefile, fimg, _x, _y, _z) {
	messageid = msgid;
	hitopaquepixelsonly = (option == 1);
	hoveranimation = animhover;
}

TextPart::TextPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z, std::string fontsprite)
	                : CompoundPart(_id, spritefile, fimg, _x, _y, _z) {
	textsprite = gallery.getImage(fontsprite);
	caos_assert(textsprite);
	caos_assert(textsprite->numframes() == 224);
	leftmargin = 8; topmargin = 8; rightmargin = 8; bottommargin = 8;
	linespacing = 0; charspacing = 0;
	left_align = false; center_align = false; bottom_align = false; middle_align = false; last_page_scroll = false;
	currpage = 0;
	recalculateData(); // ie, insert a blank first page
}

void TextPart::setText(std::string t) {
	text.clear();

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
				// TODO: handle contents of tintinfo somehow
				continue;
			}
		text += t[i];
	}
	
	recalculateData();
}

void TextEntryPart::setText(std::string t) {
	TextPart::setText(t);

	// place caret at the end of the text
	caretline = lines.size() - 1;
	caretchar = lines[caretline].text.size();
}

void TextEntryPart::handleKey(char c) {
	// TODO: this is dumb
	if (c == 0) {
		if (text.size() == 0) return;
		text.erase(text.end() - 1);
		caretchar--; // TODO: it's not this simple!
	} else {
		text += c;
		caretchar++; // TODO: it's not this simple!
	}
	recalculateData();
}

void TextPart::setFormat(int left, int top, int right, int bottom, int line, int _char, bool lefta, bool centera, bool bottoma, bool middlea, bool lastpagescroll) {
	leftmargin = left;
	topmargin = top;
	rightmargin = right;
	bottommargin = bottom;
	linespacing = line;
	charspacing = _char;
	left_align = lefta;
	center_align = centera;
	bottom_align = bottoma;
	middle_align = middlea;
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
	pages.push_back(0);
	if (text.size() == 0) {
		lines.push_back(currentdata); // blank line, so caret is rendered in TextEntryParts
		return;
	}

	unsigned int textwidth = getWidth() - leftmargin - rightmargin;
	unsigned int textheight = getHeight() - topmargin - bottommargin;

	unsigned int currenty = 0, usedheight = 0;
	unsigned int i = 0;
	while (i < text.size()) {
		bool newline = false;
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
			currenty += usedheight + linespacing;
			if (currenty > textheight) {
				currenty = 0;
				pages.push_back(lines.size());
			}
			currentdata.text += " "; // TODO: HACK THINK ABOUT THIS
			lines.push_back(currentdata);
			currentdata.reset();

			currentdata.text += word;
			currentdata.width += wordlen;
		} else {
			// TODO: word is too wide to fit on a single line
			// we should output as much as possible and then go backwards
		}

		// we force a newline here if necessary (ie, if the last char is '\n', except not in the last case)
		if ((i < text.size()) && (newline)) {
			currenty += usedheight + linespacing;
			if (currenty > textheight) {
				currenty = 0;
				pages.push_back(lines.size());
			}
			lines.push_back(currentdata);
			currentdata.reset();
		}
	}

	if (currentdata.text.size() > 0) {
		currenty += usedheight;
		if (text[text.size() -1] == ' ') currentdata.text += " "; // TODO: HACK THINK ABOUT THIS
		lines.push_back(currentdata);
	}
}

void TextPart::render(SDLBackend *renderer, int xoffset, int yoffset, TextEntryPart *caretdata) {
	CompoundPart::render(renderer, xoffset + x, yoffset + y);
	
	unsigned int xoff = xoffset + x + leftmargin;
	unsigned int yoff = yoffset + y + topmargin;
	unsigned int textwidth = getWidth() - leftmargin - rightmargin;
	unsigned int textheight = getHeight() - topmargin - bottommargin;

	unsigned int currenty = 0, usedheight = 0;
	unsigned int startline = pages[currpage];
	unsigned int endline = (currpage + 1 < pages.size() ? pages[currpage + 1] : lines.size());
	for (unsigned int i = startline; i < endline; i++) {
		unsigned int currentx = 0, somex = xoff;
		if (center_align)
			somex = somex + ((textwidth - lines[i].width) / 2);
		for (unsigned int x = 0; x < lines[i].text.size(); x++) {
			if (lines[i].text[x] < 32) continue; // TODO: replace with space or similar?
			int spriteid = lines[i].text[x] - 32;
			renderer->render(textsprite, spriteid, somex + currentx, yoff + currenty, is_transparent, transparency);
			if ((caretdata) && (caretdata->caretline == i) && (caretdata->caretchar == x))
				caretdata->renderCaret(renderer, somex + currentx, yoff + currenty);
			currentx += textsprite->width(spriteid) + charspacing;
		}
		if ((caretdata) && (caretdata->caretline == i) && (caretdata->caretchar == lines[i].text.size()))
			caretdata->renderCaret(renderer, somex + currentx, yoff + currenty);		
		currenty += textsprite->height(0) + linespacing;
	}
}

FixedTextPart::FixedTextPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		                                  unsigned int _z, std::string fontsprite) : TextPart(_id, spritefile, fimg, _x, _y, _z, fontsprite) {
	// nothing, hopefully.. :)
}

TextEntryPart::TextEntryPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		                                  unsigned int _z, unsigned int msgid, std::string fontsprite) : TextPart(_id, spritefile, fimg, _x, _y, _z, fontsprite) {
	if (!caretsprite) { caretsprite = gallery.getImage("cursor"); caos_assert(caretsprite); }

	caretpose = 0;
	caretline = 0;
	caretchar = 0;
	focused = false;
}

void TextEntryPart::render(SDLBackend *renderer, int xoffset, int yoffset) {
	TextPart::render(renderer, xoffset, yoffset, (focused ? this : 0));
}

void TextEntryPart::renderCaret(SDLBackend *renderer, int xoffset, int yoffset) {
	// TODO: fudge xoffset/yoffset as required
	renderer->render(caretsprite, caretpose, xoffset, yoffset, is_transparent, transparency);
}

void TextEntryPart::tick() {
	CompoundPart::tick();

	if (focused) {
		caretpose++;
		if (caretpose == caretsprite->numframes())
			caretpose = 0;
	}
}

void CompoundPart::tick() {
	if (!animation.empty()) {
                if (framerate > 1) {
			framedelay++;
			if (framedelay == framerate + 1)
				framedelay = 0;
		}
		
		if (framedelay == 0) {
			unsigned int f = frameno + 1;
			if (f == animation.size()) return;
			if (animation[f] == 255) {
				if (f == (animation.size() - 1)) f = 0;
				else f = animation[f + 1];
			}
			setFrameNo(f);
		}
	}
}

/* vim: set noet: */

