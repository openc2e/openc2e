#pragma once

#include <algorithm> // stable_sort
#include <exception>
#include <fmt/core.h>
#include <limits>
#include <stdint.h>
#include <utility>
#include <vector>

// DenseSlotMap
//
// This container is meant for scenarios where you want to:
//   (1) store all instances of a type in the same place
//   (2) reference objects without giving out raw pointers
//   (3) have stable integer IDs for each object
//

struct SlotMapKey {
	using IndexType = uint16_t;
	using CounterType = uint16_t;

	static constexpr auto MAX_INDEX = std::numeric_limits<IndexType>::max();
	static constexpr auto MAX_COUNTER = std::numeric_limits<CounterType>::max();

	SlotMapKey(IndexType index_, CounterType counter_)
		: index(index_), counter(counter_) {}

	IndexType index;
	CounterType counter;

	constexpr SlotMapKey()
		: index(MAX_INDEX), counter(MAX_COUNTER) {
	}

	explicit operator bool() const {
		return *this != SlotMapKey{};
	}

	bool operator==(const SlotMapKey& other) const {
		return index == other.index && counter == other.counter;
	}

	bool operator!=(const SlotMapKey& other) const {
		return !(*this == other);
	}

	uint32_t to_integral() const {
		return (static_cast<uint32_t>(counter) << 16) | static_cast<uint32_t>(index);
	}
};

template <typename T>
class DenseSlotMap {
  public:
	using ValueType = T;
	using Key = SlotMapKey;

  private:
	using IndexType = Key::IndexType;
	using CounterType = Key::CounterType;

	static constexpr IndexType NULL_INDEX = Key().index;

  public:
	Key add(T value) {
		Key new_id;
		if (m_deleted.size()) {
			new_id = m_deleted.back();
			m_deleted.pop_back();
		} else {
			size_t idx = m_sparse.size();
			if (idx >= NULL_INDEX) {
				// whoops, ran out of ids
				std::terminate();
			}
			new_id = Key(static_cast<IndexType>(idx), 0);
		}
		if (new_id.index >= m_sparse.size()) {
			m_sparse.resize(new_id.index + 1);
		}
		if (m_dense.size() >= NULL_INDEX) {
			// whoops, should never get here
			std::terminate();
		}
		m_sparse[new_id.index] = static_cast<IndexType>(m_dense.size());
		m_dense.push_back(new_id);
		m_values.push_back(std::move(value));

		return new_id;
	}

	T* try_get(Key id) {
		if (!contains(id)) {
			return nullptr;
		}
		IndexType dense_index = m_sparse[id.index];
		return &m_values[dense_index];
	}

	void erase(Key id) {
		if (!contains(id)) {
			return;
		}
		IndexType dense_index = m_sparse[id.index];

		m_sparse[id.index] = NULL_INDEX;
		if (m_dense.back().index != id.index) {
			m_sparse[m_dense.back().index] = dense_index;
		}

		std::swap(m_dense[dense_index], m_dense.back());
		std::swap(m_values[dense_index], m_values.back());
		m_dense.pop_back();
		m_values.pop_back();

		id.counter++;
		m_deleted.push_back(id);
	}

	bool contains(Key id) const {
		if (id.index == NULL_INDEX) {
			return false;
		}
		if (id.index >= m_sparse.size()) {
			return false;
		}
		if (m_sparse[id.index] == NULL_INDEX) {
			return false;
		}
		return m_dense[m_sparse[id.index]] == id;
	}

	size_t extent() const {
		return m_sparse.size();
	}

	size_t size() const {
		return m_values.size();
	}

	auto begin() {
		return m_values.begin();
	}

	auto end() {
		return m_values.end();
	}

	auto enumerate() {
		// TODO: make a const version
		// TODO: make an actual iterator
		// we do this backwards so that erasing during enumeration is okay

		struct ValueWrapper {
			T& operator*() {
				return (*values_p)[index];
			}
			T* operator->() {
				return &**this;
			}

		  private:
			friend DenseSlotMap;
			ValueWrapper(std::vector<T>* values_p_, size_t index_)
				: values_p(values_p_), index(index_) {}
			std::vector<T>* values_p = nullptr;
			size_t index = NULL_INDEX;
		};
		struct Pair {
			Key id;
			// a wrapper that ensures that if you erase an id while enumerating
			// and the values vector gets reallocated, the remaining enumerated
			// values will still be accessible
			ValueWrapper value;

		  private:
			friend DenseSlotMap;
			Pair(Key id_, std::vector<T>* values_p, size_t index)
				: id(id_), value(values_p, index) {}
		};
		std::vector<Pair> ret;
		for (size_t i = m_dense.size(); i > 0; --i) {
			ret.emplace_back(Pair(m_dense[i - 1], &m_values, i - 1));
		}
		return ret;
	}

	template <typename Compare>
	void stable_sort(Compare&& compare) {
		// Create a vector of indices into m_values, then std::stable_sort it.
		// It's then a list of permuted indices that tell us where to move values
		// so that they are sorted.
		// This could be more optimized, see https://skypjack.github.io/2019-09-25-ecs-baf-part-5/
		std::vector<IndexType> permutation(m_values.size());
		for (IndexType i = 0; i < permutation.size(); ++i) {
			permutation[i] = i;
		}
		std::stable_sort(permutation.begin(), permutation.end(), [&](auto a, auto b) {
			return compare(const_cast<const T&>(m_values[a]), const_cast<const T&>(m_values[b]));
		});

		for (IndexType pos = 0; pos < permutation.size(); ++pos) {
			auto curr = pos;
			auto next = permutation[curr];
			while (curr != next) {
				auto a = permutation[curr], b = permutation[next];
				std::swap(m_sparse[m_dense[a].index], m_sparse[m_dense[b].index]);
				std::swap(m_dense[a], m_dense[b]);
				std::swap(m_values[a], m_values[b]);
				permutation[curr] = curr;
				curr = next;
				next = permutation[curr];
			}
		}
	}

  private:
	std::vector<IndexType> m_sparse;
	std::vector<Key> m_dense;
	std::vector<T> m_values;
	// Store a list of deleted IDs that we can recycle. This could be more
	// optimized, see https://skypjack.github.io/2019-05-06-ecs-baf-part-3/
	std::vector<Key> m_deleted;
};


inline auto format_as(SlotMapKey val) {
	return fmt::format("{}:{}", val.index, val.counter);
}