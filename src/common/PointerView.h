#pragma once

#include <memory>

template <typename T>
class PointerView {
  public:
	PointerView() {}
	PointerView(T* ptr)
		: m_ptr(ptr) {}
	PointerView(const std::shared_ptr<T>& ptr)
		: m_ptr(ptr.get()) {}
	PointerView(const std::unique_ptr<T>& ptr)
		: m_ptr(ptr.get()) {}

	explicit operator bool() const {
		return m_ptr != nullptr;
	}

	T* get() {
		return m_ptr;
	}

	operator T*() {
		return m_ptr;
	}

	T& operator*() {
		return *m_ptr;
	}

	T* operator->() {
		return m_ptr;
	}

  private:
	T* m_ptr = nullptr;
};