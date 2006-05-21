#ifndef SER_CAOSVAR_H
#define SER_CAOSVAR_H 1

#include "caosVar.h"
#include "serialization.h"
#include <boost/serialization/variant.hpp>
#include "caosVM.h"

// XXX: stub so serialtest works until everything else serializes
SAVE(AgentRef) {}
LOAD(AgentRef) { obj = NULL; }

// XXX belongs in ser/caosVM.h
SERIALIZE(bytestring_t) {
    ar & obj;
}

SERIALIZE(nulltype_tag) { }

SERIALIZE(caosVar) {
    ar & obj.value;
}

#endif

