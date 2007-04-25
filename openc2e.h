/*
 *  openc2e.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Thu Jun 03 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#ifndef __OPENC2E_H
#define __OPENC2E_H

#ifdef _MSC_VER
# define M_PI           3.14159265358979323846  /* pi */
#endif

#include <iostream>
#include <cassert>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "exceptions.h"

typedef std::vector<unsigned char> bytestring_t;

class assertFailure : public creaturesException {
public:
	assertFailure(const char *x) throw() : creaturesException(x) { }
};

#define caos_assert(x) if (!(x)) { throw caosException(#x, __FILE__, __LINE__); }
#define ensure(x) do {\
	bool ensure__v = (x); \
	if (!ensure__v) \
		assert(ensure__v && (x)); \
} while (0)

// Forward declarations so we can friend them later without pulling in
// huge system headers.
namespace boost {
	namespace serialization {
		class access;
	}
}

#define SER_PROTO(friend, fname, c, const) \
    template <class Archive> friend void fname(Archive &ar, const c & obj, const unsigned int version)

// put this in serializable classes, to befriend any serializers in use
#define FRIEND_SERIALIZE(c) \
    friend class boost::serialization::access; \
    SER_PROTO(friend, o_save, c, const); \
    SER_PROTO(friend, o_load, c, ); \
    SER_PROTO(friend, o_serialize, c, );

#endif
/* vim: set noet: */
