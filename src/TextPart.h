#pragma once

#include "SpritePart.h"

struct linedata {
	std::string text;
	unsigned int offset, width;

	void reset(unsigned int o) { offset = o; text = ""; width = 0; }
	linedata() { reset(0); }
};

struct texttintinfo {
	std::shared_ptr<creaturesImage> sprite;
	unsigned int offset;
};

enum horizontalalign { leftalign, centeralign, rightalign };
enum verticalalign { top, middle, bottom };

class TextPart : public SpritePart {
protected:
	std::vector<texttintinfo> tints;
	std::vector<linedata> lines;
	std::vector<unsigned int> pages;
	std::vector<unsigned int> pageheights;
	unsigned int currpage;
	std::string text;
	
	std::shared_ptr<creaturesImage> textsprite;
	
	int leftmargin, topmargin, rightmargin, bottommargin;
	int linespacing, charspacing;
	horizontalalign horz_align;
	verticalalign vert_align;	
	bool last_page_scroll;

	TextPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z, std::string fontsprite);
	~TextPart();
	void recalculateData();
	unsigned int calculateWordWidth(std::string word);
	void addTint(std::string tintinfo);

public:
	virtual void setText(std::string t);
	std::string getText() { return text; }
	unsigned int noPages() { return pages.size(); }
	void setPage(unsigned int p) { currpage = p; }
	unsigned int getPage() { return currpage; }
	void partRender(class RenderTarget *renderer, int xoffset, int yoffset, class TextEntryPart *caretdata);
	void partRender(class RenderTarget *renderer, int xoffset, int yoffset) { partRender(renderer, xoffset, yoffset, 0); }
	void setFormat(int left, int top, int right, int bottom, int line, int _char, horizontalalign horza, verticalalign verta, bool lastpagescroll);
};