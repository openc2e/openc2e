#pragma once

#include "common/NumericCast.h"

#include <array>
#include <exception>

template <typename T, size_t N>
class StaticVector {
  public:
	StaticVector() {}

	StaticVector(const StaticVector& other)
		: StaticVector() {
		*this = other;
	}

	StaticVector& operator=(const StaticVector& other) {
		clear();
		size_ = other.size_;
		for (size_t i = 0; i < size_; ++i) {
			new (data() + i) T(other[i]);
		}
		return *this;
	}

	StaticVector(StaticVector&& other)
		: StaticVector() {
		*this = std::move(other);
	}

	StaticVector& operator=(StaticVector&& other) {
		clear();
		size_ = other.size_;
		for (size_t i = 0; i < size_; ++i) {
			new (data() + i) T(std::move(other[i]));
		}
		other.clear();
		return *this;
	}

	~StaticVector() {
		clear();
	}

	void push_back(const T& t) {
		emplace_back(t);
	}

	void push_back(T&& t) {
		emplace_back(std::move(t));
	}

	template <typename... Args>
	void emplace_back(Args&&... args) {
		if (size_ >= N) {
			// whoops
			std::terminate();
		}
		new (data() + size_) T(std::forward<Args>(args)...);
		size_++;
	}

	T& operator[](size_t i) {
		if (size_ >= N) {
			// whoops
			std::terminate();
		}
		return *(data() + i);
	}

	const T& operator[](size_t i) const {
		if (size_ >= N) {
			// whoops
			std::terminate();
		}
		return *(data() + i);
	}

	T& operator[](int32_t i) {
		return (*this)[numeric_cast<size_t>(i)];
	}

	const T& operator[](int32_t i) const {
		return (*this)[numeric_cast<size_t>(i)];
	}

	void clear() {
		while (size_) {
			pop_back();
		}
	}

	T* begin() {
		return data();
	}

	const T* begin() const {
		return data();
	}

	T* end() {
		return data() + size_;
	}

	const T* end() const {
		return data() + size_;
	}

	T& back() {
		return *(data() + size_ - 1);
	}

	const T& back() const {
		return *(data() + size_ - 1);
	}

	void pop_back() {
		if (size_ == 0) {
			// whoops
			std::terminate();
		}
		(data() + size_ - 1)->~T();
		size_ -= 1;
	}

	T* data() {
		return reinterpret_cast<T*>(data_);
	}

	const T* data() const {
		return reinterpret_cast<const T*>(data_);
	}

	size_t size() const {
		return size_;
	}

	int32_t ssize() const {
		return numeric_cast<int32_t>(size());
	}

	size_t capacity() const {
		return N;
	}

  private:
	std::aligned_storage_t<sizeof(T), alignof(T)> data_[N];
	size_t size_ = 0;
};
