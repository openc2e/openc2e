#pragma once

#include <iostream>

class spanstream : public std::istream
{
public:
	spanstream(char* buffer, size_t buffer_size)
		: std::istream(&buf), buf(buffer, buffer_size) {}

	spanstream(unsigned char* buffer, size_t buffer_size)
		: spanstream(reinterpret_cast<char*>(buffer), buffer_size) {}

private:
	class spanstreambuf : public std::streambuf
	{
	public:
		char *buffer;
		size_t buffer_size;

		spanstreambuf(char* buffer_, size_t buffer_size_)
			: buffer(buffer_), buffer_size(buffer_size_) {}

		int_type uflow() {
			return (position < buffer_size) ? buffer[position++] : EOF;
		}
	private:
		size_t position = 0;
	};
	spanstreambuf buf;
};