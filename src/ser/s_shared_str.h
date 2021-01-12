#ifndef SER_SHARED_STR_H
#define SER_SHARED_STR_H 1

#include "serialization.h"

#include <boost/serialization/std::shared_ptr.hpp>
#include <boost/serialization/string.hpp>

SERIALIZE(shared_str) {
	ar& obj.p;
}
BOOST_CLASS_IMPLEMENTATION(shared_str, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(shared_str, boost::serialization::track_never);

#endif
