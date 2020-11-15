#pragma once

#include <stdint.h>

// A handle into an AudioChannel returned by the current AudioBackend

struct AudioChannel {
	uint32_t handle = (uint32_t)-1; // unlikely to ever be valid for any implementation
	operator bool() { return handle != (uint32_t)-1; }
};
