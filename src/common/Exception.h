#pragma once

#include <fmt/core.h>
#include <stdexcept>
#include <string>

class Exception : public std::runtime_error {
  public:
	using runtime_error::runtime_error;
	virtual std::string prettyPrint() const { return std::string(what()); }
};

template <typename T = Exception>
void throw_exception(const char* message) {
	throw T(message);
}

template <typename T = Exception, typename... Args>
void throw_exception(const char* fmt, Args... args) {
	throw T(fmt::format(fmt, args...));
}