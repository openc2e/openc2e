#pragma once

#include "optional.h"
#include <type_traits>

template<class Sequence, class Predicate>
auto find_if(Sequence&& s, Predicate&& p) {
  using std::begin; using std::end; // for ADL
  auto b = begin(s);
  auto e = end(s);

  auto result = std::find_if(begin(s), end(s), p);
  using result_type = std::remove_reference_t<decltype(*result)>;
  if (result == end(s)) {
    return optional<result_type>();
  }
  return optional<result_type>(*result);
}