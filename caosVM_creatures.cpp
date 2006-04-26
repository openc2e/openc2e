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
#include "SkeletalCreature.h"
using std::cerr;

Creature *caosVM::getTargCreature() {
	caos_assert(targ);
	Creature *c = dynamic_cast<Creature *>(targ.get());
	caos_assert(c);
	return c;
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
 %status stub

 Sends a stimulus of the given type to specific Creature.
*/
void caosVM::c_STIM_WRIT() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	VM_PARAM_AGENT(creature)
	
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

	caos_assert(owner);
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
 %status stub

 Changes the target Creature to face a different direction.
*/
void caosVM::c_DIRN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(zombie)

	Creature *c = getTargCreature();
	// TODO
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
 %status stub

 Move the target Creature's foot (along with the rest of the Creature, obviously) to the given 
 coordinates.  You should use this rather than MVTO for Creatures.
*/
void caosVM::c_MVFT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	Creature *c = getTargCreature();

	// TODO: dynamic_cast to Creature *
}
	
/**
 CREA (integer) agent (agent)
 %status done

 Determines whether the given agent is a creature or not (0 or 1).
*/
void caosVM::v_CREA() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(agent)

	Creature *c = dynamic_cast<Creature *>(agent.get());
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

 Determines whether the target Creature is dead (0 or 1).
*/
void caosVM::v_DEAD() {
	Creature *c = getTargCreature();

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

 Returns the target Creature.
*/
void caosVM::v_NORN() {
	result.setAgent(world.selectedcreature);
}

/**
 URGE SIGN (command) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urges all Creatures who can see OWNR to perform the given action on OWNR.
 The two stimuli parameters can range from -1.0 (discourage) to 1.0 (encourage).
*/
void caosVM::c_URGE_SIGN() {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

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
	VM_PARAM_VALIDAGENT(creature)

	// TODO
}

/**
 DRIV (command) drive_id (integer) adjust (float)
 %status stub

 Modifies the level of a drive in target Creature by the given level, which can range from -1.0 (decrease) to 1.0 (increase).
*/
void caosVM::c_DRIV() {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(drive_id)

	Creature *c = getTargCreature();

	// TODO
}

/**
 DRIV (float) drive_id (integer)
 %status stub

 Returns the level of a drive (0.0 to 1.0) in target Creature.
*/
void caosVM::v_DRIV() {
	VM_PARAM_INTEGER(drive_id)

	Creature *c = getTargCreature();

	result.setFloat(0.0f); // TODO
}

/**
 CHEM (command) chemical_id (integer) adjust (float)
 %status maybe

 Modifies the level of a chemical in target Creature's bloodstream by adjust, which can range from -1.0 (decrease) to 1.0 (increase).
*/
void caosVM::c_CHEM() {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(chemical_id)

	Creature *c = getTargCreature();

	c->adjustChemical(chemical_id, adjust);
}

/**
 CHEM (float) chemical_id (integer)
 %status maybe

 Returns the level of a chemical (0.0 to 1.0) in target creature's bloodstream.
*/
void caosVM::v_CHEM() {
	VM_PARAM_INTEGER(chemical_id)
	
	Creature *c = getTargCreature();

	result.setFloat(c->getChemical(chemical_id));
}

/**
 ASLP (command) asleep (integer)
 %status maybe

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

 Determines whether the target Creature is asleep.
*/
void caosVM::v_ASLP() {
	Creature *c = getTargCreature();

	result.setInt(c->isAsleep());
}

/**
 APPR (command)
 %status stub

 Makes the target Creature approach the IT agent (or if none, an agent of that category using CAs), 
 blocking until it makes it there or gives up.
*/
void caosVM::c_APPR() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 UNCS (command) unconscious (integer)
 %status stub

 Makes the target Creature conscious if 0, or unconscious if 1.
*/
void caosVM::c_UNCS() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 UNCS (integer)
 %status stub

 Returns 1 if the target Creature is unconscious, or 0 otherwise.
*/
void caosVM::v_UNCS() {
	Creature *c = getTargCreature();

	result.setInt(0); // TODO
}

/**
 FACE (integer)
 %status stub
 %pragma parser new FACEhelper()

 Returns the front-facing pose for the current facial expression of the target creature.
*/
void caosVM::v_FACE() {
	Creature *c = getTargCreature();
	
	result.setInt(-1);
}

/* // TODO: doc parser needs fixing so we can include this without a conflict
 FACE (string)
 %status stub

 Returns the current sprite filename for the face of the target creature.
*/
void caosVM::s_FACE() {
	Creature *c = getTargCreature();
	
	result.setString("");
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

	caos_assert(targ);
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

 Returns the integer value of the target Creature's current life stage.
*/
void caosVM::v_CAGE() {
	Creature *c = getTargCreature();
	result.setInt((int)c->getStage());
}

/**
 BYIT (integer)
 %status stub

 Determines whether or not the target Creature can reach the IT agent (0 
 or 1).
*/
void caosVM::v_BYIT() {
	Creature *c = getTargCreature();
	result.setInt(0); // TODO
}

/**
 _IT_ (agent)
 %status maybe
 %pragma implementation caosVM::v_IT

 Returns the agent that the OWNR creature was focused on when the 
 current script began running.
*/
void caosVM::v_IT() {
	caos_assert(owner);
	caos_assert(dynamic_cast<Creature *>(owner.get())); // TODO: return null instead?
	result.setAgent(_it_);
}

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
 NEWC (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer)
 %status maybe
*/
void caosVM::c_NEWC() {
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)

	std::map<unsigned int, shared_ptr<class genomeFile> >::iterator i = gene_agent->slots.find(gene_slot);
	caos_assert(i != gene_agent->slots.end());

	// TODO: if sex is 0, randomise to 1 or 2
	// TODO: if variant is 0, randomise between 1 and 8
	SkeletalCreature *c = new SkeletalCreature(i->second, family, (sex == 2), variant);
	c->finishInit();

	world.history.getMoniker(world.history.findMoniker(i->second)).moveToCreature(c);
	i->second.reset(); // TODO: remove the slot from the gene_agent entirely

	setTarg(c);
}

/**
 NEW: CREA (command) family (integer) gene_agent (agent) gene_slot (integer) sex (integer) variant (integer)
 %status stub
*/
void caosVM::c_NEW_CREA() {
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(sex)
	VM_PARAM_INTEGER(gene_slot)
	VM_PARAM_VALIDAGENT(gene_agent)
	VM_PARAM_INTEGER(family)

	c_NEWC(); // TODO
	//targ = NULL; // TODO
}

/**
 LTCY (command) action (integer) min (integer) max (integer)
 %status stub
*/
void caosVM::c_LTCY() {
	VM_PARAM_INTEGER(max); caos_assert(max >= 0 && max <= 255);
	VM_PARAM_INTEGER(min); caos_assert(min >= 0 && min <= 255);
	VM_PARAM_INTEGER(action);

	Creature *c = getTargCreature();
	// TODO
}

/**
 MATE (command)
 %status stub
*/
void caosVM::c_MATE() {
	Creature *c = getTargCreature();
	caos_assert(_it_);
	Creature *t = dynamic_cast<Creature *>(_it_.get());
	caos_assert(t);
	// TODO
}

/**
 DRV! (integer)
 %status stub
 %pragma implementation caosVM::v_DRV
*/
void caosVM::v_DRV() {
	Creature *c = getTargCreature();
	result.setInt(0); // TODO
}

/**
 IITT (agent)
 %status stub
*/
void caosVM::v_IITT() {
	Creature *c = getTargCreature();
	result.setAgent(0); // TODO
}

/**
 AGES (command) times (integer)
 %status maybe
*/
void caosVM::c_AGES() {
	VM_PARAM_INTEGER(times)
	
	Creature *c = getTargCreature();
	for (unsigned int i = 0; i < times; i++) {
		c->ageCreature();
	}
}

/**
 LOCI (command) type (integer) organ (integer) tissue (integer) id (integer) value (float)
 %status stub
*/
void caosVM::c_LOCI() {
	VM_PARAM_FLOAT(value)
	VM_PARAM_INTEGER(id)
	VM_PARAM_INTEGER(tissue)
	VM_PARAM_INTEGER(organ)
	VM_PARAM_INTEGER(type)

	Creature *c = getTargCreature();

	// TODO
}

/**
 LOCI (float) type (integer) organ (integer) tissue (integer) id (integer)
 %status stub
*/
void caosVM::v_LOCI() {
	VM_PARAM_INTEGER(id)
	VM_PARAM_INTEGER(tissue)
	VM_PARAM_INTEGER(organ)
	VM_PARAM_INTEGER(type)

	Creature *c = getTargCreature();

	result.setFloat(0.0f); // TODO
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
*/
void caosVM::v_ORGN() {
	Creature *c = getTargCreature();
	result.setInt(c->noOrgans());
}

/**
 ORGF (float) organ (integer) value (integer)
 %status stub
*/
void caosVM::v_ORGF() {
	VM_PARAM_INTEGER(value)
	VM_PARAM_INTEGER(organ)
	
	Creature *c = getTargCreature();
	caos_assert(organ >= 0 && organ < c->noOrgans());
	Organ *o = c->getOrgan(organ);

	switch (value) {
		// TODO
		default: throw creaturesException("Unknown value for ORGF");
	}
}

/**
 ORGI (integer) organ (integer) value (integer)
 %status stub
*/
void caosVM::v_ORGI() {
	VM_PARAM_INTEGER(value)
	VM_PARAM_INTEGER(organ)
	
	Creature *c = getTargCreature();
	caos_assert(organ >= 0 && organ < c->noOrgans());
	Organ *o = c->getOrgan(organ);

	switch (value) {
		// TODO
		default: throw creaturesException("Unknown value for ORGI");
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
 %status stub
*/
void caosVM::v_DECN() {
	Creature *c = getTargCreature();
	result.setInt(0); // TODO
}

/**
 ATTN (integer)
 %status stub
*/
void caosVM::v_ATTN() {
	Creature *c = getTargCreature();
	result.setInt(0); // TODO
}

/* vim: set noet: */
