#pragma once

#include <limits>
#include <stdexcept>
#include <type_traits>

template <typename To, typename From>
constexpr inline auto numeric_cast(From value)
	-> std::enable_if_t<std::is_integral<To>::value && std::is_integral<From>::value, To> {
	// an integer type's maximum value will always fit as unsigned,
	// and its minimum value will always fit as signed
	constexpr auto high = static_cast<std::make_unsigned_t<To>>(std::numeric_limits<To>::max());
	constexpr auto low = static_cast<std::make_signed_t<To>>(std::numeric_limits<To>::lowest());

	if (
		// first check: if the value > 0, we can safely cast it to unsigned and
		// then compare it against the new maximum. if value <= 0, it can't be
		// over the new maximum
		(value > 0 && static_cast<std::make_unsigned_t<From>>(value) > high)
		// second check: if the value is signed, we can compare it against the
		// new minimum. if the value is unsigned, it can't be under the new minimum.
		|| (std::is_signed<From>::value && static_cast<std::make_signed_t<From>>(value) < low)) {
		throw std::overflow_error{"bad numeric_cast"};
	}
	return static_cast<To>(value);
}

template <typename To>
constexpr inline auto numeric_cast(float value)
	-> std::enable_if_t<std::is_integral<To>::value, To> {
	if ((value >= (std::numeric_limits<To>::max() / 2 + 1) * 2.0f) || (value - std::numeric_limits<To>::lowest() <= -1.0f)) {
		throw std::overflow_error{"bad numeric_cast"};
	}
	return static_cast<To>(value);
}

template <typename To, typename From>
constexpr inline auto numeric_cast(From value)
	-> std::enable_if_t<std::is_enum<To>::value && std::is_arithmetic<From>::value, To> {
	// if casting to an enum, make sure the original value fits in the underlying type
	return static_cast<To>(numeric_cast<std::underlying_type_t<To>>(value));
}

template <typename To, typename From>
constexpr inline auto numeric_cast(From value)
	-> std::enable_if_t<std::is_floating_point<To>::value && std::is_integral<From>::value, To> {
	// converting integers to floating point is always safe (though not necessarily exact)
	return static_cast<To>(value);
}