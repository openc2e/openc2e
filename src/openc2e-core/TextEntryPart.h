#pragma once

#include "TextPart.h"

class TextEntryPart : public TextPart {
  private:
	std::shared_ptr<creaturesImage> caretsprite;
	unsigned int caretpose;
	bool focused;
	unsigned int caretpos;
	unsigned int messageid;
	void renderCaret(class RenderTarget* renderer, int xoffset, int yoffset);

	friend class TextPart;

  public:
	TextEntryPart(Agent* p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		unsigned int _z, unsigned int msgid, std::string fontsprite);
	void setText(std::string t) override;
	bool canGainFocus() override { return true; }
	void gainFocus() override {
		focused = true;
		caretpose = 0;
	}
	void loseFocus() override { focused = false; }
	int handleClick(float, float) override;
	void handleTranslatedChar(unsigned char c) override;
	void handleRawKey(uint8_t c) override;
	void tick() override;
	void partRender(class RenderTarget* renderer, int xoffset, int yoffset) override;
	bool isTransparent() override { return false; }
};