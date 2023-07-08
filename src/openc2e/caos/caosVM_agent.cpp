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

#include "AgentHelpers.h"
#include "AnimatablePart.h"
#include "Blackboard.h"
#include "Engine.h" // C2 hack in POSE
#include "Map.h"
#include "PointerAgent.h"
#include "Room.h" // LIML/LIMT etc
#include "SpritePart.h"
#include "Vehicle.h"
#include "World.h"
#include "caosVM.h"
#include "common/Random.h"
#include "common/creaturesImage.h"
#include "common/throw_ifnot.h"
#include "creatures/SkeletalCreature.h"

#include <cassert>
#include <climits>
#include <fmt/core.h>
#include <memory>

AnimatablePart* caosVM::getCurrentAnimatablePart() {
	valid_agent(targ);
	CompoundPart* p = targ->part(part);
	if (!p)
		return 0;
	AnimatablePart* s = dynamic_cast<AnimatablePart*>(p);
	return s;
}

SpritePart* caosVM::getCurrentSpritePart() {
	valid_agent(targ);
	CompoundPart* p = targ->part(part);
	if (!p)
		return 0;
	SpritePart* s = dynamic_cast<SpritePart*>(p);
	return s;
}

/**
 TOUC (integer) first (agent) second (agent)
 %status maybe
 %variants c1 c2 cv c3 sm

 Determines whether the two given agents are touching.  Returns 0 (if not) or 1 (if so).
*/
void v_TOUC(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(second)
	VM_PARAM_AGENT(first)

	if (first && second && agentsTouching(first.get(), second.get()))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 RTAR (command) family (integer) genus (integer) species (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Sets TARG to a random agent with the given family/genus/species.
 */
void c_RTAR(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	vm->setTarg(0);

	/* XXX: maybe use a map of classifier -> agents? */
	std::vector<std::shared_ptr<Agent> > temp;
	for (auto& agent : world.agents) {
		Agent* a = agent.get();
		if (!a)
			continue;

		if (species && species != a->species)
			continue;
		if (genus && genus != a->genus)
			continue;
		if (family && family != a->family)
			continue;

		temp.push_back(agent);
	}

	if (temp.size() == 0)
		return;
	vm->setTarg(rand_choice(temp));
}

/**
 TTAR (command) family (integer) genus (integer) species (integer)
 %status maybe

 Locates a random agent that is touching OWNR (see ETCH) and that 
 matches the given classifier, and sets it to TARG.
*/
void c_TTAR(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	valid_agent(vm->owner);

	vm->setTarg(0);

	/* XXX: maybe use a map of classifier -> agents? */
	std::vector<std::shared_ptr<Agent> > temp;
	for (auto& agent : world.agents) {
		Agent* a = agent.get();
		if (!a)
			continue;

		if (species && species != a->species)
			continue;
		if (genus && genus != a->genus)
			continue;
		if (family && family != a->family)
			continue;

		if (agentsTouching(vm->owner, a))
			temp.push_back(agent);
	}

	if (temp.size() == 0)
		return;
	vm->setTarg(rand_choice(temp));
}

/**
 STAR (command) family (integer) genus (integer) species (integer)
 %status maybe
 %variants c2 cv c3 sm

 Locates a random agent that is visible to OWNR (see ESEE) and that
 matches the given classifier, then sets it to TARG.
*/
void c_STAR(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	valid_agent(vm->targ);

	Agent* seeing;
	if (vm->owner)
		seeing = vm->owner;
	else
		seeing = vm->targ;
	valid_agent(seeing);

	std::vector<std::shared_ptr<Agent> > agents = getVisibleList(seeing, family, genus, species);
	if (agents.size() == 0) {
		vm->setTarg(0);
	} else {
		vm->setTarg(rand_choice(agents));
	}
}

/**
 NEW: SIMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new simple agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, at the screen depth given by plane.
 TARG is set to the newly-created agent.
*/
void c_NEW_SIMP(caosVM* vm) {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	SimpleAgent* a = new SimpleAgent(family, genus, species, plane, sprite_file, first_image, image_count);
	a->finishInit();
	vm->setTarg(a);
	vm->part = 0; // TODO: correct?
}

/**
 NEW: SIMP (command) sprite_file (bareword) image_count (integer) first_image (integer) plane (integer) clone (integer)
 %status maybe
 %variants c1 c2
*/
void c_NEW_SIMP_c2(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(clone)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	// TODO: we ignore clone
	// TODO: should we init with 0/0/0 or with a different constructor?
	SimpleAgent* a = new SimpleAgent(0, 0, 0, plane, sprite_file, first_image, image_count);
	a->finishInit();
	vm->setTarg(a);
	vm->part = 0; // TODO: correct?
}

/**
 NEW: COMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new composite agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane.
*/
void c_NEW_COMP(caosVM* vm) {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	CompoundAgent* a = new CompoundAgent(family, genus, species, plane, sprite_file, first_image, image_count);
	a->finishInit();
	vm->setTarg(a);
	vm->part = 0; // TODO: correct?
}

/**
 NEW: COMP (command) sprite_file (bareword) image_count (integer) first_image (integer) clone (integer)
 %status maybe
 %variants c1 c2
*/
void c_NEW_COMP_c1(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(clone)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	// TODO: what does clone do?
	CompoundAgent* a = new CompoundAgent(sprite_file, first_image, image_count);
	a->finishInit();
	vm->setTarg(a);
	// TODO: should part be set here?
}

/**
 NEW: VHCL (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new vehicle agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane.
*/
void c_NEW_VHCL(caosVM* vm) {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	Vehicle* a = new Vehicle(family, genus, species, plane, sprite_file, first_image, image_count);
	a->finishInit();
	vm->setTarg(a);
	vm->part = 0; // TODO: correct?
}

/**
 NEW: VHCL (command) sprite_file (bareword) image_count (integer) first_image (integer)
 %status maybe
 %variants c1 c2
*/
void c_NEW_VHCL_c1(caosVM* vm) {
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	Vehicle* a = new Vehicle(sprite_file, first_image, image_count);
	a->finishInit();
	vm->setTarg(a);
	// TODO: should part be set here?
}

/**
 NEW: BKBD (command) sprite_file (bareword) image_count (integer) first_image (integer) background_colour (integer) chalk_colour (integer) alias_colour (integer) textx (integer) texty (integer)
 %status maybe
 %variants c1 c2
*/
void c_NEW_BKBD(caosVM* vm) {
	VM_PARAM_INTEGER(texty)
	VM_PARAM_INTEGER(textx)
	VM_PARAM_INTEGER(alias_colour)
	VM_PARAM_INTEGER(chalk_colour)
	VM_PARAM_INTEGER(background_colour)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)

	Blackboard* a = new Blackboard(sprite_file, first_image, image_count, textx, texty, background_colour, chalk_colour, alias_colour);
	a->finishInit();
	vm->setTarg(a);
}

/**
 NEW: CBUB (command) sprite_file (bareword) image_count (integer) first_image (integer) stringid (integer)
 %status maybe
 %variants c2
*/
void c_NEW_CBUB(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(stringid)
	VM_PARAM_INTEGER_UNUSED(first_image)
	VM_PARAM_INTEGER_UNUSED(image_count)
	VM_PARAM_STRING(sprite_file)

	throw Exception("compound bubble objects are not supported yet"); // TODO
}

/**
 TARG (agent)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns TARG, the currently-targeted agent.
*/
void v_TARG(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	vm->result.setAgent(vm->targ);
}

/**
 OWNR (agent)
 %status maybe
 %variants c1 c2 cv c3 sm
 
 Returns OWNR, the agent that is running the script.
*/
void v_OWNR(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	vm->result.setAgent(vm->owner);
}

/**
 NULL (agent)
 %status maybe

 Returns a null (zero) agent.
*/
void v_NULL(caosVM* vm) {
	static const AgentRef nullref;
	VM_VERIFY_SIZE(0)
	vm->result.setAgent(nullref);
}

/**
 POSE (command) pose (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Sets the displayed sprite of TARG to the frame in the sprite file with the given integer.
*/
void c_POSE(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pose)

	AnimatablePart* p = vm->getCurrentAnimatablePart();
	THROW_IFNOT(p);
	p->animation.clear();
	p->setPose(pose);
}

/**
 ATTR (command) attr (integer)
 %status maybe

 Sets attributes of the TARG agent.
*/
void c_ATTR(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(attr)
	valid_agent(vm->targ);

	bool oldfloat = vm->targ->floatable();
	vm->targ->setAttributes(attr);

	// TODO: this is an icky hack to enable floating, we should write correct floating
	// behaviour so we don't need to maintain floating lists like this :/
	if (oldfloat != vm->targ->floatable()) {
		if (vm->targ->floatable())
			vm->targ->floatSetup();
		else
			vm->targ->floatRelease();
	}
}

/**
 ATTR (variable)
 %status maybe
 %variants c1 c2 cv c3 sm

 Attributes of the TARG agent.
*/
void v_ATTR(caosVM* vm) {
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->getAttributes());
}
void s_ATTR(caosVM* vm) {
	VM_PARAM_VALUE(newvalue)
	THROW_IFNOT(newvalue.hasInt());

	valid_agent(vm->targ);
	vm->targ->setAttributes(newvalue.getInt());
}

/**
 TICK (command) tickrate (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Initiates the agent timer-- the Timer script will then be run once every tickrate ticks.
 Setting tickrate to zero will stop the timer.
*/
void c_TICK(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(tickrate)
	valid_agent(vm->targ);
	vm->targ->setTimerRate(tickrate);
}

/**
 BHVR (command) bhvr (integer)
 %status maybe

 Sets the behaviour of the TARG agent.
*/
void c_BHVR(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(bhvr)

	valid_agent(vm->targ);

	// reset bhvr
	vm->targ->cr_can_push = vm->targ->cr_can_pull = vm->targ->cr_can_stop =
		vm->targ->cr_can_hit = vm->targ->cr_can_eat = vm->targ->cr_can_pickup = false;

	if (bhvr & 0x1) // creature can push
		vm->targ->cr_can_push = true;
	if (bhvr & 0x2) // creature can pull
		vm->targ->cr_can_pull = true;
	if (bhvr & 0x4) // creature can stop
		vm->targ->cr_can_stop = true;
	if (bhvr & 0x8) // creature can hit
		vm->targ->cr_can_hit = true;
	if (bhvr & 0x10) // creature can eat
		vm->targ->cr_can_eat = true;
	if (bhvr & 0x20) // creature can pick up
		vm->targ->cr_can_pickup = true;
}

/**
 TARG (command) agent (agent)
 %status maybe
 %variants c1 c2 cv c3 sm

 Sets TARG (the target agent) to the given agent.
*/
void c_TARG(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	vm->setTarg(a);
}

/**
 FROM (agent)
 %status maybe
 %variants c1 c2 cv sm

 Returns the agent that sent the message being processed, or NULL if no agent was involved.
*/
void v_FROM(caosVM* vm) {
	vm->result.setAgent(vm->from.getAgent());
}

/**
 FROM (variable)
 %status maybe
 %variants c3

 Returns the agent that sent the message being processed, or NULL if no agent was involved.
*/
// Returns a variable because DS is insane and uses this for network events too (and so, of course, scripts abuse it).
CAOS_LVALUE_SIMPLE(FROM_ds, vm->from)

/**
 POSE (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the number of the frame in the TARG part/agent's sprite file that is currently being displayed, or -1 if part# doesn't exist on a compound agent.
*/
void v_POSE(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);

	SpritePart* p = vm->getCurrentSpritePart();
	if (p)
		vm->result.setInt(p->getPose());
	else
		vm->result.setInt(-1);
}

/**
 KILL (command) agent (agent)
 %status maybe
 %variants c1 c2 cv c3 sm

 Destroys the agent in question. However, you cannot destroy PNTR.
 Remember, use DEAD first for Creatures!
*/
void c_KILL(caosVM* vm) {
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
void c_ANIM(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_BYTESTR(bs)

	valid_agent(vm->targ);

	AnimatablePart* p = vm->getCurrentAnimatablePart();
	THROW_IFNOT(p);
	p->animation = bs;

	if (!bs.empty())
		p->setFrameNo(0); // TODO: correct?
}

/**
 ANIM (command) animstring (string)
 %status maybe
 %variants c1 c2

 Sets the animation string for TARG, in the format '1234'.
 If it ends with 'R', loops back to the beginning.
*/
void c_ANIM_c2(caosVM* vm) {
	VM_PARAM_STRING(animstring)

	valid_agent(vm->targ);

	// TODO: support creatures (using AnimatablePart, see c2e ANIM) for this (with format like "001002003R")
	if (dynamic_cast<SkeletalCreature*>(vm->targ.get()))
		return;

	SpritePart* p = vm->getCurrentSpritePart();
	THROW_IFNOT(p);

	p->animation.clear();

	for (char i : animstring) {
		if (i == 'R') {
			p->animation.push_back(255);
		} else if (i >= 48 && i <= 57) {
			p->animation.push_back(i - 48);
		} else if (i >= 97 && i <= 122) {
			// TODO: c1 grendel eggs have 'a' at the end of their animation strings, this is an untested attempt to handle that
			p->animation.push_back(i - 97);
		} else {
			p->animation.clear();
			throw Exception(std::string("old-style animation string contained '") + i + "', which we didn't understand");
		}
	}

	if (!animstring.empty())
		p->setFrameNo(0); // TODO: correct?
}

/**
 ANMS (command) poselist (string)
 %status maybe

 Exactly like ANIM, only using a string and not a bytestring for poselist source.
*/
void c_ANMS(caosVM* vm) {
	VM_PARAM_STRING(poselist)

	// TODO: technically, we should parse this properly, also do error checking
	bytestring_t animation;

	std::string t;
	for (char i : poselist) {
		if (i == ' ') {
			if (!t.empty()) {
				int n = atoi(t.c_str());
				THROW_IFNOT(n >= 0 && n < 256);
				animation.push_back(n);
				t.clear();
			}
		} else
			t = t + i;
	}

	AnimatablePart* p = vm->getCurrentAnimatablePart();
	THROW_IFNOT(p);
	p->animation = animation;

	if (!animation.empty())
		p->setFrameNo(0); // TODO: correct?
}

/**
 ABBA (integer)
 %status maybe

 Returns the first_image (ie, absolute base) value for the current agent/part, or -1 if part# doesn't exist on a compound agent.
*/
void v_ABBA(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	SpritePart* p = vm->getCurrentSpritePart();
	if (p)
		vm->result.setInt(p->getFirstImg());
	else
		vm->result.setInt(-1);
}

/**
 BASE (command) index (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Sets the frame in the TARG agent's spritefile that will be used as its base image.
 This is relative to the first image set with one of the NEW: commands.
*/
void c_BASE(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(index)

	valid_agent(vm->targ);

	SpritePart* p = vm->getCurrentSpritePart();
	THROW_IFNOT(p);
	// Note that we don't check validity here because a lot of code changes BASE and then only afterwards POSE/ANIM.
	p->setBase(index);
}

/**
 BASE (integer)
 %status maybe

 Returns the frame in the TARG agent/part's spritefile being used as the BASE image, or -1 if part# doesn't exist on a compound agent.
*/
void v_BASE(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	SpritePart* p = vm->getCurrentSpritePart();
	if (p)
		vm->result.setInt(p->getBase());
	else
		vm->result.setInt(-1);
}

/**
 BHVR (integer)
 %status maybe

 Returns the behaviour of the TARG agent.
*/
void v_BHVR(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);

	unsigned char bvr = 0;

	if (vm->targ->cr_can_push)
		bvr += 0x1;
	if (vm->targ->cr_can_pull)
		bvr += 0x2;
	if (vm->targ->cr_can_stop)
		bvr += 0x4;
	if (vm->targ->cr_can_hit)
		bvr += 0x8;
	if (vm->targ->cr_can_eat)
		bvr += 0x10;
	if (vm->targ->cr_can_pickup)
		bvr += 0x20;

	vm->result.setInt(bvr);
}

/**
 CARR (agent)
 %status maybe

 Returns the agent that is carrying the TARG agent.  If TARG is not being carried, returns 
 NULL. 
*/
void v_CARR(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);

	// TODO: muh, should totally be virtual
	if (vm->targ->invehicle)
		vm->result.setAgent(vm->targ->invehicle);
	else
		vm->result.setAgent(vm->targ->carriedby);
}

/**
 CARR (agent)
 %status maybe
 %variants c1 c2

 Returns the agent that is carrying the OWNR agent.  If OWNR is not being carried, returns 
 NULL. 
*/
void v_CARR_c1(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->owner);

	if (vm->owner->invehicle)
		vm->result.setAgent(vm->owner->invehicle);
	else
		vm->result.setAgent(vm->owner->carriedby);
}

/**
 FMLY (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the family of the TARG agent.
*/
void v_FMLY(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->family);
}

/**
 GNUS (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the genus of the TARG agent.
*/
void v_GNUS(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->genus);
}

/**
 SPCS (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the species of the TARG agent.
*/
void v_SPCS(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->species);
}

/**
 PLNE (integer)
 %status maybe

 Returns the plane (z-order) of the TARG agent.
*/
void v_PLNE(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->getZOrder());
}

/**
 PNTR (agent)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the pointer agent (the Hand).
*/
void v_PNTR(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	vm->result.setAgent(world.hand());
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
 %variants c1 c2 cv c3 sm

 Sends a message of type message_id to the given agent.  FROM will be set to OWNR unless 
 there is no agent involved in sending the message.
*/
void c_MESG_WRIT(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent);

	agent->queueScript(calculateScriptId(message_id), vm->owner.get());
}

/**
 MESG WRT+ (command) agent (agent) message_id (integer) param_1 (anything) param_2 (anything) delay (integer)
 %status maybe
 %variants c2 cv c3 sm

 Sends a message of type message_id to the given agent, much like MESG WRIT, but with the 
 addition of parameters.  The message will be sent after waiting the number of ticks set 
 in delay (except doesn't, right now.  Delay must be set to zero for now.)
*/
void c_MESG_WRT(caosVM* vm) {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER_UNUSED(delay)
	VM_PARAM_VALUE(param_2)
	VM_PARAM_VALUE(param_1)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent)

	// I'm not sure how to handle the 'delay'; is it a background delay, or do we actually block for delay ticks?
	// TODO: fuzzie can't work out how on earth delays work in c2e, someone fixit

	agent->queueScript(calculateScriptId(message_id), vm->owner.get(), param_1, param_2);
}

/**
 TOTL (integer) family (integer) genus (integer) species (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the total number of in-game agents matching the given family/genus/species.
*/
void v_TOTL(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	unsigned int x = 0;
	for (auto& agent : world.agents) {
		if (!agent)
			continue;
		if (agent->family == family || family == 0)
			if (agent->genus == genus || genus == 0)
				if (agent->species == species || species == 0)
					x++;
	}
	vm->result.setInt(x);
}

/**
 SHOW (command) visibility (integer)
 %status maybe
 
 Sets visibility of the TARG agent to cameras. 0 = invisible, 1 = visible.
*/
void c_SHOW(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(visibility)
	THROW_IFNOT((visibility == 0) || (visibility == 1));
	valid_agent(vm->targ);
	vm->targ->visible = visibility;
}

/**
 SHOW (integer)
 %status maybe
*/
void v_SHOW(caosVM* vm) {
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->visible ? 1 : 0);
}

/**
 POSX (float)
 %status maybe
 %variants c2 cv c3 sm

 Returns the X position of the TARG agent in the world.
*/
void v_POSX(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->x + (vm->targ->getWidth() / 2.0f));
}

/**
 POSY (float)
 %status maybe
 %variants c2 cv c3 sm

 Returns the Y position of the TARG agent in the world.
*/
void v_POSY(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->y + (vm->targ->getHeight() / 2.0f));
}

/**
 FRAT (command) framerate (integer)
 %status maybe

 Sets the delay between frame changes of the TARG agent or current PART.
 Must be from 1 to 255, 1 being the normal rate, 2 being half as quickly, and so on.
*/
void c_FRAT(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(framerate)

	THROW_IFNOT(framerate >= 1 && framerate <= 255);
	valid_agent(vm->targ);

	SpritePart* p = vm->getCurrentSpritePart();
	THROW_IFNOT(p);
	p->setFramerate(framerate);
	p->framedelay = 0;
}

class blockUntilOver : public blockCond {
  protected:
	AgentRef targ;
	int part;

  public:
	blockUntilOver(Agent* t, int p)
		: targ(t), part(p) {}
	virtual bool operator()() {
		bool blocking;
		int fno, animsize;

		if (!targ)
			return false;

		CompoundPart* s = targ->part(part);
		THROW_IFNOT(s);
		AnimatablePart* p = dynamic_cast<AnimatablePart*>(s);
		THROW_IFNOT(p);

		fno = p->getFrameNo();
		animsize = p->animation.size();

		if (fno == animsize)
			blocking = false;
		else if (animsize == 0)
			blocking = false;
		else
			blocking = true;
		return blocking;
	}
};


/**
 OVER (command)
 %status maybe
 %variants c1 c2 cv c3 sm

 Waits (blocks the TARG agent) until the animation of the TARG agent or PART is over.
*/
void c_OVER(caosVM* vm) {
	valid_agent(vm->targ);

	// TODO: The Burrows uses OVER in install script, so fuzzie's making this optional for now, but is this right?
	if (vm->owner)
		vm->startBlocking(new blockUntilOver(vm->targ, vm->part));
}

/**
 PUHL (command) pose (integer) x (integer) y (integer)
 %status maybe

 Sets relative x/y coordinates for TARG's pickup point.
 Pose is -1 for all poses, or a pose relative to the first image specified in NEW: (not BASE).
*/
void c_PUHL(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	valid_agent(vm->targ);

	if (pose == -1) {
		SpritePart* s = dynamic_cast<SpritePart*>(vm->targ->part(0));
		if (s) {
			for (unsigned int i = 0; i < (s->getFirstImg() + s->getSprite()->numframes()); i++) {
				vm->targ->carried_points[i] = std::pair<int, int>(x, y);
			}
		} else {
			// ..Assume a single pose for non-sprite parts.
			vm->targ->carried_points[0] = std::pair<int, int>(x, y);
		}
	} else {
		vm->targ->carried_points[pose] = std::pair<int, int>(x, y);
	}
}

/**
 SETV PUHL (command) pose (integer) x (integer) y (integer)
 %status maybe
 %variants c2

 Sets relative x/y coordinates for TARG's pickup point.
 Pose is -1 for all poses, or a pose relative to the first image specified in NEW: (not BASE).
*/
void c_SETV_PUHL(caosVM* vm) {
	c_PUHL(vm);
}

/**
 PUHL (integer) pose (integer) x_or_y (integer)
 %status maybe

 Returns the coordinate for TARG's pickup point. x_or_y should be 1 for x, or 2 for y.
*/
void v_PUHL(caosVM* vm) {
	VM_PARAM_INTEGER(x_or_y)
	VM_PARAM_INTEGER(pose)

	valid_agent(vm->targ);

	// TODO: this creates the variable if it doesn't exist yet, correct behaviour?
	if (x_or_y == 1) {
		vm->result.setInt(vm->targ->carried_points[pose].first);
	} else {
		THROW_IFNOT(x_or_y == 2);
		vm->result.setInt(vm->targ->carried_points[pose].second);
	}
}

/**
 POSL (float)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the position of the left side of TARG's bounding box.
*/
void v_POSL(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->x);
}

/**
 POST (float)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the position of the top side of TARG's bounding box.
*/
void v_POST(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->y);
}

/**
 POSR (float)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the position of the right side of TARG's bounding box.
*/
void v_POSR(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->x + vm->targ->getWidth());
}

/**
 POSB (float)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the position of the bottom side of TARG's bounding box.
*/
void v_POSB(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->y + vm->targ->getHeight());
}

/**
 WDTH (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the TARG agent's width.
*/
void v_WDTH(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->getWidth());
}

/**
 PLNE (command) depth (integer)
 %status maybe

 Sets the plane (the z-order) of the TARG agent.  Higher values are closer to the camera.
*/
void c_PLNE(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(depth)

	valid_agent(vm->targ);
	vm->targ->setZOrder(depth);
}

/**
 TINT (command) red_tint (integer) green_tint (integer) blue_tint (integer) rotation (integer) swap (integer)
 %status maybe

 Sets the tinting of the TARG agent to the given red, blue, and green values.
*/
void c_TINT(caosVM* vm) {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(swap)
	VM_PARAM_INTEGER(rotation)
	VM_PARAM_INTEGER(blue_tint)
	VM_PARAM_INTEGER(green_tint)
	VM_PARAM_INTEGER(red_tint)

	THROW_IFNOT(red_tint >= 0 && red_tint <= 256);
	THROW_IFNOT(green_tint >= 0 && green_tint <= 256);
	THROW_IFNOT(blue_tint >= 0 && blue_tint <= 256);
	THROW_IFNOT(swap >= 0 && swap <= 256);
	THROW_IFNOT(rotation >= 0 && rotation <= 256);

	SpritePart* p = vm->getCurrentSpritePart();
	THROW_IFNOT(p);
	p->tint(red_tint, green_tint, blue_tint, rotation, swap);
}

/**
 RNGE (command) distance (float)
 %status maybe

 Sets the TARG agent's range (i.e., the distance it can see and hear).
*/
void c_RNGE(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(distance)

	valid_agent(vm->targ);
	vm->targ->range = distance;
}

/**
 RNGE (integer)
 %status maybe

 Returns the TARG agent's range.
*/
void v_RNGE(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->range);
}

/**
 RNGE (variable)
 %status maybe
 %variants c2

 Returns the TARG agent's range.
*/
CAOS_LVALUE_TARG_SIMPLE(RNGE_c2, vm->targ->range)

/**
 TRAN (integer) x (integer) y (integer)
 %status maybe

 Tests if the pixel at (x,y) on the TARG agent is transparent.
 Returns 0 or 1.
*/
void v_TRAN(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	valid_agent(vm->targ);
	// TODO: check first part? or all parts?
	CompoundPart* s = vm->targ->part(0);
	assert(s);
	SpritePart* p = dynamic_cast<SpritePart*>(s);
	THROW_IFNOT(p);

	if (!(x >= 0 && x < (int)p->getWidth() && y >= 0 && y < (int)p->getHeight())) {
		// Happens in Sea-Monkeys, of course
		// TODO: because we're checking first instead of all parts?
		printf("Warning: TRAN: coordinates out of part's bounds x=%i y=%i width=%i height=%i\n", x, y, (int)p->getWidth(), (int)p->getHeight());
		vm->result.setInt(1);
		return;
	}
	if (p->transparentAt(x, y))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 TRAN (command) transparency (integer) part_no (integer)
 %status maybe

 Sets the TARG agent's behaviour with regard to transparency.  If set to 1, invisible 
 parts of the agent can't be clicked.  If 0, anywhere on the agent (including transparent 
 parts) can be clicked.
*/
void c_TRAN(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(part_no)
	VM_PARAM_INTEGER(transparency)

	valid_agent(vm->targ);
	// TODO: handle -1?
	CompoundPart* s = vm->targ->part(part_no);
	THROW_IFNOT(s);
	SpritePart* p = dynamic_cast<SpritePart*>(s);
	THROW_IFNOT(p);
	p->is_transparent = transparency;
}

/**
 HGHT (integer)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns the TARG agent's height.
*/
void v_HGHT(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);

	vm->result.setInt(vm->targ->getHeight());
}

/**
 HAND (string)
 %status maybe

 Returns the name of the Hand; default is 'hand'.
*/
void v_HAND(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setString(world.hand()->name);
}

/**
 HAND (command) name (string)
 %status maybe

 Sets the name of the Hand.
*/
void c_HAND(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)

	world.hand()->name = name;
}

/**
 TICK (integer)
 %status maybe
 %variants c2 cv c3 sm

 Return the agent timer tick rate of the TARG agent.
*/
void v_TICK(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->timerrate);
}

/**
 PUPT (command) pose (integer) x (integer) y (integer)
 %status maybe

 Sets relative x/y coordinates for the location in the world where the TARG agent picks up 
 objects.  The pose is relative to the first image set in NEW: (not BASE).
*/
void c_PUPT(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	valid_agent(vm->targ);

	// this is basically a copy of PUHL, change that first
	if (pose == -1) {
		SpritePart* s = dynamic_cast<SpritePart*>(vm->targ->part(0));
		if (s) {
			for (unsigned int i = 0; i < (s->getFirstImg() + s->getSprite()->numframes()); i++) {
				vm->targ->carry_points[i] = std::pair<int, int>(x, y);
			}
		} else {
			// ..Assume a single pose for non-sprite parts.
			vm->targ->carry_points[0] = std::pair<int, int>(x, y);
		}
	} else {
		vm->targ->carry_points[pose] = std::pair<int, int>(x, y);
	}
}

/**
 SETV PUPT (command) pose (integer) x (integer) y (integer)
 %status maybe
 %variants c2
*/
void c_SETV_PUPT(caosVM* vm) {
	c_PUPT(vm);
}

/**
 STPT (command)
 %status maybe

 Stop the script running in TARG, if any.
*/
void c_STPT(caosVM* vm) {
	valid_agent(vm->targ);
	vm->targ->stopScript();
}

/**
 DCOR (command) core_on (integer)
 %status done
 %variants c3 cv sm openc2e

 Turns the display of the TARG agent's physical core on and off. Gives a general idea of 
 its size and location (including invisible agents).
*/
void c_DCOR(caosVM* vm) {
	VM_PARAM_INTEGER(core_on)

	valid_agent(vm->targ);
	vm->targ->displaycore = core_on;
}

/**
 MIRA (command) mirror_on (integer)
 %status maybe

 Turns mirroring of the TARG agent's current sprite on or off (0 or 1).
*/
void c_MIRA(caosVM* vm) {
	VM_PARAM_INTEGER(mirror_on)

	valid_agent(vm->targ);
	THROW_IFNOT(mirror_on == 0 || mirror_on == 1);
	vm->targ->draw_mirrored = mirror_on;
}

/**
 MIRA (integer)
 %status maybe

 Determines whether or not the TARG agent's current sprite is mirrored. Returns 0 or 1.
*/
void v_MIRA(caosVM* vm) {
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->draw_mirrored);
}

/**
 FLIP (command) onoroff (integer)
 %status stub
 %variants sm

 Tell the agent to draw the current sprite vertically flipped.
*/
void c_FLIP(caosVM* vm) {
	VM_PARAM_INTEGER(onoroff);
	valid_agent(vm->targ);
	THROW_IFNOT(onoroff == 0 || onoroff == 1);
	// TODO (this isn't actually used anywhere, afaict)
}

/**
 FLIP (integer)
 %status stub
 %variants sm

 Is the current sprite for this agent vertically flipped?
*/
void v_FLIP(caosVM* vm) {
	valid_agent(vm->targ);
	// TODO (this isn't actually used anywhere, afaict)
	vm->result.setInt(0);
}

/**
 ROTA (command) angle (integer)
 %status stub
 %variants sm

 Tell the agent to draw the current sprite rotated clockwise by the given angle a value between 0 and 360.
*/
void c_ROTA(caosVM* vm) {
	VM_PARAM_INTEGER(angle)

	valid_agent(vm->targ);
	THROW_IFNOT(angle >= 0 && angle <= 360);
	// TODO: doesn't actually do anything in original engine?
}

/**
 ROTA (integer)
 %status stub
 %variants sm

 By what angle is the current sprite for this agent rotated returns angle between 0 and 360.
*/
void v_ROTA(caosVM* vm) {
	valid_agent(vm->targ);
	vm->result.setInt(0);
}

/**
 DISQ (float) other (agent)
 %status maybe

 Calculates the square of the distance between the centers of the TARG agent and the given 
 agent.
*/
void v_DISQ(caosVM* vm) {
	VM_PARAM_VALIDAGENT(other)

	valid_agent(vm->targ);

	float x = (vm->targ->x + (vm->targ->getWidth() / 2.0f)) - (other->x + (other->getWidth() / 2.0f));
	float y = (vm->targ->y + (vm->targ->getHeight() / 2.0f)) - (other->y + (other->getHeight() / 2.0f));

	vm->result.setFloat(x * x + y * y);
}

static void alph_impl(caosVM* vm, int32_t transparency, int32_t enable) {
	THROW_IFNOT(transparency >= 0 && transparency <= 256);
	if (transparency == 256) {
		transparency = 255;
	}
	THROW_IFNOT(enable == 0 || enable == 1);
	uint8_t alpha = enable ? 255 - transparency : 255;

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	if (vm->part == -1) {
		for (auto& part : c->parts) {
			part->alpha = alpha;
		}
	} else {
		CompoundPart* p = vm->targ->part(vm->part);
		THROW_IFNOT(p);
		p->alpha = alpha;
	}
}

/**
 ALPH (command) transparency (integer) enable (integer)
 %status maybe
 %variants c3 sm

 Sets the degree of alpha blending on the TARG agent, to a value from 0 (solid) to 256
 (invisible). The second parameter will turn alpha blending on and off.
*/
void c_ALPH(caosVM* vm) {
	VM_PARAM_INTEGER(enable)
	VM_PARAM_INTEGER(transparency)
	alph_impl(vm, transparency, enable);
}

/**
 ALPH (command) enable(integer) transparency (integer)
 %status maybe
 %variants cv

 Sets the degree of alpha blending on the TARG agent, to a value from 0 (solid) to 256
 (invisible). The first parameter will turn alpha blending on and off.
*/
void c_ALPH_cv(caosVM* vm) {
	VM_PARAM_INTEGER(transparency)
	VM_PARAM_INTEGER(enable)
	alph_impl(vm, transparency, enable);
}

/**
 HELD (agent)
 %status maybe

 Returns the agent currently held by the TARG agent, or a random one if there are more than one.
*/
void v_HELD(caosVM* vm) {
	valid_agent(vm->targ);

	// TODO: this whole thing perhaps belongs in a virtual function
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	if (v) {
		// TODO: it should be random .. ?
		if (v->passengers.size())
			vm->result.setAgent(v->passengers[0]);
		else
			vm->result.setAgent(0);
	} else {
		vm->result.setAgent(vm->targ->carrying);
	}
}

/**
 GALL (command) spritefile (string) first_image (integer)
 %status maybe

 Changes the sprite file and first image associated with the TARG agent or current PART.
*/
void c_GALL(caosVM* vm) {
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(spritefile)

	SpritePart* p = vm->getCurrentSpritePart();
	THROW_IFNOT(p);
	p->changeSprite(spritefile, first_image);
}

/**
 GALL (string)
 %status maybe

 Returns the name of the sprite file associated with the TARG agent or current PART, or a blank string if part# is invalid on a compound agent.
*/
void v_GALL(caosVM* vm) {
	SpritePart* p = vm->getCurrentSpritePart();
	if (p)
		vm->result.setString(p->getSprite()->getName());
	else
		vm->result.setString("");
}

/**
 SEEE (integer) first (agent) second (agent)
 %status maybe

 Returns 1 if the first agent can see the other, or 0 otherwise.
*/
void v_SEEE(caosVM* vm) {
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	if (agentIsVisible(first.get(), second.get()))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 TINT (integer) attribute (integer)
 %status stub

 Returns the tint value for TARG agent. Pass 1 for red, 2 for blue, 3 for green, 4 for rotation or 5 for swap.
*/
void v_TINT(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(attribute)

	valid_agent(vm->targ);
	vm->result.setInt(0); // TODO
}

/**
 TINO (command) red (integer) green (integer) blue (integer) rotation (integer) swap (integer)
 %status stub

 Works like the TINT command, but only applies the tint to the current frame, and discards the rest.
*/
void c_TINO(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(swap)
	VM_PARAM_INTEGER_UNUSED(rotation)
	VM_PARAM_INTEGER_UNUSED(blue)
	VM_PARAM_INTEGER_UNUSED(green)
	VM_PARAM_INTEGER_UNUSED(red)

	// TODO
}

/**
 DROP (command)
 %status maybe
 %variants c1 c2 cv c3 sm

 Causes the TARG agent to drop what it is carrying in a safe location.
*/
void c_DROP(caosVM* vm) {
	valid_agent(vm->targ);

	// TODO: what exactly are we meant to do here? firing the script directly is perhaps not right, but drops must be instant
	if (vm->targ->carrying) // TODO: valid?
		vm->targ->carrying->fireScript(5, vm->targ, caosValue(), caosValue());

	// TODO: only creatures in c1 (and c2?)
}

AgentRef findNextAgent(AgentRef previous, unsigned char family, unsigned char genus, unsigned short species, bool forward) {
	if (world.agents.size() == 0)
		return AgentRef(); // shouldn't happen..

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
		Agent* a = i->get();
		if (a && (a->family == family || family == 0))
			if (a->genus == genus || genus == 0)
				if (a->species == species || species == 0) {
					if (!firstagent)
						firstagent = a;
					if (foundagent)
						return AgentRef(a); // This is the agent we want!
					if (a == previous)
						foundagent = true;
				}

		// Step through the list. Break if we need to.
		if (!forward && i == world.agents.begin())
			break;
		if (forward)
			i++;
		else
			i--;
		if (forward && i == world.agents.end())
			break;
	}

	// Either we didn't find the previous agent, or we're at the end. Either way, return the first agent found.
	return firstagent;
}

/**
 NCLS (agent) previous (agent) family (integer) genus (integer) species (integer)
 %status maybe
*/
void v_NCLS(caosVM* vm) {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	VM_PARAM_AGENT(previous)

	vm->result.setAgent(findNextAgent(previous, family, genus, species, true));
}

/**
 PCLS (agent) previous (agent) family (integer) genus (integer) species (integer)
 %status maybe
*/
void v_PCLS(caosVM* vm) {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	VM_PARAM_AGENT(previous)

	vm->result.setAgent(findNextAgent(previous, family, genus, species, false));
}

/**
 TCOR (integer) topy (float) bottomy (float) leftx (float) rightx (float)
 %status stub

 Tests setting the bounding box of the physical core of the object TARG. May be
 set to smaller (or larger) than the sprite's rectangle. Returns 1 if OK to set
 (using CORE), 0 if not.
*/
void v_TCOR(caosVM* vm) {
	VM_PARAM_FLOAT_UNUSED(rightx)
	VM_PARAM_FLOAT_UNUSED(leftx)
	VM_PARAM_FLOAT_UNUSED(bottomy)
	VM_PARAM_FLOAT_UNUSED(topy)

	valid_agent(vm->targ);
	vm->result.setInt(1); // TODO
}

/**
 CORE (command) topy (float) bottomy (float) leftx (float) rightx (float)
 %status maybe
*/
void c_CORE(caosVM* vm) {
	VM_PARAM_FLOAT(rightx)
	VM_PARAM_FLOAT(leftx)
	VM_PARAM_FLOAT(bottomy)
	VM_PARAM_FLOAT(topy)

	valid_agent(vm->targ);

	vm->targ->has_custom_core_size = true;
	vm->targ->custom_core_xleft = leftx;
	vm->targ->custom_core_xright = rightx;
	vm->targ->custom_core_ytop = topy;
	vm->targ->custom_core_ybottom = bottomy;
}

/**
 TWIN (agent) source (agent) null_vars (integer)
 %status stub
*/
void v_TWIN(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(null_vars)
	VM_PARAM_VALIDAGENT(source)

	vm->result.setAgent(0); // TODO
}

/**
 ACTV (variable)
 %status maybe
 %variants c1 c2
*/
CAOS_LVALUE_TARG_SIMPLE(ACTV, vm->targ->actv)

/**
 THRT (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_TARG_SIMPLE(THRT, vm->targ->thrt)

/**
 SIZE (variable)
 %status stub
 %variants c2
*/
// TODO: stub because this likely == perm
CAOS_LVALUE_TARG_SIMPLE(SIZE, vm->targ->size)

/**
 GRAV (variable)
 %status maybe
 %variants c2
*/
void v_GRAV(caosVM* vm) {
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->falling);
}
void s_GRAV(caosVM* vm) {
	VM_PARAM_VALUE(newvalue)
	THROW_IFNOT(newvalue.hasInt());

	valid_agent(vm->targ);
	vm->targ->falling = newvalue.getInt();
}

/**
 SETV CLS2 (command) family (integer) genus (integer) species (integer)
 %status maybe
 %variants c2

 Creatures 2 command to set the family, genus and species of an agent.
*/
void c_SETV_CLS2(caosVM* vm) {
	VM_PARAM_INTEGER(species)
	THROW_IFNOT(species >= 0);
	THROW_IFNOT(species <= 65535);
	VM_PARAM_INTEGER(genus)
	THROW_IFNOT(genus >= 0);
	THROW_IFNOT(genus <= 255);
	VM_PARAM_INTEGER(family)
	THROW_IFNOT(family >= 0);
	THROW_IFNOT(family <= 255);

	valid_agent(vm->targ);

	vm->targ->setClassifier(family, genus, species);
}

/**
 SLIM (command)
 %status stub
 %variants c1 c2
*/
void c_SLIM(caosVM*) {
	// TODO: probably shouldn't do anything, but make sure :)
}

/**
 BHVR (command) click (integer) touch (integer)
 %status stub
 %variants c1 c2
*/
void c_BHVR_c2(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(touch)
	VM_PARAM_INTEGER(click)

	valid_agent(vm->targ);

	// see Agent::handleClick for details of what these values mean
	switch (click) {
		// clicks have no effect
		case 0:
			vm->targ->clac[0] = -1;
			vm->targ->clac[1] = -1;
			vm->targ->clac[2] = -1;
			break;
		// monostable
		case 1:
			vm->targ->clac[0] = 0;
			vm->targ->clac[1] = -1;
			vm->targ->clac[2] = -1;
			break;
		// retriggerable monostable
		case 2:
			vm->targ->clac[0] = 0;
			vm->targ->clac[1] = 0;
			vm->targ->clac[2] = -1;
			break;
		// toggle
		case 3:
			vm->targ->clac[0] = 0;
			vm->targ->clac[1] = 2;
			vm->targ->clac[2] = -1;
			break;
		// cycle
		case 4:
			vm->targ->clac[0] = 0;
			vm->targ->clac[1] = 1;
			vm->targ->clac[2] = 2;
			break;
		default:
			// C2, at least, seems to produce random garbage (going off the end of a
			// lookup table?) in this situation .. let's not
			throw Exception(fmt::format("silly BHVR click value: {}", click));
	}

	// TODO
}

/**
 SETV CLAS (command) identifier (integer)
 %status maybe
 %variants c1

 Set family, genus and species of the target agent. Creatures 1 era command.
*/
void c_SETV_CLAS(caosVM* vm) {
	VM_PARAM_INTEGER(identifier)

	valid_agent(vm->targ);

	vm->targ->setClassifier(
		(identifier >> 24) & 0xff,
		(identifier >> 16) & 0xff,
		(identifier >> 8) & 0xff);
}

/**
 LIML (integer)
 %status maybe
 %variants c1 c2
*/
void v_LIML(caosVM* vm) {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(vm->targ);

	if (vm->targ->invehicle) {
		Vehicle* v = dynamic_cast<Vehicle*>(vm->targ->invehicle.get());
		assert(v);
		vm->result.setInt((int)v->x + v->cabinleft);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(vm->targ->x + (vm->targ->getWidth() / 2.0f), vm->targ->y + (vm->targ->getHeight() / 2.0f));

		if (r)
			vm->result.setInt(r->x_left);
		else
			vm->result.setInt(0);
	}
}

/**
 LIMT (integer)
 %status maybe
 %variants c1 c2
*/
void v_LIMT(caosVM* vm) {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(vm->targ);

	if (vm->targ->invehicle) {
		Vehicle* v = dynamic_cast<Vehicle*>(vm->targ->invehicle.get());
		assert(v);
		vm->result.setInt((int)v->y + v->cabintop);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(vm->targ->x + (vm->targ->getWidth() / 2.0f), vm->targ->y + (vm->targ->getHeight() / 2.0f));

		if (r)
			vm->result.setInt(r->y_left_ceiling);
		else
			vm->result.setInt(0);
	}
}

/**
 LIMR (integer)
 %status maybe
 %variants c1 c2
*/
void v_LIMR(caosVM* vm) {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(vm->targ);

	if (vm->targ->invehicle) {
		Vehicle* v = dynamic_cast<Vehicle*>(vm->targ->invehicle.get());
		assert(v);
		vm->result.setInt((int)v->x + v->cabinright);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(vm->targ->x + (vm->targ->getWidth() / 2.0f), vm->targ->y + (vm->targ->getHeight() / 2.0f));

		if (r)
			vm->result.setInt(r->x_right);
		else
			vm->result.setInt(8352); // TODO
	}
}

/**
 LIMB (integer)
 %status maybe
 %variants c1 c2
*/
void v_LIMB_c1(caosVM* vm) {
	// TODO: is this remotely sane? if so, unstub.
	valid_agent(vm->targ);

	if (vm->targ->invehicle) {
		Vehicle* v = dynamic_cast<Vehicle*>(vm->targ->invehicle.get());
		assert(v);
		vm->result.setInt((int)v->y + v->cabinbottom);
	} else {
		std::shared_ptr<Room> r = world.map->roomAt(vm->targ->x + (vm->targ->getWidth() / 2.0f), vm->targ->y + (vm->targ->getHeight() / 2.0f));

		if (r)
			vm->result.setInt(r->y_left_floor);
		else
			vm->result.setInt(1200); // TODO
	}
}

/**
 OBJP (variable)
 %status maybe
 %variants c1 c2
*/
// TODO: c1 scripts seem to depend on this being from OWNR, but is that always the case?
CAOS_LVALUE_WITH_SIMPLE(OBJP, vm->owner, vm->owner->objp)

/**
 XIST (integer) agent (agent)
 %status maybe
 %variants c2

 Undocumented C2 command; returns 1 if specified agent exists, or 0 otherwise (ie, if it is null).
*/
void v_XIST(caosVM* vm) {
	VM_PARAM_AGENT(agent)

	if (agent.get())
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 SCLE (command) scaleby (float) yesorno (integer)
 %status done
 %variants sm
*/
void c_SCLE_sm(caosVM* vm) {
	VM_PARAM_INTEGER(yesorno)
	VM_PARAM_FLOAT(scaleby)

	valid_agent(vm->targ);

	THROW_IFNOT(yesorno == 0 || yesorno == 1);
	vm->targ->scle = yesorno ? scaleby : 1.0;
}

/**
 SCLE (command) pose(integer) scaleby (float)
 %status stub
 %variants cv
*/
void c_SCLE_cv(caosVM* vm) {
	VM_PARAM_FLOAT_UNUSED(scaleby)
	VM_PARAM_INTEGER(pose)

	valid_agent(vm->targ);

	THROW_IFNOT(pose == 1 || pose == -1);

	// TODO
}

/**
 STRC (command) width (integer) height (integer) enable (integer)
 %status stub
 %variants sm

 Draw the current agent with the given width/height (ie, stretch the sprite). Set enable to 1 to enable, or 0 to disable.
*/
void c_STRC(caosVM* vm) {
	VM_PARAM_INTEGER(enable)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)

	valid_agent(vm->targ);
	THROW_IFNOT(enable == 0 || enable == 1);
	// TODO: does this affect agent's width/height?
	vm->targ->strc = enable;
	vm->targ->strc_width = width;
	vm->targ->strc_height = height;
}

/**
 IMGE (command) filename (string)
 %status stub
 %variants cv
*/
void c_IMGE(caosVM* vm) {
	VM_PARAM_STRING(filename)

	// TODO
}

/**
 TNTW (command) index (integer)
 %status stub
 %variants cv

 TODO: also might exist in Docking Station
*/
void c_TNTW(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(index)

	// TODO
}

/**
 PRNT (command) pose (integer)
 %status stub
*/
void c_PRNT(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(pose)

	// TODO
}

/**
 TCAR (agent)
 %status maybe
 %variants c2
*/
void v_TCAR(caosVM* vm) {
	valid_agent(vm->targ);

	if (vm->targ->invehicle)
		vm->result.setAgent(vm->targ->invehicle);
	else
		vm->result.setAgent(vm->targ->carriedby);
}

/**
 EDIT (command)
 %status maybe
 %variants c1 c2

 Attach the target agent to the mouse cursor for positioning purposes.
*/
void c_EDIT(caosVM* vm) {
	valid_agent(vm->targ);

	world.hand()->editAgent = vm->targ;
}

/**
 FRZN (variable)
 %status maybe
 %variants c2
*/
void v_FRZN(caosVM* vm) {
	valid_agent(vm->targ);
	caosValue r = vm->targ->frozen ? 1 : 0;
	vm->valueStack.push_back(r);
}
void s_FRZN(caosVM* vm) {
	VM_PARAM_VALUE(newvalue)
	THROW_IFNOT(newvalue.hasInt());

	valid_agent(vm->targ);
	vm->targ->frozen = newvalue.getInt();
}

/**
 BLCK (command) width (integer) height (integer)
 %status maybe
 %variants sm
*/
void c_BLCK(caosVM* vm) {
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)

	SpritePart* p = vm->getCurrentSpritePart();
	THROW_IFNOT(p);
	std::shared_ptr<creaturesImage> img = p->getSprite();
	THROW_IFNOT(img.get());
	THROW_IFNOT(width > 0);
	THROW_IFNOT(height > 0);
	img->setBlockSize((unsigned int)width, (unsigned int)height);
}

/**
 SHAD (command) intensity (integer) x (integer) y (integer) enable (integer)
 %status stub
 %variants sm

 Enable/disable drawing of a shadow on the target agent with the specified intensity at the given x/y offset.
*/
void c_SHAD(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(enable)
	VM_PARAM_INTEGER_UNUSED(y)
	VM_PARAM_INTEGER_UNUSED(x)
	VM_PARAM_INTEGER_UNUSED(intensity)

	valid_agent(vm->targ);

	// TODO: only used by clams in Sea-Monkeys.
	// Intensity doesn't map directly to alpha 0.0–1.0, maybe 0–0.5?
	// Needs to either only be on primary part, or all shadows rendered before all parts
}

/**
 SYS: CONV (command) filename (bareword) show_progress (integer)
 %status stub
 %variants c2

 This is supposed to convert the provided image name to the display depth, displaying a progress dialog if show_progress is non-zero.
 However, it does nothing under openc2e.
*/
void c_SYS_CONV(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(show_progress)
	VM_PARAM_STRING_UNUSED(filename)
}

/* vim: set noet: */
