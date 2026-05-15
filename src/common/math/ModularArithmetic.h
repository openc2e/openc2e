#pragma once

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <type_traits>

namespace detail {

// truncating_remainder
// Remainder of dividing `value` by `modulus`. Sign of result matches sign of `value`.

inline int32_t truncating_remainder(int32_t value, int32_t modulus) {
	return value % modulus;
}

inline float truncating_remainder(float value, float modulus) {
	return fmod(value, modulus);
}

// euclidean_remainder
// Least non-negative remainder of dividing `value` by `modulus`.

template <typename T>
T euclidean_remainder(T value, T modulus) {
	// Centered remainder could also be used here with no change to the math.
	T result = truncating_remainder(value, modulus);
	return result + (result < 0 ? modulus : 0);
}

} // namespace detail

// mod_remainder
// Least non-negative remainder of dividing `value` by `modulus`. Also known as the Euclidean
// remainder, or least non-negative residue. Differs from the % operator and the fmod/remainder
// functions because it returns a non-negative number even when given a negative `value` argument.

template <typename T>
T mod_remainder(T value, T modulus) {
	assert(modulus > 0);
	return ::detail::euclidean_remainder(value, modulus);
}


// mod_distance
// Distance between two values in modular arithmetic system defined by `modulus`.

template <typename T>
T mod_distance(T a, T b, T modulus) {
	T x = mod_remainder(a - b, modulus);
	T y = mod_remainder(b - a, modulus);
	return x < y ? x : y;
}

// mod_point_in_right_open_interval
// Test that point x is in the interval [a, a+a_width) in modular arithmetic system defined
// by `modulus`.

template <typename T>
bool mod_point_in_right_open_interval(T x, T a, T a_width, T modulus) {
	return mod_remainder(x - a, modulus) < a_width;
}

// mod_intervals_overlap
// Test that intervals [a, a+a_width) and [b, b+b_width) overlap in modular arithmetic
// system defined by `modulus`.

template <typename T>
bool mod_intervals_overlap(T a, T a_width, T b, T b_width, T modulus) {
	return mod_remainder(b - a, modulus) < a_width || mod_remainder(a - b, modulus) < b_width;
}