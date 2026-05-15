#pragma once

// Wrapper around smart pointers (namely, std::unique_ptr), which allows passing
// a T** into a C API to be written into.
//
// Use like:
//
//     std::unique_ptr<uint8_t, SDLFreeDeleter> data;
//     uint32_t datalen = 0;
//     SDL_LoadWAV_IO(io, out_ptr(data), &datalen);
//

template <typename T>
class OutPointer {
	T& inner;
	typename T::pointer ptr = nullptr;

  public:
	explicit OutPointer(T& inner_)
		: inner(inner_) {
		inner.reset();
	}
	OutPointer(const OutPointer&) = delete;
	OutPointer& operator=(const OutPointer&) = delete;
	OutPointer(OutPointer&& other)
		: inner(other.inner), ptr(other.ptr) {
		other.ptr = nullptr;
	}
	OutPointer& operator=(OutPointer&&) = delete;

	~OutPointer() {
		if (ptr) {
			inner.reset(ptr);
		}
	}

	operator typename T::pointer *() { return &ptr; }
};

template <typename T>
auto out_ptr(T& inner) {
	return OutPointer<T>(inner);
}