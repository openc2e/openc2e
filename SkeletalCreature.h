/*
 *  SkeletalCreature.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Thu 10 Mar 2005.
 *  Copyright (c) 2005-2006 Alyssa Milburn. All rights reserved.
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
#include "creaturesImage.h"
#include "attFile.h"

// for enum lifestage (0 to 6)
#include "genome.h"

#include "openc2e.h"
#include "CreatureAgent.h"

class SkeletalCreature : public CreatureAgent {
private:
	class SkeletonPart *skeleton;

	unsigned int ticks; // TODO: unnecessary?

	unsigned int direction;
	unsigned int pose[17];
	unsigned int facialexpression;
	unsigned int pregnancy;
	bool eyesclosed;

	bool calculated;
	int oldfootx, oldfooty;
	bool downfoot_left;
	shared_ptr<class Room> downfootroom;

	shared_ptr<creaturesImage> images[17];
	attFile att[17];

	int width, height, adjustx, adjusty;
	int partx[17], party[17];

	std::string dataString(unsigned int _stage, bool sprite, unsigned int dataspecies, unsigned int databreed);

	unsigned int gaiti;
	creatureGaitGene *gaitgene;

	void physicsTick();
	void snapDownFoot();

public:
	SkeletalCreature(unsigned char _family, Creature *c);
	virtual ~SkeletalCreature();

	void render(Surface *renderer, int xoffset, int yoffset);
	virtual void handleClick(float, float);
	
	void skeletonInit();
	void recalculateSkeleton();	
	
	unsigned int getPose(unsigned int i) { return pose[i]; }
	void setPose(unsigned int p);
	void setPose(std::string s);
	void setPoseGene(unsigned int p);
	void setGaitGene(unsigned int g);
	void gaitTick();
	
	void tick();
	
	unsigned int getPregnancy() { return pregnancy; }
	void setPregnancy(unsigned int p) { assert(p < 4); pregnancy = p; }
	bool getEyesClosed() { return eyesclosed; }
	void setEyesClosed(bool e) { eyesclosed = e; }
	unsigned int getFacialExpression() { return facialexpression; }
	void setFacialExpression(unsigned int f) { assert (f < 6); facialexpression = f; }
	unsigned int getSkelWidth() { return width; }
	unsigned int getSkelHeight() { return height; }
	
	int attachmentX(unsigned int part, unsigned int id);
	int attachmentY(unsigned int part, unsigned int id);
	bool isLeftFootDown() { return downfoot_left; }

	CompoundPart *part(unsigned int id);
	void setZOrder(unsigned int plane);

	void creatureAged();

	std::string getFaceSpriteName();
	unsigned int getFaceSpriteFrame();
};

class SkeletonPart : public CompoundPart {
public:
	SkeletonPart(SkeletalCreature *p);
	void tick();
	void partRender(class Surface *renderer, int xoffset, int yoffset);
	unsigned int getWidth() { return ((SkeletalCreature *)parent)->getSkelWidth(); }
	unsigned int getHeight() { return ((SkeletalCreature *)parent)->getSkelHeight(); }
};

/* vim: set noet: */
