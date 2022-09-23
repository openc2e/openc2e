#ifndef SER_CAOSVAR_H
#define SER_CAOSVAR_H 1

#include "caosVM.h"
#include "caosValue.h"
#include "ser/s_physics.h"
#include "serialization.h"

#include <boost/serialization/variant.hpp>

// XXX: stub so serialtest works until everything else serializes
SAVE(AgentRef) {
}
LOAD(AgentRef) {
	obj = NULL;
}
BOOST_CLASS_IMPLEMENTATION(AgentRef, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(AgentRef, boost::serialization::track_never);

SERIALIZE(nulltype_tag) {
}
BOOST_CLASS_IMPLEMENTATION(nulltype_tag, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(nulltype_tag, boost::serialization::track_never);


SERIALIZE(caosValue) {
	ar& obj.value;
}
BOOST_CLASS_IMPLEMENTATION(caosValue, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(caosValue, boost::serialization::track_never);


#endif
