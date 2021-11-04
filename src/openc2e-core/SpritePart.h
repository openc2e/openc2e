#pragma once

#include "AnimatablePart.h"

class SpritePart : public AnimatablePart {
  protected:
	std::shared_ptr<creaturesImage> origsprite, sprite;
	unsigned int firstimg, pose, base, spriteno;
	SpritePart(Agent* p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		unsigned int _z);

  public:
	bool is_transparent;
	unsigned char framerate;
	unsigned int framedelay;
	std::shared_ptr<creaturesImage> getSprite() { return sprite; }
	virtual void partRender(class RenderTarget* renderer, int xoffset, int yoffset);
	virtual void tick();
	unsigned int getPose() { return pose; }
	unsigned int getBase() { return base; }
	unsigned int getCurrentSprite() { return spriteno; }
	unsigned int getFirstImg() { return firstimg; }
	unsigned int getWidth();
	unsigned int getHeight();
	void setFrameNo(unsigned int f);
	void setPose(unsigned int p);
	void setFramerate(unsigned char f) {
		framerate = f;
		framedelay = 0;
	}
	void setBase(unsigned int b);
	void changeSprite(std::string spritefile, unsigned int fimg);
	void changeSprite(std::shared_ptr<creaturesImage> spr);
	void tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap);
	virtual bool isTransparent() { return is_transparent; }
	bool transparentAt(unsigned int x, unsigned int y);

	virtual ~SpritePart();
};