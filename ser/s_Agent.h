#ifndef SER_AGENT_H
#define SER_AGENT_H 1

#include "ser/s_AgentRef.h"
#include "ser/s_caosVar.h"
#include <serialization.h>
#include <Agent.h>

SERIALIZE(Agent) {
    assert(!obj.dying);
    
    ar & obj.var;
    ar & obj.name_variables;
#if 0
    if (1 <= version)
        ar & slots;
#endif
    
    ar & obj.vm;

    ar & obj.unid & obj.zorder & obj.tickssincelasttimer & obj.timerrate;
    ar & obj.vmStack;
    ar & obj.floated;


    // XXX: sound?
    
    ar & obj.carry_points & obj.carried_points;
    ar & obj.carrying & obj.carriedby;

    // attr
	ar & obj.carryable;
	ar & obj.mouseable;
	ar & obj.activateable;
	ar & obj.greedycabin;
	ar & obj.invisible;
	ar & obj.floatable;
	ar & obj.suffercollisions;
	ar & obj.sufferphysics;
	ar & obj.camerashy;
	ar & obj.openaircabin;
	ar & obj.rotatable;
	ar & obj.presence;
	// bhvr
	ar & obj.cr_can_push;
	ar & obj.cr_can_pull;
	ar & obj.cr_can_stop;
	ar & obj.cr_can_hit;
	ar & obj.cr_can_eat;
	ar & obj.cr_can_pickup;
	// imsk
	ar & obj.imsk_key_down;
	ar & obj.imsk_key_up;
	ar & obj.imsk_mouse_move;
	ar & obj.imsk_mouse_down;
	ar & obj.imsk_mouse_up;
	ar & obj.imsk_mouse_wheel;
	ar & obj.imsk_translated_char;
	
	ar & obj.paused;
	ar & obj.visible;
	ar & obj.displaycore;

    ar & obj.clac & obj.clik;
    ar & obj.family & obj.genus;
    ar & obj.species;

    ar & obj.velx & obj.vely;
    ar & obj.accg & obj.aero;
    ar & obj.friction & obj.perm & obj.elas;
    ar & obj.x & obj.y & obj.falling;

    ar & obj.range;
    ar & obj.floatingagent;
    
}

#endif

