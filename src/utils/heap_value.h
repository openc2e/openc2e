#pragma once

template <typename T>
class heap_value {
  public:
	template <typename... Args>
	explicit heap_value(Args&&... args) {
		ptr = new T(std::forward<Args>(args)...);
	}
	heap_value(const T& other) {
		ptr = new T(other);
	}
	heap_value(T&& other) {
		ptr = new T(std::move(other));
	}
	heap_value(const heap_value& other) {
		ptr = new T(*other.ptr);
	}
	heap_value(heap_value&& other) {
		ptr = new T(std::move(*other.ptr));
	}
	heap_value& operator=(const heap_value& other) {
		(*ptr) = (*other.ptr);
		return *this;
	}
	heap_value& operator=(const T& other) {
		(*ptr) = other;
		return *this;
	}
	heap_value& operator=(heap_value&& other) {
		(*ptr) = std::move(*other.ptr);
		return *this;
	}
	heap_value& operator=(T&& other) {
		(*ptr) = std::move(other);
		return *this;
	}
	~heap_value() {
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