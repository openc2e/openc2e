#include "WindowsRegistry.h"

#ifdef _WIN32

#include "common/Exception.h"

#include <codecvt>
#include <string>
#include <windows.h>

static std::string to_string(const std::wstring& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	return converter.to_bytes(s);
}

static std::wstring to_wstring(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	return converter.from_bytes(s);
}

std::string registry_get_string_value(RegistryRootKey root_, const std::string& key_name_, const std::string& value_name_) {
	HKEY root = nullptr;
	switch (root_) {
		case REGISTRY_HKEY_LOCAL_MACHINE:
			root = HKEY_LOCAL_MACHINE;
	}
	if (root == nullptr) {
		throw Exception("registry_get_string_value: unknown root_key");
	}

	std::wstring key_name = to_wstring(key_name_);
	std::wstring value_name = to_wstring(value_name_);

	DWORD data_size = 0;
	LSTATUS result = RegGetValueW(root, key_name.c_str(), value_name.c_str(), RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, nullptr, &data_size);
	if (result != ERROR_SUCCESS) {
		// TODO: throw Execption? return Optional?
		return "";
	}

	std::wstring data;
	data.resize(data_size);

	result = RegGetValueW(root, key_name.c_str(), value_name.c_str(), RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, (void*)data.data(), &data_size);

	if (result != ERROR_SUCCESS) {
		// TODO: throw Execption? return Optional?
		return "";
	}

	data.erase(data.find(L'\0'));
	return to_string(data);
}

#endif
