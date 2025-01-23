#pragma once

#include "common/io/Writer.h"

#include <fmt/base.h>

namespace fmt {

inline void vprint(Writer& w, string_view fmt, format_args args) {
	auto buf = memory_buffer();
	detail::vformat_to(buf, fmt, args);
	w.write(buf.data(), buf.size());
}

template <typename... T>
void print(Writer& w, format_string<T...> fmt, T&&... args) {
	vargs<T...> va = {{args...}};
	vprint(w, fmt.str, va);
}

} // namespace fmt