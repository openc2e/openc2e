#pragma once

#include <utility>

template <typename Iter>
class enumerate_view {
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
		friend class enumerate_view;
		iterator(size_t i_, Iter iter_)
			: i(i_), iter(iter_) {}
		size_t i = 0;
		Iter iter;
	};

	enumerate_view(Iter begin_, Iter end_)
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
	return enumerate_view<decltype(r.begin())>(r.begin(), r.end());
}

template <typename R, typename Pred>
void erase_if(R&& r, Pred pred) {
	for (auto it = r.begin(); it != r.end();) {
		if (pred(*it)) {
			it = r.erase(it);
		} else {
			++it;
		}
	}
}

template <typename R, typename Pred>
int32_t index_if(const R& r, Pred pred) {
	int32_t i = 0;
	auto it = r.begin();
	const auto end = r.end();
	for (; it != end; ++it, ++i) {
		if (pred(*it)) {
			return i;
		}
	}
	return -1;
}