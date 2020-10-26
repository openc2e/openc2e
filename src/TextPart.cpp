#include "caos_assert.h"
#include "TextPart.h"
#include "Backend.h"
#include "encoding.h"
#include "Engine.h"
#include "creaturesImage.h"
#include "imageManager.h"
#include "TextEntryPart.h"
#include "World.h"

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
	} else {
		t.sprite = textsprite;
	}

	tints.push_back(t);
}

void TextPart::setText(std::string t) {
	t = ensure_cp1252(t);
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
	word = ensure_cp1252(word);

	unsigned int x = 0;
	for (unsigned int i = 0; i < word.size(); i++) {
		if (((unsigned char)word[i]) < 32) continue; // TODO: replace with space or similar?
		int spriteid = ((unsigned char)word[i]) - 32;

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
			} else {
				currenty += usedheight + linespacing;
			}
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
			} else {
				currenty += usedheight + linespacing;
			}
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
	
	int xoff = xoffset + x + leftmargin;
	int yoff = yoffset + y + topmargin;
	unsigned int textwidth = getWidth() - leftmargin - rightmargin;
	unsigned int textheight = getHeight() - topmargin - bottommargin;

	int currenty = 0, usedheight = 0;
	if (vert_align == bottom) {
		currenty = textheight - pageheights[currpage];
	} else if (vert_align == middle) {
		currenty = (textheight - pageheights[currpage]) / 2;
	} else if (vert_align == top) {
		// TODO: this sort of makes sense— when height of the part is smaller
		// than height of the layed-out page, we need to shift the text up so we
		// see the bottom of the page. But is this correct? And is this the best
		// place to do it?
		currenty = std::min(0, (int)textheight - (int)pageheights[currpage]);
	}

	unsigned int startline = pages[currpage];
	unsigned int endline = (currpage + 1 < pages.size() ? pages[currpage + 1] : lines.size());
	std::shared_ptr<creaturesImage> sprite_to_use = textsprite;
	unsigned int currtint = 0;
	for (unsigned int i = startline; i < endline; i++) {	
		int currentx = 0, somex = xoff;
		if (horz_align == rightalign)
			somex = somex + (textwidth - lines[i].width);
		else if (horz_align == centeralign)
			somex = somex + ((textwidth - lines[i].width) / 2);

		for (unsigned int x = 0; x < lines[i].text.size(); x++) {
			if (currtint < tints.size() && tints[currtint].offset == lines[i].offset + x) {
				sprite_to_use = tints[currtint].sprite;
				currtint++;
			}
		
			if (((unsigned char)lines[i].text[x]) < 32) continue; // TODO: replace with space or similar?
			int spriteid = ((unsigned char)lines[i].text[x]) - 32;
			renderer->renderCreaturesImage(sprite_to_use, spriteid, somex + currentx, yoff + currenty, has_alpha ? alpha : 0);
			if ((caretdata) && (caretdata->caretpos == lines[i].offset + x))
				caretdata->renderCaret(renderer, somex + currentx, yoff + currenty);
			currentx += textsprite->width(spriteid) + charspacing;
		}
		if ((caretdata) && (caretdata->caretpos == lines[i].offset + lines[i].text.size()))
			caretdata->renderCaret(renderer, somex + currentx, yoff + currenty);		
		currenty += textsprite->height(0) + linespacing;
	}
}