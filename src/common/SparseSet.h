#pragma once

#include <type_traits>
#include <vector>

template <typename T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
class SparseSet {
  public:
	using ValueType = T;
	static constexpr T NULL_VALUE = static_cast<T>(~0);

	void add(T value) {
		if (contains(value)) {
			return;
		}
		dense.push_back(value);
		if (value >= sparse.size()) {
			sparse.resize(value + 1, NULL_VALUE);
		}
		sparse[value] = dense.size() - 1;
	}

	bool contains(T value) const {
		if (value >= sparse.size()) {
			return false;
		}
		if (sparse[value] == NULL_VALUE) {
			return false;
		}
		return dense[sparse[value]] == value;
	}

	T dense_location(T value) {
		if (!contains(value)) {
			return NULL_VALUE; // ?
		}
		return sparse[value];
	}

	void erase(T value) {
		if (!contains(value)) {
			return;
		}
		T dense_index = sparse[value];
		sparse[value] = NULL_VALUE;
		sparse[dense.back()] = dense_index;
		std::swap(dense[dense_index], dense.back());
		dense.resize(dense.size() - 1);
	}

	void clear() {
		sparse.clear();
		dense.clear();
	}

	size_t size() const {
		return dense.size();
	}

	auto begin() {
		return dense.begin();
	}
	auto begin() const {
		return dense.begin();
	}
	auto end() {
		return dense.end();
	}
	auto end() const {
		return dense.end();
	}

  private:
	std::vector<T> sparse;
	std::vector<T> dense;
};

template <typename T, typename U>
constexpr T SparseSet<T, U>::NULL_VALUE;