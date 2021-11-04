#pragma once

#include <string>

#ifdef __GNUG__
#include "scope_guard.h"

#include <cxxabi.h>
#endif

inline std::string demangle(const char* name) {
#ifdef __GNUG__
	int status;
	char* demangled = abi::__cxa_demangle(name, NULL, NULL, &status);
	auto demangled_guard = make_scope_guard([&] { free(demangled); demangled = NULL; });
	if (status == 0) {
		return std::string(demangled);
	}
#endif
	return name;
}
