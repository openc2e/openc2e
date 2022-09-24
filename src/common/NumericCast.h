#pragma once

#include "SafeCompare.h"

#include <limits>
#include <stdint.h>
#include <type_traits>
#include <typeinfo>

class BadNumericCast : public std::bad_cast {
  public:
	const char* what() const noexcept {
		return "bad numeric cast";
	}
};

template <typename To, typename From, typename = std::enable_if_t<std::is_arithmetic<To>::value && std::is_arithmetic<From>::value>>
constexpr inline To numeric_cast(From value) {
	if (cmp_greater(value, std::numeric_limits<To>::max())) {
		throw BadNumericCast();
	}
	if (cmp_less(value, std::numeric_limits<To>::lowest())) {
		throw BadNumericCast();
	}
	return static_cast<To>(value);
}