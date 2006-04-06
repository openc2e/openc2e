#ifndef SER_CAOSVAR_H
#define SER_CAOSVAR_H 1

#include "caosVar.h"
#include "serialization.h"

BOOST_SERIALIZATION_SPLIT_FREE(caosVar)
WRAP_SPLIT(caosVar)
BOOST_CLASS_EXPORT(caosVar)
    
template<class Archive>
void o_save(Archive & ar, const caosVar & obj, const unsigned int version) {
    ar & obj.type;
    switch (obj.type) {
        case NULLTYPE: break;
        case AGENT: assert(0); break;
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
        case AGENT: assert(0); break;
        case INTEGER: ar >> obj.values.intValue; break;
        case FLOAT: ar >> obj.values.floatValue; break;
        case STRING:
            obj.values.stringValue = new(caosVarSlab) stringwrap();
            ar >> obj.values.stringValue->str; break;
        default: assert(0);
    }
}

#endif

