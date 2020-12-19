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
    new(&storage_) T(value);
  }
  ~optional() {
    if (has_value_) {
      // needs std::launder in C++17
      reinterpret_cast<T*>(&storage_)->~T();
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
  T& value() {
    if (has_value_) {
      // needs std::launder in C++17
      return *reinterpret_cast<T*>(&storage_);
    } else {
      throw bad_optional_access();
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