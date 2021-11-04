/*
 *  serialization.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 06 Apr 2006.
 *  Copyright (c) 2006 Bryan Donlan. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */
#ifndef SERIALIZATION_H
#define SERIALIZATION_H 1

// Some convenience stuff for serialization

// Note: The order is important on some compilers. All boost/serialization
// includes should go here! (except boost/archive, which should be placed
// before this header in the .cpp file ... I think)

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/std::shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>


template <class Archive, class Object>
void pre_save(Archive& ar, const Object& obj, const unsigned int version) {
}
template <class Archive, class Object>
void post_save(Archive& ar, const Object& obj, const unsigned int version) {
}
template <class Archive, class Object>
void pre_load(Archive& ar, Object& obj, const unsigned int version) {
}
template <class Archive, class Object>
void post_load(Archive& ar, Object& obj, const unsigned int version) {
}

#define WRAP_SPLIT(c) \
	BOOST_SERIALIZATION_SPLIT_FREE(c); \
	BOOST_CLASS_EXPORT(c); \
	namespace boost { \
	namespace serialization { \
	SER_PROTO(inline, load, c, ) { \
		pre_load(ar, obj, version); \
		o_load(ar, obj, version); \
		post_load(ar, obj, version); \
	} \
	SER_PROTO(inline, save, c, const) { \
		pre_save(ar, obj, version); \
		o_save(ar, obj, version); \
		post_save(ar, obj, version); \
	} \
	} \
	}

#define WRAP_SERIALIZE(c) \
	SAVE(c) { o_serialize(ar, *const_cast<c*>(&obj), version); } \
	LOAD(c) { o_serialize(ar, obj, version); }

#define SER_BASE(ar, bc) \
	do { \
		ar& boost::serialization::base_object<bc>(obj); \
	} while (0)

#define SAVE(c) \
	WRAP_SPLIT(c); \
	SER_PROTO(, o_save, c, const)
#define LOAD(c) SER_PROTO(, o_load, c, )
#define PRE_SAVE(c) SER_PROTO(, pre_save, c, const)
#define POST_SAVE(c) SER_PROTO(, post_save, c, const)
#define PRE_LOAD(c) SER_PROTO(, pre_load, c, )
#define POST_LOAD(c) SER_PROTO(, post_load, c, )
#define SERIALIZE(c) \
	WRAP_SERIALIZE(c); \
	SER_PROTO(, o_serialize, c, )

inline static void STUB_DIE(const std::string& msg, const char* f, unsigned int l) {
	std::cerr << "A trickery! SER_STUB'd: " << msg << "@" << f << ":" << l << std::endl;
	abort();
}

#define SER_STUB_BASE(c, type) \
	type(c) { \
		STUB_DIE(std::string(#type " stubbed for " #c), __FILE__, __LINE__); \
	}

#define SAVE_STUB(c) SER_STUB_BASE(c, SAVE)
#define LOAD_STUB(c) SER_STUB_BASE(c, LOAD)
#define SERIALIZE_STUB(c) SER_STUB_BASE(c, SERIALIZE)

#endif
