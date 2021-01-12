/*
 *  SkeletalCreature.cpp
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
/* 20th feb 2005, by fuzzie
   with thanks to vadim for helping out

   TODO:
	* hair tidiness/untidiness (offset by 0 to 2 multiplied by 16)
	* render hairs/ears .. at the moment we avoid them, we lack zorder sanity too
	* support clothes parts
	* interpolate between differing poses, since c2e seems to
	* sanity checks: eg check that sprites have enough frames at load time
*/

#include "caos_assert.h"
#include "caosValue.h"
#include "SkeletalCreature.h"
#include "Creature.h"
#include "World.h"
#include "Engine.h"
#include "Backend.h"
#include "MetaRoom.h"
#include "Room.h"
#include "creaturesImage.h"
#include "imageManager.h"
#include "Map.h"

#include <cassert>
#include <memory>
#include <typeinfo> // TODO: remove when genome system is fixed
#include <fmt/core.h>

struct bodypartinfo {
	char letter;
	int parent;
	int attorder;
};

bodypartinfo cee_bodyparts[17] = {
	{ 'b', -1, -1 }, // body
	{ 'a', 0, 0 }, // head - attached to body
	{ 'c', 0, 1 }, // left thigh - attached to body
	{ 'f', 0, 2 }, // right thigh - attached to body
	{ 'i', 0, 3 }, // left humerus - attached to body
	{ 'k', 0, 4 }, // right humerus - attached to body
	{ 'm', 0, 5 }, // tail root - attached to body
	{ 'd', 2, 1 }, // left shin - attached to left thigh
	{ 'g', 3, 1 }, // right shin - attached to right thigh
	{ 'j', 4, 1 }, // left radius - attached to left humerus
	{ 'l', 5, 1 }, // right radius - attached to right humerus
	{ 'e', 7, 1 }, // left foot - attached to left shin
	{ 'h', 8, 1 }, // right foot - attached to right shin
	{ 'n', 6, 1 }, // tail tip - attached to tail root
	// Creatures Village only:
	{ 'o', 1, 2 }, // left ear - attached to head
	{ 'p', 1, 3 }, // right ear - attached to head
	{ 'q', 1, 4 } // hair - attached to head
};

unsigned int cee_zorder[4][17] = {
	{ 6, 13, 2, 7, 11, 4, 9, 0, 14, 1, 16, 15, 5, 3, 8, 10, 12 },
	{ 6, 13, 3, 8, 12, 5, 10, 0, 15, 1, 16, 14, 2, 7, 11, 4, 9 },
	{ 6, 13, 2, 4, 9, 5, 3, 7, 8, 10, 0, 11, 12, 16, 14, 15, 1 },
	{ 2, 4, 9, 1, 14, 15, 16, 5, 3, 7, 8, 10, 11, 12, 0, 6, 13 }
};

// needed for setPose(string) at least .. maybe cee_bodyparts should be indexed by letter
unsigned int cee_lookup[17] = { 1, 0, 2, 7, 11, 3, 8, 12, 4, 9, 5, 10, 6, 13, 14, 15, 16 };

int SkeletalPartCount() {
	if (engine.gametype == "cv")
		return 17;
	else
		return 14;
}

SkeletalCreature::SkeletalCreature(unsigned char _family) : Agent(_family, 0, 0, 0) {
	facialexpression = 0;
	pregnancy = 0;
	eyesclosed = false;

	// TODO: set zorder randomly :) should be somewhere between 1000-2700, at a /guess/
	zorder = 1500;

	ticks = 0;
	gaitgene = 0;
	
	calculated = false;

	if (engine.version == 1) {
		setAttributes(64 + 4 + 2); // mouseable, activateable, groundbound(?!)
	} else if (engine.version == 2) {
		setAttributes(128 + 64 + 4 + 2); // mouseable, activateable, suffersphysics, sufferscollisions
		// default values from original engine
		size = 224;
		rest = 70;
		accg = 15;
		aero = 10;
	}

	for (auto & appearancegene : appearancegenes) appearancegene = 0;

	skeleton = new SkeletonPart(this);
}

SkeletalCreature::~SkeletalCreature() {
	delete skeleton;
}

std::string SkeletalCreature::dataString(unsigned int _stage, bool tryfemale, unsigned int dataspecies, unsigned int databreed) {
	char _postfix[4] = "XXX";
	_postfix[0] = '0' + dataspecies + (tryfemale ? 4 : 0);
	_postfix[1] = '0' + _stage;
	if (engine.version == 1)
		_postfix[2] = '0' + databreed;
	else
		_postfix[2] = 'a' + databreed;
	return _postfix;
}

void SkeletalCreature::processGenes() {
	std::shared_ptr<genomeFile> genome = creature->getGenome();

	for (auto & gene : genome->genes) {
		if (!creature->shouldProcessGene(gene.get())) continue;

		if (typeid(*gene) == typeid(creatureAppearanceGene)) {
			creatureAppearanceGene *x = (creatureAppearanceGene *)gene.get();
			if (x->part > 5) continue;
			appearancegenes[x->part] = x;
		} else if (typeid(*gene) == typeid(creaturePoseGene)) {
			creaturePoseGene *x = (creaturePoseGene *)gene.get();
			posegenes[x->poseno] = x;
		} else if (typeid(*gene) == typeid(creatureGaitGene)) {
			creatureGaitGene *x = (creatureGaitGene *)gene.get();
			gaitgenes[x->drive] = x;
		}
	}
}

creatureAppearanceGene *SkeletalCreature::appearanceGeneForPart(char x) {
	// TODO: tail madness?

	if (x == 'a' || x >= 'o') {
		// head
		return appearancegenes[0];
	} else if (x == 'b') {
		// body
		return appearancegenes[1];
	} else if (x >= 'c' && x <= 'h') {
		// legs
		return appearancegenes[2];
	} else if (x >= 'i' && x <= 'l') {
		// arms
		return appearancegenes[3];
	} else if (x == 'm' || x == 'n') {
		// tail
		return appearancegenes[4];
	}
	
	return 0;
}

void SkeletalCreature::skeletonInit() {
	// TODO: the exception throwing in here needs some more thought
	// TODO: if we throw an exception when we need to kill the creature off, else segfault :/

	for (int i = 0; i < SkeletalPartCount(); i++) { // CV hackery
		if (engine.version == 1) // TODO: this is hackery to skip tails for C1
			if (i == 6 || i == 13) continue;

		// reset the existing image reference
		images[i].reset();

		// find the relevant gene
		char x = cee_bodyparts[i].letter;
		creatureAppearanceGene *partapp = appearanceGeneForPart(x);

		int partspecies, partvariant;
		partspecies = creature->getGenus();
		if (partapp) {
			if (engine.version > 1) partspecies = partapp->species;
			partvariant = partapp->variant;
		} else {
			partvariant = creature->getVariant();
		}

		int spe = partspecies;

		/*
		 * In order to find a sprite, we try our current stage first, then the stages below us,
		 * and then we try again with all lower variants, and if that fails for female sprites
		 * then we try the whole thing again but trying to find a male sprite.
		 */

		// find relevant sprite
		bool tryfemale = creature->isFemale();
		while (!images[i]) {
			int var = partvariant;
			while (var > -1 && !images[i]) {
				int stage_to_try = creature->getStage();
				while (stage_to_try > -1 && !images[i]) {
					images[i] = world.gallery->getImage(x + dataString(stage_to_try, tryfemale, spe, var));
					stage_to_try--;
				}
				var--;
			}
			if (!tryfemale) break;
			tryfemale = false;
		}
		if (!images[i])
			throw creaturesException(fmt::format("SkeletalCreature couldn't find an image for part {:c} of species {}, variant {}, stage {}", x, (int)partspecies, (int)partvariant, (int)creature->getStage()));

		// find relevant ATT data
		std::string attfilename;
		int var = partvariant;
		while (var > -1 && attfilename.empty()) {
			int stage_to_try = creature->getStage();
			while (stage_to_try > -1 && attfilename.empty()) {
				attfilename = world.findFile(std::string("Body Data/") + x + dataString(stage_to_try, false, spe, var) + ".att");
				stage_to_try--;
			}
			var--;
		}
		if (attfilename.empty())
			throw creaturesException(fmt::format("SkeletalCreature couldn't find body data for part {:c} of species {}, variant {}, stage {}", x, (int)partspecies, (int)partvariant, creature->getStage()));

		// load ATT file
		std::ifstream in(attfilename.c_str());
		if (in.fail())
			throw creaturesException(fmt::format("SkeletalCreature couldn't load body data for part {:c} of species {}, variant {}, stage {} (tried file {})", x, (int)partspecies, (int)partvariant, creature->getStage(), attfilename));
		in >> att[i];
		
		images[i] = tintBodySprite(images[i]);
	}
}

std::shared_ptr<creaturesImage> SkeletalCreature::tintBodySprite(std::shared_ptr<creaturesImage> s) {
	// TODO: don't bother tinting if we don't need to
	
	// TODO: work out tinting for other engine versions
	if (engine.version > 2) {
		std::shared_ptr<creaturesImage> newimage = world.gallery->tint(s, creature->getTint(0), creature->getTint(1), creature->getTint(2), creature->getTint(3), creature->getTint(4));
		return newimage;
	}

	return s;
}

void SkeletalCreature::render(RenderTarget *renderer, int xoffset, int yoffset) {
	bool mirror_body_parts = (world.variables["engine_mirror_creature_body_parts"] == 1);

	for (int j = 0; j < 17; j++) {
		int i = cee_zorder[posedirection][j];
		if (i >= SkeletalPartCount()) continue; // CV hackery

		if (engine.version == 1) // TODO: this is hackery to skip tails for C1
			if (i == 6 || i == 13) continue;

		bodypartinfo *part = &cee_bodyparts[i];

		unsigned int ourpose = pose[i];

		bool mirror = false;
		if (i != 14 && i != 15 && mirror_body_parts && ourpose >= 4 && ourpose <= 7) {
			ourpose -= 4;
			mirror = true;
		}

		// adjust for pregnancy/facial expressions/etc as necessary
		if (part->parent == -1) { // body
			ourpose += (pregnancy * (engine.version < 3 ? 10 : 16));
		} else if (i == 1) { // head
			ourpose += (eyesclosed ? (engine.version < 3 ? 10 : 16) : 0)
				+ (facialexpression * (engine.version < 3 ? 20 : 32));
		} else if (i == 16) { // hair
			ourpose += 0; // TODO: 16 * hair
		}

		assert(images[i]);

		renderer->renderCreaturesImage(images[i], ourpose, partx[i] + adjustx + xoffset, party[i] + adjusty + yoffset, 0, mirror);

		if (displaycore) {
			// TODO: we draw a lot of points twice here :)
			int atx = attachmentX(i, 0) + xoffset, aty = attachmentY(i, 0) + yoffset;
			renderer->renderLine(atx - 1, aty, atx + 1, aty, 0xFF0000CC);
			renderer->renderLine(atx, aty - 1, atx, aty + 1, 0xFF0000CC);
			atx = attachmentX(i, 1) + xoffset; aty = attachmentY(i, 1) + yoffset;
			renderer->renderLine(atx - 1, aty, atx + 1, aty, 0xFF0000CC);
			renderer->renderLine(atx, aty - 1, atx, aty + 1, 0xFF0000CC);
		}
	}
}

int SkeletalCreature::attachmentX(unsigned int part, unsigned int id) {
	return partx[part] + att[part].attachments[pose[part]][0 + (id * 2)] + adjustx;
}

int SkeletalCreature::attachmentY(unsigned int part, unsigned int id) {
	return party[part] + att[part].attachments[pose[part]][1 + (id * 2)] + adjusty;
}

void SkeletalCreature::recalculateSkeleton() {
	int lowestx = 0, lowesty = 0, highestx = 0, highesty = 0;

	for (int i = 0; i < SkeletalPartCount(); i++) {
		if (engine.version == 1) // TODO: this is hackery to skip tails for C1
			if (i == 6 || i == 13) continue;

		bodypartinfo *part = &cee_bodyparts[i];

		if (part->parent == -1) {
			partx[i] = 0; party[i] = 0;
		} else {
			attFile &bodyattinfo = att[0];
			attFile &attinfo = att[i];

			int attachx = att[i].attachments[pose[i]][0];
			int attachy = att[i].attachments[pose[i]][1];
			int x, y;

			if (part->parent == 0) { // linking to body
				x = bodyattinfo.attachments[pose[0]][part->attorder * 2];
				y = bodyattinfo.attachments[pose[0]][(part->attorder * 2) + 1];
			} else { // extra limb
				attFile &parentattinfo = att[part->parent];

				x = partx[part->parent] + parentattinfo.attachments[pose[part->parent]][part->attorder * 2]; 
				y = party[part->parent] + parentattinfo.attachments[pose[part->parent]][(part->attorder * 2) + 1];
			}

			x = x - attachx;
			y = y - attachy;

			partx[i] = x; party[i] = y;

			if (x < lowestx) { lowestx = x; }
			if (y < lowesty) { lowesty = y; }
			if (x + (int)images[i]->width(pose[i]) > highestx) { highestx = x + images[i]->width(pose[i]); }
			if (y + (int)images[i]->height(pose[i]) > highesty) { highesty = y + images[i]->height(pose[i]); }
		}
	}

	adjustx = -lowestx;
	adjusty = -lowesty;

	// TODO: muh, we should cooperate with physics system etc
	if (!carriedby && !invehicle && calculated) {
		int orig_footpart = (downfoot_left ? 11 : 12);
		// adjust location to match foot
		moveTo(x - (attachmentX(orig_footpart, 1) - oldfootx), y - (attachmentY(orig_footpart, 1) - oldfooty));
	}

	// work out which foot is down
	int leftfoot = attachmentY(11, 1);
	int rightfoot = attachmentY(12, 1);
	downfoot_left = (rightfoot < leftfoot);

	calculated = true;
	int orig_footpart = (downfoot_left ? 11 : 12);
	oldfootx = attachmentX(orig_footpart, 1);
	oldfooty = attachmentY(orig_footpart, 1);
	
	// recalculate width/height
	height = downfoot_left ? leftfoot : rightfoot;
	width = 50; // TODO: arbitary values bad

	// TODO: muh, we should cooperate with physics system etc
	/*if (carriedby || invehicle)
		downfootroom.reset();
	else
		snapDownFoot();*/
}

void SkeletalCreature::snapDownFoot() {
	// TODO: this isn't very well thought-out.

	int orig_footpart = (downfoot_left ? 11 : 12);
	float footx = x + attachmentX(orig_footpart, 1);
	float footy = y + attachmentY(orig_footpart, 1);

	MetaRoom *m = world.map->metaRoomAt(x, y);
	if (!m) return; // TODO: exceptiony death

	std::shared_ptr<Room> newroom;

	if (downfootroom) {
		if (downfootroom->containsPoint(footx, footy)) {
			newroom = downfootroom;
		} else {
			if (downfootroom->x_left <= footx && downfootroom->x_right >= footx) {
				newroom = downfootroom; // TODO, we're just forcing for now
			} else {
				float ydiff = 10000.0f; // TODO: big number
				for (auto i = downfootroom->doors.begin(); i != downfootroom->doors.end(); i++) {
					std::shared_ptr<Room> thisroom = i->first.lock();
					if (engine.version == 2 && size > i->second->perm) continue;
					if (thisroom->x_left <= footx && thisroom->x_right >= footx) {
						float thisydiff = fabs(footy - thisroom->floorYatX(footx));
						if (thisydiff < ydiff) {
							newroom = thisroom;
							ydiff = thisydiff;
						}
					}
				}
			}
		}
	} else {	
		newroom = bestRoomAt(footx, footy, 3, m, std::shared_ptr<Room>());

		// insane emergency handling
		float newfooty = footy;
		while (!newroom && newfooty > (footy - 500.0f)) {
			newroom = m->roomAt(footx, newfooty);
			newfooty--;
		}

		// TODO: give up here

		footy = newfooty;
	}

	bool newroomchosen = (newroom != downfootroom) && downfootroom;
	bool hadroom = downfootroom != nullptr;
	downfootroom = newroom;
	
	if (!downfootroom /*|| !falling */) {
		// TODO: hackery to cope with scripts moving us, this needs handling correctly somewhere
		if (fabs(lastgoodfootx - attachmentX(orig_footpart, 1) - x) > 50.0f || fabs(lastgoodfooty - attachmentY(orig_footpart, 1) - y) > 50.0f) {
			downfootroom = bestRoomAt(footx, footy, 3, m, std::shared_ptr<Room>());
			if (downfootroom) {
				snapDownFoot();
				return;
			} else {
				std::cerr << "Creature out of room system at (" << footx << ", " << footy << ")!" << std::endl;
				// TODO: exceptiony death?
				return;
			}
		}

		// We fell out of the room system! How did that happen? Push ourselves back in, run collision script.
		// std::cout << "Creature out of room system at (" << footx << ", " << footy << "), pushing it back in." << std::endl;

		// TODO: sucky code
		x = lastgoodfootx - attachmentX(orig_footpart, 1);
		footx = lastgoodfootx;
		footy = lastgoodfooty;
		downfootroom = m->roomAt(footx, footy);
		queueScript(6);

		if (!downfootroom) {
			std::cerr << "no down foot room! (" << footx << ", " << footy << ")" << std::endl;
			// TODO: exceptiony death
			return;
		}
	}

	bool belowfloor = false;
	float newy = downfootroom->floorYatX(footx);
	if (engine.version == 2 && hadroom && y > newy) {
		// TODO: hilar hack: cope with walking below floors
		belowfloor = true;
		newy = downfootroom->bot.pointAtX(footx).y;
	}

	if (engine.version > 1) {
		// TODO: hilar hack: enable gravity if we're snapping by much
		if (newroomchosen && abs(y - (newy - (footy - y))) > 20) {
			falling = true;
			return;
		}
	}

	moveTo(x, newy - (footy - y));

	lastgoodfootx = footx;
	lastgoodfooty = footy;

	if (engine.version > 2) {
		if (engine.version == 2 && !belowfloor && downfootroom->floorpoints.size()) {
			// TODO: hilar hack: same as above for floorvalue
			if (size <= downfootroom->floorvalue) {
				falling = true;
				return;
			}
		} else {
			// TODO: hilar hack: same as above for perm
			std::shared_ptr<Room> downroom = world.map->roomAt(footx, downfootroom->y_left_floor + 1);
			if (downfootroom->doors.find(downroom) != downfootroom->doors.end()) {
				int permsize = (engine.version == 2 ? size : perm);
				if (permsize <= downfootroom->doors[downroom]->perm) {
					falling = true;
					return;
				}
			}
		}
	}
}

void SkeletalCreature::setPose(unsigned int p) {
	posedirection = 0;
	for (int i = 0; i < SkeletalPartCount(); i++)
		pose[i] = p;
	recalculateSkeleton();
}

void SkeletalCreature::setPose(std::string s) {
	switch (s[0]) {
		case '?': // towards object of attention
			switch (direction) {
				case 0: posedirection = 3; break; // north, TODO
				case 1: posedirection = 2; break; // south, TODO
				case 2: posedirection = 0; break; // right
				case 3: posedirection = 1; break; // left
				default: assert(false);
			}
			break;
		case '!': // away from object of attention
			switch (direction) {
				case 0: posedirection = 2; break; // north, TODO
				case 1: posedirection = 3; break; // south, TODO
				case 2: posedirection = 1; break; // right
				case 3: posedirection = 0; break; // left
				default: assert(false);
			}
			break;
		case '0': posedirection = 3; break;
		case '1': posedirection = 2; break;
		case '2': posedirection = 0; break;
		case '3': posedirection = 1; break;
		case 'X': break; // do nothing
		default: 
			std::cout << "internal warning: SkeletalCreature::setPose didn't understand direction " << s[0] << " in pose '" << s << "'." << std::endl;
			break;
	}

	for (int i = 0; i < 14; i++) {
		int newpose = -1;

		switch (s[i + 1]) {
			case '0': newpose = 0; break;
			case '1': newpose = 1; break;
			case '2': newpose = 2; break;
			case '3': newpose = 3; break;
			case '4': newpose = (engine.version < 3) ? 8 : 10; break; // 'to camera'
			case '?': assert(i == 0); { // TODO
					// make the head look in the posedirection of _IT_
					float attachmenty = attachmentY(1, 0) + y; // head attachment point, which we'll use to 'look' from atm
					
					// TODO: this is horrible, but i have no idea how the head angle is calculated
					AgentRef attention = creature->getAttentionFocus();
					if (attention && attention->y > (attachmenty + 30)) newpose = 0;
					else if (attention && attention->y < (attachmenty - 70)) newpose = 3;
					else if (attention && attention->y < (attachmenty - 30)) newpose = 2;
					else newpose = 1;
				}
				break;
			// TODO: '!' also?
			case 'X': continue; // do nothing
			default:
				  std::cout << "internal warning: SkeletalCreature::setPose didn't understand " << s[i + 1] << " in pose '" << s << "'." << std::endl;
				  continue;
		}

		assert(newpose != -1);

		if (newpose < 4) {
			// newpose gives the angle, now we need to add the offset for left/right/forward/back.

			if (engine.version < 3 && posedirection > 1)
				newpose = 6 + posedirection; // only one forward/back pose in c1/c2
			else
				newpose += (posedirection * 4);
		}

		pose[cee_lookup[i]] = newpose;

		// TODO: this is some hackery for CV, 
		if (engine.gametype != "cv") continue;
		if (i == 0) { // head
			pose[14] = newpose; pose[15] = newpose; // ears
			pose[16] = newpose; // hair
		} else if (i == 1) {
			pose[6] = newpose; // tail root
			pose[13] = newpose; // tail tip
		}
	}
		
	recalculateSkeleton();
}

void SkeletalCreature::setPoseGene(unsigned int poseno) {
	std::map<unsigned int, creaturePoseGene *>::iterator i = posegenes.find(poseno);
	if (i == posegenes.end()) return; // TODO: is there a better behaviour here?

	creaturePoseGene *g = i->second;
	assert(g->poseno == poseno);
	gaitgene = 0;
	walking = false; // TODO: doesn't belong here, does it? really the idea of a 'walking' bool is horrid
	setPose(g->getPoseString());
}

void SkeletalCreature::setGaitGene(unsigned int gaitdrive) { // TODO: not sure if this is *useful*
	std::map<unsigned int, creatureGaitGene *>::iterator i = gaitgenes.find(gaitdrive);
	if (i == gaitgenes.end()) return; // TODO: is there a better behaviour here?

	creatureGaitGene *g = i->second;
	assert(g->drive == gaitdrive);

	if (g == gaitgene) return;

	// reset our gait details to default
	gaitgene = g;
	gaiti = 0;
	skeleton->animation.clear();
}

void SkeletalCreature::tick() {
	Agent::tick();

	if (paused) return;
	
	CreatureAgent::tick();

	eyesclosed = creature->isAsleep() || !creature->isAlive();

	// TODO: every 2 ticks = correct? what about the engine var?
	ticks++;
	if (ticks % 2 == 0) return;

	//if (eyesclosed) return; // TODO: hack, this is wrong :)

	// TODO: hack!
	if (!eyesclosed && !creature->isZombie()) {
		if (approaching && approachtarget) {
			// TODO: more sane approaching skillz
			if (approachtarget->x < x)
				direction = 3;
			else
				direction = 2;
		}

		if (walking || approaching) {
			// TODO: we shouldn't bother with this unless it changed?
			setGaitGene(creature->getGait());

			// TODO: we should only do this if we're moving :-P
			gaitTick();
		}
		
		approaching = false;
	}

	// TODO: this kinda duplicates what physicsTick is doing below
	if ((engine.version == 1 || !falling) && !carriedby && !invehicle)
		snapDownFoot();
	else
		downfootroom.reset();
}

void SkeletalCreature::physicsTick() {
	// TODO: mmh

	if (engine.version > 1 && falling) {
		if (validInRoomSystem() || engine.version == 2) {
			Agent::physicsTick();
		} else if (!validInRoomSystem()) {
			falling = false;
		}
	}

	if ((engine.version == 1 || !falling) && !carriedby && !invehicle)
		snapDownFoot();
	else
		downfootroom.reset();
}

void SkeletalCreature::gaitTick() {
	if (!gaitgene) return;
	uint8_t pose = gaitgene->pose[gaiti];
	if (pose == 0) {
		if (gaiti == 0) return; // non-worky gait

		gaiti = 0;
		gaitTick();
		return;
	}

	std::map<unsigned int, creaturePoseGene *>::iterator i = posegenes.find(pose);
	if (i != posegenes.end()) {
		creaturePoseGene *poseg = i->second;
		assert(poseg->poseno == pose);
		setPose(poseg->getPoseString());
	}

	gaiti++; if (gaiti > 7) gaiti = 0;
}

CompoundPart *SkeletalCreature::part(unsigned int id) {
	return skeleton;
}

void SkeletalCreature::setZOrder(unsigned int plane) {
	Agent::setZOrder(plane);
	skeleton->zapZOrder();
	skeleton->addZOrder();
}

SkeletonPart::SkeletonPart(SkeletalCreature *p) : AnimatablePart(p, 0, 0, 0, 0) {
}

void SkeletonPart::tick() {
	updateAnimation();
}

void SkeletonPart::setPose(unsigned int p) {
	((SkeletalCreature *)parent)->setPoseGene(p);
}

void SkeletonPart::setFrameNo(unsigned int p) {
	assert(p < animation.size());
	frameno = p;
}

void SkeletonPart::partRender(class RenderTarget *renderer, int xoffset, int yoffset) {
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(parent);
	c->render(renderer, xoffset, yoffset);	
}

void SkeletalCreature::finishInit() {
	Agent::finishInit();

	processGenes();
	skeletonInit();
	setPose(0);
}

void SkeletalCreature::creatureAged() {
	// TODO: adjust position to account for any changes..

	processGenes();
	skeletonInit();
	recalculateSkeleton();
}

std::string SkeletalCreature::getFaceSpriteName() {
	// TODO: we should store the face sprite when we first search for sprites (since it
	// has to be the baby sprite), rather than this horrible hackery
	for (auto & gene : creature->getGenome()->genes) {
		//if ((*i)->header.switchontime != creature->getStage()) continue;

		if (typeid(*gene) == typeid(creatureAppearanceGene)) {
			creatureAppearanceGene *x = (creatureAppearanceGene *)gene.get();
			if (x->part == 0) {
				return std::string("a") + dataString(0, creature->isFemale(), x->species, x->variant);
			}
		}
	}

	caos_assert(false); // TODO: mmh
}

unsigned int SkeletalCreature::getFaceSpriteFrame() {
	return 9 + (eyesclosed ? 16 : 0) + (facialexpression * 32);
}

int SkeletalCreature::handleClick(float clickx, float clicky) {
	// TODO: muh, horror

	clicky -= y;
	if (clicky >= getSkelHeight() / 2.0) {
		return 0; // slap
	} else {
		return 1; // tickle
	}
}

std::pair<int, int> SkeletalCreature::getCarryPoint() {
	std::pair<int, int> carrypoint;

	// TODO: 9 is left hand, how do we work out which hand a norn is carrying with?
	carrypoint.first = attachmentX(9, 1);
	carrypoint.second = attachmentY(9, 1);

	return carrypoint;
}

/* vim: set noet: */
