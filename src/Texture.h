#pragma once

#include <memory>

class Texture {
public:
  Texture() = default;
  ~Texture() {
    if (data) {
      deleter(data);
    }
  }
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture(Texture&& other) {
    data = other.data;
    deleter = other.deleter;
    other.data = nullptr;
    other.deleter = nullptr;
  };
  Texture& operator=(Texture&& other) {
    data = other.data;
    deleter = other.deleter;
    other.data = nullptr;
    other.deleter = nullptr;
    return *this;
  }
  
  void *data = nullptr;
  void (*deleter)(void*) = nullptr;
};