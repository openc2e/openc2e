#pragma once

#include <type_traits>

template <class Left, class Right>
constexpr inline std::enable_if_t<std::is_arithmetic<Left>::value && std::is_arithmetic<Right>::value && std::is_signed<Left>::value == std::is_signed<Right>::value, bool>
cmp_less(Left left, Right right) {
	return left < right;
}

template <class Left, class Right>
constexpr inline std::enable_if_t<std::is_arithmetic<Left>::value && std::is_arithmetic<Right>::value && std::is_signed<Left>::value && !std::is_signed<Right>::value, bool>
cmp_less(Left left, Right right) {
	return left < 0 || static_cast<std::make_unsigned_t<Left>>(left) < right;
}

template <class Left, class Right>
constexpr inline std::enable_if_t<std::is_arithmetic<Left>::value && std::is_arithmetic<Right>::value && !std::is_signed<Left>::value && std::is_signed<Right>::value, bool>
cmp_less(Left left, Right right) {
	return right >= 0 && left < static_cast<std::make_unsigned_t<Right>>(right);
}

template <class Left, class Right>
constexpr inline bool
cmp_greater(Left left, Right right) noexcept {
	return cmp_less(right, left);
}