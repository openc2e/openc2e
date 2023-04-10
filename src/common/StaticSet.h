#pragma once

#include "NumericCast.h"
#include "StaticVector.h"

#include <functional>
#include <stdint.h>

template <class T>
class StaticSetIterator {
  public:
	explicit StaticSetIterator(T* ptr_)
		: ptr(ptr_) {}

	T& operator*() const {
		return *ptr;
	}

	T* operator->() const {
		return ptr;
	}

	bool operator==(const StaticSetIterator& other) const {
		return ptr == other.ptr;
	}

	bool operator!=(const StaticSetIterator& other) const {
		return !(*this == other);
	}

	int32_t operator-(const StaticSetIterator& other) const {
		return numeric_cast<int32_t>(ptr - other.ptr);
	}

	StaticSetIterator& operator++() {
		++ptr;
		return *this;
	}

  private:
	T* ptr = nullptr;
};

template <typename T, size_t N>
class StaticSet {
  public:
	using iterator = StaticSetIterator<T>;
	using const_iterator = StaticSetIterator<const T>;

	StaticSet() {}

	void insert(const T& value) {
		for (auto& t : m_data) {
			if (value == t) {
				return;
			}
		}
		m_data.push_back(value);
	}

	iterator find(const T& value) {
		auto it = begin();
		while (*it != value && it != end()) {
			++it;
		}
		return it;
	}

	void erase(const T& value) {
		auto it = find(value);
		if (it != end()) {
			erase(it);
		}
	}

	iterator erase(iterator pos) {
		int32_t idx = pos - begin();
		if (idx == m_data.ssize() - 1) {
			m_data.pop_back();
		} else {
			// we don't care about element order, so just swap the back element
			// to this position
			m_data[idx] = std::move(m_data.back());
			m_data.pop_back();
		}
		return pos;
	}

	iterator begin() { return iterator(m_data.begin()); }

	const_iterator begin() const { return const_iterator(m_data.begin()); }

	iterator end() { return iterator(m_data.end()); }

	const_iterator end() const { return const_iterator(m_data.end()); }

	size_t size() const {
		return m_data.size();
	}

	int32_t ssize() const {
		return m_data.ssize();
	}

  private:
	StaticVector<T, N> m_data;
};