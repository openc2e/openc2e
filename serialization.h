#ifndef SERIALIZATION_H
#define SERIALIZATION_H 1

// Some convenience stuff for serialization

// Note: The order is important on some compilers. All boost/serialization
// includes should go here! (except boost/archive, which should be placed
// before this header in the .cpp file ... I think)

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

// put this in serializable classes, to befriend any serializers in use
#define FRIEND_SERIALIZE(c) \
    friend class boost::serialization::access; \
    template<class Archive> friend void o_save(Archive & ar, const c & obj, const unsigned int version); \
    template<class Archive> friend void o_load(Archive & ar, c & obj, const unsigned int version); \
    template<class Archive> friend void o_serialize(Archive & ar, c & obj, const unsigned int version);

#define WRAP_SPLIT(c) \
    namespace boost { namespace serialization { \
        template <class Archive> inline void load(Archive & ar, c & obj, const unsigned int version) { \
            o_load(ar, obj, version); \
        } \
        template <class Archive> inline void save(Archive & ar, const c & obj, const unsigned int version) { \
            o_save(ar, obj, version); \
        } \
    } }

#define WRAP_SERIALIZE(c) \
    namespace boost { namespace serialization { \
        template <class Archive> inline void serialize(Archive & ar, c & obj, const unsigned int version) { \
            o_serialize(ar, obj, version); \
        } \
    } }
    

#endif

