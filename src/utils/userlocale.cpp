// SDL has an active ticket to add SDL_GetPreferredLocales - once that's in,
// we should use it instead: https://bugzilla.libsdl.org/show_bug.cgi?id=2131

#include "userlocale.h"

#include <algorithm>

#if defined(_WIN32)
#include <codecvt>
#include <windows.h>
#elif defined(__APPLE__)
#import <CoreFoundation/CoreFoundation.h>
#else
#include <cstring>
#endif

std::vector<std::string> get_preferred_languages() {
	std::vector<std::string> languages;

#if defined(_WIN32)

	ULONG num_languages = 0;
	ULONG size_buffer = 0;
	if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, NULL, &size_buffer)) {
		std::vector<wchar_t> buffer(size_buffer);
		if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, buffer.data(), &size_buffer)) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
			size_t p = 0;
			for (size_t i = 0; p < size_buffer && i < num_languages; ++i) {
				std::wstring wlang(buffer.data() + p);
				if (wlang.empty()) {
					break;
				}
				std::string lang = converter.to_bytes(wlang);
				languages.push_back(lang);
				p += wlang.size() + 1;
			}
		}
	}

#elif defined(__APPLE__)

	CFArrayRef cflanguages = CFLocaleCopyPreferredLanguages();
	for (CFIndex i = 0; i < CFArrayGetCount(cflanguages); ++i) {
		CFStringRef cflang = static_cast<CFStringRef>(CFArrayGetValueAtIndex(cflanguages, i));
		CFIndex bufsize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cflang), kCFStringEncodingUTF8) + 1;
		std::vector<char> buf(bufsize, '\0');
		CFStringGetCString(cflang, buf.data(), bufsize, kCFStringEncodingUTF8);
		std::string lang(buf.data());
		languages.push_back(lang);
	}
	CFRelease(cflanguages);

#else

	const char* language_env_p = std::getenv("LANGUAGE");
	if (language_env_p && strlen(language_env_p) != 0) {
		std::string language_env(language_env_p);
		while (!language_env.empty()) {
			std::string lang = language_env.substr(0, language_env.find(":"));
			if (language_env.find(":") != std::string::npos) {
				language_env = language_env.substr(language_env.find(":") + 1);
			} else {
				language_env = "";
			}
			languages.push_back(lang);
		}
	}

	const char* lc_all_env_p = std::getenv("LC_ALL");
	if (lc_all_env_p && strlen(lc_all_env_p) != 0) {
		languages.push_back({lc_all_env_p});
	}

	const char* lang_env_p = std::getenv("LANG");
	if (lang_env_p && strlen(lang_env_p) != 0) {
		languages.push_back({lang_env_p});
	}

#endif

	std::vector<std::string> languages_unique;

	for (auto lang : languages) {
		lang = lang.substr(0, lang.find("-"));
		lang = lang.substr(0, lang.find("_"));
		lang = lang.substr(0, lang.find("."));
		if (lang == "C") {
			continue;
		}
		if (std::find(languages_unique.begin(), languages_unique.end(), lang) == languages_unique.end()) {
			languages_unique.push_back(lang);
		}
	}

	return languages_unique;
}