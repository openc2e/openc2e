#include "common/EntityPool.h"

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
	int value = -1;
};

TEST(EntityPool, EntityPool) {
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

	// iterate ids
	for (auto i : pool.enumerate()) {
		if (i.id == first) {
			EXPECT_EQ(i.value->value, 5);
		} else if (i.id == third) {
			EXPECT_EQ(i.value->value, 132);
		} else if (i.id == fourth) {
			EXPECT_EQ(i.value->value, 81);
		} else {
			printf("unknown id %i\n", i.id.to_integral());
			EXPECT_TRUE(false);
		}
	}
}

TEST(EntityPool, erase_at_back_of_dense_array) {
	// this used to segfault because there was a bug when erasing items at the back
	// of the dense array
	EntityPool<MyTestItem> pool;
	auto first = pool.add(5);
	pool.erase(first);
	EXPECT_FALSE(pool.contains(first));
	EXPECT_EQ(pool.try_get(first), nullptr);
}

TEST(EntityPool, erase_during_enumeration) {
	// this used to segfault
	EntityPool<MyTestItem> pool;
	auto zero = pool.add(0);
	auto one = pool.add(1);
	auto two = pool.add(2);

	for (auto i : pool.enumerate()) {
		if (i.id == zero) {
			pool.erase(i.id);
			continue;
		}
		if (i.id == one) {
			EXPECT_EQ(i.value->value, 1);
		} else if (i.id == two) {
			EXPECT_EQ(i.value->value, 2);
		} else {
			EXPECT_TRUE(false);
		}
	}
}

TEST(EntityPool, get_null_id) {
	// this used to segfault
	EntityPool<MyTestItem> pool;
	EntityPool<MyTestItem>::Id id;
	EXPECT_EQ(pool.contains(id), false);
}

TEST(EntityPool, sort) {
	EntityPool<MyTestItem> pool;
	auto zero = pool.add(3);
	auto one = pool.add(2);
	auto two = pool.add(1);
	auto three = pool.add(0);

	pool.stable_sort([](const MyTestItem& a, const MyTestItem& b) {
		return a.value < b.value;
	});

	auto it = pool.begin();
	EXPECT_EQ(it++->value, 0);
	EXPECT_EQ(it++->value, 1);
	EXPECT_EQ(it++->value, 2);
	EXPECT_EQ(it++->value, 3);

	EXPECT_EQ(pool.try_get(zero)->value, 3);
	EXPECT_EQ(pool.try_get(one)->value, 2);
	EXPECT_EQ(pool.try_get(two)->value, 1);
	EXPECT_EQ(pool.try_get(three)->value, 0);
}

TEST(EntityPool, sort_scenario1) {
	// this used to segfault
	EntityPool<MyTestItem> pool;
	auto zero = pool.add(1);
	for (int i = 0; i < 3; ++i) {
		pool.add(0);
	}

	EXPECT_NE(pool.try_get(zero), nullptr);

	pool.stable_sort([](const MyTestItem& a, const MyTestItem& b) {
		return a.value < b.value;
	});

	EXPECT_NE(pool.try_get(zero), nullptr);

	auto it = pool.begin();
	for (size_t i = 0; i < pool.size() - 1; ++i) {
		EXPECT_EQ(it++->value, 0);
	}
	EXPECT_EQ(it->value, 1);
}