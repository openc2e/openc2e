/*
 *  caosVM_creatures.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Jun 01 2004.
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
#include <cassert>
#include <iostream>
#include <memory>
#include "World.h"
#include "Engine.h"
#include "creatures/SkeletalCreature.h"
#include "creatures/CompoundCreature.h"
#include "creatures/oldCreature.h"
#include "creatures/c2eCreature.h"
#include "historyManager.h"
using std::cerr;

CreatureAgent *caosVM::getTargCreatureAgent() {
	valid_agent(targ);
	CreatureAgent *c = dynamic_cast<CreatureAgent *>(targ.get());
	caos_assert(c);
	return c;
}

Creature *caosVM::getTargCreature() {
	return getTargCreatureAgent()->getCreature();
}

oldCreature *getoldCreature(Agent *a) {
	if (!a) return 0;
	CreatureAgent *b = dynamic_cast<CreatureAgent *>(a);
	if (!b) return 0;
	oldCreature *c = dynamic_cast<oldCreature *>(b->getCreature());
	return c;
}

c2eCreature *getc2eCreature(Agent *a) {
	if (!a) return 0;
	CreatureAgent *b = dynamic_cast<CreatureAgent *>(a);
	if (!b) return 0;
	c2eCreature *c = dynamic_cast<c2eCreature *>(b->getCreature());
	return c;
}

/**
 STM# SHOU (command) stimulusno (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STM_SHOU(caosVM *vm) {
	VM_PARAM_INTEGER(stimulusno)

	// TODO
}

/**
 STM# SIGN (command) stimulusno (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STM_SIGN(caosVM *vm) {
	VM_PARAM_INTEGER(stimulusno)

	// TODO
}

/**
 STM# TACT (command) stimulusno (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STM_TACT(caosVM *vm) {
	VM_PARAM_INTEGER(stimulusno)

	// TODO
}

/**
 STM# WRIT (command) object (agent) stimulusno (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STM_WRIT(caosVM *vm) {
	VM_PARAM_INTEGER(stimulusno)
	VM_PARAM_VALIDAGENT(object)

	oldCreature *c = getoldCreature(object.get());
	if (!c) return; // ignored on non-creatures

	// TODO: ownr?
	c->handleStimulus(stimulusno);
}

/**
 STIM SHOU (command) stimulus (integer) strength (float)
 %status stub

 Send a stimulus of the given type to all Creatures who can hear OWNR.
*/
void c_STIM_SHOU(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)

	// TODO
}

/**
 STIM SIGN (command) stimulus (integer) strength (float)
 %status stub

 Sends a stimulus of the given type to all Creatures who can see OWNR.
*/
void c_STIM_SIGN(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	
	// TODO
}

/**
 STIM TACT (command) stimulus (integer) strength (float)
 %status stub

 Sends a stimulus of the given type to all Creatures who are touching OWNR.
*/
void c_STIM_TACT(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	
	// TODO
}

/**
 STIM WRIT (command) creature (agent) stimulus (integer) strength (float)
 %status maybe

 Sends a stimulus of the given type to specific Creature.
*/
void c_STIM_WRIT(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	VM_PARAM_VALIDAGENT(creature)

	c2eCreature *c = getc2eCreature(creature.get());
	if (!c) return; // ignored on non-creatures

	// TODO: ownr?
	c->handleStimulus(stimulus, strength);
}

/**
 STIM SHOU (command) significance (integer) input (integer) intensity (integer) features (integer) chem0 (integer) amount0 (integer) chem1 (integer) amount1 (integer) chem2 (integer) amount2 (integer) chem3 (integer) amount3 (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STIM_SHOU_c2(caosVM *vm) {
	VM_PARAM_INTEGER(amount3)
	VM_PARAM_INTEGER(chem3)
	VM_PARAM_INTEGER(amount2)
	VM_PARAM_INTEGER(chem2)
	VM_PARAM_INTEGER(amount1)
	VM_PARAM_INTEGER(chem1)
	VM_PARAM_INTEGER(amount0)
	VM_PARAM_INTEGER(chem0)
	VM_PARAM_INTEGER(features)
	VM_PARAM_INTEGER(intensity)
	VM_PARAM_INTEGER(input)
	VM_PARAM_INTEGER(significance)

	// TODO
}

/**
 STIM SIGN (command) significance (integer) input (integer) intensity (integer) features (integer) chem0 (integer) amount0 (integer) chem1 (integer) amount1 (integer) chem2 (integer) amount2 (integer) chem3 (integer) amount3 (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STIM_SIGN_c2(caosVM *vm) {
	VM_PARAM_INTEGER(amount3)
	VM_PARAM_INTEGER(chem3)
	VM_PARAM_INTEGER(amount2)
	VM_PARAM_INTEGER(chem2)
	VM_PARAM_INTEGER(amount1)
	VM_PARAM_INTEGER(chem1)
	VM_PARAM_INTEGER(amount0)
	VM_PARAM_INTEGER(chem0)
	VM_PARAM_INTEGER(features)
	VM_PARAM_INTEGER(intensity)
	VM_PARAM_INTEGER(input)
	VM_PARAM_INTEGER(significance)

	// TODO
}

/**
 STIM TACT (command) significance (integer) input (integer) intensity (integer) features (integer) chem0 (integer) amount0 (integer) chem1 (integer) amount1 (integer) chem2 (integer) amount2 (integer) chem3 (integer) amount3 (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STIM_TACT_c2(caosVM *vm) {
	VM_PARAM_INTEGER(amount3)
	VM_PARAM_INTEGER(chem3)
	VM_PARAM_INTEGER(amount2)
	VM_PARAM_INTEGER(chem2)
	VM_PARAM_INTEGER(amount1)
	VM_PARAM_INTEGER(chem1)
	VM_PARAM_INTEGER(amount0)
	VM_PARAM_INTEGER(chem0)
	VM_PARAM_INTEGER(features)
	VM_PARAM_INTEGER(intensity)
	VM_PARAM_INTEGER(input)
	VM_PARAM_INTEGER(significance)

	// TODO
}

/**
 STIM WRIT (command) creature (agent) significance (integer) input (integer) intensity (integer) features (integer) chem0 (integer) amount0 (integer) chem1 (integer) amount1 (integer) chem2 (integer) amount2 (integer) chem3 (integer) amount3 (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STIM_WRIT_c2(caosVM *vm) {
	VM_PARAM_INTEGER(amount3)
	VM_PARAM_INTEGER(chem3)
	VM_PARAM_INTEGER(amount2)
	VM_PARAM_INTEGER(chem2)
	VM_PARAM_INTEGER(amount1)
	VM_PARAM_INTEGER(chem1)
	VM_PARAM_INTEGER(amount0)
	VM_PARAM_INTEGER(chem0)
	VM_PARAM_INTEGER(features)
	VM_PARAM_INTEGER(intensity)
	VM_PARAM_INTEGER(input)
	VM_PARAM_INTEGER(significance)
	VM_PARAM_AGENT(creature)

	// TODO
	//oldCreature *c = getoldCreature(creature.get());
	//if (!c) return; // ignored on non-creatures

	// TODO
}

/**
 STIM FROM (command) significance (integer) input (integer) intensity (integer) features (integer) chem0 (integer) amount0 (integer) chem1 (integer) amount1 (integer) chem2 (integer) amount2 (integer) chem3 (integer) amount3 (integer)
 %status stub
 %variants c1 c2
 %cost c1,c2 0
*/
void c_STIM_FROM_c1(caosVM *vm) {
	VM_PARAM_INTEGER(amount3)
	VM_PARAM_INTEGER(chem3)
	VM_PARAM_INTEGER(amount2)
	VM_PARAM_INTEGER(chem2)
	VM_PARAM_INTEGER(amount1)
	VM_PARAM_INTEGER(chem1)
	VM_PARAM_INTEGER(amount0)
	VM_PARAM_INTEGER(chem0)
	VM_PARAM_INTEGER(features)
	VM_PARAM_INTEGER(intensity)
	VM_PARAM_INTEGER(input)
	VM_PARAM_INTEGER(significance)

	valid_agent(vm->from.getAgent());
	// TODO
}

/**
 SWAY SHOU (command) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in all Creatures who can hear OWNR.
*/
void c_SWAY_SHOU(caosVM *vm) {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)

	valid_agent(vm->owner);
	//TODO
}

/**
 SWAY SIGN (command) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in all Creatures who can see OWNR.
*/
void c_SWAY_SIGN(caosVM *vm) {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)

	valid_agent(vm->owner);
	//TODO
}

/**
 SWAY TACT (command) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in all Creatures who are touching OWNR.
*/
void c_SWAY_TACT(caosVM *vm) {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)

	valid_agent(vm->owner);
	//TODO
}

/**
 SWAY WRIT (command) creature (agent) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in the specified creature.
*/
void c_SWAY_WRIT(caosVM *vm) {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)
	VM_PARAM_VALIDAGENT(creature)

	valid_agent(vm->owner);
	//TODO
}

/**
 NOHH (command)
 %status stub

 Tells the target Creature to stop holding hands with the pointer.
*/
void c_NOHH(caosVM *vm) {
	VM_VERIFY_SIZE(0)

	if (!vm->targ) return; // DS agent help, at least, does 'targ hhld nohh'
	
	Creature *c = vm->getTargCreature();
	// TODO
}

/**
 ZOMB (command) zombie (integer)
 %status maybe

 Turns zombification of the target Creature on and off.  Set to 1 to disconnect the brain and 
 motor of the target Creature, and 0 to undo.
*/
void c_ZOMB(caosVM *vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(zombie)

	Creature *c = vm->getTargCreature();

	c->setZombie(zombie);
}

/**
 ZOMB (integer)
 %status maybe

 Returns 1 if target Creature is zombified, or 0 if otherwise.
*/
void v_ZOMB(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setInt(c->isZombie());
}

/**
 DIRN (command) direction (integer)
 %status maybe

 Changes the target Creature to face a different direction.
*/
void c_DIRN(caosVM *vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(direction)

	caos_assert(direction >= 0 && direction <= 3);

	CreatureAgent *c = vm->getTargCreatureAgent();
	c->setDirection(direction);
}

/**
 DIRN (integer)
 %status maybe
 %variants c2 cv c3

 Returns the direction the target Creatures is facing.
*/
void v_DIRN(caosVM *vm) {
	CreatureAgent *c = vm->getTargCreatureAgent();
	
	vm->result.setInt(c->getDirection());
}
	
/**
 HHLD (agent)
 %status stub

 Returns Creatures that are holding hands with pointer, or NULL if none.
*/
void v_HHLD(caosVM *vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setAgent(0); // TODO
}

/**
 MVFT (command) x (float) y (float)
 %status maybe

 Move the target Creature's foot (along with the rest of the Creature, obviously) to the given 
 coordinates.  You should use this rather than MVTO for Creatures.
*/
void c_MVFT(caosVM *vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(vm->targ);
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(vm->targ.get());
	caos_assert(c);

	// TODO: this should be nicer
	float downfootxoffset = c->attachmentX(c->isLeftFootDown() ? 11 : 12, 0);
	float downfootyoffset = c->attachmentY(c->isLeftFootDown() ? 11 : 12, 0);

	vm->targ->moveTo(x - downfootxoffset, y - downfootyoffset);
}
	
/**
 CREA (integer) agent (agent)
 %status done

 Determines whether the given agent is a creature or not (0 or 1).
*/
void v_CREA(caosVM *vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(agent)

	CreatureAgent *c = dynamic_cast<CreatureAgent *>(agent.get());
	if (c) vm->result.setInt(1);
	else vm->result.setInt(0);
}

/**
 VOCB (command)
 %status stub

 Makes the target Creature learn all vocabulary words immediately.
*/
void c_VOCB(caosVM *vm) {
	VM_VERIFY_SIZE(0)
	
	Creature *c = vm->getTargCreature();

	// TODO
}

/**
 DEAD (command)
 %status maybe

 Kill the target Creature biologically.
*/
void c_DEAD(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	c->die();
}

/**
 DEAD (integer)
 %status maybe
 %variants c1 c2 cv c3

 Determines whether the target Creature is alive (0) or dead (255 in c1/c2, 1 otherwise).
*/
void v_DEAD(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	if (engine.version < 3 && !c->isAlive())
		vm->result.setInt(255);
	else
		vm->result.setInt(!c->isAlive());
}

/**
 NORN (command) creature (agent)
 %status maybe

 Sets the target Creature to the given one.
*/
void c_NORN(caosVM *vm) {
	VM_PARAM_AGENT(creature)

	world.selectCreature(creature);
}

/**
 NORN (agent)
 %status maybe
 %variants cv c3

 Returns the target Creature.
*/
/**
 NORN (variable)
 %status maybe
 %variants c1 c2

 Returns the target Creature.
*/
CAOS_LVALUE(NORN, (void)0,
		caosValue(world.selectedcreature),
		world.selectCreature(newvalue.getAgent())
	)

#include "AgentHelpers.h"

/**
 URGE SHOU (command) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urges all Creatures who can hear OWNR to perform the given action on OWNR.
 The two stimuli parameters can range from -1.0 (discourage) to 1.0 (encourage).
*/
void c_URGE_SHOU(caosVM *vm) {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

	valid_agent(vm->owner);

	// TODO
}

/**
 URGE SIGN (command) noun_stim (float) verb_id (integer) verb_stim (float)
 %status maybe

 Urges all Creatures who can see OWNR to perform the given action on OWNR.
 The two stimuli parameters can range from -1.0 (discourage) to 1.0 (encourage).
*/
void c_URGE_SIGN(caosVM *vm) {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

	valid_agent(vm->owner);

	// TODO: sanitise stim params (bind to -1.0/1.0)
	c2eStim stim;
	stim.noun_id = vm->owner->category;
	if (stim.noun_id == -1) return; // TODO: correct?
	stim.noun_amount = noun_stim;
	stim.verb_id = verb_id;
	stim.verb_amount = verb_stim;

	std::vector<std::shared_ptr<Agent> > agents = getVisibleList(vm->owner, 0, 0, 0);
	for (std::vector<std::shared_ptr<Agent> >::iterator i = agents.begin(); i != agents.end(); i++) {
		std::shared_ptr<Agent> a = *i; // guaranteed to be valid from getVisibleList

		CreatureAgent *ca = dynamic_cast<CreatureAgent *>(a.get());
		if (!ca) continue;
		c2eCreature *c = dynamic_cast<c2eCreature *>(ca->getCreature());
		if (!c) continue;

		c->handleStimulus(stim);
	}
}

/**
 URGE TACT (command) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urges all Creatures who are touching OWNR to perform the given action on OWNR.
 The two stimuli parameters can range from -1.0 (discourage) to 1.0 (encourage).
*/
void c_URGE_TACT(caosVM *vm) {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

	valid_agent(vm->owner);
	
	// TODO
}

/**
 URGE WRIT (command) creature (agent) noun_id (integer) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urges the specified Creature to perform the specified action (verb) on the specified object type 
 (noun).  Provide a stim greater than 1 to force, and an id of -1 and a stim greater than 1 to unforce.
*/
void c_URGE_WRIT(caosVM *vm) {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)
	VM_PARAM_INTEGER(noun_id)
	VM_PARAM_AGENT(creature)

	if (!creature) return;
	c2eCreature *c = getc2eCreature(creature.get());
	if (!c) return; // ignored on non-creatures
	
	// TODO
}

/**
 DRIV (command) drive_id (integer) adjust (float)
 %status maybe

 Modifies the level of a drive in target Creature by the given level, which can range from -1.0 (decrease) to 1.0 (increase).
*/
void c_DRIV(caosVM *vm) {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(drive_id) caos_assert(drive_id >= 0 && drive_id < 20);
	
	valid_agent(vm->targ);
	c2eCreature *c = getc2eCreature(vm->targ.get());
	if (!c) return; // ignored on non-creatures

	c->adjustDrive(drive_id, adjust);
}

/**
 DRIV (float) drive_id (integer)
 %status maybe

 Returns the level of a drive (0.0 to 1.0) in target Creature.
*/
void v_DRIV(caosVM *vm) {
	VM_PARAM_INTEGER(drive_id) caos_assert(drive_id >= 0 && drive_id < 20);

	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	vm->result.setFloat(c->getDrive(drive_id));
}

/**
 CHEM (command) chemical_id (integer) adjust (float)
 %status maybe

 Modifies the level of a chemical in target Creature's bloodstream by adjust, which can range from -1.0 (decrease) to 1.0 (increase).
*/
void c_CHEM(caosVM *vm) {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);

	valid_agent(vm->targ);
	c2eCreature *c = getc2eCreature(vm->targ.get());
	if (!c) return; // ignored on non-creatures
	
	c->adjustChemical(chemical_id, adjust);
}

/**
 CHEM (command) chemical_id (integer) adjust (integer)
 %status maybe
 %variants c1 c2
 %cost c1,c2 0

 Set the level of a chemical (0 to 255) in target creature's bloodstream.
*/
void c_CHEM_c1(caosVM *vm) {
	VM_PARAM_INTEGER(adjust)
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);

	// TODO: can adjust be negative?

	valid_agent(vm->targ);
	oldCreature *c = getoldCreature(vm->targ.get());
	if (!c) return; // ignored on non-creatures
	
	c->addChemical(chemical_id, adjust);
}

/**
 CHEM (float) chemical_id (integer)
 %status maybe

 Returns the level of a chemical (0.0 to 1.0) in target creature's bloodstream.
*/
void v_CHEM(caosVM *vm) {
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);
	
	valid_agent(vm->targ);
	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	vm->result.setFloat(c->getChemical(chemical_id));
}

/**
 CHEM (integer) chemical_id (integer)
 %status maybe
 %variants c1 c2
 
 Returns the level of a chemical (0 to 255) in target creature's bloodstream.
*/
void v_CHEM_c1(caosVM *vm) {
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);
	
	valid_agent(vm->targ);
	oldCreature *c = getoldCreature(vm->targ.get());
	caos_assert(c);

	vm->result.setInt(c->getChemical(chemical_id));
}

/**
 ASLP (command) asleep (integer)
 %status maybe
 %variants c1 c2 cv c3

 If asleep is 1, makes the target creature sleep. If asleep is 0, makes the target creature wake.
*/
void c_ASLP(caosVM *vm) {
	VM_PARAM_INTEGER(asleep)
	
	Creature *c = vm->getTargCreature();

	c->setAsleep(asleep);
}

/**
 ASLP (integer)
 %status maybe
 %variants c1 c2 cv c3
 %cost c1,c2 0

 Determines whether the target Creature is asleep.
*/
void v_ASLP(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	vm->result.setInt(c->isAsleep());
}

#include "AgentHelpers.h"

class blockUntilApproached : public blockCond {
	protected:
		AgentRef parent, it;
		CreatureAgent *parentcreature;

	public:
		blockUntilApproached(AgentRef p, AgentRef i) : parent(p), it(i) {
			assert(parent);
			parentcreature = dynamic_cast<CreatureAgent *>(parent.get());
			assert(parentcreature);
		}
		virtual bool operator()() {
			if (!parent) return false;
			if (!it) return false; // TODO: CAs

			if (agentsTouching(parent, it)) return false;

			// parentcreature is guaranteed to be valid if parent is
			
			// TODO: cope with this problem (eg: another APPR, creature paused, non-skeletal creature, etc)
			//if (parentcreature->isApproaching()) return false;

			// note that this merely sets up the approach to be done on the next tick of the creature
			parentcreature->approach(it);

			return true;
		}
};

/**
 APPR (command)
 %status maybe
 %variants c1 c2 cv c3

 Makes the target Creature approach the IT agent (or if none, an agent of that category using CAs), 
 blocking until it makes it there or gives up.
*/
void c_APPR(caosVM *vm) {
	CreatureAgent *a = vm->getTargCreatureAgent();
	caos_assert(a);
	
	vm->startBlocking(new blockUntilApproached(vm->targ, vm->_it_));
}

/**
 UNCS (command) unconscious (integer)
 %status stub

 Makes the target Creature conscious if 0, or unconscious if 1.
*/
void c_UNCS(caosVM *vm) {
	VM_PARAM_INTEGER(unconscious)

	Creature *c = vm->getTargCreature();

	// TODO
}

/**
 UNCS (integer)
 %status stub
 %variants c2 cv c3

 Returns 1 if the target Creature is unconscious, or 0 otherwise.
*/
void v_UNCS(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	vm->result.setInt(0); // TODO
}

/**
 FACE (command) number (integer)
 %status stub
*/
void c_FACE(caosVM *vm) {
	VM_PARAM_INTEGER(number)

	caos_assert(vm->targ);
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(vm->targ.get());
	caos_assert(c);

	// TODO
}

/**
 FACE (facevalue)
 %status maybe

 Returns the front-facing pose for the current facial expression or the current
 sprite filename of the target creature, depending on the desired return type.
*/
void v_FACE(caosVM *vm) {
	caos_assert(vm->targ);
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(vm->targ.get());
	caos_assert(c);

	vm->result = FaceValue{
		(int)c->getFaceSpriteFrame(),
		c->getFaceSpriteName()
	};
}

/**
 LIKE (command) creature (agent)
 %status stub
 
 Causes the target Creature to state an opinion about the specified Creature.
*/
void c_LIKE(caosVM *vm) {
	VM_PARAM_VALIDAGENT(creature)

	Creature *c = vm->getTargCreature();

	// TODO
}

/**
 LIMB (string) bodypart (integer) genus (integer) gender (integer) age (integer) variant (integer)
 %status stub
 
 Returns the filename for the specified part of a Creature, substituting as necessary.
*/
void v_LIMB(caosVM *vm) {
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(age)
	VM_PARAM_INTEGER(gender)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(bodypart)
	
	Creature *c = vm->getTargCreature();
	
	vm->result.setString(""); // TODO
}

/**
 ORDR SHOU (command) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to all creatures in hearing range.
*/
void c_ORDR_SHOU(caosVM *vm) {
	VM_PARAM_STRING(speech)

	valid_agent(vm->targ);
	// TODO
}

/**
 ORDR SIGN (command) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to all creatures that can see it.
*/
void c_ORDR_SIGN(caosVM *vm) {
	VM_PARAM_STRING(speech)

	valid_agent(vm->targ);
	// TODO
}

/**
 ORDR TACT (command) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to all creatures that are touching it.
*/
void c_ORDR_TACT(caosVM *vm) {
	VM_PARAM_STRING(speech)

	valid_agent(vm->targ);
	// TODO
}

/**
 ORDR WRIT (command) creature (agent) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to the specified creature.
*/
void c_ORDR_WRIT(caosVM *vm) {
	VM_PARAM_STRING(speech)
	VM_PARAM_VALIDAGENT(creature)

	valid_agent(vm->targ);
	// TODO
}

/**
 DREA (command) dream (integer)
 %status maybe

 Turns a Creature's dreaming on and off.  A Creature's instincts are 
 processed while it is dreaming.  If it is not asleep already, then it 
 will be made to sleep before dreaming begins.
*/
void c_DREA(caosVM *vm) {
	VM_PARAM_INTEGER(dream)

	Creature *c = vm->getTargCreature();
	c->setDreaming(dream);
}

/**
 DREA (integer)
 %status maybe

 Returns whether or not the target Creature is dreaming (0 or 1).
*/
void v_DREA(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setInt(c->isDreaming());
}

/**
 BORN (command)
 %status maybe

 Registers the birth of the target Creature, and sends a birth event to 
 the game.
*/
void c_BORN(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	c->born();
}

/**
 CAGE (integer)
 %status maybe
 %variants c1 c2 cv c3

 Returns the integer value of the target Creature's current life stage.
*/
void v_CAGE(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setInt((int)c->getStage());
}

/**
 BYIT (integer)
 %status maybe

 Determines whether or not the target Creature can reach the IT agent (0 
 or 1).
*/
void v_BYIT(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	if (vm->_it_ && agentsTouching(vm->targ, vm->_it_)) // TODO
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 _IT_ (agent)
 %status maybe
 %variants c1 c2 cv c3

 Returns the agent that the OWNR creature was focused on when the 
 current script began running.
*/
void v_IT(caosVM *vm) {
	valid_agent(vm->owner);
	caos_assert(dynamic_cast<CreatureAgent *>(vm->owner.get())); // TODO: return null instead?
	vm->result.setAgent(vm->_it_);
}

/**
 NEWC (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer)
 %status maybe
 
 Creates a new creature over the space of a few ticks, using the specified agent/slot for genetic data. sex is 0 for random, 1 for male or 2 for female.
*/
void c_NEWC(caosVM *vm) {
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)

	// TODO: creation should be blocking and multiple-tick!

	std::map<unsigned int, std::shared_ptr<class genomeFile> >::iterator i = gene_agent->genome_slots.find(gene_slot);
	caos_assert(i != gene_agent->genome_slots.end());

	// randomise sex if necessary
	if (sex == 0) sex = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	caos_assert(sex == 1 || sex == 2); // TODO: correct?

	// TODO: if variant is 0, randomise between 1 and 8
	SkeletalCreature *a = new SkeletalCreature(family);
	try {
		c2eCreature *c = new c2eCreature(i->second, (sex == 2), variant, a);
		a->setCreature(c);
	} catch (...) {
		delete a;
		throw;
	}
	
	a->finishInit();

	world.history->getMoniker(world.history->findMoniker(i->second)).moveToCreature(a);
	gene_agent->genome_slots.erase(i);

	vm->setTarg(a);
}

/**
 NEW: CREA (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer)
 %status stub
 
 Creates a new creature using the specified agent/slot for genetic data. sex is 0 for random, 1 for male or 2 for female.
*/
void c_NEW_CREA(caosVM *vm) {
	/*VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)*/

	c_NEWC(vm); // TODO
	//targ = NULL; // TODO
}

/**
 NEW: CREA (command) moniker (integer) sex (integer)
 %status maybe
 %variants c1 c2

 Creates a new creature using the specified moniker for genetic data. sex is 0 for random, 1 for male or 2 for female.
*/
void c_NEW_CREA_c1(caosVM *vm) {
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(moniker)

	caos_assert(moniker != 0);

	std::string realmoniker = std::string((char *)&moniker, 4);
	std::shared_ptr<genomeFile> genome = world.loadGenome(realmoniker);
	if (!genome)
		throw creaturesException("failed to find genome file '" + realmoniker + "'");

	caos_assert(genome->getVersion() == engine.version);

	// randomise sex if necessary
	if (sex == 0) sex = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	caos_assert(sex == 1 || sex == 2); // TODO: correct?

	SkeletalCreature *a = new SkeletalCreature(4);
	
	// TODO: why do we even need to pass a variant here?
	oldCreature *c;

	try {
		if (engine.version == 1) c = new c1Creature(genome, (sex == 2), 0, a);
		else c = new c2Creature(genome, (sex == 2), 0, a);
	} catch (...) {
		delete a;
		throw;
	}

	a->setCreature(c);
	a->finishInit();

	a->genome_slots[0] = genome;
	world.newMoniker(genome, realmoniker, a);
	world.history->getMoniker(world.history->findMoniker(genome)).moveToCreature(a);

	vm->setTarg(a);
}

/**
 NEW: CRAG (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe
 %variants c3 sm
*/
void c_NEW_CRAG(caosVM *vm) {
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)

       	std::map<unsigned int, std::shared_ptr<class genomeFile> >::iterator i = gene_agent->genome_slots.find(gene_slot);
	caos_assert(i != gene_agent->genome_slots.end());

	// randomise sex if necessary
	if (sex == 0) sex = 1 + (int) (2.0 * (rand() / (RAND_MAX + 1.0)));
	caos_assert(sex == 1 || sex == 2); // TODO: correct?

	// TODO: if variant is 0, randomise between 1 and 8
	CompoundCreature *a = new CompoundCreature(family, plane, sprite_file, first_image, image_count);
	try {
		c2eCreature *c = new c2eCreature(i->second, (sex == 2), variant, a);
		a->setCreature(c);
	} catch (...) {
		delete a;
		throw;
	}
	
	a->finishInit();

	world.history->getMoniker(world.history->findMoniker(i->second)).moveToCreature(a);
	i->second.reset(); // TODO: remove the slot from the gene_agent entirely

	vm->setTarg(a);

}

int calculateRand(int value1, int value2); // caosVM_variables.cpp

/**
 LTCY (command) action (integer) min (integer) max (integer)
 %status maybe
 %variants c1 c2 cv c3
*/
void c_LTCY(caosVM *vm) {
	VM_PARAM_INTEGER(max); caos_assert(max >= 0 && max <= 255);
	VM_PARAM_INTEGER(min); caos_assert(min >= 0 && min <= 255);
	VM_PARAM_INTEGER(action);

	int n = calculateRand(min, max);

	Creature *c = vm->getTargCreature();
	
	c2eCreature *c2ec = dynamic_cast<c2eCreature *>(c);
	if (c2ec) {
		c2ec->setInvolActionLatency(action, n);
	}
	oldCreature *oldc = dynamic_cast<oldCreature *>(c);
	if (oldc) {
		oldc->setInvolActionLatency(action, n);
	}
}

/**
 F**K (command)
 %status stub
 %variants c1
*/

/**
 MATE (command)
 %variants c2 cv c3
 %status stub
*/
void c_MATE(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	caos_assert(vm->_it_);
	CreatureAgent *t = dynamic_cast<CreatureAgent *>(vm->_it_.get());
	caos_assert(t);
	Creature *d = t->getCreature();

	// TODO
}

/**
 DRV! (integer)
 %status stub
 %variants c1 c2 cv c3
*/
void v_DRV(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setInt(0); // TODO
}

/**
 IITT (agent)
 %status maybe

 Return the agent which the target creature is currently focused on. Note that you should probably use _IT_ in creature scripts.
*/
void v_IITT(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setAgent(c->getAttentionFocus());
}

/**
 AGES (command) times (integer)
 %status maybe

 Age (ie, increase the life stage of) the target creature the specified number of times.
*/
void c_AGES(caosVM *vm) {
	VM_PARAM_INTEGER(times)
	caos_assert(times >= 0);
	
	Creature *c = vm->getTargCreature();
	for (int i = 0; i < times; i++) {
		c->ageCreature();
	}
}

/**
 LOCI (command) type (integer) organ (integer) tissue (integer) id (integer) value (float)
 %status maybe

 Set the value of the specified loci of the target creature. 'type' is 0 for receptor loci and 1 for emitter loci.
 See genetics documentation for details of the parameters.
*/
void c_LOCI(caosVM *vm) {
	VM_PARAM_FLOAT(value)
	VM_PARAM_INTEGER(id)
	VM_PARAM_INTEGER(tissue)
	VM_PARAM_INTEGER(organ)
	VM_PARAM_INTEGER(type)

	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	float *f = c->getLocusPointer(!type, organ, tissue, id);
	caos_assert(f);
	*f = value;
}

/**
 LOCI (float) type (integer) organ (integer) tissue (integer) id (integer)
 %status maybe
 
 Return the current value of the specified loci of the target creature. 'type' is 0 for receptor loci and 1 for emitter loci.
 See genetics documentation for details of thei parameters.
*/
void v_LOCI(caosVM *vm) {
	VM_PARAM_INTEGER(id)
	VM_PARAM_INTEGER(tissue)
	VM_PARAM_INTEGER(organ)
	VM_PARAM_INTEGER(type)

	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	float *f = c->getLocusPointer(!type, organ, tissue, id);
	caos_assert(f);
	vm->result.setFloat(*f);
}

/**
 TAGE (integer)
 %status maybe

 Returns age of target creature, in ticks. Only counts ticks since it was BORN.
*/
void v_TAGE(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setInt(c->getAge());
}

/**
 ORGN (integer)
 %status maybe

 Return the number of organs the target creature has.
*/
void v_ORGN(caosVM *vm) {
	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);
	vm->result.setInt(c->noOrgans());
}

/**
 ORGF (float) organ (integer) value (integer)
 %status maybe

 Return some data about the specified organ (numbered starting at zero) of the target creature.

 value should be one of the following types of data:
 0: clock rate
 1: short term life force as a proportion of the initial life force
 2: repair rate
 3: injury to apply
 4: initial life force
 5: short term life force
 6: long term life force
 7: damage rate
 8: energy cost
 9: atp damage coefficient

 Returns -1 if the specified organ or value is invalid.
*/
void v_ORGF(caosVM *vm) {
	VM_PARAM_INTEGER(value)
	VM_PARAM_INTEGER(organ)
	
	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);
	if (organ < 0 || (unsigned int)organ >= c->noOrgans()) {
		vm->result.setFloat(-1.0f);
		return;
	}
	
	std::shared_ptr<c2eOrgan> o = c->getOrgan(organ);

	switch (value) {
		case 0: vm->result.setFloat(o->getClockRate()); break;
		case 1: vm->result.setFloat(o->getShortTermLifeforce() / o->getInitialLifeforce()); break;
		case 2: vm->result.setFloat(o->getRepairRate()); break;
		case 3: vm->result.setFloat(o->getInjuryToApply()); break;
		case 4: vm->result.setFloat(o->getInitialLifeforce()); break;
		case 5: vm->result.setFloat(o->getShortTermLifeforce()); break;
		case 6: vm->result.setFloat(o->getLongTermLifeforce()); break;
		case 7: vm->result.setFloat(o->getDamageRate()); break;
		case 8: vm->result.setFloat(o->getEnergyCost()); break;
		case 9: vm->result.setFloat(o->getATPDamageCoefficient()); break;
		default: vm->result.setFloat(-1.0f); break;
	}
}

/**
 ORGI (integer) organ (integer) value (integer)
 %status maybe

 Returns a count of receptors (value 0), emitters (value 1) or reactions (value 2) in the specified organ (numbered starting at zero) of the target creature.
 
 Returns -1 if the specified organ or value is invalid.
*/
void v_ORGI(caosVM *vm) {
	VM_PARAM_INTEGER(value)
	VM_PARAM_INTEGER(organ)
	
	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);
	if (organ < 0 || (unsigned int)organ >= c->noOrgans()) {
		vm->result.setFloat(-1.0f);
		return;
	}
	
	std::shared_ptr<c2eOrgan> o = c->getOrgan(organ);

	switch (value) {
		case 0: vm->result.setInt(o->getReceptorCount()); break;
		case 1: vm->result.setInt(o->getEmitterCount()); break;
		case 2: vm->result.setInt(o->getReactionCount()); break;
		default: vm->result.setFloat(-1.0f); break;
	}
}

/**
 SOUL (command) part (integer) on (integer)
 %status stub
*/
void c_SOUL(caosVM *vm) {
	VM_PARAM_INTEGER(on)
	VM_PARAM_INTEGER(part)
	caos_assert(part >= 0 && part <= 8);

	Creature *c = vm->getTargCreature();
	// TODO
}
	
/**
 SOUL (integer) part (integer)
 %status stub
*/
void v_SOUL(caosVM *vm) {
	VM_PARAM_INTEGER(part)
	caos_assert(part >= 0 && part <= 8);
		
	Creature *c = vm->getTargCreature();
	vm->result.setInt(1); // TODO
}

/**
 DECN (integer)
 %status maybe
*/
void v_DECN(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setInt(c->getDecisionId());
}

/**
 ATTN (integer)
 %status maybe
*/
void v_ATTN(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	vm->result.setInt(c->getAttentionId());
}

/**
 TOUC (command)
 %status stub
 %variants c1 c2 cv c3
*/
void c_TOUC(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	// TODO
}

/**
 FORF (command) creature (agent)
 %status stub
*/
void c_FORF(caosVM *vm) {
	VM_PARAM_VALIDAGENT(creature)
		
	Creature *c = vm->getTargCreature();
	// TODO: do we handle pointer as well?
	Creature *learn = dynamic_cast<Creature *>(creature.get());
	if (!learn) return;

	// TODO
}

/**
 WALK (command)
 %status maybe
 %variants c1 c2 cv c3
*/
void c_WALK(caosVM *vm) {
	CreatureAgent *c = vm->getTargCreatureAgent();
	
	c->startWalking();
}

/**
 DONE (command)
 %status stub
 %variants c1 c2 cv c3
*/
void c_DONE(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	// TODO
}

/**
 SAYN (command)
 %status stub
 %variants c1 c2 cv c3
*/
void c_SAYN(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	// TODO
}

/**
 IMPT (command) nudge (integer)
 %status stub
 %variants c1 c2
*/
void c_IMPT(caosVM *vm) {
	VM_PARAM_INTEGER(nudge)

	// TODO: check for creature targ?
	// TODO
}

/**
 AIM: (command) actionno (integer)
 %status stub
 %variants c1 c2
*/
void c_AIM(caosVM *vm) {
	VM_PARAM_INTEGER(actionno)

	// TODO: check for creature targ? who knows?
	// TODO
}

/**
 BABY (variable)
 %status maybe
 %variants c1 c2
*/
CAOS_LVALUE_TARG_SIMPLE(BABY, vm->targ->babymoniker) // TODO

/**
 SNEZ (command)
 %status stub
 %variants c1 c2
*/
void c_SNEZ(caosVM *vm) {
	Creature *c = vm->getTargCreature();

	// TODO
}

/**
 DRIV (integer) drive (integer)
 %status maybe
 %variants c1 c2

 Returns the value for the specified drive of the target creature.
*/
void v_DRIV_c1(caosVM *vm) {
	VM_PARAM_INTEGER(drive)
	caos_assert(drive < 16);

	oldCreature *c = getoldCreature(vm->targ.get());
	// TODO: c2 support
	caos_assert(c);

	vm->result.setInt(c->getDrive(drive));
}

/**
 DREA (command) max (integer)
 %status stub
 %variants c1 c2

 Start dreaming and process (at most?) max instincts.
*/
void c_DREA_c1(caosVM *vm) {
	VM_PARAM_INTEGER(max)

	oldCreature *c = getoldCreature(vm->targ.get());
	caos_assert(c);

	// TODO
}

/**
 BRED (integer) part (integer)
 %status stub
*/
void v_BRED(caosVM *vm) {
	VM_PARAM_INTEGER(part)

	vm->result.setInt(0); // TODO
}

/**
 BVAR (integer)
 %status stub
*/
void v_BVAR(caosVM *vm) {
	vm->result.setInt(0); // TODO
}

/**
 EXPR (command) index (integer) ticks (integer)
 %status stub
*/
void c_EXPR(caosVM *vm) {
	VM_PARAM_INTEGER(ticks)
	VM_PARAM_INTEGER(index)

	// TODO
}

/**
 EXPR (integer)
 %status stub
*/
void v_EXPR(caosVM *vm) {
	vm->result.setInt(0); // TODO
}

/**
 TNTC (command) tintindex (integer) part (integer) commit (integer)
 %status stub
*/
void c_TNTC(caosVM *vm) {
	VM_PARAM_INTEGER(commit)
	VM_PARAM_INTEGER(part)
	VM_PARAM_INTEGER(tintindex)

	// TODO
}

/**
 INJR (command) organ (integer) amount (integer)
 %status stub
 %variants c2
*/
void c_INJR(caosVM *vm) {
	VM_PARAM_INTEGER(amount)
	VM_PARAM_INTEGER(organ)

	valid_agent(vm->targ);
	// TODO
}

/**
 SAY$ (command) string (string)
 %status stub
 %variants c1 c2
*/
void c_SAY(caosVM *vm) {
	VM_PARAM_STRING(string)

	valid_agent(vm->targ);
	// TODO
}

/**
 TRIG (command) lobe (integer) cell (integer) amount (integer)
 %status stub
 %variants c2
*/
void c_TRIG(caosVM *vm) {
	VM_PARAM_INTEGER(amount)
	VM_PARAM_INTEGER(cell)
	VM_PARAM_INTEGER(lobe)

	valid_agent(vm->targ);
	// TODO
}

/**
 MONK (integer)
 %status stub
 %variants c2
*/
void v_MONK(caosVM *vm) {
	oldCreature *c = getoldCreature(vm->targ.get());
	caos_assert(c);

	vm->result.setInt(0); // TODO
}

/**
 MOTR (command) enable (integer)
 %status stub
*/
void c_MOTR(caosVM *vm) {
	VM_PARAM_INTEGER(enable)

	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	// TODO
}

/**
 MOTR (integer)
 %status stub
*/
void v_MOTR(caosVM *vm) {
	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	vm->result.setInt(1); // TODO
}

/**
 MIND (command) enable (integer)
 %status stub
*/
void c_MIND(caosVM *vm) {
	VM_PARAM_INTEGER(enable)

	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	// TODO
}

/**
 MIND (integer)
 %status stub
*/
void v_MIND(caosVM *vm) {
	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	vm->result.setInt(1); // TODO
}

/**
 STEP (command) faculty (integer)
 %status stub
*/
void c_STEP(caosVM *vm) {
	VM_PARAM_INTEGER(faculty)

	c2eCreature *c = getc2eCreature(vm->targ.get());
	caos_assert(c);

	// TODO
}

/**
 SEEN (agent) category (integer)
 %status maybe
*/
void v_SEEN(caosVM *vm) {
	VM_PARAM_INTEGER(category)

	Creature *c = vm->getTargCreature();
	caos_assert(c);

	caos_assert(category >= 0);
	caos_assert((unsigned int)category < c->getNoCategories());

	vm->result.setAgent(c->getChosenAgentForCategory(category));
}

/**
 DOIN (command) noinstincts (integer)
 %status stub

 Make the target creature process the specified number of instincts.
*/
void c_DOIN(caosVM *vm) {
	VM_PARAM_INTEGER(noinstincts)

	Creature *c = vm->getTargCreature();
	caos_assert(c);

	// TODO
}

/**
 INS# (integer)
 %status maybe

 Return the number of unprocessed instincts left in the instinct queue for the target creature.
*/
void v_INS(caosVM *vm) {
	Creature *c = vm->getTargCreature();
	caos_assert(c);

	vm->result.setInt(c->getNoUnprocessedInstincts());
}

// clothes

/**
 BODY (command) set (integer) layer (integer)
 %status stub
*/
void c_BODY(caosVM *vm) {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(set)

	// TODO
}

/**
 BODY (integer) part (integer)
 %status stub
*/
void v_BODY(caosVM *vm) {
	VM_PARAM_INTEGER(part)

	vm->result.setInt(-1); // TODO
}

/**
 DYED (command) part (integer) overlay (integer) set (integer) layer (integer)
 %status stub
*/
void c_DYED(caosVM *vm) {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(set)
	VM_PARAM_INTEGER(overlay)
	VM_PARAM_INTEGER(part)

	// TODO
}

/**
 HAIR (command) ruffleness (integer)
 %status stub
*/
void c_HAIR(caosVM *vm) {
	VM_PARAM_INTEGER(ruffleness)

	// TODO
}

/**
 NUDE (command)
 %status stub
*/
void c_NUDE(caosVM*) {
	// TODO
}

/**
 RSET (command)
 %status stub
*/
void c_RSET(caosVM*) {
	// TODO
}

/**
 STRE (command)
 %status stub
*/
void c_STRE(caosVM*) {
	// TODO
}

/**
 SWAP (command) variant (integer) part (integer) commit (integer)
 %status stub
*/
void c_SWAP(caosVM *vm) {
	VM_PARAM_INTEGER(commit)
	VM_PARAM_INTEGER(part)
	VM_PARAM_INTEGER(variant)

	// TODO
}

/**
 WEAR (command) part (integer) set (integer) layer (integer)
 %status stub
*/
void c_WEAR(caosVM *vm) {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(set)
	VM_PARAM_INTEGER(part)

	// TODO
}

/**
 WEAR (integer) part (integer) layer (integer)
 %status stub
*/
void v_WEAR(caosVM *vm) {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(part)

	vm->result.setInt(-1); // TODO
}

/**
 TNTO (command) tintindex (integer) part (integer) set (integer) layer (integer)
 %status stub
*/
void c_TNTO(caosVM *vm) {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(set)
	VM_PARAM_INTEGER(part)
	VM_PARAM_INTEGER(tintindex)

	// TODO
}

// attachment locations

/**
 DFTX (float)
 %status stub
*/
void v_DFTX(caosVM *vm) {
	vm->result.setFloat(0); // TODO
}

/**
 DFTY (float)
 %status stub
*/
void v_DFTY(caosVM *vm) {
	vm->result.setFloat(0); // TODO
}

/**
 UFTX (float)
 %status stub
*/
void v_UFTX(caosVM *vm) {
	vm->result.setFloat(0); // TODO
}

/**
 UFTY (float)
 %status stub
*/
void v_UFTY(caosVM *vm) {
	vm->result.setFloat(0); // TODO
}

/**
 HEDX (float) index (integer)
 %status stub
*/
void v_HEDX(caosVM *vm) {
	VM_PARAM_INTEGER(index)

	vm->result.setFloat(0); // TODO
}

/**
 HEDY (float) index (integer)
 %status stub
*/
void v_HEDY(caosVM *vm) {
	VM_PARAM_INTEGER(index)

	vm->result.setFloat(0); // TODO
}

/**
 MTHX (float)
 %status stub
*/
void v_MTHX(caosVM *vm) {
	vm->result.setFloat(0); // TODO
}

/**
 MTHY (float)
 %status stub
*/
void v_MTHY(caosVM *vm) {
	vm->result.setFloat(0); // TODO
}

