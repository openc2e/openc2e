/*
 *  caosVM_agent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun May 30 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "caosVM.h"
#include "openc2e.h"
#include "Vehicle.h"
#include "PointerAgent.h"
#include "World.h"
#include "creaturesImage.h"
#include <iostream>
using std::cerr;

/**
 TOUC (integer) first (agent) second (agent)
 %status maybe
*/
void caosVM::v_TOUC() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	result.setInt(0);

	// I did this at 4:30am and have no idea if it works - fuzzie
	if (first->x < second->x) {
		if ((first->x + first->getWidth()) < second->x)
			return;
	} else {
		if ((second->x + second->getWidth()) < first->x)
			return;
	}

	if (first->y < second->y) {
		if ((first->y + first->getHeight()) < second->y)
			return;
	} else {
		if ((second->y + second->getHeight()) < first->y)
			return;
	}

	result.setInt(1);
}

/**
 RTAR (command) family (integer) genus (integer) species (integer)
 %status maybe

 set targ to random agent with given family/genus/species
 */ 
void caosVM::c_RTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	setTarg(0);

	/* XXX: maybe use a map of classifier -> agents? */
	std::vector<Agent *> temp;
	for (std::multiset<Agent *, agentzorder>::iterator i
		= world.agents.begin(); i != world.agents.end(); i++) {
		
		Agent *a = (*i);
		
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		temp.push_back(a);
	}

	if (temp.size() == 0) return;
	int i = rand() % temp.size();
	setTarg(temp[i]);
}

/**
 NEW: SIMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 create a new simple agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, at the screen depth given by plane
*/
void caosVM::c_NEW_SIMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	SimpleAgent *a = new SimpleAgent(family, genus, species, plane, sprite_file, first_image, image_count);
	setTarg(a);
}

/**
 NEW: COMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 create a new composite agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane
*/
void caosVM::c_NEW_COMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	CompoundAgent *a = new CompoundAgent(family, genus, species, plane, sprite_file, first_image, image_count);
	setTarg(a);
}

/**
 NEW: VHCL (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 create a new vehicle agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane
*/
void caosVM::c_NEW_VHCL() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	Vehicle *a = new Vehicle(family, genus, species, plane, sprite_file, first_image, image_count);
	setTarg(a);
}

/**
 TARG (agent)
 %status maybe

 return TARG
*/
void caosVM::v_TARG() {
	VM_VERIFY_SIZE(0)
	result.setAgent(targ);
}

/**
 OWNR (agent)
 %status maybe
 
 return OWNR
*/
void caosVM::v_OWNR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(owner);
}

/**
 NULL (agent)
 %status maybe

 return null (zero) agent
*/
void caosVM::v_NULL() {
	static const AgentRef nullref;
	VM_VERIFY_SIZE(0)
	result.setAgent(nullref);
}

/**
 POSE (command) pose (integer)
 %status maybe
*/
void caosVM::c_POSE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pose)

	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	p->setPose(pose);
}

/**
 ATTR (command) attr (integer)
 %status maybe

 set attributes of TARG agent
*/
void caosVM::c_ATTR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(attr)
	caos_assert(targ);
	targ->setAttributes(attr);
}

/**
 ATTR (integer)
 %status maybe

 return attributes of TARG agent
*/
void caosVM::v_ATTR() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->getAttributes());
}

/**
 TICK (command) tickrate (integer)
 %status maybe
*/
void caosVM::c_TICK() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(tickrate)
	caos_assert(targ);
	targ->setTimerRate(tickrate);
}

/**
 BHVR (command) bhvr (integer)
 %status maybe
*/
void caosVM::c_BHVR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(bhvr)
	
	caos_assert(targ);

	// reset bhvr
	targ->cr_can_push = targ->cr_can_pull = targ->cr_can_stop =
		targ->cr_can_hit = targ->cr_can_eat = targ->cr_can_pickup = false;
	
	if (bhvr & 0x1) // creature can push
		targ->cr_can_push = true;
	if (bhvr & 0x2) // creature can pull
		targ->cr_can_pull = true;
	if (bhvr & 0x4) // creature can stop
		targ->cr_can_stop = true;
	if (bhvr & 0x8) // creature can hit
		targ->cr_can_hit = true;
	if (bhvr & 0x10) // creature can eat
		targ->cr_can_eat = true;
	if (bhvr & 0x20) // creature can pick up
		targ->cr_can_pickup = true;
}

/**
 TARG (command) agent (agent)
 %status maybe

 set TARG to given agent
*/
void caosVM::c_TARG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	setTarg(a);
}

/**
 FROM (agent)
 %status maybe
*/
void caosVM::v_FROM() {
	result.setAgent(from);
}

/**
 POSE (integer)
 %status maybe
*/
void caosVM::v_POSE() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);

	CompoundAgent *n = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(n);
	CompoundPart *p = n->part(part);
	caos_assert(p);
	result.setInt(p->getPose());
}

/**
 KILL (command) agent (agent)
 %status maybe

 destroy the agent in question. you can't destroy PNTR.
 remember, use DEAD first for creatures!
*/
void caosVM::c_KILL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALIDAGENT(a)

	if (a != AgentRef(world.hand()))
		a->kill();
}

/**
 ANIM (command) poselist (bytestring)
 %status maybe

 set the animation string for TARG, in the format '1 2 3 4'
 if it ends with '255', loop back to beginning; if it ends with '255 X', loop back to frame X

 <i>todo: compound agent stuff</i>
*/
void caosVM::c_ANIM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_BYTESTR(bs)

	caos_assert(targ);

 	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	c->part(part)->animation = bs;
	
	if (!bs.empty()) c->part(part)->setFrameNo(0); // TODO: correct?
}

/**
 ANMS (command) poselist (string)
 %status stub

 exactly like ANIM, only using a string and not a bytestring for source.
*/
void caosVM::c_ANMS() {
	VM_PARAM_STRING(poselist)

	// TODO: technically, we should parse this properly, also do error checking
	std::vector<unsigned int> animation;

	std::string t;
	for (unsigned int i = 0; i < poselist.size(); i++) {
		if (poselist[i] == ' ')
			if (!t.empty())
				animation.push_back(atoi(t.c_str()));
		else
			t = t + poselist[i];
	}

	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	c->part(part)->animation = animation;
	
	if (!animation.empty()) c->part(part)->setFrameNo(0); // TODO: correct?
}

/**
 ABBA (integer)
 %status stub
*/
void caosVM::v_ABBA() {
	VM_VERIFY_SIZE(0)
	// TODO
}

/**
 BASE (command) index (integer)
 %status maybe
*/
void caosVM::c_BASE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(index)

	caos_assert(targ);

	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	p->setBase(index);
}

/**
 BASE (integer)
 %status maybe
*/
void caosVM::v_BASE() {
	VM_VERIFY_SIZE(0)
		
	caos_assert(targ);
 	
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	result.setInt(p->getBase());
}

/**
 BHVR (integer)
 %status maybe
*/
void caosVM::v_BHVR() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	
	unsigned char bvr = 0;

	if (targ->cr_can_push) bvr += 0x1;
	if (targ->cr_can_pull) bvr += 0x2;
	if (targ->cr_can_stop) bvr += 0x4;
	if (targ->cr_can_hit) bvr += 0x8;
	if (targ->cr_can_eat) bvr += 0x10;
	if (targ->cr_can_pickup) bvr += 0x20;

	result.setInt(bvr);
}

/**
 CARR (agent)
 %status stub
*/
void caosVM::v_CARR() {
	VM_VERIFY_SIZE(0)
	// TODO
	result.setAgent(0);
}

/**
 FMLY (integer)
 %status maybe

 return family of TARG agent
*/
void caosVM::v_FMLY() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->family);
}

/**
 GNUS (integer)
 %status maybe

 return genus of TARG agent
*/
void caosVM::v_GNUS() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->genus);
}

/**
 SPCS (integer)
 %status maybe

 return species of TARG agent
*/
void caosVM::v_SPCS() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->species);
}

/**
 PLNE (integer)
 %status maybe

 return plane (z-order) of TARG agent
*/
void caosVM::v_PLNE() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->getZOrder());
}

/**
 PNTR (agent)
 %status maybe

 return the pointer agent (the hand)
*/
void caosVM::v_PNTR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(world.hand());
}

unsigned int calculateScriptId(unsigned int message_id) {
	// aka, why oh why is this needed? Silly CL.

	switch (message_id) {
		case 2: /* deactivate */
			return 0;
		case 0: /* activate 1 */
		case 1: /* activate 2 */
			return message_id + 1;
	}

	return message_id;
}

/**
 MESG WRIT (command) agent (agent) message_id (integer)
 %status maybe
*/
void caosVM::c_MESG_WRIT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent);

	agent->fireScript(calculateScriptId(message_id), owner.get());
}

/**
 MESG WRT+ (command) agent (agent) message_id (integer) param_1 (anything) param_2 (anything) delay (integer)
 %status maybe
*/
void caosVM::c_MESG_WRT() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(delay)
	VM_PARAM_VALUE(param_2)
	VM_PARAM_VALUE(param_1)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent)

	// I'm not sure how to handle the 'delay'; is it a background delay, or do we actually block for delay ticks?
	
	agent->fireScript(calculateScriptId(message_id), owner.get());
	agent->vm->setVariables(param_1, param_2);
	// TODO: set _p_ in agent's VM to param_1 and param_2
}

/**
 TOTL (integer) family (integer) genus (integer) species (integer)
 %status maybe
*/
void caosVM::v_TOTL() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	unsigned int x = 0;
	for (std::multiset<Agent *, agentzorder>::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if ((*i)->family == family)
			if ((*i)->genus == genus)
				if ((*i)->species == species)
					x++;
	}
	result.setInt(x);
}

/**
 SHOW (command) visibility (integer)
 %status maybe
 
 set visibility of agent to cameras. 0 = invisible, 1 = visible.
*/
void caosVM::c_SHOW() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(visibility)
	caos_assert((visibility == 0) || (visibility == 1));
	caos_assert(targ);
	targ->visible = visibility;
}

/**
 POSX (float)
 %status maybe
*/
void caosVM::v_POSX() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setFloat(targ->x + (targ->getWidth() / 2.0));
}

/**
 POSY (float)
 %status maybe
*/
void caosVM::v_POSY() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setFloat(targ->y + (targ->getHeight() / 2.0));
}

/**
 FRAT (command) framerate (integer)
 %status maybe
*/
void caosVM::c_FRAT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(framerate)

	caos_assert(targ);

	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	p->setFramerate(framerate);
	p->framedelay = 0;
}

class blockUntilOver : public blockCond {
	protected:
		AgentRef targ;
		int part;
	public:
		blockUntilOver(Agent *t, int p) : targ(t), part(p) {}
		virtual bool operator()() {
			bool blocking;
			int fno, animsize;

			if (!targ) return false;
			
			CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
			caos_assert(c);
			CompoundPart *p = c->part(part);
			caos_assert(p);
			fno = p->getFrameNo();
			animsize = p->animation.size();

			if (fno + 1 == animsize) blocking = false;
			else if (animsize == 0) blocking = false;
			else blocking = true; 
			return blocking;
		}
};
  

/**
 OVER (command)
 %status maybe

 wait until the animation of the target agent or part is over
*/
void caosVM::c_OVER() {
	caos_assert(targ);
	
	startBlocking(new blockUntilOver(targ, part));
}

/**
 PUHL (command) pose (integer) x (integer) y (integer)
 %status stub

 set relative x/y coords for TARG's pickup point
 pose is -1 for all poses, or a pose relative to the base specified in NEW: (not BASE)
*/
void caosVM::c_PUHL() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	caos_assert(targ);
	// TODO
}

/**
 POSL (float)
 %status maybe
*/
void caosVM::v_POSL() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->x);
}

/**
 POST (float)
 %status maybe
*/
void caosVM::v_POST() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->y);
}

/**
 POSR (float)
 %status maybe
*/
void caosVM::v_POSR() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->x + targ->getWidth());
}

/**
 POSB (float)
 %status maybe
*/
void caosVM::v_POSB() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->y + targ->getHeight());
}

/**
 WDTH (integer)
 %status maybe
*/
void caosVM::v_WDTH() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->getWidth());
}

/**
 PLNE (command) depth (integer)
 %status maybe
*/
void caosVM::c_PLNE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(depth)

	caos_assert(targ);
	targ->setZOrder(depth);
}

/**
 TINT (command) red_tint (integer) green_tint (integer) blue_tint (integer) rotation (integer) swap (integer)
 %status stub
*/
void caosVM::c_TINT() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(swap)
	VM_PARAM_INTEGER(rotation)
	VM_PARAM_INTEGER(blue_tint)
	VM_PARAM_INTEGER(green_tint)
	VM_PARAM_INTEGER(red_tint)

	caos_assert(targ);

	// TODO
}

/**
 RNGE (command) distance (float)
 %status maybe
*/
void caosVM::c_RNGE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(distance)

	caos_assert(targ)
	targ->range = distance;
}

/**
 RNGE (integer)
 %status maybe
*/
void caosVM::v_RNGE() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setFloat(targ->range);
}

/**
 TRAN (integer) x (integer) y (integer)
 %status maybe

 Tests if the pixel at (x,y) on TARG is transparent
*/
void caosVM::v_TRAN() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	caos_assert(targ);
	// TODO
	result.setInt(0);
	/*SimpleAgent *a = dynamic_cast<SimpleAgent *>(targ.get());
	if (!a) {
		// TODO: TRAN on other agents
		// (if lc2e even allows that)
		// it seems to work for compound agents - fuzzie
		result.setInt(0);
		return;
	}

	creaturesImage *i = a->getSprite();
	int index = a->getCurrentSprite();

	unsigned char *data = (unsigned char *)i->data(index);
	// XXX: do we measure from center?
	int w = i->width(index);
	int h = i->height(index);

	caos_assert(x < w);
	caos_assert(x >= 0);
	caos_assert(y < h);
	caos_assert(y >= 0);
	
	if (data[w * y + x] == 0)
		result.setInt(1);
	else
		result.setInt(0); */
}
	
/**
 TRAN (command) transparency (integer) part_no (integer)
 %status stub
*/
void caosVM::c_TRAN() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(part_no)
	VM_PARAM_INTEGER(transparency)

	caos_assert(targ);
	// TODO
}

/**
 HGHT (integer)
 %status maybe
*/
void caosVM::v_HGHT() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);

	result.setInt(targ->getHeight());
}

/**
 HAND (string)
 %status maybe

 returns the name of the hand; default is 'hand'
*/
void caosVM::v_HAND() {
	VM_VERIFY_SIZE(0)

	result.setString(world.hand()->name);
}

/**
 HAND (command) name (string)
 %status maybe

 set the name of the hand
*/
void caosVM::c_HAND() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)

	world.hand()->name = name;
}

/**
 TICK (integer)
 %status maybe

 return tick rate of TARG
*/
void caosVM::v_TICK() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->timerrate);
}

/**
 PUPT (command) pose (integer) x (integer) y (integer)
 %status stub
*/
void caosVM::c_PUPT() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	// TODO
}

/**
 STPT (command)
 %status maybe

 Stop the script running in TARG, if any.
 */
void caosVM::c_STPT() {
	caos_assert(targ)
	targ->stopScript();
}

/**
 DCOR (command) core_on (integer)
 %status done
*/
void caosVM::c_DCOR() {
	VM_PARAM_INTEGER(core_on)

	caos_assert(targ);
	targ->displaycore = core_on;
}

/**
 MIRA (integer)
 %status stub
*/
void caosVM::v_MIRA() {
	caos_assert(targ);

	result.setInt(0); // TODO
}
 
/**
 MIRA (command) mirror_on (integer)
 %status stub
*/
void caosVM::c_MIRA() {
	VM_PARAM_INTEGER(mirror_on)

	caos_assert(targ);

	// TODO
}

/**
 DISQ (float) other (agent)
 %status maybe

 return square of distance of centre points of TARG and other agents
*/
void caosVM::v_DISQ() {
	VM_PARAM_VALIDAGENT(other)

	caos_assert(targ);
	
	float x = (targ->x + (targ->getWidth() / 2)) - (other->x + (other->getWidth() / 2));
	float y = (targ->y + (targ->getHeight() / 2)) - (other->y + (other->getHeight() / 2));

	result.setFloat(x*x + y*y);
}

/**
 ALPH (command) alpha_value (integer) enable (integer)
 %status maybe
*/
void caosVM::c_ALPH() {
	VM_PARAM_INTEGER(enable)
	VM_PARAM_INTEGER(alpha_value) caos_assert(0 <= alpha_value <= 255);

	caos_assert(targ);

	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	p->is_transparent = enable;
	p->transparency = alpha_value;
}

/**
 HELD (agent)
 %status stub

 Returns the agent currently held by the target, or a random one if there are more than one.
*/
void caosVM::v_HELD() {
	result.setAgent(0);

	// TODO
}

/**
 GALL (command) spritefile (string) first_image (integer)
 %status stub

 Change the sprite file and first image associated with the current agent or part.
*/
void caosVM::c_GALL() {
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(spritefile)
}

/**
 GALL (string)
 %status maybe

 Return the name of the sprite file associated with the current agent or part.
*/
void caosVM::v_GALL() {
	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	result.setString(p->getSprite()->name);
}

/**
 SEEE (integer) first (agent) second (agent)
 %status maybe

 Return 1 if the first agent can see the other, or 0 otherwise.
*/
void caosVM::v_SEEE() {
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	// TODO: handle walls, creature invisibility
	float x = (first->x + (first->getWidth() / 2)) - (second->x + (second->getWidth() / 2));
	float y = (first->y + (first->getHeight() / 2)) - (second->y + (second->getHeight() / 2));
	float z = sqrt(x*x + y*y);

	if (z > first->range)
		result.setInt(0);
	else
		result.setInt(1);
}

/**
 TINT (integer) attribute (integer)
 %status stub

 Return the tint value for target agent. Pass 1 for red, 2 for blue, 3 for green, 4 for rotation or 5 for swap.
*/
void caosVM::v_TINT() {
	VM_PARAM_INTEGER(attribute)

	caos_assert(targ);
	result.setInt(0); // TODO
}

/**
 TINO (command) red (integer) green (integer) blue (integer) rotation (integer) swap (integer)
 %status stub

 Works like the TINT command, but only applies the tint to the current frame, and discards the rest.
*/
void caosVM::c_TINO() {
	VM_PARAM_INTEGER(swap)
	VM_PARAM_INTEGER(rotation)
	VM_PARAM_INTEGER(blue)
	VM_PARAM_INTEGER(green)
	VM_PARAM_INTEGER(red)

	// TODO
}

/* vim: set noet: */
