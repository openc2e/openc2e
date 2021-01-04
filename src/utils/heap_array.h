#pragma once

#include <stddef.h>

template <typename T, size_t N>
class heap_array {
public:
    heap_array() {
        ptr = new T[N];
    }
    heap_array(const heap_array& other) = delete;
    heap_array& operator=(const heap_array& other) = delete;
    ~heap_array() {
        delete[] ptr;
    }
    size_t size() const {
        return N;
    }
    T& operator[](size_t pos) {
        return ptr[pos];
    }
    const T& operator[](size_t pos) const {
        return ptr[pos];
    }
    T* begin() {
        return ptr;
    }
    T* end() {
        return ptr + N;
    }
private:
    T* ptr = nullptr;
};