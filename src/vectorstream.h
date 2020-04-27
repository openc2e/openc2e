#pragma once

#include <iostream>

class vectorstream : public std::iostream
{
public:
	vectorstream()
		: std::iostream(&buf), buf(vector_) {}

    const std::vector<unsigned char>& vector() { return vector_; }

private:
	class vectorstreambuf : public std::streambuf
	{
	public:
		std::vector<unsigned char> &vector;

		vectorstreambuf(std::vector<unsigned char> &vector_)
			: vector(vector_) {}
            
        pos_type seekpos(pos_type pos, std::ios_base::openmode which) {
			return seekoff(pos, std::ios_base::beg, which);
        }
		
		pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
  		{
			if (which == std::ios_base::out) {
				return pos_type(off_type(-1)); // can't change output position
			}
			switch (dir) {
				case std::ios_base::cur:
					position = position + off;
					return position;
				case std::ios_base::end:
					position = vector.size() + off;
					return position;
				case std::ios_base::beg:
					position = off;
					return position;
			}
		}

        int_type overflow(int_type c) {
            vector.push_back(c);
			return c;
        }
		
		// int_type underflow() {
		// 	return traits_type::eof();
		// }

		int_type uflow() {
			return (position < vector.size()) ? vector[position++] : traits_type::eof();
		}
	private:
		size_t position = 0;
	};
    std::vector<unsigned char> vector_;
	vectorstreambuf buf;
};