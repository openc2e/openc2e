#pragma once

#include <atomic>
#include <assert.h>
#include <stddef.h>

template <typename T>
class shared_array {
public:
  shared_array() {}
  explicit shared_array(size_t n) : refcount_(new std::atomic<size_t>(1)), ptr_(new T[n]), size_(n)  {}
  ~shared_array() {
    release();
  } 
 
  shared_array(const shared_array& other) {
    refcount_ = nullptr;
    *this = other;
  }
  
  shared_array(shared_array &&other) {
    refcount_ = nullptr;
    *this = other;
  }
  
  shared_array& operator=(const shared_array& other) {
    release();
    other.addref();
    refcount_ = other.refcount_;
    ptr_ = other.ptr_;
    size_ = other.size_;
    return *this;
  }
  shared_array& operator=(shared_array&& other) {
    release();
    refcount_ = other.refcount_;
    ptr_ = other.ptr_;
    size_ = other.size_;
    other.refcount_ = nullptr;
    other.ptr_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  T& operator[](ptrdiff_t i) {
    assert(ptr_);
    return ptr_[i];
  }
  const T& operator[](ptrdiff_t i) const {
    assert(ptr_);
    return ptr_[i];
  }

  T* data() {
    return ptr_;
  }
  const T* data() const {
    return ptr_;
  }

  size_t size() const {
    return size_;
  }
  
  T* begin() {
    return ptr_;
  }
  const T* begin() const {
    return ptr_;
  }
  
  T* end() {
    return ptr_ + size_;
  }
  const T* end() const {
    return ptr_ + size_;
  }
  
  explicit operator bool() const {
    return ptr_ != nullptr;
  }
  
private:
    void addref() const {
      if (refcount_ != nullptr) {
         refcount_->fetch_add(1, std::memory_order_relaxed);
      }
    }
    void release() {
      if (refcount_ != nullptr && refcount_->fetch_sub(1, std::memory_order_acq_rel) == 0) {
        delete refcount_;
        delete[] ptr_;
        size_ = 0;
      }
    }

    mutable std::atomic<size_t> * refcount_ = nullptr;
    T * ptr_ = nullptr;
    size_t size_ = 0;
};
