#pragma once

#include <type_traits>

template<typename... Ts> struct make_void { typedef void type;};
template<typename... Ts> using void_t = typename make_void<Ts...>::type;

template <typename T, typename = void>
constexpr bool is_complete = false;

template <typename T>
constexpr bool is_complete<T, ::void_t<decltype(sizeof(T) != 0)>> = true;
