#pragma once

#include <vector>

template <typename R>
auto to_vector(R&& r) {
  using std::begin;
  using std::end;
  using Value = typename decltype(begin(r))::value_type;
  return std::vector<Value>(begin(r), end(r));
}