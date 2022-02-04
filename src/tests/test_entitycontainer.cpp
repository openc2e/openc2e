#include <gtest/gtest.h>
#include <iterator>
#include <vector>


template <typename T>
class EntityContainer {
  private:
	using IndexType = uint16_t;
	using VersionType = uint16_t;

	static const IndexType NULL_INDEX = static_cast<IndexType>(~0);

	std::vector<IndexType> m_sparse;
	std::vector<IndexType> m_dense;
	std::vector<T> m_values;


  public:
	using ValueType = T;
	struct Id {
	  private:
		friend EntityContainer;
		Id(IndexType index_, VersionType version_)
			: index(index_), version(version_) {}
		IndexType index;
		VersionType version;

	  public:
		constexpr Id() {
			index = ~0;
			version = ~0;
		}
		bool operator==(const Id& other) const {
			return index == other.index && version == other.version;
		}
		bool operator!=(const Id& other) const {
			return !(*this == other);
		}
		uint32_t to_integral() const {
			return (version << 16) | index;
		}
	};
	static_assert(sizeof(Id) == sizeof(std::declval<Id>().to_integral()), "");


	Id add(T value) {
		Id new_id;
		for (size_t i = 0; i < m_sparse.size(); ++i) {
			if (m_sparse[i] == NULL_INDEX) {
				new_id.index = i;
				new_id.version = 0;
				break;
			}
		}
		if (new_id.index == NULL_INDEX) {
			new_id = Id(m_sparse.size(), 0);
			if (new_id.index == NULL_INDEX) {
				// whoops, ran out of ids
				std::terminate();
			}
			m_sparse.resize(m_sparse.size() + 1);
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
};

template <typename T>
constexpr typename EntityContainer<T>::IndexType EntityContainer<T>::NULL_INDEX;

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