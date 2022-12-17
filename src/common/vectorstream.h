#pragma once

#include <iostream>
#include <vector>

class vectorstream : public std::iostream {
  public:
	vectorstream()
		: std::iostream(&buf), buf(vector_) {}

	const std::vector<unsigned char>& vector() { return vector_; }

  private:
	class vectorstreambuf : public std::streambuf {
	  public:
		vectorstreambuf(std::vector<unsigned char>& vector_)
			: vector(vector_) {}

		pos_type seekpos(pos_type pos, std::ios_base::openmode which) override {
			return seekoff(pos, std::ios_base::beg, which);
		}

		pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override {
			if (which == std::ios_base::out) {
				if (off == 0) {
					return size_();
				} else {
					return pos_type(off_type(-1)); // can't change output position
				}
			}
			switch (dir) {
				case std::ios_base::cur:
					position = position + off;
					return position;
				case std::ios_base::end:
					position = size_() + off;
					return position;
				case std::ios_base::beg:
					position = off;
					return position;
			}
		}

		int_type overflow(int_type c) override {
			// numeric_cast is okay here because `c` is only negative in case of
			// EOF, when it's -1
			vector.push_back(numeric_cast<unsigned char>(c));
			return c;
		}

		int_type underflow() override {
			return (position < size_()) ? vector[numeric_cast<size_t>(position)] : traits_type::eof();
		}

		int_type uflow() override {
			int_type ret = underflow();
			if (ret != traits_type::eof()) {
				position++;
			}
			return ret;
		}

	  private:
		unsigned char at_(std::streamsize index) const {
			return vector[numeric_cast<size_t>(index)];
		}

		std::streamsize size_() const {
			return numeric_cast<std::streamsize>(vector.size());
		}

		std::vector<unsigned char>& vector;
		std::streamsize position = 0;
	};
	std::vector<unsigned char> vector_;
	vectorstreambuf buf;
};