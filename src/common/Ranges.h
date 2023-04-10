#pragma once

#include "optional.h"

#include <type_traits>
#include <utility>

template <typename R, typename T>
bool contains(const R& r, T&& value) {
	for (const auto& t : r) {
		if (t == value) {
			return true;
		}
	}
	return false;
}

template <typename R, typename T>
bool contains(const std::initializer_list<R>& r, T&& value) {
	return contains<std::initializer_list<R>, T>(r, std::forward<T>(value));
}

template <typename Iter>
class EnumerateView {
  public:
	class iterator {
	  public:
		bool operator!=(const iterator& other) const {
			return iter != other.iter;
		}
		void operator++() {
			++iter, ++i;
		}
		auto operator*() const {
			// TODO: should this use std::reference_wrapper?
			return std::pair<size_t, decltype(*std::declval<Iter>())>{i, *iter};
		}

	  private:
		friend class EnumerateView;
		iterator(size_t i_, Iter iter_)
			: i(i_), iter(iter_) {}
		size_t i = 0;
		Iter iter;
	};

	EnumerateView(Iter begin_, Iter end_)
		: m_begin(begin_), m_end(end_) {}

	auto begin() const {
		return iterator{0, m_begin};
	}

	auto end() const {
		return iterator{~0u, m_end};
	}

	Iter m_begin;
	Iter m_end;
};

template <typename R>
auto enumerate(R&& r) {
	using std::begin;
	using std::end;
	return EnumerateView<decltype(begin(r))>(begin(r), end(r));
}

template <typename R, typename Pred>
void erase_if(R&& r, Pred pred) {
	using std::begin;
	using std::end;
	for (auto it = begin(r); it != end(r);) {
		if (pred(*it)) {
			it = r.erase(it);
		} else {
			++it;
		}
	}
}

template <class R, class Pred>
auto find_if(R&& r, Pred&& pred) {
	using std::begin;
	using std::end;
	using result_type = std::remove_reference_t<decltype(*begin(r))>;
	auto it = begin(r);
	for (; it != end(r); ++it) {
		if (pred(*it)) {
			return optional<result_type>(*it);
		}
	}
	return optional<result_type>();
}

template <typename R, typename Pred>
int32_t index_if(const R& r, Pred pred) {
	using std::begin;
	using std::end;
	int32_t i = 0;
	auto it = begin(r);
	for (; it != end(r); ++it, ++i) {
		if (pred(*it)) {
			return i;
		}
	}
	return -1;
}

template <typename R>
auto into_vector(R&& r) {
	using std::begin;
	using std::end;
	using Value = std::decay_t<decltype(*begin(r))>;
	// TODO: allocate ahead of time if end(r) - begin(r) works?
	std::vector<Value> result;
	for (auto it = begin(r); it != end(r); ++it) {
		result.push_back(*it);
	}
	return result;
}

template <typename T>
class IotaView {
  public:
	IotaView(T start, T end)
		: start_(start), end_(end) {}

	class iterator {
	  public:
		iterator(T i_)
			: i(i_) {}
		iterator operator++() { return iterator(i++); }
		bool operator!=(iterator other) { return i != other.i; }
		T operator*() { return i; }

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
auto irange(T end) {
	return IotaView<T>(0, end);
}

template <typename T>
auto irange(T start, T end) {
	return IotaView<T>(start, end);
}

template <typename T, typename U>
struct ZipView {
	struct iterator {
		using TIter = decltype(std::declval<T>().begin());
		using UIter = decltype(std::declval<U>().begin());
		TIter left_iter;
		UIter right_iter;
		TIter left_end;
		UIter right_end;
		iterator(TIter left_iter_, UIter right_iter_, TIter left_end_, UIter right_end_)
			: left_iter(left_iter_), right_iter(right_iter_), left_end(left_end_), right_end(right_end_) {}
		iterator& operator++() {
			left_iter++;
			right_iter++;
			if (left_iter == left_end)
				right_iter = right_end;
			if (right_iter == right_end)
				left_iter = left_end;
			return *this;
		}
		auto operator*() {
			return std::pair<decltype(*left_iter), decltype(*right_iter)>{*left_iter, *right_iter};
		}
		bool operator==(iterator& other) {
			return left_iter == other.left_iter && right_iter == other.right_iter;
		}
		bool operator!=(iterator& other) {
			return !(*this == other);
		}
	};

	T left;
	U right;

	ZipView(T left_, U right_)
		: left(left_), right(right_) {}

	auto begin() {
		return iterator(left.begin(), right.begin(), left.end(), right.end());
	}
	auto end() {
		return iterator(left.end(), right.end(), left.end(), right.end());
	}
};

template <typename T, typename U>
auto zip(const T& left, const U& right) {
	return ZipView<T, U>(left, right);
}