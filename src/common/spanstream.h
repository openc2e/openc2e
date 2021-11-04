#pragma once

#include <iostream>

class spanstream : public std::istream {
  public:
	spanstream(const unsigned char* buffer, size_t buffer_size)
		: std::istream(&buf), buf(buffer, buffer_size) {}

	spanstream(const char* buffer, size_t buffer_size)
		: spanstream(reinterpret_cast<const unsigned char*>(buffer), buffer_size) {}

	spanstream()
		: spanstream(static_cast<unsigned char*>(nullptr), 0) {}

	template <typename T>
	spanstream(const T& t)
		: spanstream(t.data(), t.size()) {}

  protected:
	class spanstreambuf : public std::streambuf {
	  public:
		spanstreambuf(const unsigned char* buffer_, size_t buffer_size_)
			: buffer(buffer_), buffer_size(buffer_size_) {}

		spanstreambuf(const char* buffer_, size_t buffer_size_)
			: buffer(reinterpret_cast<const unsigned char*>(buffer_)), buffer_size(buffer_size_) {}

	  protected:
		pos_type seekpos(pos_type pos, std::ios_base::openmode which) override {
			return seekoff(pos, std::ios_base::beg, which);
		}

		pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override {
			if (which == std::ios_base::out) {
				return pos_type(off_type(-1)); // can't change output position
			}
			switch (dir) {
				case std::ios_base::cur:
					position = position + off;
					return position;
				case std::ios_base::end:
					position = buffer_size + off;
					return position;
				case std::ios_base::beg:
					position = off;
					return position;
			}
		}

		std::streamsize xsgetn(char* s, std::streamsize n) override {
			if (position + n <= buffer_size) {
				std::copy(buffer + position, buffer + position + n, s);
				position += n;
				return n;
			}
			return 0;
		}

		int_type underflow() override {
			return (position < buffer_size) ? buffer[position] : traits_type::eof();
		}

		int_type uflow() override {
			int_type ret = underflow();
			if (ret != traits_type::eof()) {
				position++;
			}
			return ret;
		}

	  private:
		const unsigned char* buffer;
		size_t buffer_size;
		size_t position = 0;
	};
	spanstreambuf buf;
};