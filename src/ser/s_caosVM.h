#ifndef SER_CAOSVM_H
#define SER_CAOSVM_H 1

#include "ser/s_caosScript.h"
#include "ser/s_AgentRef.h"
#include "serialization.h"
#include "caosScript.h"


WRAP_SERIALIZE(vmStackItem);
SER_PROTO(o_serialize, vmStackItem, ) {
	ar & obj.type;
	ar & obj.i_val;
	ar & obj.p_val;
	ar & obj.bytestring;
}

WRAP_SERIALIZE(callStackItem);
SER_PROTO(o_serialize, callStackItem, ) {
	ar & obj.valueStack;
	ar & obj.nip;
}

WRAP_SERIALIZE(caosVM);
SER_PROTO(o_serialize, caosVM, ) {
	ar & obj.trace;

	// FIXME: blocking
	ar & obj.currentscript;
	ar & obj.nip & obj.cip;

	ar & obj.inst & obj.lock & obj.stop_loop;
	ar & obj.timeslice;

	ar & obj.valueStack & obj.callStack;

	// Don't serialize I/O stuff
	ar & obj.var;
	ar & obj._p_;
	ar & obj.targ & obj.owner;
	ar & obj._it_ & obj.from;
	ar & obj.part;
	ar & obj.camera;
	ar & obj.result; // XXX: is this needed?
}

#endif

