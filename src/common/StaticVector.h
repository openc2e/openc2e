#pragma once

#include "common/NumericCast.h"

#include <stdexcept>

template <typename T, size_t N>
class StaticVector {
  public:
	StaticVector() {}

	StaticVector(const StaticVector& other)
		: StaticVector() {
		*this = other;
	}

	StaticVector(StaticVector&& other)
		: StaticVector() {
		*this = std::move(other);
	}

	StaticVector& operator=(const StaticVector& other) {
		// TODO: call copy assignment on items that exist
		clear();
		size_ = other.size_;
		for (size_t i = 0; i < size_; ++i) {
			new (&data_[i]) T(other[i]);
		}
		return *this;
	}

	StaticVector& operator=(StaticVector&& other) {
		// TODO: call move assignment on items that exist
		clear();
		size_ = other.size_;
		for (size_t i = 0; i < size_; ++i) {
			new (&data_[i]) T(std::move(other[i]));
		}
		other.clear();
		return *this;
	}

	~StaticVector() {
		clear();
	}

	void push_back(const T& t) {
		if (size_ >= N) {
			throw std::length_error("StaticVector push_back exceeds capacity");
		}
		new (&data_[size_]) T(t);
		size_ += 1;
	}

	T& operator[](int32_t i) {
		return const_cast<T&>(const_cast<const StaticVector&>(*this)[i]);
	}

	const T& operator[](int32_t i) const {
		if (size_ >= N) {
			throw std::length_error("StaticVector operator[] exceeds capacity");
		}
		if (i >= ssize() || i < 0) {
			throw std::out_of_range("StaticVector operator[] out of range");
		}
		return data_[i];
	}

	T& operator[](size_t i) {
		return (*this)[numeric_cast<int32_t>(i)];
	}

	const T& operator[](size_t i) const {
		return (*this)[numeric_cast<int32_t>(i)];
	}

	T* begin() {
		return data_;
	}

	const T* begin() const {
		return data_;
	}

	T* end() {
		return &data_[size_];
	}

	const T* end() const {
		return &data_[size_];
	}

	T& back() {
		return data_[size_ - 1];
	}

	const T& back() const {
		return data_[size_ - 1];
	}

	void clear() {
		while (size_) {
			pop_back();
		}
	}

	void pop_back() {
		if (size_ == 0) {
			throw std::out_of_range("StaticVector pop_back() empty vector");
		}
		data_[size_ - 1].~T();
		size_ -= 1;
	}

	T* data() {
		return data_;
	}

	const T* data() const {
		return data_;
	}

	size_t size() const {
		return size_;
	}

	int32_t ssize() const {
		return numeric_cast<int32_t>(size_);
	}

	size_t capacity() const {
		return N;
	}

  private:
	union {
		T data_[N];
	};
	size_t size_ = 0;
};
