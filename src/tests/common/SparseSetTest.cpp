#include "common/SparseSet.h"

#include <gtest/gtest.h>
#include <iterator>

template <typename R>
auto count(R&& range) {
	return std::distance(range.begin(), range.end());
}

TEST(SparseSet, SparseSet) {
	SparseSet<uint8_t> s;
	using ValueType = decltype(s)::ValueType;

	// starts empty
	EXPECT_EQ(s.size(), 0);
	EXPECT_EQ(count(s), 0);
	for (ValueType i = 0; i < s.NULL_VALUE; ++i) {
		EXPECT_FALSE(s.contains(i));
	}

	// add some values
	s.add(5);
	s.add(47);
	s.add(132);
	EXPECT_EQ(s.size(), 3);
	EXPECT_EQ(count(s), 3);
	for (ValueType i = 0; i < s.NULL_VALUE; ++i) {
		if (i == 5 || i == 47 || i == 132) {
			EXPECT_TRUE(s.contains(i));
		} else {
			EXPECT_FALSE(s.contains(i));
		}
	}

	// erase one value
	s.erase(47);
	EXPECT_EQ(s.size(), 2);
	EXPECT_EQ(count(s), 2);
	for (ValueType i = 0; i < s.NULL_VALUE; ++i) {
		if (i == 5 || i == 132) {
			EXPECT_TRUE(s.contains(i));
		} else {
			EXPECT_FALSE(s.contains(i));
		}
	}

	// clear
	s.clear();
	EXPECT_EQ(s.size(), 0);
	EXPECT_EQ(count(s), 0);
	for (ValueType i = 0; i < s.NULL_VALUE; ++i) {
		EXPECT_FALSE(s.contains(i));
	}
}