#pragma once

#include <memory>

class Texture {
public:
  Texture() {};

  template <typename T>
  Texture(T *ptr_, unsigned int width_, unsigned int height_, void(*deleter_)(T*))
    : ptr(ptr_, deleter_), width(width_), height(height_) {}

  explicit operator bool() const {
    return ptr.get() != nullptr;
  }

  template <typename T>
  T* as() {
    return reinterpret_cast<T*>(ptr.get());
  }
  
  template <typename T>
  const T* as() const {
    return reinterpret_cast<const T*>(ptr.get());
  }

  std::shared_ptr<void> ptr;
  unsigned int width = 0;
  unsigned int height = 0;
};