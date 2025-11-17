#pragma once

#include <fmt/core.h>
#include <limits>
#include <stdint.h>

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


inline auto format_as(SlotMapKey val) {
	return fmt::format("{}:{}", val.index, val.counter);
}