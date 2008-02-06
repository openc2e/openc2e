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
*/

#include "SkeletalCreature.h"
#include "Creature.h"
#include "World.h"
#include "Engine.h"
#include "Backend.h"
#include "Room.h"

#include <typeinfo> // TODO: remove when genome system is fixed
#include <boost/format.hpp>

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

std::string SkeletalCreature::dataString(unsigned int _stage, bool sprite, unsigned int dataspecies, unsigned int databreed) {
	// TODO: dataspecies is nonsense in c1
	char _postfix[4] = "XXX";
	_postfix[0] = '0' + dataspecies + ((sprite && creature->isFemale()) ? 4 : 0);
	_postfix[1] = '0' + _stage;
	if (engine.version == 1)
		_postfix[2] = '0' + databreed;
	else
		_postfix[2] = 'a' + databreed;
	return _postfix;
}

SkeletalCreature::SkeletalCreature(unsigned char _family, Creature *c) : CreatureAgent(_family, c) {
	facialexpression = 0;
	pregnancy = 0;
	eyesclosed = false;

	ticks = 0;
	gaitgene = 0;
	
	calculated = false;
	skeletonInit();

	if (engine.version == 1) {
		setAttributes(64 + 4 + 2); // mouseable, activateable, groundbound(?!)
	} else if (engine.version == 2) {
		setAttributes(128 + 64 + 4 + 2); // mouseable, activateable, suffersphysics, sufferscollisions
		// default values from original engine
		size.setInt(224);
		rest.setInt(70);
		accg.setInt(15);
		aero.setInt(10);
	}

	// needs to go last for now, so we can throw exceptions from skeletonInit
	skeleton = new SkeletonPart(this);
}

int SkeletalPartCount() {
	if (world.gametype == "cv")
		return 17;
	else
		return 14;
}

// required for tinting, for now, until we get a saner image system in place
#include "c16Image.h"

void SkeletalCreature::skeletonInit() {
	//TODO: the exception throwing in here needs some more thought

	creatureAppearanceGene *appearance[5] = { 0, 0, 0, 0, 0 };
	for (vector<gene *>::iterator i = creature->getGenome()->genes.begin(); i != creature->getGenome()->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(creatureAppearanceGene)) {
			creatureAppearanceGene *x = (creatureAppearanceGene *)(*i);
			if (x->part > 4)
				throw creaturesException(boost::str(boost::format("SkeletalCreature didn't understand a gene with a part# of %d") % (int)x->part));
			if (appearance[x->part])
				throw creaturesException(boost::str(boost::format("SkeletalCreature got a duplicated gene for part# %d") % (int)x->part));
			appearance[x->part] = x;
		}
	}

	for (int i = 0; i < SkeletalPartCount(); i++) { // CV hackery
		if (engine.version == 1) // TODO: this is hackery to skip tails for C1
			if (i == 6 || i == 13) continue;

		// reset the existing image reference
		images[i].reset();

		// find the relevant gene
		char x = cee_bodyparts[i].letter;
		creatureAppearanceGene *partapp = 0;
		if (x == 'a' || x >= 'o') {
			// head
			partapp = appearance[0];
		} else if (x == 'b') {
			// body
			partapp = appearance[1];
		} else if (x >= 'c' && x <= 'h') {
			// legs
			partapp = appearance[2];
		} else if (x >= 'i' && x <= 'm') {
			// arms
			partapp = appearance[3];
		} else if (x == 'n') {
			// tail
			partapp = appearance[4];
		} else
			// TODO: this exception won't necessary be handled, neither will the one below
			throw creaturesException(boost::str(boost::format("SkeletalCreature doesn't understand appearance id '%c'") % (unsigned char)x));

		int partspecies, partvariant;
		if (partapp) {
			partspecies = partapp->species;
			partvariant = partapp->variant;
		} else {
			// TODO: good defaults?
			partspecies = creature->getGenus();
			partvariant = creature->getVariant();
		}

		// find relevant sprite
		int stage_to_try = creature->getStage();
		while (stage_to_try > -1 && !images[i]) {
			int spe = (engine.version == 1) ? 0 : partspecies;
			while (spe > -1 && !images[i]) {
				int var = partvariant;
				/*while (var > -1 && !images[i]) {*/
					images[i] = world.gallery.getImage(x + dataString(stage_to_try, true, spe, var));
					/*if (!images[i]) var--;
				}*/
				if (!images[i]) spe--;
			}
			if (!images[i]) stage_to_try--;
		}
		if (!images[i])
			throw creaturesException(boost::str(boost::format("SkeletalCreature couldn't find an image for species %d, variant %d, stage %d") % (int)partspecies % (int)partvariant % (int)creature->getStage()));
		
		// TODO: don't bother tinting if we don't need to
		if (engine.version > 1) { // TODO: make this work for c1 :(
			assert(dynamic_cast<duppableImage *>(images[i].get()));
			s16Image *newimage = new s16Image();
			((duppableImage *)images[i].get())->duplicateTo(newimage);
			newimage->tint(creature->getTint(0), creature->getTint(1), creature->getTint(2), creature->getTint(3), creature->getTint(4));
			images[i] = shared_ptr<creaturesImage>(newimage);
		}

		// find relevant ATT data
		stage_to_try = creature->getStage();
		std::string attfilename;
		while (stage_to_try > -1 && attfilename.empty()) {
			int spe = (engine.version == 1) ? 0 : partspecies;
			while (spe > -1 && attfilename.empty()) {
				int var = partvariant;
				while (var > -1 && attfilename.empty()) {
					attfilename = world.findFile(std::string("/Body Data/") + x + dataString(stage_to_try, false, spe, var) + ".att");
					if (attfilename.empty()) var--;
				}
				if (attfilename.empty()) spe--;
			}
			if (attfilename.empty()) stage_to_try--;
		}
		if (attfilename.empty())
			throw creaturesException(boost::str(boost::format("SkeletalCreature couldn't find body data for species %d, variant %d, stage %d") % (int)partspecies % (int)partvariant % creature->getStage()));

		// load ATT file
		std::ifstream in(attfilename.c_str());
		if (in.fail())
			throw creaturesException(boost::str(boost::format("SkeletalCreature couldn't load body data for species %d, variant %d, stage %d") % (int)partspecies % (int)partvariant % stage_to_try));
		in >> att[i];
	}

	setPose(0);
}

SkeletalCreature::~SkeletalCreature() {
	delete skeleton;
}

void SkeletalCreature::render(Surface *renderer, int xoffset, int yoffset) {
	for (int j = 0; j < 17; j++) {
		int i = cee_zorder[posedirection][j];
		if (i >= SkeletalPartCount()) continue; // CV hackery

		if (engine.version == 1) // TODO: this is hackery to skip tails for C1
			if (i == 6 || i == 13) continue;

		bodypartinfo *part = &cee_bodyparts[i];

		unsigned int ourpose = pose[i];

		bool mirror = false;
		// TODO: ack, move this check out of the loop
		if (i != 14 && i != 15 && world.variables["engine_mirror_creature_body_parts"] == 1 && ourpose >= 4 && ourpose <= 7) {
			ourpose -= 4;
			mirror = true;
		}

		// adjust for pregnancy/facial expressions/etc as necessary
		if (part->parent == -1) // body
			ourpose += (pregnancy * 16);
		else if (i == 1) // head
			ourpose += (eyesclosed ? 16 : 0) + (facialexpression * 32);
		else if (i == 16) // hair
			ourpose += 0; // TODO: 16 * hair

		assert(images[i]);

		renderer->render(images[i], ourpose, partx[i] + adjustx + xoffset, party[i] + adjusty + yoffset, false, 0, mirror);

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
		x -= (attachmentX(orig_footpart, 1) - oldfootx);
		y -= (attachmentY(orig_footpart, 1) - oldfooty);
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

	shared_ptr<Room> newroom;

	if (downfootroom) {
		if (downfootroom->containsPoint(footx, footy)) {
			newroom = downfootroom;
		} else {
			if (downfootroom->x_left <= footx && downfootroom->x_right >= footx) {
				newroom = downfootroom; // TODO, we're just forcing for now
			} else {
				float ydiff = 10000.0f; // TODO: big number
				for (std::map<weak_ptr<Room>,RoomDoor *>::iterator i = downfootroom->doors.begin(); i != downfootroom->doors.end(); i++) {
					shared_ptr<Room> thisroom = i->first.lock();
					if (engine.version == 2 && size.getInt() > i->second->perm) continue;
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
	}

	if (!newroom) {
		// TODO
		newroom = bestRoomAt(footx, footy, 3, shared_ptr<Room>());
		// insane emergency handling
		float newfooty = footy;
		while (!newroom && newfooty > (footy - 500.0f)) {
			newroom = world.map.roomAt(footx, newfooty);
			newfooty--;
		}
	}

	bool newroomchosen = (newroom != downfootroom) && downfootroom;
	bool hadroom = (downfootroom);
	downfootroom = newroom;
	
	if (!downfootroom /*|| !falling */) {
		std::cout << "no down foot room! (" << footx << ", " << footy << ")" << std::endl;
		return;
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
			if (engine.version == 2) grav.setInt(1);
			else falling = true;
			return;
		}
	}
	y = newy - (footy - y);
	if (engine.version == 2) {
		if (!belowfloor && downfootroom->floorpoints.size()) {
			// TODO: hilar hack: same as above for floorvalue
			if (size.getInt() <= downfootroom->floorvalue.getInt()) {
				grav.setInt(1);
				return;
			}
		} else {
			// TODO: hilar hack: same as above for perm
			shared_ptr<Room> downroom = world.map.roomAt(footx, downfootroom->y_left_floor + 1);
			if (downfootroom->doors.find(downroom) != downfootroom->doors.end()) {
				if (size.getInt() <= downfootroom->doors[downroom]->perm) {
					grav.setInt(1);
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
		case '?':
			switch (direction) {
				case 0: break; // north, TODO
				case 1: break; // south, TODO
				case 2: posedirection = 0; break; // right
				case 3: posedirection = 1; break; // left
				default: assert(false);
			}
			break;
		case '!':
			switch (direction) {
				case 0: break; // north, TODO
				case 1: break; // south, TODO
				case 2: posedirection = 1; break; // right
				case 3: posedirection = 0; break; // left
				default: assert(false);
			}
			break;
		case '0': posedirection = 3; break;
		case '1': posedirection = 2; break;
		case '2': posedirection = 0; break;
		case '3': posedirection = 1; break;
		default: 
			std::cout << "internal warning: SkeletalCreature::setPose didn't understand direction " << s[0] << " in pose '" << s << "'." << std::endl;
			break;
	}

	for (int i = 0; i < 14; i++) {
		int newpose;

		switch (s[i + 1]) {
			case '0': newpose = 0 + (posedirection * 4); break;
			case '1': newpose = 1 + (posedirection * 4); break;
			case '2': newpose = 2 + (posedirection * 4); break;
			case '3': newpose = 3 + (posedirection * 4); break;
			case '?': assert(i == 0); {
					// make the head look in the posedirection of _IT_
					float attachmenty = attachmentY(1, 0) + y; // head attachment point, which we'll use to 'look' from atm
					
					// TODO: this is horrible, but i have no idea how the head angle is calculated
					AgentRef attention = creature->getAttentionFocus();
					if (attention && attention->y > (attachmenty + 30)) newpose = 0;
					else if (attention && attention->y < (attachmenty - 70)) newpose = 3;
					else if (attention && attention->y < (attachmenty - 30)) newpose = 2;
					else newpose = 1;
					newpose += (posedirection * 4);
				}
				break;
			// TODO: '!' also?
			case 'X': continue; // do nothing
			default:
				  std::cout << "internal warning: SkeletalCreature::setPose didn't understand " << s[i + 1] << " in pose '" << s << "'." << std::endl;
				  continue;
		}

		pose[cee_lookup[i]] = newpose;

		// TODO: this is some hackery for CV, 
		if (world.gametype != "cv") continue;
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
	/* TODO: this sets by sequence, now, not the 'poseno' inside the gene.
	 * this is what the POSE caos command does. is this right? - fuzzie */
	//creaturePoseGene *g = (creaturePoseGene *)creature->getGenome()->getGene(2, 3, poseno);
	
	// TODO: upon second thought i think poseno is good - fuzzie
	// TODO: this needs thought, darnit
	for (vector<gene *>::iterator i = creature->getGenome()->genes.begin(); i != creature->getGenome()->genes.end(); i++) {
		if ((*i)->header.switchontime != creature->getStage()) continue;

		if (typeid(*(*i)) == typeid(creaturePoseGene)) {
			creaturePoseGene *g = (creaturePoseGene *)(*i);
			if (g->poseno == poseno) {
				gaitgene = 0;
				walking = false; // TODO: doesn't belong here, does it? really the idea of a 'walking' bool is horrid
				setPose(g->getPoseString());
				return;
			}
		}
	}
}

void SkeletalCreature::setGaitGene(unsigned int gaitdrive) { // TODO: not sure if this is *useful*
	for (vector<gene *>::iterator i = creature->getGenome()->genes.begin(); i != creature->getGenome()->genes.end(); i++) {
		if ((*i)->header.switchontime != creature->getStage()) continue;

		if (typeid(*(*i)) == typeid(creatureGaitGene)) {
			creatureGaitGene *g = (creatureGaitGene *)(*i);
			if (g->drive == gaitdrive) {
				// If we're picking a new gait..
				if (g != gaitgene) {
					// .. reset our gait details to default.
					// TODO: shouldn't we set an animation or something here, to simplify the way things work?
					gaitgene = g;
					gaiti = 0;
				}
				return;
			}
		}
	}

	// explode!
	gaitgene = 0;
}

void SkeletalCreature::tick() {
	CreatureAgent::tick();

	if (paused) return;

	eyesclosed = creature->isAsleep() || !creature->isAlive();

	// TODO: every 2 ticks = correct? what about the engine var?
	ticks++;
	if (ticks % 2 == 0) return;

	if (eyesclosed) return; // TODO: hack, this is wrong :)

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

	if ((engine.version == 1 || (engine.version == 2 && grav.getInt() == 0) || (engine.version == 3 && !falling)) && !carriedby && !invehicle)
		snapDownFoot();
	else
		downfootroom.reset();
}

void SkeletalCreature::physicsTick() {
	if (engine.version > 1) {
		if (falling && (engine.version == 2 || validInRoomSystem())) Agent::physicsTick();
		if (!carriedby && !invehicle) {
			if ((engine.version == 2 && grav.getInt() == 0) || (engine.version == 3 && !falling))
				snapDownFoot();
		}
	}

	// TODO
	// disable physics for now, it gets in the way
}

void SkeletalCreature::gaitTick() {
	if (!gaitgene) return;
	uint8 pose = gaitgene->pose[gaiti];
	if (pose == 0) {
		if (gaiti == 0) return; // non-worky gait

		gaiti = 0;
		gaitTick();
		return;
	}
	creaturePoseGene *poseg = 0;
	for (vector<gene *>::iterator i = creature->getGenome()->genes.begin(); i != creature->getGenome()->genes.end(); i++) {
		if ((*i)->header.switchontime != creature->getStage()) continue;

		if (typeid(*(*i)) == typeid(creaturePoseGene)) {
			creaturePoseGene *g = (creaturePoseGene *)(*i);
			if (g->poseno == pose)
				poseg = g;
			
		}
	}
	assert(poseg); // TODO: don't assert. caos_assert? but this means a bad genome file, always.
	setPose(poseg->getPoseString());
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

SkeletonPart::SkeletonPart(SkeletalCreature *p) : CompoundPart(p, 0, 0, 0, 0) {
}

void SkeletonPart::tick() {
}

void SkeletonPart::partRender(class Surface *renderer, int xoffset, int yoffset) {
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(parent);
	c->render(renderer, xoffset, yoffset);	
}

void SkeletalCreature::creatureAged() {
	// TODO: adjust position to account for any changes..

	skeletonInit();
}

std::string SkeletalCreature::getFaceSpriteName() {
	for (vector<gene *>::iterator i = creature->getGenome()->genes.begin(); i != creature->getGenome()->genes.end(); i++) {
		if ((*i)->header.switchontime != creature->getStage()) continue;

		if (typeid(*(*i)) == typeid(creatureAppearanceGene)) {
			creatureAppearanceGene *x = (creatureAppearanceGene *)(*i);
			if (x->part == 0) {
				return std::string("a") + dataString(0, true, x->species, x->variant);
			}
		}
	}

	caos_assert(false); // TODO: mmh
}

unsigned int SkeletalCreature::getFaceSpriteFrame() {
	return 9 + (eyesclosed ? 16 : 0) + (facialexpression * 32);
}

void SkeletalCreature::handleClick(float clickx, float clicky) {
	// TODO: muh, horror

	clicky -= y;
	if (clicky >= getSkelHeight() / 2.0) {
		// slap
		queueScript(0, (Agent *)world.hand());
	} else {
		// tickle
		queueScript(1, (Agent *)world.hand());
	}
}

/* vim: set noet: */
