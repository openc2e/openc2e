#pragma once

#include <iterator>
#include <type_traits>

template <typename T>
class IotaViewImpl {
  public:
	IotaViewImpl(T start, T end)
		: start_(start), end_(end) {}

	class iterator {
	  public:
		iterator(T i_)
			: i(i_) {}
		iterator operator++() { return iterator(i++); }
		bool operator!=(iterator other) { return i != other.i; }
		T operator*() { return i; }

		using difference_type = std::make_signed_t<T>;
		using value_type = T;
		using pointer = T;
		using reference = T;
		using iterator_category = std::forward_iterator_tag;

	  private:
		T i;
	};

	iterator begin() const { return iterator(start_); }
	iterator end() const { return iterator(end_); }

  private:
	const T start_;
	const T end_;
};

template <typename T>
auto iota_view(T start, T end) {
	return IotaViewImpl<T>(start, end);
}