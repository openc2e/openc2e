#pragma once

// Once openc2e goes to C++17, replace this with std::optional

#include <exception>
#include <type_traits>

class bad_optional_access : public std::exception {
};

template <class T>
class optional {
  public:
	optional() = default;
	template <class U>
	optional(U value) {
		has_value_ = true;
		new (&storage_) T(value);
	}
	optional(const optional& other) {
		*this = other;
	}
	optional(optional&& other) {
		*this = std::move(other);
	}
	optional& operator=(const optional& other) {
		this->~optional();
		has_value_ = other.has_value_;
		if (has_value_) {
			new (&storage_) T(*other);
		}
		return *this;
	}
	optional& operator=(optional&& other) {
		this->~optional();
		has_value_ = other.has_value_;
		if (has_value_) {
			storage_ = other.storage_;
			other.has_value_ = false;
		}
		return *this;
	}
	~optional() {
		if (has_value_) {
			(&**this)->~T();
			has_value_ = false;
		}
	}
	explicit operator bool() const {
		return has_value_;
	}
	bool has_value() const {
		return has_value_;
	}
	T& operator*() {
		// needs std::launder in C++17
		return *reinterpret_cast<T*>(&storage_);
	}
	const T& operator*() const {
		// needs std::launder in C++17
		return *reinterpret_cast<const T*>(&storage_);
	}
	T& value() {
		if (has_value_) {
			return **this;
		} else {
			throw bad_optional_access();
		}
	}
	T value_or(const T& default_value) {
		if (has_value_) {
			return **this;
		} else {
			return default_value;
		}
	}

  private:
	bool has_value_ = false;
	typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_;
};

template <class T>
optional<T> make_optional(T value) {
	return optional<T>(value);
}