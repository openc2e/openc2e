#pragma once

#include <stddef.h>

// Generic interface for anything with size() and resize() methods
// Used by MFCReader to clean up the header and hide implementation details

struct ResizableContainerView {
	template <typename T>
	ResizableContainerView(T& self_)
		: m_self(&self_),
		  m_size([](const void* self) -> size_t {
			  return static_cast<const T*>(self)->size();
		  }),
		  m_resize([](void* self, size_t n) {
			  static_cast<T*>(self)->resize(n);
		  }) {}

	size_t size() const {
		return m_size(m_self);
	}

	void resize(size_t n) {
		m_resize(m_self, n);
	}

	void* m_self;
	size_t (*m_size)(const void*);
	void (*m_resize)(void*, size_t);
};