#pragma once

#include <iostream>

class spanstream : public std::istream
{
public:
	spanstream(unsigned char* buffer, size_t buffer_size)
		: std::istream(&buf), buf(buffer, buffer_size) {}

	spanstream(char* buffer, size_t buffer_size)
		: spanstream(reinterpret_cast<unsigned char*>(buffer), buffer_size) {}

private:
	class spanstreambuf : public std::streambuf
	{
	public:
		spanstreambuf(unsigned char* buffer_, size_t buffer_size_)
			: buffer(buffer_), buffer_size(buffer_size_) {}

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

		int_type uflow() override {
			return (position < buffer_size) ? buffer[position++] : traits_type::eof();
		}
	private:
		unsigned char *buffer;
		size_t buffer_size;
		size_t position = 0;
	};
	spanstreambuf buf;
};