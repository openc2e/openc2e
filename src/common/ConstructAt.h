#pragma once

#include <utility>

// Once openc2e goes to C++20, replace this with std::construct_at

template <class T, class... Args>
T* construct_at(T* p, Args&&... args) {
	return new (p) T(std::forward<Args>(args)...);
}

// Once openc2e goes to C++17, replace this with std::destroy_at

template <class T>
void destroy_at(T* p) {
	p->~T();
}