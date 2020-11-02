#include "caos_assert.h"
#include "TextEntryPart.h"
#include "Agent.h"
#include "Backend.h"
#include "Engine.h"
#include "creaturesImage.h"
#include "imageManager.h"
#include "encoding.h"
#include "keycodes.h"
#include "World.h"

TextEntryPart::TextEntryPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		                                  unsigned int _z, unsigned int msgid, std::string fontsprite) : TextPart(p, _id, spritefile, fimg, _x, _y, _z, fontsprite) {
	caretsprite = world.gallery->getImage("cursor");
	caos_assert(caretsprite);

	caretpose = 0;
	caretpos = 0;
	focused = false;
	messageid = msgid;
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

void TextEntryPart::handleTranslatedChar(unsigned char c) {
	if (!cp1252_isprint(c)) {
		return;
	}
	text.insert(caretpos, 1, c);
	caretpos++;
	recalculateData();
}

void TextEntryPart::handleRawKey(uint8_t c) {
	switch (c) {
		case OPENC2E_KEY_BACKSPACE:
			if (caretpos == 0) return;
			text.erase(text.begin() + (caretpos - 1));
			caretpos--;
			break;

		case OPENC2E_KEY_RETURN:
			// TODO: check if we should do this or a newline
			parent->queueScript(calculateScriptId(messageid), 0); // TODO: is a null FROM correct?
			return;

		case OPENC2E_KEY_LEFT:
			if (caretpos == 0) return;
			caretpos--;
			return;

		case OPENC2E_KEY_RIGHT:
			if (caretpos == text.size()) return;
			caretpos++;
			return;

		case OPENC2E_KEY_UP:
		case OPENC2E_KEY_DOWN:
			return;

		case OPENC2E_KEY_DELETE:
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

void TextEntryPart::partRender(RenderTarget *renderer, int xoffset, int yoffset) {
	TextPart::partRender(renderer, xoffset, yoffset, (focused ? this : 0));
}

void TextEntryPart::renderCaret(RenderTarget *renderer, int xoffset, int yoffset) {
	// TODO: fudge xoffset/yoffset as required
	renderer->renderCreaturesImage(caretsprite, caretpose, xoffset, yoffset, has_alpha ? alpha : 0);
}

void TextEntryPart::tick() {
	SpritePart::tick();

	if (focused) {
		caretpose++;
		if (caretpose == caretsprite->numframes())
			caretpose = 0;
	}
}