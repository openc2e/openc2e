#pragma once

#include "creaturesException.h"
#include "utils/macro_stringify.h"

#include <cassert>

class assertFailure : public creaturesException {
  public:
	using creaturesException::creaturesException;
};

#define caos_assert(x) \
	if (!(x)) { \
		throw assertFailure("Assertion " #x " thrown from " __FILE__ ":" stringify(__LINE__)); \
	}

#define ensure_assert(x) \
	do { \
		bool ensure__v = (x); \
		if (!ensure__v) \
			assert(ensure__v && (x)); \
	} while (0)
