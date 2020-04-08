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

#include "caosVM.h"
#include <iostream>
#include "openc2e.h"
#include "World.h"
#include "Engine.h"
#include "creatures/SkeletalCreature.h"
#include "creatures/CompoundCreature.h"
#include "creatures/oldCreature.h"
#include "creatures/c2eCreature.h"
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
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STM_SHOU() {
	VM_PARAM_INTEGER(stimulusno)

	// TODO
}

/**
 STM# SIGN (command) stimulusno (integer)
 %status stub
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STM_SIGN() {
	VM_PARAM_INTEGER(stimulusno)

	// TODO
}

/**
 STM# TACT (command) stimulusno (integer)
 %status stub
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STM_TACT() {
	VM_PARAM_INTEGER(stimulusno)

	// TODO
}

/**
 STM# WRIT (command) object (agent) stimulusno (integer)
 %status stub
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STM_WRIT() {
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
void caosVM::c_STIM_SHOU() {
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
void caosVM::c_STIM_SIGN() {
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
void caosVM::c_STIM_TACT() {
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
void caosVM::c_STIM_WRIT() {
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
 %pragma implementation caosVM::c_STIM_SHOU_c2
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STIM_SHOU_c2() {
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
 %pragma implementation caosVM::c_STIM_SIGN_c2
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STIM_SIGN_c2() {
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
 %pragma implementation caosVM::c_STIM_TACT_c2
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STIM_TACT_c2() {
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
 %pragma implementation caosVM::c_STIM_WRIT_c2
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STIM_WRIT_c2() {
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
 %pragma implementation caosVM::c_STIM_FROM_c1
 %pragma variants c1 c2
 %cost c1,c2 0
*/
void caosVM::c_STIM_FROM_c1() {
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

	valid_agent(from.getAgent());
	// TODO
}

/**
 SWAY SHOU (command) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in all Creatures who can hear OWNR.
*/
void caosVM::c_SWAY_SHOU() {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)

	valid_agent(owner);
	//TODO
}

/**
 SWAY SIGN (command) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in all Creatures who can see OWNR.
*/
void caosVM::c_SWAY_SIGN() {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)

	valid_agent(owner);
	//TODO
}

/**
 SWAY TACT (command) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in all Creatures who are touching OWNR.
*/
void caosVM::c_SWAY_TACT() {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)

	valid_agent(owner);
	//TODO
}

/**
 SWAY WRIT (command) creature (agent) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjusts these four drives in the specified creature.
*/
void caosVM::c_SWAY_WRIT() {
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

	valid_agent(owner);
	//TODO
}

/**
 NOHH (command)
 %status stub

 Tells the target Creature to stop holding hands with the pointer.
*/
void caosVM::c_NOHH() {
	VM_VERIFY_SIZE(0)

	if (!targ) return; // DS agent help, at least, does 'targ hhld nohh'
	
	Creature *c = getTargCreature();
	// TODO
}

/**
 ZOMB (command) zombie (integer)
 %status maybe

 Turns zombification of the target Creature on and off.  Set to 1 to disconnect the brain and 
 motor of the target Creature, and 0 to undo.
*/
void caosVM::c_ZOMB() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(zombie)

	Creature *c = getTargCreature();

	c->setZombie(zombie);
}

/**
 ZOMB (integer)
 %status maybe

 Returns 1 if target Creature is zombified, or 0 if otherwise.
*/
void caosVM::v_ZOMB() {
	Creature *c = getTargCreature();
	result.setInt(c->isZombie());
}

/**
 DIRN (command) direction (integer)
 %status maybe

 Changes the target Creature to face a different direction.
*/
void caosVM::c_DIRN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(direction)

	caos_assert(direction >= 0 && direction <= 3);

	CreatureAgent *c = getTargCreatureAgent();
	c->setDirection(direction);
}

/**
 DIRN (integer)
 %status maybe
 %pragma variants c2 cv c3

 Returns the direction the target Creatures is facing.
*/
void caosVM::v_DIRN() {
	CreatureAgent *c = getTargCreatureAgent();
	
	result.setInt(c->getDirection());
}
	
/**
 HHLD (agent)
 %status stub

 Returns Creatures that are holding hands with pointer, or NULL if none.
*/
void caosVM::v_HHLD() {
	VM_VERIFY_SIZE(0)

	result.setAgent(0); // TODO
}

/**
 MVFT (command) x (float) y (float)
 %status maybe

 Move the target Creature's foot (along with the rest of the Creature, obviously) to the given 
 coordinates.  You should use this rather than MVTO for Creatures.
*/
void caosVM::c_MVFT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ);
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(targ.get());
	caos_assert(c);

	// TODO: this should be nicer
	float downfootxoffset = c->attachmentX(c->isLeftFootDown() ? 11 : 12, 0);
	float downfootyoffset = c->attachmentY(c->isLeftFootDown() ? 11 : 12, 0);

	targ->moveTo(x - downfootxoffset, y - downfootyoffset);
}
	
/**
 CREA (integer) agent (agent)
 %status done

 Determines whether the given agent is a creature or not (0 or 1).
*/
void caosVM::v_CREA() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(agent)

	CreatureAgent *c = dynamic_cast<CreatureAgent *>(agent.get());
	if (c) result.setInt(1);
	else result.setInt(0);
}

/**
 VOCB (command)
 %status stub

 Makes the target Creature learn all vocabulary words immediately.
*/
void caosVM::c_VOCB() {
	VM_VERIFY_SIZE(0)
	
	Creature *c = getTargCreature();

	// TODO
}

/**
 DEAD (command)
 %status maybe

 Kill the target Creature biologically.
*/
void caosVM::c_DEAD() {
	Creature *c = getTargCreature();

	c->die();
}

/**
 DEAD (integer)
 %status maybe
 %pragma variants c1 c2 cv c3

 Determines whether the target Creature is alive (0) or dead (255 in c1/c2, 1 otherwise).
*/
void caosVM::v_DEAD() {
	Creature *c = getTargCreature();

	if (engine.version < 3 && !c->isAlive())
		result.setInt(255);
	else
		result.setInt(!c->isAlive());
}

/**
 NORN (command) creature (agent)
 %status maybe

 Sets the target Creature to the given one.
*/
void caosVM::c_NORN() {
	VM_PARAM_AGENT(creature)

	world.selectCreature(creature);
}

/**
 NORN (agent)
 %status maybe
 %pragma variants cv c3

 Returns the target Creature.
*/
/**
 NORN (variable)
 %status maybe
 %pragma variants c1 c2

 Returns the target Creature.
*/
CAOS_LVALUE(NORN, (void)0,
		caosVar(world.selectedcreature),
		world.selectCreature(newvalue.getAgent())
	)

#include "AgentHelpers.h"

/**
 URGE SHOU (command) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urges all Creatures who can hear OWNR to perform the given action on OWNR.
 The two stimuli parameters can range from -1.0 (discourage) to 1.0 (encourage).
*/
void caosVM::c_URGE_SHOU() {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

	valid_agent(owner);

	// TODO
}

/**
 URGE SIGN (command) noun_stim (float) verb_id (integer) verb_stim (float)
 %status maybe

 Urges all Creatures who can see OWNR to perform the given action on OWNR.
 The two stimuli parameters can range from -1.0 (discourage) to 1.0 (encourage).
*/
void caosVM::c_URGE_SIGN() {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

	valid_agent(owner);

	// TODO: sanitise stim params (bind to -1.0/1.0)
	c2eStim stim;
	stim.noun_id = owner->category;
	if (stim.noun_id == -1) return; // TODO: correct?
	stim.noun_amount = noun_stim;
	stim.verb_id = verb_id;
	stim.verb_amount = verb_stim;

	std::vector<std::shared_ptr<Agent> > agents = getVisibleList(owner, 0, 0, 0);
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
void caosVM::c_URGE_TACT() {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

	valid_agent(owner);
	
	// TODO
}

/**
 URGE WRIT (command) creature (agent) noun_id (integer) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urges the specified Creature to perform the specified action (verb) on the specified object type 
 (noun).  Provide a stim greater than 1 to force, and an id of -1 and a stim greater than 1 to unforce.
*/
void caosVM::c_URGE_WRIT() {
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
void caosVM::c_DRIV() {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(drive_id) caos_assert(drive_id >= 0 && drive_id < 20);
	
	valid_agent(targ);
	c2eCreature *c = getc2eCreature(targ.get());
	if (!c) return; // ignored on non-creatures

	c->adjustDrive(drive_id, adjust);
}

/**
 DRIV (float) drive_id (integer)
 %status maybe

 Returns the level of a drive (0.0 to 1.0) in target Creature.
*/
void caosVM::v_DRIV() {
	VM_PARAM_INTEGER(drive_id) caos_assert(drive_id >= 0 && drive_id < 20);

	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	result.setFloat(c->getDrive(drive_id));
}

/**
 CHEM (command) chemical_id (integer) adjust (float)
 %status maybe

 Modifies the level of a chemical in target Creature's bloodstream by adjust, which can range from -1.0 (decrease) to 1.0 (increase).
*/
void caosVM::c_CHEM() {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);

	valid_agent(targ);
	c2eCreature *c = getc2eCreature(targ.get());
	if (!c) return; // ignored on non-creatures
	
	c->adjustChemical(chemical_id, adjust);
}

/**
 CHEM (command) chemical_id (integer) adjust (integer)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::c_CHEM_c1
 %cost c1,c2 0

 Set the level of a chemical (0 to 255) in target creature's bloodstream.
*/
void caosVM::c_CHEM_c1() {
	VM_PARAM_INTEGER(adjust)
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);

	// TODO: can adjust be negative?

	valid_agent(targ);
	oldCreature *c = getoldCreature(targ.get());
	if (!c) return; // ignored on non-creatures
	
	c->addChemical(chemical_id, adjust);
}

/**
 CHEM (float) chemical_id (integer)
 %status maybe

 Returns the level of a chemical (0.0 to 1.0) in target creature's bloodstream.
*/
void caosVM::v_CHEM() {
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);
	
	valid_agent(targ);
	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	result.setFloat(c->getChemical(chemical_id));
}

/**
 CHEM (integer) chemical_id (integer)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::v_CHEM_c1
 
 Returns the level of a chemical (0 to 255) in target creature's bloodstream.
*/
void caosVM::v_CHEM_c1() {
	VM_PARAM_INTEGER(chemical_id) caos_assert(chemical_id >= 0 && chemical_id < 256);
	
	valid_agent(targ);
	oldCreature *c = getoldCreature(targ.get());
	caos_assert(c);

	result.setInt(c->getChemical(chemical_id));
}

/**
 ASLP (command) asleep (integer)
 %status maybe
 %pragma variants c1 c2 cv c3

 If asleep is 1, makes the target creature sleep. If asleep is 0, makes the target creature wake.
*/
void caosVM::c_ASLP() {
	VM_PARAM_INTEGER(asleep)
	
	Creature *c = getTargCreature();

	c->setAsleep(asleep);
}

/**
 ASLP (integer)
 %status maybe
 %pragma variants c1 c2 cv c3
 %cost c1,c2 0

 Determines whether the target Creature is asleep.
*/
void caosVM::v_ASLP() {
	Creature *c = getTargCreature();

	result.setInt(c->isAsleep());
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
 %pragma variants c1 c2 cv c3

 Makes the target Creature approach the IT agent (or if none, an agent of that category using CAs), 
 blocking until it makes it there or gives up.
*/
void caosVM::c_APPR() {
	CreatureAgent *a = getTargCreatureAgent();
	caos_assert(a);
	
	startBlocking(new blockUntilApproached(targ, _it_));
}

/**
 UNCS (command) unconscious (integer)
 %status stub

 Makes the target Creature conscious if 0, or unconscious if 1.
*/
void caosVM::c_UNCS() {
	VM_PARAM_INTEGER(unconscious)

	Creature *c = getTargCreature();

	// TODO
}

/**
 UNCS (integer)
 %status stub
 %pragma variants c2 cv c3

 Returns 1 if the target Creature is unconscious, or 0 otherwise.
*/
void caosVM::v_UNCS() {
	Creature *c = getTargCreature();

	result.setInt(0); // TODO
}

/**
 FACE (command) number (integer)
 %status stub
*/
void caosVM::c_FACE() {
	VM_PARAM_INTEGER(number)

	caos_assert(targ);
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(targ.get());
	caos_assert(c);

	// TODO
}

/**
 FACE INT (integer)
 %status maybe

 Returns the front-facing pose for the current facial expression of the target creature.
*/
void caosVM::v_FACE_INT() {
	caos_assert(targ);
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(targ.get());
	caos_assert(c);

	result.setInt(c->getFaceSpriteFrame());
}

/**
 FACE STRING (string)
 %status maybe

 Returns the current sprite filename for the face of the target creature.
*/
void caosVM::v_FACE_STRING() {
	caos_assert(targ);
	SkeletalCreature *c = dynamic_cast<SkeletalCreature *>(targ.get());
	caos_assert(c);

	result.setString(c->getFaceSpriteName());
}

/**
 LIKE (command) creature (agent)
 %status stub
 
 Causes the target Creature to state an opinion about the specified Creature.
*/
void caosVM::c_LIKE() {
	VM_PARAM_VALIDAGENT(creature)

	Creature *c = getTargCreature();

	// TODO
}

/**
 LIMB (string) bodypart (integer) genus (integer) gender (integer) age (integer) variant (integer)
 %status stub
 
 Returns the filename for the specified part of a Creature, substituting as necessary.
*/
void caosVM::v_LIMB() {
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(age)
	VM_PARAM_INTEGER(gender)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(bodypart)
	
	Creature *c = getTargCreature();
	
	result.setString(""); // TODO
}

/**
 ORDR SHOU (command) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to all creatures in hearing range.
*/
void caosVM::c_ORDR_SHOU() {
	VM_PARAM_STRING(speech)

	valid_agent(targ);
	// TODO
}

/**
 ORDR SIGN (command) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to all creatures that can see it.
*/
void caosVM::c_ORDR_SIGN() {
	VM_PARAM_STRING(speech)

	valid_agent(targ);
	// TODO
}

/**
 ORDR TACT (command) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to all creatures that are touching it.
*/
void caosVM::c_ORDR_TACT() {
	VM_PARAM_STRING(speech)

	valid_agent(targ);
	// TODO
}

/**
 ORDR WRIT (command) creature (agent) speech (string)
 %status stub

 Makes the target Creature speak the specified speech to the specified creature.
*/
void caosVM::c_ORDR_WRIT() {
	VM_PARAM_STRING(speech)
	VM_PARAM_VALIDAGENT(creature)

	valid_agent(targ);
	// TODO
}

/**
 DREA (command) dream (integer)
 %status maybe

 Turns a Creature's dreaming on and off.  A Creature's instincts are 
 processed while it is dreaming.  If it is not asleep already, then it 
 will be made to sleep before dreaming begins.
*/
void caosVM::c_DREA() {
	VM_PARAM_INTEGER(dream)

	Creature *c = getTargCreature();
	c->setDreaming(dream);
}

/**
 DREA (integer)
 %status maybe

 Returns whether or not the target Creature is dreaming (0 or 1).
*/
void caosVM::v_DREA() {
	Creature *c = getTargCreature();
	result.setInt(c->isDreaming());
}

/**
 BORN (command)
 %status maybe

 Registers the birth of the target Creature, and sends a birth event to 
 the game.
*/
void caosVM::c_BORN() {
	Creature *c = getTargCreature();
	c->born();
}

/**
 CAGE (integer)
 %status maybe
 %pragma variants c1 c2 cv c3

 Returns the integer value of the target Creature's current life stage.
*/
void caosVM::v_CAGE() {
	Creature *c = getTargCreature();
	result.setInt((int)c->getStage());
}

/**
 BYIT (integer)
 %status maybe

 Determines whether or not the target Creature can reach the IT agent (0 
 or 1).
*/
void caosVM::v_BYIT() {
	Creature *c = getTargCreature();

	if (_it_ && agentsTouching(targ, _it_)) // TODO
		result.setInt(1);
	else
		result.setInt(0);
}

/**
 _IT_ (agent)
 %status maybe
 %pragma implementation caosVM::v_IT
 %pragma variants c1 c2 cv c3

 Returns the agent that the OWNR creature was focused on when the 
 current script began running.
*/
void caosVM::v_IT() {
	valid_agent(owner);
	caos_assert(dynamic_cast<CreatureAgent *>(owner.get())); // TODO: return null instead?
	result.setAgent(_it_);
}

/**
 NEWC (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer)
 %status maybe
 
 Creates a new creature over the space of a few ticks, using the specified agent/slot for genetic data. sex is 0 for random, 1 for male or 2 for female.
*/
void caosVM::c_NEWC() {
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)

	// TODO: creation should be blocking and multiple-tick!

	std::map<unsigned int, shared_ptr<class genomeFile> >::iterator i = gene_agent->genome_slots.find(gene_slot);
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

	world.history.getMoniker(world.history.findMoniker(i->second)).moveToCreature(a);
	i->second.reset(); // TODO: remove the slot from the gene_agent entirely

	setTarg(a);
}

/**
 NEW: CREA (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer)
 %status stub
 
 Creates a new creature using the specified agent/slot for genetic data. sex is 0 for random, 1 for male or 2 for female.
*/
void caosVM::c_NEW_CREA() {
	/*VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)*/

	c_NEWC(); // TODO
	//targ = NULL; // TODO
}

/**
 NEW: CREA (command) moniker (integer) sex (integer)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::c_NEW_CREA_c1

 Creates a new creature using the specified moniker for genetic data. sex is 0 for random, 1 for male or 2 for female.
*/
void caosVM::c_NEW_CREA_c1() {
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(moniker)

	caos_assert(moniker != 0);

	std::string realmoniker = std::string((char *)&moniker, 4);
	shared_ptr<genomeFile> genome = world.loadGenome(realmoniker);
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
	world.history.getMoniker(world.history.findMoniker(genome)).moveToCreature(a);

	setTarg(a);
}

/**
 NEW: CRAG (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe
 %pragma variants c3 sm
*/
void caosVM::c_NEW_CRAG() {
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)

       	std::map<unsigned int, shared_ptr<class genomeFile> >::iterator i = gene_agent->genome_slots.find(gene_slot);
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

	world.history.getMoniker(world.history.findMoniker(i->second)).moveToCreature(a);
	i->second.reset(); // TODO: remove the slot from the gene_agent entirely

	setTarg(a);

}

int calculateRand(int value1, int value2); // caosVM_variables.cpp

/**
 LTCY (command) action (integer) min (integer) max (integer)
 %status maybe
 %pragma variants c1 c2 cv c3
*/
void caosVM::c_LTCY() {
	VM_PARAM_INTEGER(max); caos_assert(max >= 0 && max <= 255);
	VM_PARAM_INTEGER(min); caos_assert(min >= 0 && min <= 255);
	VM_PARAM_INTEGER(action);

	int n = calculateRand(min, max);

	Creature *c = getTargCreature();
	
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
 %pragma variants c1
 %pragma implementation caosVM::c_MATE
*/

/**
 MATE (command)
 %pragma variants c2 cv c3
 %status stub
*/
void caosVM::c_MATE() {
	Creature *c = getTargCreature();
	caos_assert(_it_);
	CreatureAgent *t = dynamic_cast<CreatureAgent *>(_it_.get());
	caos_assert(t);
	Creature *d = t->getCreature();

	// TODO
}

/**
 DRV! (integer)
 %status stub
 %pragma implementation caosVM::v_DRV
 %pragma variants c1 c2 cv c3
*/
void caosVM::v_DRV() {
	Creature *c = getTargCreature();
	result.setInt(0); // TODO
}

/**
 IITT (agent)
 %status maybe

 Return the agent which the target creature is currently focused on. Note that you should probably use _IT_ in creature scripts.
*/
void caosVM::v_IITT() {
	Creature *c = getTargCreature();
	result.setAgent(c->getAttentionFocus());
}

/**
 AGES (command) times (integer)
 %status maybe

 Age (ie, increase the life stage of) the target creature the specified number of times.
*/
void caosVM::c_AGES() {
	VM_PARAM_INTEGER(times)
	caos_assert(times >= 0);
	
	Creature *c = getTargCreature();
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
void caosVM::c_LOCI() {
	VM_PARAM_FLOAT(value)
	VM_PARAM_INTEGER(id)
	VM_PARAM_INTEGER(tissue)
	VM_PARAM_INTEGER(organ)
	VM_PARAM_INTEGER(type)

	c2eCreature *c = getc2eCreature(targ.get());
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
void caosVM::v_LOCI() {
	VM_PARAM_INTEGER(id)
	VM_PARAM_INTEGER(tissue)
	VM_PARAM_INTEGER(organ)
	VM_PARAM_INTEGER(type)

	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	float *f = c->getLocusPointer(!type, organ, tissue, id);
	caos_assert(f);
	result.setFloat(*f);
}

/**
 TAGE (integer)
 %status maybe

 Returns age of target creature, in ticks. Only counts ticks since it was BORN.
*/
void caosVM::v_TAGE() {
	Creature *c = getTargCreature();
	result.setInt(c->getAge());
}

/**
 ORGN (integer)
 %status maybe

 Return the number of organs the target creature has.
*/
void caosVM::v_ORGN() {
	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);
	result.setInt(c->noOrgans());
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
void caosVM::v_ORGF() {
	VM_PARAM_INTEGER(value)
	VM_PARAM_INTEGER(organ)
	
	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);
	if (organ < 0 || (unsigned int)organ >= c->noOrgans()) {
		result.setFloat(-1.0f);
		return;
	}
	
	shared_ptr<c2eOrgan> o = c->getOrgan(organ);

	switch (value) {
		case 0: result.setFloat(o->getClockRate()); break;
		case 1: result.setFloat(o->getShortTermLifeforce() / o->getInitialLifeforce()); break;
		case 2: result.setFloat(o->getRepairRate()); break;
		case 3: result.setFloat(o->getInjuryToApply()); break;
		case 4: result.setFloat(o->getInitialLifeforce()); break;
		case 5: result.setFloat(o->getShortTermLifeforce()); break;
		case 6: result.setFloat(o->getLongTermLifeforce()); break;
		case 7: result.setFloat(o->getDamageRate()); break;
		case 8: result.setFloat(o->getEnergyCost()); break;
		case 9: result.setFloat(o->getATPDamageCoefficient()); break;
		default: result.setFloat(-1.0f); break;
	}
}

/**
 ORGI (integer) organ (integer) value (integer)
 %status maybe

 Returns a count of receptors (value 0), emitters (value 1) or reactions (value 2) in the specified organ (numbered starting at zero) of the target creature.
 
 Returns -1 if the specified organ or value is invalid.
*/
void caosVM::v_ORGI() {
	VM_PARAM_INTEGER(value)
	VM_PARAM_INTEGER(organ)
	
	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);
	if (organ < 0 || (unsigned int)organ >= c->noOrgans()) {
		result.setFloat(-1.0f);
		return;
	}
	
	shared_ptr<c2eOrgan> o = c->getOrgan(organ);

	switch (value) {
		case 0: result.setInt(o->getReceptorCount()); break;
		case 1: result.setInt(o->getEmitterCount()); break;
		case 2: result.setInt(o->getReactionCount()); break;
		default: result.setFloat(-1.0f); break;
	}
}

/**
 SOUL (command) part (integer) on (integer)
 %status stub
*/
void caosVM::c_SOUL() {
	VM_PARAM_INTEGER(on)
	VM_PARAM_INTEGER(part)
	caos_assert(part >= 0 && part <= 8);

	Creature *c = getTargCreature();
	// TODO
}
	
/**
 SOUL (integer) part (integer)
 %status stub
*/
void caosVM::v_SOUL() {
	VM_PARAM_INTEGER(part)
	caos_assert(part >= 0 && part <= 8);
		
	Creature *c = getTargCreature();
	result.setInt(1); // TODO
}

/**
 DECN (integer)
 %status maybe
*/
void caosVM::v_DECN() {
	Creature *c = getTargCreature();
	result.setInt(c->getDecisionId());
}

/**
 ATTN (integer)
 %status maybe
*/
void caosVM::v_ATTN() {
	Creature *c = getTargCreature();
	result.setInt(c->getAttentionId());
}

/**
 TOUC (command)
 %status stub
 %pragma variants c1 c2 cv c3
*/
void caosVM::c_TOUC() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 FORF (command) creature (agent)
 %status stub
*/
void caosVM::c_FORF() {
	VM_PARAM_VALIDAGENT(creature)
		
	Creature *c = getTargCreature();
	// TODO: do we handle pointer as well?
	Creature *learn = dynamic_cast<Creature *>(creature.get());
	if (!learn) return;

	// TODO
}

/**
 WALK (command)
 %status maybe
 %pragma variants c1 c2 cv c3
*/
void caosVM::c_WALK() {
	CreatureAgent *c = getTargCreatureAgent();
	
	c->startWalking();
}

/**
 DONE (command)
 %status stub
 %pragma variants c1 c2 cv c3
*/
void caosVM::c_DONE() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 SAYN (command)
 %status stub
 %pragma variants c1 c2 cv c3
*/
void caosVM::c_SAYN() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 IMPT (command) nudge (integer)
 %status stub
 %pragma variants c1 c2
*/
void caosVM::c_IMPT() {
	VM_PARAM_INTEGER(nudge)

	// TODO: check for creature targ?
	// TODO
}

/**
 AIM: (command) actionno (integer)
 %status stub
 %pragma variants c1 c2
*/
void caosVM::c_AIM() {
	VM_PARAM_INTEGER(actionno)

	// TODO: check for creature targ? who knows?
	// TODO
}

/**
 BABY (variable)
 %status maybe
 %pragma variants c1 c2
*/
CAOS_LVALUE_TARG_SIMPLE(BABY, targ->babymoniker) // TODO

/**
 SNEZ (command)
 %status stub
 %pragma variants c1 c2
*/
void caosVM::c_SNEZ() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 DRIV (integer) drive (integer)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::v_DRIV_c1

 Returns the value for the specified drive of the target creature.
*/
void caosVM::v_DRIV_c1() {
	VM_PARAM_INTEGER(drive)
	caos_assert(drive < 16);

	oldCreature *c = getoldCreature(targ.get());
	// TODO: c2 support
	caos_assert(c);

	result.setInt(c->getDrive(drive));
}

/**
 DREA (command) max (integer)
 %status stub
 %pragma variants c1 c2
 %pragma implementation caosVM::c_DREA_c1

 Start dreaming and process (at most?) max instincts.
*/
void caosVM::c_DREA_c1() {
	VM_PARAM_INTEGER(max)

	oldCreature *c = getoldCreature(targ.get());
	caos_assert(c);

	// TODO
}

/**
 BRED (integer) part (integer)
 %status stub
*/
void caosVM::v_BRED() {
	VM_PARAM_INTEGER(part)

	result.setInt(0); // TODO
}

/**
 BVAR (integer)
 %status stub
*/
void caosVM::v_BVAR() {
	result.setInt(0); // TODO
}

/**
 EXPR (command) index (integer) ticks (integer)
 %status stub
*/
void caosVM::c_EXPR() {
	VM_PARAM_INTEGER(ticks)
	VM_PARAM_INTEGER(index)

	// TODO
}

/**
 EXPR (integer)
 %status stub
*/
void caosVM::v_EXPR() {
	result.setInt(0); // TODO
}

/**
 TNTC (command) tintindex (integer) part (integer) commit (integer)
 %status stub
*/
void caosVM::c_TNTC() {
	VM_PARAM_INTEGER(commit)
	VM_PARAM_INTEGER(part)
	VM_PARAM_INTEGER(tintindex)

	// TODO
}

/**
 INJR (command) organ (integer) amount (integer)
 %status stub
 %pragma variants c2
*/
void caosVM::c_INJR() {
	VM_PARAM_INTEGER(amount)
	VM_PARAM_INTEGER(organ)

	valid_agent(targ);
	// TODO
}

/**
 SAY$ (command) string (string)
 %status stub
 %pragma variants c1 c2
*/
void caosVM::c_SAY() {
	VM_PARAM_STRING(string)

	valid_agent(targ);
	// TODO
}

/**
 TRIG (command) lobe (integer) cell (integer) amount (integer)
 %status stub
 %pragma variants c2
*/
void caosVM::c_TRIG() {
	VM_PARAM_INTEGER(amount)
	VM_PARAM_INTEGER(cell)
	VM_PARAM_INTEGER(lobe)

	valid_agent(targ);
	// TODO
}

/**
 MONK (integer)
 %status stub
 %pragma variants c2
*/
void caosVM::v_MONK() {
	oldCreature *c = getoldCreature(targ.get());
	caos_assert(c);

	result.setInt(0); // TODO
}

/**
 MOTR (command) enable (integer)
 %status stub
*/
void caosVM::c_MOTR() {
	VM_PARAM_INTEGER(enable)

	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	// TODO
}

/**
 MOTR (integer)
 %status stub
*/
void caosVM::v_MOTR() {
	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	result.setInt(1); // TODO
}

/**
 MIND (command) enable (integer)
 %status stub
*/
void caosVM::c_MIND() {
	VM_PARAM_INTEGER(enable)

	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	// TODO
}

/**
 MIND (integer)
 %status stub
*/
void caosVM::v_MIND() {
	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	result.setInt(1); // TODO
}

/**
 STEP (command) faculty (integer)
 %status stub
*/
void caosVM::c_STEP() {
	VM_PARAM_INTEGER(faculty)

	c2eCreature *c = getc2eCreature(targ.get());
	caos_assert(c);

	// TODO
}

/**
 SEEN (agent) category (integer)
 %status maybe
*/
void caosVM::v_SEEN() {
	VM_PARAM_INTEGER(category)

	Creature *c = getTargCreature();
	caos_assert(c);

	caos_assert(category >= 0);
	caos_assert((unsigned int)category < c->getNoCategories());

	result.setAgent(c->getChosenAgentForCategory(category));
}

/**
 DOIN (command) noinstincts (integer)
 %status stub

 Make the target creature process the specified number of instincts.
*/
void caosVM::c_DOIN() {
	VM_PARAM_INTEGER(noinstincts)

	Creature *c = getTargCreature();
	caos_assert(c);

	// TODO
}

/**
 INS# (integer)
 %status maybe

 Return the number of unprocessed instincts left in the instinct queue for the target creature.
*/
void caosVM::v_INS() {
	Creature *c = getTargCreature();
	caos_assert(c);

	result.setInt(c->getNoUnprocessedInstincts());
}

// clothes

/**
 BODY (command) set (integer) layer (integer)
 %status stub
*/
void caosVM::c_BODY() {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(set)

	// TODO
}

/**
 BODY (integer) part (integer)
 %status stub
*/
void caosVM::v_BODY() {
	VM_PARAM_INTEGER(part)

	result.setInt(-1); // TODO
}

/**
 DYED (command) part (integer) overlay (integer) set (integer) layer (integer)
 %status stub
*/
void caosVM::c_DYED() {
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
void caosVM::c_HAIR() {
	VM_PARAM_INTEGER(ruffleness)

	// TODO
}

/**
 NUDE (command)
 %status stub
*/
void caosVM::c_NUDE() {
	// TODO
}

/**
 RSET (command)
 %status stub
*/
void caosVM::c_RSET() {
	// TODO
}

/**
 STRE (command)
 %status stub
*/
void caosVM::c_STRE() {
	// TODO
}

/**
 SWAP (command) variant (integer) part (integer) commit (integer)
 %status stub
*/
void caosVM::c_SWAP() {
	VM_PARAM_INTEGER(commit)
	VM_PARAM_INTEGER(part)
	VM_PARAM_INTEGER(variant)

	// TODO
}

/**
 WEAR (command) part (integer) set (integer) layer (integer)
 %status stub
*/
void caosVM::c_WEAR() {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(set)
	VM_PARAM_INTEGER(part)

	// TODO
}

/**
 WEAR (integer) part (integer) layer (integer)
 %status stub
*/
void caosVM::v_WEAR() {
	VM_PARAM_INTEGER(layer)
	VM_PARAM_INTEGER(part)

	result.setInt(-1); // TODO
}

/**
 TNTO (command) tintindex (integer) part (integer) set (integer) layer (integer)
 %status stub
*/
void caosVM::c_TNTO() {
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
void caosVM::v_DFTX() {
	result.setFloat(0); // TODO
}

/**
 DFTY (float)
 %status stub
*/
void caosVM::v_DFTY() {
	result.setFloat(0); // TODO
}

/**
 UFTX (float)
 %status stub
*/
void caosVM::v_UFTX() {
	result.setFloat(0); // TODO
}

/**
 UFTY (float)
 %status stub
*/
void caosVM::v_UFTY() {
	result.setFloat(0); // TODO
}

/**
 HEDX (float) index (integer)
 %status stub
*/
void caosVM::v_HEDX() {
	VM_PARAM_INTEGER(index)

	result.setFloat(0); // TODO
}

/**
 HEDY (float) index (integer)
 %status stub
*/
void caosVM::v_HEDY() {
	VM_PARAM_INTEGER(index)

	result.setFloat(0); // TODO
}

/**
 MTHX (float)
 %status stub
*/
void caosVM::v_MTHX() {
	result.setFloat(0); // TODO
}

/**
 MTHY (float)
 %status stub
*/
void caosVM::v_MTHY() {
	result.setFloat(0); // TODO
}

