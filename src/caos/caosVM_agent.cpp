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

#include "caos_assert.h"
#include "caosVM.h"
#include "Vehicle.h"
#include "Blackboard.h"
#include "PointerAgent.h"
#include "creatures/SkeletalCreature.h"
#include "World.h"
#include "creaturesImage.h"
#include <cassert>
#include <fmt/core.h>
#include <memory>
#include "AgentHelpers.h"
#include "AnimatablePart.h"
#include "Room.h" // LIML/LIMT etc
#include "Engine.h" // C2 hack in POSE
#include "Map.h"
#include "SpritePart.h"
#include <climits>

using std::cerr;

AnimatablePart *caosVM::getCurrentAnimatablePart() {
	valid_agent(targ);
	CompoundPart *p = targ->part(part);
	if (!p) return 0;
	AnimatablePart *s = dynamic_cast<AnimatablePart *>(p);
	return s;
}

SpritePart *caosVM::getCurrentSpritePart() {
	valid_agent(targ);
	CompoundPart *p = targ->part(part);
	if (!p) return 0;
	SpritePart *s = dynamic_cast<SpritePart *>(p);
	return s;
}

/**
 TOUC (integer) first (agent) second (agent)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Determines whether the two given agents are touching.  Returns 0 (if not) or 1 (if so).
*/
void caosVM::v_TOUC() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(second)
	VM_PARAM_AGENT(first)

	if (first && second && agentsTouching(first.get(), second.get()))
		result.setInt(1);
	else
		result.setInt(0);
}

/**
 RTAR (command) family (integer) genus (integer) species (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Sets TARG to a random agent with the given family/genus/species.
 */ 
void caosVM::c_RTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);
	
	setTarg(0);

	/* XXX: maybe use a map of classifier -> agents? */
	std::vector<std::shared_ptr<Agent> > temp;
	for (std::list<std::shared_ptr<Agent> >::iterator i
		= world.agents.begin(); i != world.agents.end(); i++) {
		
		Agent *a = i->get();
		if (!a) continue;
		
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		temp.push_back(*i);
	}

	if (temp.size() == 0) return;
	int i = rand() % temp.size(); // TODO: better randomness
	setTarg(temp[i]);
}

/**
 TTAR (command) family (integer) genus (integer) species (integer)
 %status maybe

 Locates a random agent that is touching OWNR (see ETCH) and that 
 matches the given classifier, and sets it to TARG.
*/
void caosVM::c_TTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	valid_agent(owner);

	setTarg(0);

	/* XXX: maybe use a map of classifier -> agents? */
	std::vector<std::shared_ptr<Agent> > temp;
	for (std::list<std::shared_ptr<Agent> >::iterator i
		= world.agents.begin(); i != world.agents.end(); i++) {
		
		Agent *a = i->get();
		if (!a) continue;
		
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		if (agentsTouching(owner, a))
			temp.push_back(*i);
	}

	if (temp.size() == 0) return;
	int i = rand() % temp.size(); // TODO: better randomness
	setTarg(temp[i]);
}

/**
 STAR (command) family (integer) genus (integer) species (integer)
 %status maybe
 %pragma variants c2 cv c3 sm

 Locates a random agent that is visible to OWNR (see ESEE) and that
 matches the given classifier, then sets it to TARG.
*/
void caosVM::c_STAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	valid_agent(targ);

	Agent *seeing;
	if (owner) seeing = owner; else seeing = targ;
	valid_agent(seeing);

	std::vector<std::shared_ptr<Agent> > agents = getVisibleList(seeing, family, genus, species);
	if (agents.size() == 0) {
		setTarg(0);
	} else {
		unsigned int i = (int) (agents.size() * (rand() / (RAND_MAX + 1.0)));
		setTarg(agents[i]);
	}
}

/**
 NEW: SIMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new simple agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, at the screen depth given by plane.
 TARG is set to the newly-created agent.
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
	a->finishInit();
	setTarg(a);
	part = 0; // TODO: correct?
}

/**
 NEW: SIMP (command) sprite_file (bareword) image_count (integer) first_image (integer) plane (integer) clone (integer)
 %status maybe
 %pragma variants c1 c2
 %cost c1,c2 1
*/
void caosVM::c_NEW_SIMP_c2() {
	VM_PARAM_INTEGER(clone)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	// TODO: we ignore clone
	// TODO: should we init with 0/0/0 or with a different constructor?
	SimpleAgent *a = new SimpleAgent(0, 0, 0, plane, sprite_file, first_image, image_count);
	a->finishInit();
	setTarg(a);
	part = 0; // TODO: correct?
}

/**
 NEW: COMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new composite agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane.
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
	a->finishInit();
	setTarg(a);
	part = 0; // TODO: correct?
}

/**
 NEW: COMP (command) sprite_file (bareword) image_count (integer) first_image (integer) clone (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::c_NEW_COMP_c1() {
	VM_PARAM_INTEGER(clone)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	// TODO: what does clone do?
	CompoundAgent *a = new CompoundAgent(sprite_file, first_image, image_count);
	a->finishInit();
	setTarg(a);
	// TODO: should part be set here?
}

/**
 NEW: VHCL (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new vehicle agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane.
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
	a->finishInit();
	setTarg(a);
	part = 0; // TODO: correct?
}

/**
 NEW: VHCL (command) sprite_file (bareword) image_count (integer) first_image (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::c_NEW_VHCL_c1() {
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	Vehicle *a = new Vehicle(sprite_file, first_image, image_count);
	a->finishInit();
	setTarg(a);
	// TODO: should part be set here?
}

/**
 NEW: BKBD (command) sprite_file (bareword) image_count (integer) first_image (integer) background_colour (integer) chalk_colour (integer) alias_colour (integer) textx (integer) texty (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::c_NEW_BKBD() {
	VM_PARAM_INTEGER(texty)
	VM_PARAM_INTEGER(textx)
	VM_PARAM_INTEGER(alias_colour)
	VM_PARAM_INTEGER(chalk_colour)
	VM_PARAM_INTEGER(background_colour)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	Blackboard *a = new Blackboard(sprite_file, first_image, image_count, textx, texty, background_colour, chalk_colour, alias_colour);
	a->finishInit();
	setTarg(a);
}

/**
 NEW: CBUB (command) sprite_file (bareword) image_count (integer) first_image (integer) stringid (integer)
 %status maybe
 %pragma variants c2
*/
void caosVM::c_NEW_CBUB() {
	VM_PARAM_INTEGER(stringid)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	throw creaturesException("compound bubble objects are not supported yet"); // TODO
}

/**
 TARG (agent)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns TARG, the currently-targeted agent.
*/
void caosVM::v_TARG() {
	VM_VERIFY_SIZE(0)
	result.setAgent(targ);
}

/**
 OWNR (agent)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Returns OWNR, the agent that is running the script.
*/
void caosVM::v_OWNR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(owner);
}

/**
 NULL (agent)
 %status maybe

 Returns a null (zero) agent.
*/
void caosVM::v_NULL() {
	static const AgentRef nullref;
	VM_VERIFY_SIZE(0)
	result.setAgent(nullref);
}

/**
 POSE (command) pose (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 1

 Sets the displayed sprite of TARG to the frame in the sprite file with the given integer.
*/
void caosVM::c_POSE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pose)

	AnimatablePart *p = getCurrentAnimatablePart();
	caos_assert(p);

	SpritePart *s = dynamic_cast<SpritePart *>(p);
	if (s && engine.version != 2) // C2 has special handling for invalid poses, see SpritePart::setPose
		caos_assert(s->getFirstImg() + s->getBase() + pose < s->getSprite()->numframes());

	p->animation.clear();
	p->setPose(pose);
}

/**
 ATTR (command) attr (integer)
 %status maybe

 Sets attributes of the TARG agent.
*/
void caosVM::c_ATTR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(attr)
	valid_agent(targ);
	
	bool oldfloat = targ->floatable();
	targ->setAttributes(attr);

	// TODO: this is an icky hack to enable floating, we should write correct floating
	// behaviour so we don't need to maintain floating lists like this :/
	if (oldfloat != targ->floatable()) {
		if (targ->floatable())
			targ->floatSetup();
		else
			targ->floatRelease();
	}
}

/**
 ATTR (variable)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Attributes of the TARG agent.
*/
void caosVM::v_ATTR() {
	valid_agent(targ);
	result.setInt(targ->getAttributes());
}
void caosVM::s_ATTR() {
	VM_PARAM_VALUE(newvalue)
	caos_assert(newvalue.hasInt());

	valid_agent(targ);
	targ->setAttributes(newvalue.getInt());
}

/**
 TICK (command) tickrate (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Initiates the agent timer-- the Timer script will then be run once every tickrate ticks.
 Setting tickrate to zero will stop the timer.
*/
void caosVM::c_TICK() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(tickrate)
	valid_agent(targ);
	targ->setTimerRate(tickrate);
}

/**
 BHVR (command) bhvr (integer)
 %status maybe

 Sets the behaviour of the TARG agent.
*/
void caosVM::c_BHVR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(bhvr)
	
	valid_agent(targ);

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
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Sets TARG (the target agent) to the given agent.
*/
void caosVM::c_TARG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	setTarg(a);
}

/**
 FROM (agent)
 %status maybe
 %pragma variants c1 c2 cv sm

 Returns the agent that sent the message being processed, or NULL if no agent was involved.
*/
void caosVM::v_FROM() {
	result.setAgent(from.getAgent());
}

/**
 FROM (variable)
 %status maybe
 %pragma variants c3

 Returns the agent that sent the message being processed, or NULL if no agent was involved.
*/
 	// Returns a variable because DS is insane and uses this for network events too (and so, of course, scripts abuse it).
CAOS_LVALUE_SIMPLE(FROM_ds, from)

/**
 POSE (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the number of the frame in the TARG part/agent's sprite file that is currently being displayed, or -1 if part# doesn't exist on a compound agent.
*/
void caosVM::v_POSE() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);

	SpritePart *p = getCurrentSpritePart();
	if (p)
		result.setInt(p->getPose());
	else
		result.setInt(-1);
}

/**
 KILL (command) agent (agent)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 1

 Destroys the agent in question. However, you cannot destroy PNTR.
 Remember, use DEAD first for Creatures!
*/
void caosVM::c_KILL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALIDAGENT(a)

	// note that kill is a virtual function which doesn't work on PNTR
	a->kill();
}

/**
 ANIM (command) poselist (bytestring)
 %status maybe

 Sets the animation string for TARG, in the format '1 2 3 4'.
 If it ends with '255', loops back to beginning; if it ends with '255 X', loops back to frame X.

 <i>todo: compound agent stuff</i>
*/
void caosVM::c_ANIM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_BYTESTR(bs)

	valid_agent(targ);

	AnimatablePart *p = getCurrentAnimatablePart();
	caos_assert(p);
	p->animation = bs;
	
	if (!bs.empty()) p->setFrameNo(0); // TODO: correct?
}

/**
 ANIM (command) animstring (string)
 %status maybe
 %pragma variants c1 c2
 %cost c1,c2 1

 Sets the animation string for TARG, in the format '1234'.
 If it ends with 'R', loops back to the beginning.
*/
void caosVM::c_ANIM_c2() {
	VM_PARAM_STRING(animstring)

	valid_agent(targ);
	
	// TODO: support creatures (using AnimatablePart, see c2e ANIM) for this (with format like "001002003R")
	if (dynamic_cast<SkeletalCreature *>(targ.get())) return;

	SpritePart *p = getCurrentSpritePart();
	caos_assert(p);

	p->animation.clear();

	for (unsigned int i = 0; i < animstring.size(); i++) {
		if (animstring[i] == 'R') {
			p->animation.push_back(255);
		} else if (animstring[i] >= 48 && animstring[i] <= 57) {
			p->animation.push_back(animstring[i] - 48);
		} else if (animstring[i] >= 97 && animstring[i] <= 122) {
			// TODO: c1 grendel eggs have 'a' at the end of their animation strings, this is an untested attempt to handle that
			p->animation.push_back(animstring[i] - 97);
		} else {
			p->animation.clear();
			throw creaturesException(std::string("old-style animation string contained '") + animstring[i] + "', which we didn't understand");
		}
	}

	if (!animstring.empty()) p->setFrameNo(0); // TODO: correct?
}

/**
 ANMS (command) poselist (string)
 %status maybe

 Exactly like ANIM, only using a string and not a bytestring for poselist source.
*/
void caosVM::c_ANMS() {
	VM_PARAM_STRING(poselist)

	// TODO: technically, we should parse this properly, also do error checking
	bytestring_t animation;

	std::string t;
	for (unsigned int i = 0; i < poselist.size(); i++) {
		if (poselist[i] == ' ') {
			if (!t.empty()) {
				int n = atoi(t.c_str());
				caos_assert(n >= 0 && n < 256);
				animation.push_back(n);
				t.clear();
			}
		} else
			t = t + poselist[i];
	}

	AnimatablePart *p = getCurrentAnimatablePart();
	caos_assert(p);
	p->animation = animation;
	
	if (!animation.empty()) p->setFrameNo(0); // TODO: correct?
}

/**
 ABBA (integer)
 %status maybe

 Returns the first_image (ie, absolute base) value for the current agent/part, or -1 if part# doesn't exist on a compound agent.
*/
void caosVM::v_ABBA() {
	VM_VERIFY_SIZE(0)
	
	SpritePart *p = getCurrentSpritePart();
	if (p)
		result.setInt(p->getFirstImg());
	else
		result.setInt(-1);
}

/**
 BASE (command) index (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Sets the frame in the TARG agent's spritefile that will be used as its base image.
 This is relative to the first image set with one of the NEW: commands.
*/
void caosVM::c_BASE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(index)

	valid_agent(targ);

	SpritePart *p = getCurrentSpritePart();
	caos_assert(p);
	// Note that we don't check validity here because a lot of code changes BASE and then only afterwards POSE/ANIM.
	p->setBase(index);
}

/**
 BASE (integer)
 %status maybe

 Returns the frame in the TARG agent/part's spritefile being used as the BASE image, or -1 if part# doesn't exist on a compound agent.
*/
void caosVM::v_BASE() {
	VM_VERIFY_SIZE(0)
		
	SpritePart *p = getCurrentSpritePart();
	if (p)
		result.setInt(p->getBase());
	else
		result.setInt(-1);
}

/**
 BHVR (integer)
 %status maybe

 Returns the behaviour of the TARG agent.
*/
void caosVM::v_BHVR() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	
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
 %status maybe

 Returns the agent that is carrying the TARG agent.  If TARG is not being carried, returns 
 NULL. 
*/
void caosVM::v_CARR() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);

	// TODO: muh, should totally be virtual
	if (targ->invehicle)
		result.setAgent(targ->invehicle);
	else
		result.setAgent(targ->carriedby);
}

/**
 CARR (agent)
 %status maybe
 %pragma variants c1 c2

 Returns the agent that is carrying the OWNR agent.  If OWNR is not being carried, returns 
 NULL. 
*/
void caosVM::v_CARR_c1() {
	VM_VERIFY_SIZE(0)
	valid_agent(owner);
	
	if (owner->invehicle)
		result.setAgent(owner->invehicle);
	else
		result.setAgent(owner->carriedby);
}

/**
 FMLY (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the family of the TARG agent.
*/
void caosVM::v_FMLY() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setInt(targ->family);
}

/**
 GNUS (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the genus of the TARG agent.
*/
void caosVM::v_GNUS() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setInt(targ->genus);
}

/**
 SPCS (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the species of the TARG agent.
*/
void caosVM::v_SPCS() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setInt(targ->species);
}

/**
 PLNE (integer)
 %status maybe

 Returns the plane (z-order) of the TARG agent.
*/
void caosVM::v_PLNE() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setInt(targ->getZOrder());
}

/**
 PNTR (agent)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the pointer agent (the Hand).
*/
void caosVM::v_PNTR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(world.hand());
}

#include "Engine.h" // for engine.version check, sigh
unsigned int calculateScriptId(unsigned int message_id) {
	// aka, why oh why is this needed? Silly CL.

	switch (message_id) {
		case 2: /* deactivate */
			return 0;
		case 8: /* hilarious special case to fix c2 airlock */
			if (engine.version < 3)
				return 7;
			else
				return 8;
		case 0: /* activate 1 */
		case 1: /* activate 2 */
			return message_id + 1;
	}

	return message_id;
}

/**
 MESG WRIT (command) agent (agent) message_id (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Sends a message of type message_id to the given agent.  FROM will be set to OWNR unless 
 there is no agent involved in sending the message.
*/
void caosVM::c_MESG_WRIT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent);

	agent->queueScript(calculateScriptId(message_id), owner.get());
}

/**
 MESG WRT+ (command) agent (agent) message_id (integer) param_1 (anything) param_2 (anything) delay (integer)
 %status maybe
 %pragma variants c2 cv c3 sm
 %cost c1,c2 0

 Sends a message of type message_id to the given agent, much like MESG WRIT, but with the 
 addition of parameters.  The message will be sent after waiting the number of ticks set 
 in delay (except doesn't, right now.  Delay must be set to zero for now.)
*/
void caosVM::c_MESG_WRT() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(delay)
	VM_PARAM_VALUE(param_2)
	VM_PARAM_VALUE(param_1)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent)

	// I'm not sure how to handle the 'delay'; is it a background delay, or do we actually block for delay ticks?
	// TODO: fuzzie can't work out how on earth delays work in c2e, someone fixit

	agent->queueScript(calculateScriptId(message_id), owner.get(), param_1, param_2);
}

/**
 TOTL (integer) family (integer) genus (integer) species (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the total number of in-game agents matching the given family/genus/species.
*/
void caosVM::v_TOTL() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	unsigned int x = 0;
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->family == family || family == 0)
			if ((*i)->genus == genus || genus == 0)
				if ((*i)->species == species || species == 0)
					x++;
	}
	result.setInt(x);
}

/**
 SHOW (command) visibility (integer)
 %status maybe
 
 Sets visibility of the TARG agent to cameras. 0 = invisible, 1 = visible.
*/
void caosVM::c_SHOW() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(visibility)
	caos_assert((visibility == 0) || (visibility == 1));
	valid_agent(targ);
	targ->visible = visibility;
}

/**
 SHOW (integer)
 %status maybe
*/
void caosVM::v_SHOW() {
	valid_agent(targ);
	result.setInt(targ->visible ? 1 : 0);
}

/**
 POSX (float)
 %status maybe
 %pragma variants c2 cv c3 sm

 Returns the X position of the TARG agent in the world.
*/
void caosVM::v_POSX() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setFloat(targ->x + (targ->getWidth() / 2.0f));
}

/**
 POSY (float)
 %status maybe
 %pragma variants c2 cv c3 sm

 Returns the Y position of the TARG agent in the world.
*/
void caosVM::v_POSY() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setFloat(targ->y + (targ->getHeight() / 2.0f));
}

/**
 FRAT (command) framerate (integer)
 %status maybe

 Sets the delay between frame changes of the TARG agent or current PART.
 Must be from 1 to 255, 1 being the normal rate, 2 being half as quickly, and so on.
*/
void caosVM::c_FRAT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(framerate)

	caos_assert(framerate >= 1 && framerate <= 255);
	valid_agent(targ);

	SpritePart *p = getCurrentSpritePart();
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

			CompoundPart *s = targ->part(part);
			caos_assert(s);
			AnimatablePart *p = dynamic_cast<AnimatablePart *>(s);
			caos_assert(p);
			
			fno = p->getFrameNo();
			animsize = p->animation.size();

			if (fno == animsize) blocking = false;
			else if (animsize == 0) blocking = false;
			else blocking = true; 
			return blocking;
		}
};
  

/**
 OVER (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Waits (blocks the TARG agent) until the animation of the TARG agent or PART is over.
*/
void caosVM::c_OVER() {
	valid_agent(targ);

	// TODO: The Burrows uses OVER in install script, so fuzzie's making this optional for now, but is this right?
	if (owner)
		startBlocking(new blockUntilOver(targ, part));
}

/**
 PUHL (command) pose (integer) x (integer) y (integer)
 %status maybe

 Sets relative x/y coordinates for TARG's pickup point.
 Pose is -1 for all poses, or a pose relative to the first image specified in NEW: (not BASE).
*/
void caosVM::c_PUHL() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	valid_agent(targ);

	if (pose == -1) {
		SpritePart *s = dynamic_cast<SpritePart *>(targ->part(0));
		if (s) {
			for (unsigned int i = 0; i < (s->getFirstImg() + s->getSprite()->numframes()); i++) {
				targ->carried_points[i] = std::pair<int, int>(x, y);
			}
		} else {
			// ..Assume a single pose for non-sprite parts.
			targ->carried_points[0] = std::pair<int, int>(x, y);
		}
	} else {
		targ->carried_points[pose] = std::pair<int, int>(x, y);
	}
}

/**
 SETV PUHL (command) pose (integer) x (integer) y (integer)
 %status maybe
 %pragma variants c2

 Sets relative x/y coordinates for TARG's pickup point.
 Pose is -1 for all poses, or a pose relative to the first image specified in NEW: (not BASE).
*/
void caosVM::c_SETV_PUHL() {
	c_PUHL();
}

/**
 PUHL (integer) pose (integer) x_or_y (integer)
 %status maybe

 Returns the coordinate for TARG's pickup point. x_or_y should be 1 for x, or 2 for y.
*/
void caosVM::v_PUHL() {
	VM_PARAM_INTEGER(x_or_y)
	VM_PARAM_INTEGER(pose)

	valid_agent(targ);

	// TODO: this creates the variable if it doesn't exist yet, correct behaviour?
	if (x_or_y == 1) {
		result.setInt(targ->carried_points[pose].first);
	} else {
		caos_assert(x_or_y == 2);
		result.setInt(targ->carried_points[pose].second);
	}
}

/**
 POSL (float)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the position of the left side of TARG's bounding box.
*/
void caosVM::v_POSL() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setFloat(targ->x);
}

/**
 POST (float)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the position of the top side of TARG's bounding box.
*/
void caosVM::v_POST() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setFloat(targ->y);
}

/**
 POSR (float)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the position of the right side of TARG's bounding box.
*/
void caosVM::v_POSR() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setFloat(targ->x + targ->getWidth());
}

/**
 POSB (float)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the position of the bottom side of TARG's bounding box.
*/
void caosVM::v_POSB() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setFloat(targ->y + targ->getHeight());
}

/**
 WDTH (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the TARG agent's width.
*/
void caosVM::v_WDTH() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setInt(targ->getWidth());
}

/**
 PLNE (command) depth (integer)
 %status maybe

 Sets the plane (the z-order) of the TARG agent.  Higher values are closer to the camera.
*/
void caosVM::c_PLNE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(depth)

	valid_agent(targ);
	targ->setZOrder(depth);
}

/**
 TINT (command) red_tint (integer) green_tint (integer) blue_tint (integer) rotation (integer) swap (integer)
 %status maybe

 Sets the tinting of the TARG agent to the given red, blue, and green values.
*/
void caosVM::c_TINT() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(swap)
	VM_PARAM_INTEGER(rotation)
	VM_PARAM_INTEGER(blue_tint)
	VM_PARAM_INTEGER(green_tint)
	VM_PARAM_INTEGER(red_tint)

	caos_assert(red_tint >= 0 && red_tint <= 256);
	caos_assert(green_tint >= 0 && green_tint <= 256);
	caos_assert(blue_tint >= 0 && blue_tint <= 256);
	caos_assert(swap >= 0 && swap <= 256);
	caos_assert(rotation >= 0 && rotation <= 256);

	SpritePart *p = getCurrentSpritePart();
	caos_assert(p);
	p->tint(red_tint, green_tint, blue_tint, rotation, swap);
}

/**
 RNGE (command) distance (float)
 %status maybe

 Sets the TARG agent's range (i.e., the distance it can see and hear).
*/
void caosVM::c_RNGE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(distance)

	valid_agent(targ);
	targ->range = distance;
}

/**
 RNGE (integer)
 %status maybe

 Returns the TARG agent's range.
*/
void caosVM::v_RNGE() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setFloat(targ->range);
}

/**
 RNGE (variable)
 %status maybe
 %pragma variants c2

 Returns the TARG agent's range.
*/
CAOS_LVALUE_TARG_SIMPLE(RNGE_c2, targ->range)

/**
 TRAN (integer) x (integer) y (integer)
 %status maybe

 Tests if the pixel at (x,y) on the TARG agent is transparent.
 Returns 0 or 1.
*/
void caosVM::v_TRAN() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	valid_agent(targ);
	CompoundPart *s = targ->part(0); assert(s);
	SpritePart *p = dynamic_cast<SpritePart *>(s);
	caos_assert(p);
	caos_assert(x >= 0 && x <= (int)p->getWidth());
	caos_assert(y >= 0 && y <= (int)p->getHeight());
	if (p->transparentAt(x, y))
		result.setInt(1);
	else
		result.setInt(0);
}
	
/**
 TRAN (command) transparency (integer) part_no (integer)
 %status maybe

 Sets the TARG agent's behaviour with regard to transparency.  If set to 1, invisible 
 parts of the agent can't be clicked.  If 0, anywhere on the agent (including transparent 
 parts) can be clicked.
*/
void caosVM::c_TRAN() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(part_no)
	VM_PARAM_INTEGER(transparency)

	valid_agent(targ);
	// TODO: handle -1?
	CompoundPart *s = targ->part(part_no);
	caos_assert(s);
	SpritePart *p = dynamic_cast<SpritePart *>(s);
	caos_assert(p);
	p->is_transparent = transparency;
}

/**
 HGHT (integer)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns the TARG agent's height.
*/
void caosVM::v_HGHT() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);

	result.setInt(targ->getHeight());
}

/**
 HAND (string)
 %status maybe

 Returns the name of the Hand; default is 'hand'.
*/
void caosVM::v_HAND() {
	VM_VERIFY_SIZE(0)

	result.setString(world.hand()->name);
}

/**
 HAND (command) name (string)
 %status maybe

 Sets the name of the Hand.
*/
void caosVM::c_HAND() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)

	world.hand()->name = name;
}

/**
 TICK (integer)
 %status maybe
 %pragma variants c2 cv c3 sm

 Return the agent timer tick rate of the TARG agent.
*/
void caosVM::v_TICK() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setInt(targ->timerrate);
}

/**
 PUPT (command) pose (integer) x (integer) y (integer)
 %status maybe

 Sets relative x/y coordinates for the location in the world where the TARG agent picks up 
 objects.  The pose is relative to the first image set in NEW: (not BASE).
*/
void caosVM::c_PUPT() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	valid_agent(targ);
	
	// this is basically a copy of PUHL, change that first
	if (pose == -1) {
		SpritePart *s = dynamic_cast<SpritePart *>(targ->part(0));
		if (s) {
			for (unsigned int i = 0; i < (s->getFirstImg() + s->getSprite()->numframes()); i++) {
				targ->carry_points[i] = std::pair<int, int>(x, y);
			}
		} else {
			// ..Assume a single pose for non-sprite parts.
			targ->carry_points[0] = std::pair<int, int>(x, y);
		}
	} else {
		targ->carry_points[pose] = std::pair<int, int>(x, y);
	}
}

/**
 SETV PUPT (command) pose (integer) x (integer) y (integer)
 %status maybe
 %pragma variants c2
*/
void caosVM::c_SETV_PUPT() {
	c_PUPT();
}

/**
 STPT (command)
 %status maybe

 Stop the script running in TARG, if any.
*/
void caosVM::c_STPT() {
	valid_agent(targ);
	targ->stopScript();
}

/**
 DCOR (command) core_on (integer)
 %status done

 %pragma variants all

 Turns the display of the TARG agent's physical core on and off. Gives a general idea of 
 its size and location (including invisible agents).
*/
void caosVM::c_DCOR() {
	VM_PARAM_INTEGER(core_on)

	valid_agent(targ);
	targ->displaycore = core_on;
}

/**
 MIRA (integer)
 %status maybe

 Determines whether or not the TARG agent's current sprite is mirrored. Returns 0 or 1.
*/
void caosVM::v_MIRA() {
	valid_agent(targ);
	
	// TODO: correct?
	SpritePart *p = getCurrentSpritePart();
	caos_assert(p);

	result.setInt(p->draw_mirrored);
}
 
/**
 MIRA (command) mirror_on (integer)
 %status maybe

 Turns mirroring of the TARG agent's current sprite on or off (0 or 1).
*/
void caosVM::c_MIRA() {
	VM_PARAM_INTEGER(mirror_on)

	valid_agent(targ);

	// TODO: what does 'current sprite' mean?
	// TODO: correct?
	SpritePart *p = getCurrentSpritePart();
	caos_assert(p);

	p->draw_mirrored = mirror_on;
}

/**
 DISQ (float) other (agent)
 %status maybe

 Calculates the square of the distance between the centers of the TARG agent and the given 
 agent.
*/
void caosVM::v_DISQ() {
	VM_PARAM_VALIDAGENT(other)

	valid_agent(targ);
	
	float x = (targ->x + (targ->getWidth() / 2.0f)) - (other->x + (other->getWidth() / 2.0f));
	float y = (targ->y + (targ->getHeight() / 2.0f)) - (other->y + (other->getHeight() / 2.0f));

	result.setFloat(x*x + y*y);
}

/**
 ALPH (command) alpha_value (integer) enable (integer)
 %status maybe

 Sets the degree of alpha blending on the TARG agent, to a value from 0 (solid) to 256 
 (invisible). The second parameter will turn alpha blending on and off.
*/
void caosVM::c_ALPH() {
	VM_PARAM_INTEGER(enable)
	VM_PARAM_INTEGER(alpha_value)
	
	if (alpha_value < 0) alpha_value = 0;
	else if (alpha_value > 255) alpha_value = 255;

	valid_agent(targ);

	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	if (c && part == -1) {
		for (std::vector<CompoundPart *>::iterator i = c->parts.begin(); i != c->parts.end(); i++) {
			(*i)->has_alpha = enable;
			(*i)->alpha = alpha_value;
		}
	} else {
		CompoundPart *p = targ->part(part);
		caos_assert(p);
		p->has_alpha = enable;
		p->alpha = alpha_value;
	}
}

/**
 HELD (agent)
 %status maybe

 Returns the agent currently held by the TARG agent, or a random one if there are more than one.
*/
void caosVM::v_HELD() {
	valid_agent(targ);

	// TODO: this whole thing perhaps belongs in a virtual function
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	if (v) {
		// TODO: it should be random .. ?
		if (v->passengers.size())
			result.setAgent(v->passengers[0]);
		else
			result.setAgent(0);
	} else {
		result.setAgent(targ->carrying);
	}
}

/**
 GALL (command) spritefile (string) first_image (integer)
 %status maybe

 Changes the sprite file and first image associated with the TARG agent or current PART.
*/
void caosVM::c_GALL() {
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(spritefile)

	SpritePart *p = getCurrentSpritePart();
	caos_assert(p);
	p->changeSprite(spritefile, first_image);
}

/**
 GALL (string)
 %status maybe

 Returns the name of the sprite file associated with the TARG agent or current PART, or a blank string if part# is invalid on a compound agent.
*/
void caosVM::v_GALL() {
	SpritePart *p = getCurrentSpritePart();
	if (p)
		result.setString(p->getSprite()->getName());
	else
		result.setString("");
}

/**
 SEEE (integer) first (agent) second (agent)
 %status maybe

 Returns 1 if the first agent can see the other, or 0 otherwise.
*/
void caosVM::v_SEEE() {
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	if (agentIsVisible(first.get(), second.get()))
		result.setInt(1);
	else
		result.setInt(0);
}

/**
 TINT (integer) attribute (integer)
 %status stub

 Returns the tint value for TARG agent. Pass 1 for red, 2 for blue, 3 for green, 4 for rotation or 5 for swap.
*/
void caosVM::v_TINT() {
	VM_PARAM_INTEGER(attribute)

	valid_agent(targ);
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

/**
 DROP (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Causes the TARG agent to drop what it is carrying in a safe location.
*/
void caosVM::c_DROP() {
	valid_agent(targ);

	// TODO: what exactly are we meant to do here? firing the script directly is perhaps not right, but drops must be instant
	if (targ->carrying) // TODO: valid?
		targ->carrying->fireScript(5, targ, caosValue(), caosValue());

	// TODO: only creatures in c1 (and c2?)
}

AgentRef findNextAgent(AgentRef previous, unsigned char family, unsigned char genus, unsigned short species, bool forward) {
	if (world.agents.size() == 0) return AgentRef(); // shouldn't happen..

	AgentRef firstagent;
	bool foundagent = false;

	std::list<std::shared_ptr<Agent> >::iterator i;
	if (forward)
		i = world.agents.begin();
	else {
		// TODO: i doubt this works
		i = world.agents.end();
		i--;
	}

	// Loop through all the agents.
	while (true) {
		Agent *a = i->get();
		if (a && (a->family == family || family == 0))
			if (a->genus == genus || genus == 0)
				if (a->species == species || species == 0) {
					if (!firstagent) firstagent = a;
					if (foundagent) return AgentRef(a); // This is the agent we want!
					if (a == previous) foundagent = true;
				}
		
		// Step through the list. Break if we need to.
		if (!forward && i == world.agents.begin()) break;
		if (forward) i++; else i--;
		if (forward && i == world.agents.end()) break;
	}
	
	// Either we didn't find the previous agent, or we're at the end. Either way, return the first agent found.
	return firstagent;
}

/**
 NCLS (agent) previous (agent) family (integer) genus (integer) species (integer)
 %status maybe
*/
void caosVM::v_NCLS() {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	VM_PARAM_AGENT(previous)

	result.setAgent(findNextAgent(previous, family, genus, species, true));
}

/**
 PCLS (agent) previous (agent) family (integer) genus (integer) species (integer)
 %status maybe
*/
void caosVM::v_PCLS() {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	VM_PARAM_AGENT(previous)

	result.setAgent(findNextAgent(previous, family, genus, species, false));
}

/**
 TCOR (integer) topy (float) bottomy (float) leftx (float) rightx (float)
 %status stub
*/
void caosVM::v_TCOR() {
	VM_PARAM_FLOAT(rightx)
	VM_PARAM_FLOAT(leftx)
	VM_PARAM_FLOAT(bottomy)
	VM_PARAM_FLOAT(topy)

	result.setInt(0); // TODO
}

/**
 CORE (command) topy (float) bottomy (float) leftx (float) rightx (float)
 %status maybe
*/
void caosVM::c_CORE() {
	VM_PARAM_FLOAT(rightx)
	VM_PARAM_FLOAT(leftx)
	VM_PARAM_FLOAT(bottomy)
	VM_PARAM_FLOAT(topy)

	valid_agent(targ);

	targ->has_custom_core_size = true;
	targ->custom_core_xleft = leftx;
	targ->custom_core_xright = rightx;
	targ->custom_core_ytop = topy;
	targ->custom_core_ybottom = bottomy;
}

/**
 TWIN (agent) source (agent) null_vars (integer)
 %status stub
*/
void caosVM::v_TWIN() {
	VM_PARAM_INTEGER(null_vars)
	VM_PARAM_VALIDAGENT(source)

	result.setAgent(0); // TODO
}

/**
 ACTV (variable)
 %status maybe
 %pragma variants c1 c2
*/
CAOS_LVALUE_TARG_SIMPLE(ACTV, targ->actv)

/**
 THRT (variable)
 %status maybe
 %pragma variants c2
*/
CAOS_LVALUE_TARG_SIMPLE(THRT, targ->thrt)

/**
 SIZE (variable)
 %status stub
 %pragma variants c2
*/
	// TODO: stub because this likely == perm
CAOS_LVALUE_TARG_SIMPLE(SIZE, targ->size)

/**
 GRAV (variable)
 %status maybe
 %pragma variants c2
*/
void caosVM::v_GRAV() {
	valid_agent(targ);
	result.setInt(targ->falling);
}
void caosVM::s_GRAV() {
	VM_PARAM_VALUE(newvalue)
	caos_assert(newvalue.hasInt());

	valid_agent(targ);
	targ->falling = newvalue.getInt();
}

/**
 SETV CLS2 (command) family (integer) genus (integer) species (integer)
 %status maybe
 %pragma variants c2

 Creatures 2 command to set the family, genus and species of an agent.
*/
void caosVM::c_SETV_CLS2() {
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	valid_agent(targ);

	targ->setClassifier(family, genus, species);
}

/**
 SLIM (command)
 %status stub
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_SLIM() {
	// TODO: probably shouldn't do anything, but make sure :)
}

/**
 BHVR (command) click (integer) touch (integer)
 %status stub
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_BHVR_c2() {
	VM_PARAM_INTEGER(touch)
	VM_PARAM_INTEGER(click)

	valid_agent(targ);

	// see Agent::handleClick for details of what these values mean
	switch (click) {
		// clicks have no effect
		case 0: targ->clac[0] = -1; targ->clac[1] = -1; targ->clac[2] = -1; break;
		// monostable
		case 1: targ->clac[0] = 0; targ->clac[1] = -1; targ->clac[2] = -1; break;
		// retriggerable monostable
		case 2: targ->clac[0] = 0; targ->clac[1] = 0; targ->clac[2] = -1; break;
		// toggle
		case 3: targ->clac[0] = 0; targ->clac[1] = 2; targ->clac[2] = -1; break;
		// cycle
		case 4: targ->clac[0] = 0; targ->clac[1] = 1; targ->clac[2] = 2; break;
		default:
			// C2, at least, seems to produce random garbage (going off the end of a
			// lookup table?) in this situation .. let's not
			throw creaturesException(fmt::format("silly BHVR click value: {}", click));
	}

	// TODO
}

/**
 SETV CLAS (command) identifier (integer)
 %status maybe
 %pragma variants c1

 Set family, genus and species of the target agent. Creatures 1 era command.
*/
void caosVM::c_SETV_CLAS() {
	VM_PARAM_INTEGER(identifier)

	valid_agent(targ);

	targ->setClassifier(
		(identifier >> 24) & 0xff,
		(identifier >> 16) & 0xff,
		(identifier >> 8) & 0xff
		);
}

/**
 LIML (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::v_LIML() {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(targ);

	if (targ->invehicle) {
		Vehicle *v = dynamic_cast<Vehicle *>(targ->invehicle.get()); assert(v);
		result.setInt((int)v->x + v->cabinleft);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(targ->x + (targ->getWidth() / 2.0f), targ->y + (targ->getHeight() / 2.0f));

		if (r) result.setInt(r->x_left);
		else result.setInt(0);
	}
}

/**
 LIMT (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::v_LIMT() {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(targ);

	if (targ->invehicle) {
		Vehicle *v = dynamic_cast<Vehicle *>(targ->invehicle.get()); assert(v);
		result.setInt((int)v->y + v->cabintop);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(targ->x + (targ->getWidth() / 2.0f), targ->y + (targ->getHeight() / 2.0f));

		if (r) result.setInt(r->y_left_ceiling);
		else result.setInt(0);
	}
}

/**
 LIMR (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::v_LIMR() {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(targ);

	if (targ->invehicle) {
		Vehicle *v = dynamic_cast<Vehicle *>(targ->invehicle.get()); assert(v);
		result.setInt((int)v->x + v->cabinright);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(targ->x + (targ->getWidth() / 2.0f), targ->y + (targ->getHeight() / 2.0f));

		if (r) result.setInt(r->x_right);
		else result.setInt(8352); // TODO
	}
}

/**
 LIMB (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::v_LIMB_c1() {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(targ);

	if (targ->invehicle) {
		Vehicle *v = dynamic_cast<Vehicle *>(targ->invehicle.get()); assert(v);
		result.setInt((int)v->y + v->cabinbottom);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(targ->x + (targ->getWidth() / 2.0f), targ->y + (targ->getHeight() / 2.0f));

		if (r) result.setInt(r->y_left_floor);
		else result.setInt(1200); // TODO
	}
}

/**
 OBJP (variable)
 %status maybe
 %pragma variants c1 c2
*/
	// TODO: c1 scripts seem to depend on this being from OWNR, but is that always the case?
CAOS_LVALUE_WITH_SIMPLE(OBJP, owner, owner->objp)

/**
 XIST (integer) agent (agent)
 %status maybe
 %pragma variants c2

 Undocumented C2 command; returns 1 if specified agent exists, or 0 otherwise (ie, if it is null).
*/
void caosVM::v_XIST() {
	VM_PARAM_AGENT(agent)

	if (agent.get())
		result.setInt(1);
	else
		result.setInt(0);
}

/**
 SCLE (command) pose (integer) scaleby (integer)
 %status stub
*/
void caosVM::c_SCLE() {
	VM_PARAM_INTEGER(scaleby)
	VM_PARAM_INTEGER(pose)

	valid_agent(targ);

	// TODO
}

/**
 STRC (command) width (integer) height (integer) enable (integer)
 %status stub

 Draw the current agent (or part? don't know) with the given width/height (ie, stretch the sprite). Set enable to 1 to enable, or 0 to disable.
*/
void caosVM::c_STRC() {
	VM_PARAM_INTEGER(enable)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)

	valid_agent(targ);

	// TODO
}

/**
 IMGE (command) filename (string)
 %status stub
*/
void caosVM::c_IMGE() {
	VM_PARAM_STRING(filename)

	// TODO
}

/**
 TNTW (command) index (integer)
 %status stub
*/
void caosVM::c_TNTW() {
	VM_PARAM_INTEGER(index)

	// TODO
}

/**
 PRNT (command) pose (integer)
 %status stub
*/
void caosVM::c_PRNT() {
	VM_PARAM_INTEGER(pose)

	// TODO
}

/**
 TCAR (agent)
 %status maybe
 %pragma variants c2
*/
void caosVM::v_TCAR() {
	valid_agent(targ);

	if (targ->invehicle)
		result.setAgent(targ->invehicle);
	else
		result.setAgent(targ->carriedby);
}

/**
 EDIT (command)
 %status maybe
 %pragma variants c1 c2

 Attach the target agent to the mouse cursor for positioning purposes.
*/
void caosVM::c_EDIT() {
	valid_agent(targ);

	world.hand()->editAgent = targ;
}

/**
 FRZN (variable)
 %status maybe
 %pragma variants c2
*/
void caosVM::v_FRZN() {
	valid_agent(targ);
	caosValue r = targ->frozen ? 1 : 0;
	valueStack.push_back(r);
}
void caosVM::s_FRZN() {
	VM_PARAM_VALUE(newvalue)
	caos_assert(newvalue.hasInt());
	
	valid_agent(targ);
	targ->frozen = newvalue.getInt();
}

/**
 BLCK (command) width (integer) height (integer)
 %status maybe
 %pragma variants sm
*/
void caosVM::c_BLCK() {
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)

	SpritePart *p = getCurrentSpritePart();
	caos_assert(p);
	std::shared_ptr<creaturesImage> img = p->getSprite();
	caos_assert(img.get());
	img->setBlockSize(width, height);
}

/**
 SHAD (command) intensity (integer) x (integer) y (integer) enable (integer)
 %status stub
 %pragma variants sm

 Enable/disable drawing of a shadow on the target agent with the specified intensity at the given x/y offset.
*/
void caosVM::c_SHAD() {
	VM_PARAM_INTEGER(enable)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(intensity)

	valid_agent(targ);

	// TODO
}

/**
 SYS: CONV (command) filename (bareword) show_progress (integer)
 %status stub
 %pragma variants c2

 This is supposed to convert the provided image name to the display depth, displaying a progress dialog if show_progress is non-zero.
 However, it does nothing under openc2e.
*/
void caosVM::c_SYS_CONV() {
	VM_PARAM_INTEGER(show_progress)
	VM_PARAM_STRING(filename)
}

/* vim: set noet: */
