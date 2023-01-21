#pragma once

// Once openc2e goes to C++17, replace this with std::optional

#include <exception>
#include <stdint.h>

class bad_optional_access : public std::exception {
};

template <class T>
class optional {
  public:
	optional() {}
	template <class U>
	optional(U value) {
		has_value_ = true;
		new (&storage_) T(value);
	}
	optional(const optional& other) {
		has_value_ = other.has_value_;
		if (has_value_) {
			new (&storage_) T(*other);
		}
	}
	optional(optional&& other) {
		has_value_ = other.has_value_;
		if (has_value_) {
			new (&storage_) T(std::move(*other));
		}
	}
	optional& operator=(const optional& other) {
		has_value_ = other.has_value_;
		if (has_value_) {
			new (&storage_) T(*other);
		}
		return *this;
	}
	optional& operator=(optional&& other) {
		has_value_ = other.has_value_;
		if (has_value_) {
			new (&storage_) T(std::move(*other));
		}
		return *this;
	}
	~optional() {
		if (has_value_) {
			storage_.~T();
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
		return storage_;
	}
	const T& operator*() const {
		return storage_;
	}
	T* operator->() {
		return &storage_;
	}
	const T* operator->() const {
		return &storage_;
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
	union {
		T storage_;
	};
};

template <class T>
optional<T> make_optional(T value) {
	return optional<T>(value);
}
