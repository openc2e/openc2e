#pragma once

#include <assert.h>
#include <memory>
#include <stddef.h>

template <typename T>
class shared_array {
  public:
	shared_array() = default;
	explicit shared_array(size_t n)
		: ptr_(new T[n], [](T* t) { delete[] t; }), size_(n) {}
	template <typename Iterator>
	shared_array(Iterator begin, Iterator end)
		: shared_array(end - begin) {
		std::copy(begin, end, data());
	}

	T& operator[](ptrdiff_t i) {
		assert(ptr_);
		return ptr_.get()[i];
	}
	const T& operator[](ptrdiff_t i) const {
		assert(ptr_);
		return ptr_.get()[i];
	}

	T* data() {
		return ptr_.get();
	}
	const T* data() const {
		return ptr_.get();
	}

	size_t size() const {
		return size_;
	}

	T* begin() {
		return ptr_.get();
	}
	const T* begin() const {
		return ptr_.get();
	}

	T* end() {
		return ptr_.get() + size_;
	}
	const T* end() const {
		return ptr_.get() + size_;
	}

	explicit operator bool() const {
		return ptr_.get() != nullptr;
	}

  private:
	std::shared_ptr<T> ptr_;
	size_t size_ = 0;
};
