#pragma once

#include "Exception.h"
#include "macro_stringify.h"

#include <cassert>

class assertFailure : public Exception {
  public:
	using Exception::Exception;
};

#define THROW_IFNOT(x) \
	if (!(x)) { \
		throw assertFailure("Assertion " #x " thrown from " __FILE__ ":" stringify(__LINE__)); \
	}
