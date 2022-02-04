#include <gtest/gtest.h>
#include <iterator>
#include <vector>


template <typename T>
class EntityPool {
  private:
	using IndexType = uint16_t;
	using GenerationType = uint16_t;

	static const IndexType NULL_INDEX = static_cast<IndexType>(~0);

  public:
	using ValueType = T;
	struct Id {
	  private:
		friend EntityPool;
		Id(IndexType index_, GenerationType generation_)
			: index(index_), generation(generation_) {}
		IndexType index;
		GenerationType generation;

	  public:
		constexpr Id() {
			index = ~0;
			generation = ~0;
		}
		bool operator==(const Id& other) const {
			return index == other.index && generation == other.generation;
		}
		bool operator!=(const Id& other) const {
			return !(*this == other);
		}
		uint32_t to_integral() const {
			return (generation << 16) | index;
		}
	};
	static_assert(sizeof(Id) == sizeof(std::declval<Id>().to_integral()), "");


	Id add(T value) {
		Id new_id;
		if (m_deleted.size()) {
			new_id = m_deleted.back();
			m_deleted.pop_back();
		} else {
			new_id = Id(m_sparse.size(), 0);
			if (new_id.index == NULL_INDEX) {
				// whoops, ran out of ids
				std::terminate();
			}
		}
		if (new_id.index >= m_sparse.size()) {
			m_sparse.resize(new_id.index + 1);
		}
		m_sparse[new_id.index] = m_dense.size();
		m_dense.push_back(new_id.index);
		m_values.push_back(value);

		return new_id;
	}

	T* try_get(Id id) {
		if (!contains(id)) {
			return nullptr;
		}
		IndexType dense_index = m_sparse[id.index];
		return &m_values[dense_index];
	}

	void erase(Id id) {
		if (!contains(id)) {
			return;
		}
		IndexType dense_index = m_sparse[id.index];

		m_sparse[id.index] = NULL_INDEX;
		m_sparse[m_dense.back()] = dense_index;

		std::swap(m_dense[dense_index], m_dense.back());
		std::swap(m_values[dense_index], m_values.back());
		m_dense.resize(m_dense.size() - 1);
		m_values.resize(m_values.size() - 1);

		m_deleted.push_back(id);
	}

	bool contains(Id id) {
		if (id.index >= m_sparse.size()) {
			return false;
		}
		if (m_sparse[id.index] == NULL_INDEX) {
			return false;
		}
		return m_dense[m_sparse[id.index]] == id.index;
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

  private:
	std::vector<IndexType> m_sparse;
	std::vector<Id> m_deleted;
	std::vector<IndexType> m_dense;
	std::vector<T> m_values;
};

template <typename T>
constexpr typename EntityPool<T>::IndexType EntityPool<T>::NULL_INDEX;

template <typename R>
auto count(R&& range) {
	return std::distance(range.begin(), range.end());
}

struct MyTestItem {
	MyTestItem() {}
	MyTestItem(int value_)
		: value(value_) {}
	int value = -1;
};

TEST(common, entitypool) {
	EntityPool<MyTestItem> pool;

	// starts empty
	EXPECT_EQ(pool.size(), 0);
	EXPECT_EQ(pool.extent(), 0);
	EXPECT_EQ(count(pool), 0);

	// add some values
	auto first = pool.add(5);
	auto second = pool.add(47);
	auto third = pool.add(132);
	EXPECT_EQ(pool.size(), 3);
	EXPECT_EQ(pool.extent(), 3);
	EXPECT_EQ(count(pool), 3);

	EXPECT_TRUE(pool.contains(first));
	EXPECT_EQ(pool.try_get(first)->value, 5);

	EXPECT_TRUE(pool.contains(second));
	EXPECT_EQ(pool.try_get(second)->value, 47);

	EXPECT_TRUE(pool.contains(third));
	EXPECT_EQ(pool.try_get(third)->value, 132);

	// erase a value
	pool.erase(second);
	EXPECT_EQ(pool.size(), 2);
	EXPECT_EQ(count(pool), 2);
	EXPECT_EQ(pool.extent(), 3);

	EXPECT_TRUE(pool.contains(first));
	EXPECT_EQ(pool.try_get(first)->value, 5);

	EXPECT_FALSE(pool.contains(second));
	EXPECT_EQ(pool.try_get(second), nullptr);

	EXPECT_TRUE(pool.contains(third));
	EXPECT_EQ(pool.try_get(third)->value, 132);

	// add a value after erasing a value
	auto fourth = pool.add(23);
	EXPECT_EQ(pool.size(), 3);
	EXPECT_EQ(fourth, second); // recycle ids
	EXPECT_EQ(pool.extent(), 3); // recycle ids
	EXPECT_EQ(count(pool), 3);

	EXPECT_TRUE(pool.contains(first));
	EXPECT_EQ(pool.try_get(first)->value, 5);

	EXPECT_TRUE(pool.contains(third));
	EXPECT_EQ(pool.try_get(third)->value, 132);

	EXPECT_TRUE(pool.contains(fourth));
	EXPECT_EQ(pool.try_get(fourth)->value, 23);

	// change a value
	pool.try_get(fourth)->value = 81;
	EXPECT_EQ(pool.try_get(fourth)->value, 81);

	// iterate values
	EXPECT_EQ(count(pool), 3);
	size_t i = 0;
	for (const auto& item : pool) {
		int expected = -1;
		if (i == 0) {
			expected = 5;
		} else if (i == 1) {
			expected = 132;
		} else if (i == 2) {
			expected = 81;
		}
		EXPECT_EQ(item.value, expected);
		i++;
	}
}