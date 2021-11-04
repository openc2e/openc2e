#pragma once

#include "optional.h"

#include <type_traits>

template <class Sequence, class Predicate>
auto find_if(Sequence&& s, Predicate&& p) {
	using std::begin;
	using std::end; // for ADL
	auto it = begin(s);
	using result_type = std::remove_reference_t<decltype(*it)>;
	for (; it != end(s); ++it) {
		if (p(*it)) {
			return optional<result_type>(*it);
		}
	}
	return optional<result_type>();
}