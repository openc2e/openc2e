#pragma once

// Once openc2e goes to C++20, replace this with std::span

#include <cstddef>

template <class T>
class span {
public:
    span() {}
    span(T* first, size_t count) : data_(first), size_(count) {}
    span(T* first, T* last) : data_(first), size_(last - first) {}
    template <class R>
    span(R&& r) : data_(r.data()), size_(r.size()) {}
    
    T& front() const { return data_[0]; }
    T& back() const { return data_[size_ - 1]; }
    T& operator[](size_t i) const { return data_[i]; }
    
    T* begin() const { return data_; }
    T* end() const { return data_[size_]; }
    
    T* data() const { return data_; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    
private:
    T* data_ = nullptr;
    size_t size_ = 0;
};
