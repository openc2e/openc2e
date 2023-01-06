#pragma once

#ifdef _WIN32

#include <string>

enum RegistryRootKey {
	REGISTRY_HKEY_LOCAL_MACHINE
};

std::string registry_get_string_value(RegistryRootKey root, const std::string& key_name, const std::string& value_name);

#endif
