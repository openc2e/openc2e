#pragma once

#include <combaseapi.h>

template <typename T>
class ComPtr {
  public:
	ComPtr() {}
	ComPtr(const ComPtr&) = delete;
	ComPtr& operator=(const ComPtr&) = delete;
	ComPtr(ComPtr&& other) {
		data = other.data;
		other.data = nullptr;
	}
	ComPtr& operator=(ComPtr&& other) {
		if (data != other.data) {
			release();
			data = other.data;
			other.data = nullptr;
		}
		return *this;
	}
	~ComPtr() {
		reset();
	}
	T* operator->() {
		return data;
	}
	T** receive() {
		assert(data == nullptr);
		return &data;
	}
	void** receive_vpp() {
		return reinterpret_cast<void**>(receive());
	}

  private:
	void reset() {
		T* temp = data;
		if (temp) {
			data = nullptr;
			temp->Release();
		}
	}
	T* data = nullptr;
};

template <typename T>
class ComHeapPtr {
  public:
	ComHeapPtr() {}
	ComHeapPtr(const ComHeapPtr&) = delete;
	ComHeapPtr& operator=(const ComHeapPtr&) = delete;
	ComHeapPtr(ComHeapPtr&& other) {
		data = other.data;
		other.data = nullptr;
	}
	ComHeapPtr& operator=(ComHeapPtr&& other) {
		if (data != other.data) {
			reset();
			data = other.data;
			other.data = nullptr;
		}
		return *this;
	}
	~ComHeapPtr() {
		reset();
	}
	T* get() {
		return data;
	}
	T* operator->() {
		return data;
	}
	T** receive() {
		assert(data == nullptr);
		return &data;
	}

  private:
	void reset() {
		T* temp = data;
		if (temp) {
			data = nullptr;
			CoTaskMemFree(temp);
		}
	}
	T* data = nullptr;
};