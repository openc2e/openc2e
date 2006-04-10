#ifndef SER_CAOSVAR_H
#define SER_CAOSVAR_H 1

#include <caosVar.h>
#include <serialization.h>
#include <boost/serialization/variant.hpp>
#include <caosVM.h>

// XXX: stub so serialtest works until everything else serializes
SAVE(AgentRef) {}
LOAD(AgentRef) { obj = NULL; }

// XXX belongs in ser/caosVM.h
SAVE(bytestring_t) {
    ar & *obj;
}

LOAD(bytestring_t) {
    std::vector<unsigned int> *v = new std::vector<unsigned int>;
    ar & *v;
    obj = bytestring_t(v);
}

SERIALIZE(caosVar) {
    ar & obj.type;
    ar & obj.value;
}

#endif

