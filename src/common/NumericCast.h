#pragma once

#include <stdint.h>
#include <typeinfo>

class BadNumericCast : public std::bad_cast {
  public:
	const char* what() const noexcept {
		return "bad numeric cast";
	}
};

template <typename To, typename From>
To numeric_cast(From value);

template <>
inline uint32_t numeric_cast(int32_t value) {
	if (value < 0) {
		throw BadNumericCast();
	}
	return static_cast<uint32_t>(value);
}

template <>
inline int32_t numeric_cast(uint32_t value) {
	if (value >= static_cast<uint32_t>(std::numeric_limits<int32_t>::max())) {
		throw BadNumericCast();
	}
	return static_cast<int32_t>(value);
}

template <>
inline int32_t numeric_cast(size_t value) {
	if (value >= static_cast<size_t>(std::numeric_limits<int32_t>::max())) {
		throw BadNumericCast();
	}
	return static_cast<int32_t>(value);
}