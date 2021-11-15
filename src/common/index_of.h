#pragma once

template <typename T, typename U>
size_t index_of(T&& t, U&& u) {
	size_t i = 0;
	for (const auto& el : t) {
		if (el == u) {
			return i;
		}
		i++;
	}
	return (size_t)-1;
}