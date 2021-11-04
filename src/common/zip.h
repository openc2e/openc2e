#pragma once

#include <utility>

template <typename T, typename U>
struct zip_impl {
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
			return std::make_pair(*left_iter, *right_iter);
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

	zip_impl(T left_, U right_)
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
	return zip_impl<T, U>(left, right);
}