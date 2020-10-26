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
 
#pragma once
 
#include <cassert>
#include <memory>

#include "AnimatablePart.h"
#include "fileformats/attFile.h"
#include "fileformats/genomeFile.h"
#include "CreatureAgent.h"

class creaturesImage;

class SkeletalCreature : public Agent, public CreatureAgent {
protected:
	class SkeletonPart *skeleton;

	unsigned int ticks; // TODO: unnecessary?

	unsigned int posedirection;
	unsigned int pose[17];
	unsigned int facialexpression;
	unsigned int pregnancy;
	bool eyesclosed;

	bool calculated;
	int oldfootx, oldfooty;
	int lastgoodfootx, lastgoodfooty; // TODO: sucky code
	bool downfoot_left;
	std::shared_ptr<class Room> downfootroom;

	creatureAppearanceGene *appearancegenes[6];
	std::map<unsigned int, creaturePoseGene *> posegenes;
	std::map<unsigned int, creatureGaitGene *> gaitgenes;

	std::shared_ptr<creaturesImage> images[17];
	attFile att[17];

	int width, height, adjustx, adjusty;
	int partx[17], party[17];

	std::string dataString(unsigned int _stage, bool sprite, unsigned int dataspecies, unsigned int databreed);

	unsigned int gaiti;
	creatureGaitGene *gaitgene;

	std::pair<int, int> getCarryPoint();
	
	void physicsTick();
	void snapDownFoot();
	
	void gaitTick();

	void processGenes();

	creatureAppearanceGene *appearanceGeneForPart(char p);
	std::shared_ptr<creaturesImage> tintBodySprite(std::shared_ptr<creaturesImage>);

	Agent *getAgent() { return this; }

public:
	SkeletalCreature(unsigned char _family);
	virtual ~SkeletalCreature();

	void render(RenderTarget *renderer, int xoffset, int yoffset);
	virtual int handleClick(float, float);
	
	void skeletonInit();
	void recalculateSkeleton();	
	
	unsigned int getPose(unsigned int i) { return pose[i]; }
	void setPose(unsigned int p);
	void setPose(std::string s);
	void setPoseGene(unsigned int p);
	void setGaitGene(unsigned int g);
	
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

	void finishInit();
	void creatureAged();

	std::string getFaceSpriteName();
	unsigned int getFaceSpriteFrame();
};

class SkeletonPart : public AnimatablePart {
	friend class SkeletalCreature;

protected:
	SkeletonPart(SkeletalCreature *p);

public:
	void tick();
	void partRender(class RenderTarget *renderer, int xoffset, int yoffset);
	unsigned int getWidth() { return ((SkeletalCreature *)parent)->getSkelWidth(); }
	unsigned int getHeight() { return ((SkeletalCreature *)parent)->getSkelHeight(); }
	void setPose(unsigned int p);
	void setFrameNo(unsigned int f);
};

/* vim: set noet: */
