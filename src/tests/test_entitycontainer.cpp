#include <gtest/gtest.h>
#include <iterator>
#include <vector>


template <typename T, typename I = uint32_t>
class EntityContainer {
  public:
	using IdType = I;
	using ValueType = T;
	static constexpr I NULL_ID = static_cast<I>(~0);

	I add(T value) {
		I new_id = NULL_ID;
		for (size_t i = 0; i < m_sparse.size(); ++i) {
			if (m_sparse[i] == NULL_ID) {
				new_id = i;
				break;
			}
		}
		if (new_id == NULL_ID) {
			new_id = m_sparse.size();
			m_sparse.push_back(NULL_ID);
		}
		m_sparse[new_id] = m_dense.size();
		m_dense.push_back(new_id);
		m_values.push_back(value);

		return new_id;
	}

	T* try_get(I id) {
		if (!contains(id)) {
			return nullptr;
		}
		I dense_index = m_sparse[id];
		return &m_values[dense_index];
	}

	void erase(I id) {
		if (!contains(id)) {
			return;
		}
		I dense_index = m_sparse[id];

		m_sparse[id] = NULL_ID;
		m_sparse[m_dense.back()] = dense_index;

		std::swap(m_dense[dense_index], m_dense.back());
		std::swap(m_values[dense_index], m_values.back());
		m_dense.resize(m_dense.size() - 1);
		m_values.resize(m_values.size() - 1);
	}

	bool contains(I id) {
		if (id >= m_sparse.size()) {
			return false;
		}
		if (m_sparse[id] == NULL_ID) {
			return false;
		}
		return m_dense[m_sparse[id]] == id;
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
	std::vector<I> m_sparse;
	std::vector<I> m_dense;
	std::vector<T> m_values;
};

template <typename T, typename I>
constexpr I EntityContainer<T, I>::NULL_ID;

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

TEST(common, entitycontainer) {
	EntityContainer<MyTestItem> items;

	// starts empty
	EXPECT_EQ(items.size(), 0);
	EXPECT_EQ(items.extent(), 0);
	EXPECT_EQ(count(items), 0);

	// add some values
	auto first = items.add(5);
	auto second = items.add(47);
	auto third = items.add(132);
	EXPECT_EQ(items.size(), 3);
	EXPECT_EQ(items.extent(), 3);
	EXPECT_EQ(count(items), 3);

	EXPECT_TRUE(items.contains(first));
	EXPECT_EQ(items.try_get(first)->value, 5);

	EXPECT_TRUE(items.contains(second));
	EXPECT_EQ(items.try_get(second)->value, 47);

	EXPECT_TRUE(items.contains(third));
	EXPECT_EQ(items.try_get(third)->value, 132);

	// erase a value
	items.erase(second);
	EXPECT_EQ(items.size(), 2);
	EXPECT_EQ(count(items), 2);
	EXPECT_EQ(items.extent(), 3);

	EXPECT_TRUE(items.contains(first));
	EXPECT_EQ(items.try_get(first)->value, 5);

	EXPECT_FALSE(items.contains(second));
	EXPECT_EQ(items.try_get(second), nullptr);

	EXPECT_TRUE(items.contains(third));
	EXPECT_EQ(items.try_get(third)->value, 132);

	// add a value after erasing a value
	auto fourth = items.add(23);
	EXPECT_EQ(items.size(), 3);
	EXPECT_EQ(fourth, second); // recycle id
	EXPECT_EQ(items.extent(), 3); // recycle id
	EXPECT_EQ(count(items), 3);

	EXPECT_TRUE(items.contains(first));
	EXPECT_EQ(items.try_get(first)->value, 5);

	EXPECT_TRUE(items.contains(third));
	EXPECT_EQ(items.try_get(third)->value, 132);

	EXPECT_TRUE(items.contains(fourth));
	EXPECT_EQ(items.try_get(fourth)->value, 23);

	// change a value
	items.try_get(fourth)->value = 81;
	EXPECT_EQ(items.try_get(fourth)->value, 81);

	// iterate values
	EXPECT_EQ(count(items), 3);
	size_t i = 0;
	for (const auto& item : items) {
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