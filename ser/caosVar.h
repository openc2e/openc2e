#ifndef SER_CAOSVAR_H
#define SER_CAOSVAR_H 1

#include <caosVar.h>
#include <serialization.h>

SAVE(caosVar) {
    ar & obj.type;
    switch (obj.type) {
        case NULLTYPE: break;
        case AGENT: /*ar << obj.values.agentValue.ref;*/ assert(0); break;
        case INTEGER: ar << obj.values.intValue; break;
        case FLOAT: ar << obj.values.floatValue; break;
        case STRING: ar << obj.values.stringValue->str; break;
        default: assert(0);
    }
}

template<class Archive>
void o_load(Archive & ar, caosVar & obj, const unsigned int version)
{
    obj.reset();
    ar & obj.type;
    switch (obj.type) {
        case NULLTYPE: break;
        case AGENT: /* ar >> obj.values.agentValue.ref; */ assert(0); break;
        case INTEGER: ar >> obj.values.intValue; break;
        case FLOAT: ar >> obj.values.floatValue; break;
        case STRING:
            obj.values.stringValue = new(caosVarSlab) stringwrap();
            ar >> obj.values.stringValue->str; break;
        default: assert(0);
    }
}

#endif

