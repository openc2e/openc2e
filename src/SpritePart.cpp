#include "SpritePart.h"

#include "Agent.h"
#include "Backend.h"
#include "Engine.h"
#include "World.h"
#include "caos_assert.h"
#include "imageManager.h"

#include <fmt/core.h>

SpritePart::SpritePart(Agent* p, unsigned int _id, std::string spritefile, unsigned int fimg,
	int _x, int _y, unsigned int _z)
	: AnimatablePart(p, _id, _x, _y, _z) {
	origsprite = sprite = world.gallery->getImage(spritefile);
	firstimg = fimg;
	caos_assert(sprite);

	pose = 0;
	base = 0;
	spriteno = firstimg;
	is_transparent = (engine.version > 2);
	framerate = 1;
	framedelay = 0;

	if (sprite->numframes() <= firstimg) {
		if (engine.gametype == "cv") {
			// Creatures Village allows you to create sprites with crazy invalid data, do the same as it does
			// (obviously the firstimg data is invalid so all attempts to change the pose/etc will fail, same as in CV)
			spriteno = 0;
		} else if (engine.bmprenderer) {
			// BLCK hasn't been called yet, so we can't check validity yet
		} else {
			throw creaturesException(fmt::format("Failed to create sprite part: first sprite {} is beyond {} sprite(s) in file {}", firstimg, sprite->numframes(), sprite->getName()));
		}
	}
}

SpritePart::~SpritePart() {
}

void SpritePart::partRender(RenderTarget* renderer, int xoffset, int yoffset) {
	// TODO: we need a nicer way to handle such errors
	if (getCurrentSprite() >= getSprite()->numframes()) {
		if (engine.version == 2) {
			// hack for invalid poses - use the last sprite in the file (as real C2 does)
			spriteno = getSprite()->numframes() - 1;
		} else {
			throw creaturesException(fmt::format("pose to be rendered {} (firstimg {}, base {}) was past end of sprite file '{}' ({} sprites)",
				pose, firstimg, base, getSprite()->getName(), getSprite()->numframes()));
		}
	}
	assert(getCurrentSprite() < getSprite()->numframes());
	RenderOptions render_opts;
	render_opts.alpha = parent->alpha;
	render_opts.mirror = parent->draw_mirrored;
	render_opts.scale = parent->scle;
	if (parent->strc) {
		render_opts.override_drawsize = true;
		render_opts.overridden_drawwidth = parent->strc_width;
		render_opts.overridden_drawheight = parent->strc_height;
	}
	renderer->renderCreaturesImage(getSprite(), getCurrentSprite(), xoffset + x, yoffset + y, render_opts);
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
			throw creaturesException(fmt::format("new pose {} (firstimg {}, base {}) was past end of sprite file '{}' ({} sprites)",
				p, firstimg, base, getSprite()->getName(), getSprite()->numframes()));
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

void SpritePart::changeSprite(std::string spritefile, unsigned int fimg) {
	std::shared_ptr<creaturesImage> spr = world.gallery->getImage(spritefile);
	caos_assert(spr->numframes() > fimg);
	firstimg = fimg;
	changeSprite(spr);
}

void SpritePart::changeSprite(std::shared_ptr<creaturesImage> spr) {
	caos_assert(spr);
	// TODO: should we preserve tint?
	base = 0; // TODO: should we preserve base?

	// TODO: this is a hack for the bmprenderer, is it really a good idea?
	if (engine.bmprenderer) {
		if (sprite->numframes() > 0)
			spr->setBlockSize(sprite->width(0), sprite->height(0));
	}

	origsprite = sprite = spr;

	setPose(pose); // TODO: we need to preserve pose, but shouldn't we do some sanity checking?
}

unsigned int SpritePart::getWidth() {
	return sprite->width(getCurrentSprite());
}

unsigned int SpritePart::getHeight() {
	return sprite->height(getCurrentSprite());
}

void SpritePart::tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap) {
	sprite = world.gallery->tint(origsprite, r, g, b, rotation, swap);
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