#pragma once

#include <utility>

template <typename T>
class HeapValue {
  public:
	template <typename... Args>
	explicit HeapValue(Args&&... args) {
		ptr = new T(std::forward<Args>(args)...);
	}
	HeapValue(const T& other) {
		ptr = new T(other);
	}
	HeapValue(T&& other) {
		ptr = new T(std::move(other));
	}
	HeapValue(const HeapValue& other) {
		ptr = new T(*other.ptr);
	}
	HeapValue(HeapValue&& other) {
		ptr = new T(std::move(*other.ptr));
	}
	HeapValue& operator=(const HeapValue& other) {
		(*ptr) = (*other.ptr);
		return *this;
	}
	HeapValue& operator=(const T& other) {
		(*ptr) = other;
		return *this;
	}
	HeapValue& operator=(HeapValue&& other) {
		(*ptr) = std::move(*other.ptr);
		return *this;
	}
	HeapValue& operator=(T&& other) {
		(*ptr) = std::move(other);
		return *this;
	}
	~HeapValue() {
		delete ptr;
	}
	T* get() {
		return ptr;
	}
	T& operator*() {
		return *ptr;
	}
	const T& operator*() const {
		return *ptr;
	}
	T* operator->() {
		return ptr;
	}
	const T* operator->() const {
		return ptr;
	}

  private:
	T* ptr = nullptr;
};