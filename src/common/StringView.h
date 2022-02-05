#pragma once

#include <string>

class StringView {
  public:
	StringView() {}
	StringView(const char* data)
		: data_(data) {}
	StringView(const std::string& s)
		: data_(s.c_str()) {}

	char operator[](size_t pos) const {
		return data_[pos];
	}

	explicit operator bool() const {
		return data_ != nullptr;
	}

	const char* data() const {
		return data_;
	}

  private:
	const char* data_ = nullptr;
}