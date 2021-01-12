#pragma once

// Forward declarations so we can friend them later without pulling in
// huge system headers.
namespace boost {
namespace serialization {
class access;
}
} // namespace boost

#define SER_PROTO(friend, fname, c, const) \
	template <class Archive> \
	friend void fname(Archive& ar, const c& obj, const unsigned int version)

// put this in serializable classes, to befriend any serializers in use
#define FRIEND_SERIALIZE(c) \
	friend class boost::serialization::access; \
	SER_PROTO(friend, o_save, c, const); \
	SER_PROTO(friend, o_load, c, ); \
	SER_PROTO(friend, o_serialize, c, );
