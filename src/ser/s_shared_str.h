#ifndef SER_SHARED_STR_H
#define SER_SHARED_STR_H 1

#include "serialization.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>

SERIALIZE(shared_str) { ar & obj.p; }
SERIALIZE(shared_str_b) { ar & obj.s; }
BOOST_CLASS_IMPLEMENTATION(shared_str, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(shared_str, boost::serialization::track_never);
BOOST_CLASS_IMPLEMENTATION(shared_str_b, boost::serialization::object_class_info);
BOOST_CLASS_TRACKING(shared_str_b, boost::serialization::track_always);

#endif
