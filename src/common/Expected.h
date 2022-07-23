#pragma once

#include "ConstructAt.h"
#include "Monostate.h"

#include <type_traits>

template <class Error>
class BadExpectedAccess {
  public:
	BadExpectedAccess(const Error& error)
		: error_(error) {}

	Error& error() {
		return error_;
	}

  private:
	Error error_;
};

template <>
class BadExpectedAccess<void> {};

template <class Error>
class Unexpected {
  public:
	Unexpected(){};
	Unexpected(const Error& t)
		: value_(t) {}
	Unexpected(Error&& t)
		: value_(std::move(t)) {}
	Unexpected(const Unexpected& other) {
		value_ = other.value_;
	}
	Unexpected(Unexpected&& other) {
		value_ = std::move(other.value_);
	}
	Unexpected& operator=(const Unexpected& other) {
		value_ = other.value_;
		return *this;
	}
	Unexpected& operator=(Unexpected&& other) {
		value_ = std::move(other.value_);
		return *this;
	}
	Error& value() { return value_; }

  private:
	Error value_;
};

template <class Error>
Unexpected<Error> make_unexpected(Error&& err) {
	return Unexpected<Error>(std::forward<Error>(err));
}

template <class Value, class Error>
class Expected {
  public:
	Expected()
		: has_value_(true), value_() {}
	template <typename T = Value>
	Expected(T&& val)
		: has_value_(true), value_(std::forward<T>(val)) {}

	Expected(const Unexpected<Error>& err)
		: has_value_(false), error_(err.value()) {}
	Expected(Unexpected<Error>&& err)
		: has_value_(false), error_(std::move(err.value())) {}

	Expected(const Expected& other) {
		has_value_ = other.has_value_;
		if (has_value_) {
			construct_at(&value_, other.value_);
		} else {
			construct_at(&error_, other.error_);
		}
	}
	Expected(Expected&& other) {
		has_value_ = other.has_value_;
		if (has_value_) {
			construct_at(&value_, std::move(other.value_));
		} else {
			construct_at(&error_, std::move(other.error_));
		}
		other.reset();
	}

	Expected& operator=(const Expected& other) {
		if (has_value_ == other.has_value_) {
			if (has_value_) {
				value_ = other.value_;
			} else {
				error_ = other.error_;
			}
		} else {
			destroy();
			has_value_ = other.has_value_;
			if (has_value_) {
				construct_at(&value_, other.value_);
			} else {
				construct_at(&error_, other.error_);
			}
		}
		return *this;
	}
	Expected& operator=(Expected&& other) {
		if (has_value_ == other.has_value_) {
			if (has_value_) {
				value_ = std::move(other.value_);
			} else {
				error_ = std::move(other.error_);
			}
		} else {
			destroy();
			has_value_ = other.has_value_;
			if (has_value_) {
				construct_at(&value_, std::move(other.value_));
			} else {
				construct_at(&error_, std::move(other.error_));
			}
		}
		other.reset();
		return *this;
	}

	~Expected() { destroy(); }

	bool has_value() const { return has_value_; }
	explicit operator bool() const { return has_value(); }

	template <class T = Value, class = std::enable_if_t<!std::is_void<T>::value>>
	auto& value() {
		if (has_value()) {
			return value_;
		} else {
			throw BadExpectedAccess<Error>(error_);
		}
	}

	template <class T = Value, class = std::enable_if_t<!std::is_void<T>::value>>
	auto& operator*() { return value(); }

	Error& error() {
		if (has_value()) {
			throw BadExpectedAccess<void>();
		} else {
			return error_;
		}
	}

  private:
	void destroy() {
		if (has_value_) {
			destroy_at(&value_);
		} else {
			destroy_at(&error_);
		}
	}
	void reset() {
		destroy();
		has_value_ = true;
		construct_at(&value_);
	}

	bool has_value_;
	union {
		std::conditional_t<std::is_void<Value>::value, Monostate, Value> value_;
		Error error_;
	};
};