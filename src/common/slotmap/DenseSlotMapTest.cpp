#include "DenseSlotMap.h"

#include <gtest/gtest.h>
#include <iterator>

template <typename R>
auto count(R&& range) {
	return std::distance(range.begin(), range.end());
}

struct MyTestItem {
	MyTestItem() {}
	MyTestItem(int value_)
		: value(value_) {}
	bool operator==(const MyTestItem& other) const {
		return value == other.value;
	}
	bool operator<(const MyTestItem& other) const {
		return value < other.value;
	}

	int value = -1;
};

TEST(DenseSlotMap, DenseSlotMap) {
	DenseSlotMap<MyTestItem> pool;

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
	EXPECT_NE(fourth, second); // don't recycle ids
	EXPECT_EQ(pool.extent(), 3); // but do recycle sparse slots
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
	std::vector<MyTestItem> seen_values;
	for (const auto& item : pool) {
		seen_values.push_back(item);
	}

	const std::vector<MyTestItem> expected{5, 81, 132};
	std::sort(seen_values.begin(), seen_values.end());
	EXPECT_EQ(seen_values, expected);
}

TEST(DenseSlotMap, erase_at_back_of_dense_array) {
	// this used to segfault because there was a bug when erasing items at the back
	// of the dense array
	DenseSlotMap<MyTestItem> pool;
	auto first = pool.add(5);
	pool.erase(first);
	EXPECT_FALSE(pool.contains(first));
	EXPECT_EQ(pool.try_get(first), nullptr);
}

TEST(DenseSlotMap, erase_during_iteration) {
	// this used to segfault
	DenseSlotMap<MyTestItem> pool;
	pool.add(0);
	pool.add(1);
	pool.add(2);

	std::vector<MyTestItem> seen_values;

	for (auto it = pool.begin(); it != pool.end();) {
		seen_values.push_back(*it);
		if (it->value == 0) {
			it = pool.erase(it);
			continue;
		} else {
			it++;
		}
	}

	const std::vector<MyTestItem> expected{0, 1, 2};
	std::sort(seen_values.begin(), seen_values.end());
	EXPECT_EQ(seen_values, expected);
}

TEST(DenseSlotMap, get_null_id) {
	// this used to segfault
	DenseSlotMap<MyTestItem> pool;
	DenseSlotMap<MyTestItem>::Key id;
	EXPECT_EQ(pool.contains(id), false);
}
