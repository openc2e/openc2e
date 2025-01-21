#pragma once

// Once openc2e goes to C++20, replace this with std::span

#include <cstddef>
#include <type_traits>

template <typename T>
class span {
  public:
	span() {}
	span(T* first, size_t count)
		: data_(first), size_(count) {}
	span(T* first, T* last)
		: data_(first), size_(last - first) {}
	template <typename R,
		typename = decltype(std::declval<R>().size()),
		typename = std::enable_if_t<std::is_convertible<decltype(std::declval<R>().data()), T*>::value>>
	span(R&& r)
		: data_(r.data()), size_(r.size()) {}

	// construct a span from a C-style array. disable this for char arrays because
	// const char array literals always have a nul character at the end, which is
	// likely _not_ what you want! use StringView instead, or if you really need
	// a span over a char array use the span<char>(char*, size_t) overload.
	template <size_t N, typename U = T,
		typename = std::enable_if_t<!std::is_same<std::remove_cv_t<U>, char>::value>>
	span(T (&data)[N])
		: span(data, N) {}

	T& front() const { return data_[0]; }
	T& back() const { return data_[size_ - 1]; }
	T& operator[](size_t i) const { return data_[i]; }

	T* begin() const { return data_; }
	T* end() const { return data_ + size_; }

	T* data() const { return data_; }
	size_t size() const { return size_; }
	bool empty() const { return size_ == 0; }

  private:
	T* data_ = nullptr;
	size_t size_ = 0;
};
