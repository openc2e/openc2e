#include "common/WindowsRegistry.h"

#include <fmt/core.h>

int main() {
	auto path = registry_get_string_value(REGISTRY_HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Gameware Development\\Creatures 1\\1.0", "Main Directory");
	fmt::print("Data: '{}'\n", path);
}
