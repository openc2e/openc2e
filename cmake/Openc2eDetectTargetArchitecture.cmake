# it's notoriously difficult to get the target architecture from CMake
# sometimes CMAKE_SYSTEM_PROCESSOR, sometimes ANDROID_ABI, sometimes
# CMAKE_VS_PLATFORM_NAME, sometimes CMAKE_OSX_ARCHITECTURES

if(APPLE AND CMAKE_OSX_ARCHITECTURES)
	if("x86_64" IN_LIST CMAKE_OSX_ARCHITECTURES)
		set(TARGETARCH_X64 TRUE)
	else()
		set(TARGETARCH_X64 FALSE)
	endif()
	if("arm64" IN_LIST CMAKE_OSX_ARCHITECTURES)
		set(TARGETARCH_ARM64 TRUE)
	else()
		set(TARGETARCH_ARM64 FALSE)
	endif()
	return()
endif()

include(CheckCSourceCompiles)
check_c_source_compiles("#if defined(__x86_64__) || defined(_M_X64)\nint main() { return 0; }\n#else\n#error\n#endif" TARGETARCH_X64)
check_c_source_compiles("#if defined(__aarch64__) || defined(_M_ARM64)\nint main() { return 0; }\n#else\n#error\n#endif" TARGETARCH_ARM64)

