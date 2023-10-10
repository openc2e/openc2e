#pragma once

#include <fmt/core.h>
#include <stdexcept>
#include <string.h>
#include <string>

class StringView {
  public:
	StringView() {}
	StringView(const char* data, size_t count)
		: data_(data), size_(count) {}
	StringView(const char* data)
		: StringView(data, strlen(data)) {}
	StringView(const std::string& s)
		: data_(s.data()), size_(s.size()) {}

	bool empty() const {
		return size_ == 0;
	}

	char operator[](size_t pos) const {
		if (pos >= size_) {
			throw std::out_of_range(fmt::format("index pos {} >= size() {}", pos, size_));
		}
		return data_[pos];
	}

	const char* data() const {
		return data_;
	}

	size_t size() const {
		return size_;
	}

	StringView substr(size_t pos, size_t count = std::string::npos) {
		if (pos > size_) {
			throw std::out_of_range(fmt::format("substr pos {} > size() {}", pos, size_));
		}
		return StringView(data_ + pos, (size_ - pos < count) ? (size_ - pos) : count);
	}

	auto begin() const {
		return data_;
	}

	auto end() const {
		return data_ + size_;
	}

  private:
	const char* data_ = nullptr;
	size_t size_ = 0;
};

template <>
struct fmt::formatter<StringView> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(StringView sv, format_context& ctx) {
		auto out = ctx.out();
		for (auto c : sv) {
			*out++ = c;
		}
		return out;
	}
};