#pragma once

#include <cassert>
#include "macro_stringify.h"
#include "creaturesException.h"

class assertFailure : public creaturesException {
public:
	using creaturesException::creaturesException;
};

#define caos_assert(x) if (!(x)) { throw assertFailure("Assertion " #x " thrown from " __FILE__ ":" stringify(__LINE__)); }

#define ensure_assert(x) do {\
	bool ensure__v = (x); \
	if (!ensure__v) \
		assert(ensure__v && (x)); \
} while (0)
