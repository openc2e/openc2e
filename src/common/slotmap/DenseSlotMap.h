#pragma once

#include "SlotMapKey.h"

#include <algorithm> // stable_sort
#include <exception>
#include <utility>
#include <vector>

// DenseSlotMap
//
// This container is meant for scenarios where you want to:
//   (1) store all instances of a type in the same place
//   (2) reference objects without giving out raw pointers
//   (3) have stable integer IDs for each object
//

template <typename T>
class DenseSlotMap {
  public:
	using ValueType = T;
	using Key = SlotMapKey;

  private:
	using IndexType = Key::IndexType;
	using CounterType = Key::CounterType;

	static constexpr IndexType NULL_INDEX = Key().index;

	struct Iterator {
		using difference_type = ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::random_access_iterator_tag;

		reference operator*() const {
			return parent->m_values[dense_index];
		}

		pointer operator->() const {
			return &(**this);
		}

		bool operator==(const Iterator& other) const {
			return parent == other.parent && dense_index == other.dense_index;
		}

		bool operator!=(const Iterator& other) const {
			return !(*this == other);
		}

		Iterator& operator++() {
			dense_index++;
			return *this;
		}

		Iterator operator++(int) {
			Iterator old = *this;
			++(*this);
			return old;
		}

		difference_type operator-(const Iterator& other) const {
			return static_cast<difference_type>(dense_index - other.dense_index);
		}

	  private:
		friend class DenseSlotMap;
		Iterator(DenseSlotMap* parent_, size_t dense_index_)
			: parent(parent_), dense_index(dense_index_) {}
		DenseSlotMap* parent;
		size_t dense_index;
	};

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

	Iterator erase(const Iterator& it) {
		// invalidates end() iterator, like std::vector but unlike std::map
		Key key = m_dense[it.dense_index];
		erase(key);
		return it;
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
		return Iterator{this, 0};
	}

	auto end() {
		return Iterator{this, m_values.size()};
	}

  private:
	std::vector<IndexType> m_sparse;
	std::vector<Key> m_dense;
	std::vector<T> m_values;
	// Store a list of deleted IDs that we can recycle. This could be more
	// optimized, see https://skypjack.github.io/2019-05-06-ecs-baf-part-3/
	std::vector<Key> m_deleted;
};
