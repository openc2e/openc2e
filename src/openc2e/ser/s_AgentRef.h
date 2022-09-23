#ifndef SER_AGENTREF_H
#define SER_AGENTREF_H 1

#include "AgentRef.h"
#include "ser/s_Agent.h" // This loop is safe; template instantiation is deferred
#include "serialization.h"

LOAD(AgentRef) {
	Agent* agent;
	ar& agent;
	obj.set(agent);
}

SAVE(AgentRef) {
	Agent* a = obj.get();
	ar& a;
}

#endif
