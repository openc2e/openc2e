#pragma once

#include <stdint.h>
#include <type_traits>

class fixed24_8_t {
  public:
	fixed24_8_t() {}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
	fixed24_8_t(T t) {
		m_value = t * (1 << 8);
	}

	static fixed24_8_t from_raw(int32_t value) {
		fixed24_8_t result;
		result.m_value = value;
		return result;
	}

	bool operator==(fixed24_8_t other) const {
		return m_value == other.m_value;
	}

	fixed24_8_t operator+(fixed24_8_t other) {
		fixed24_8_t result(*this);
		result += other;
		return result;
	}

	fixed24_8_t& operator+=(fixed24_8_t other) {
		m_value += other.m_value;
		return *this;
	}

	explicit operator float() const {
		return static_cast<float>(m_value) / (1 << 8);
	}

	int32_t raw() const {
		return m_value;
	}

	int32_t trunc() const {
		return m_value / (1 << 8);
	}

  private:
	int32_t m_value = 0;
};