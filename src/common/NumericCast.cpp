#include "NumericCast.h"

#include "demangle.h"

#include <fmt/format.h>
#include <stdexcept>

template <typename From>
void throw_numeric_cast_error(const std::type_info& to, From value) {
	throw std::overflow_error{fmt::format("couldn't numeric_cast {} to {}", value, demangle(to.name()))};
}

// instantiate for the fundamental types rather than the fixed-width types like uint32_t.
// the fundamental types can have the same size as each other but are still considered
// distinct types, so e.g. if long and long long are both 8 bytes then only one of them
// will be the underlying type for uint64_t. this can differ per compiler and platform
// so the safest thing is to instantiate for every fundamental type and hope all the
// fixed-width types are covered.

template void throw_numeric_cast_error<>(const std::type_info&, signed char);
template void throw_numeric_cast_error<>(const std::type_info&, unsigned char);
template void throw_numeric_cast_error<>(const std::type_info&, signed short);
template void throw_numeric_cast_error<>(const std::type_info&, unsigned short);
template void throw_numeric_cast_error<>(const std::type_info&, signed int);
template void throw_numeric_cast_error<>(const std::type_info&, unsigned int);
template void throw_numeric_cast_error<>(const std::type_info&, signed long);
template void throw_numeric_cast_error<>(const std::type_info&, unsigned long);
template void throw_numeric_cast_error<>(const std::type_info&, signed long long);
template void throw_numeric_cast_error<>(const std::type_info&, unsigned long long);
template void throw_numeric_cast_error<>(const std::type_info&, float);
template void throw_numeric_cast_error<>(const std::type_info&, double);