#pragma once

#include <limits>
#include <type_traits>
#include <typeinfo>

template <typename From>
[[noreturn]] void throw_numeric_cast_error(const std::type_info&, From);

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
		throw_numeric_cast_error(typeid(To), value);
	}
	return static_cast<To>(value);
}

template <typename To>
constexpr inline auto numeric_cast(float value)
	-> std::enable_if_t<std::is_integral<To>::value, To> {
	if ((value >= (std::numeric_limits<To>::max() / 2 + 1) * 2.0f) || (value - std::numeric_limits<To>::lowest() <= -1.0f)) {
		throw_numeric_cast_error(typeid(To), value);
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
	// float usually has 24 digits, double has 53 digits. any integer size smaller than that should be fine.
	if (std::numeric_limits<To>::digits >= std::numeric_limits<From>::digits) {
		return static_cast<To>(value);
	}

	// would lose precision
	if (value > (1 << std::numeric_limits<To>::digits) ||
		(std::is_signed<From>::value && value < 0 && value < -(1 << std::numeric_limits<To>::digits))) {
		throw_numeric_cast_error(typeid(To), value);
	}
	return static_cast<To>(value);
}