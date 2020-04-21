#pragma once

template <class... Fs> struct overloaded;

template <class F0, class... Frest>
struct overloaded<F0, Frest...> : F0, overloaded<Frest...> {
  overloaded(F0 f0, Frest... rest) : F0(f0), overloaded<Frest...>(rest...) {}
  using F0::operator();
  using overloaded<Frest...>::operator();
};

template <class F0> struct overloaded<F0> : F0 {
  overloaded(F0 f0) : F0(f0) {}
  using F0::operator();
};

template <typename... Fs>
overloaded<Fs...> make_overloaded_function(Fs... args) {
  return overloaded<Fs...>(args...);
};

template <typename V, typename... Fs> void visit_overloads(V v, Fs... fs) {
  return visit(make_overloaded_function(fs...), v);
}
