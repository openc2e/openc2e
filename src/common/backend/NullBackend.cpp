#include "NullBackend.h"

Backend* NullBackend::get_instance() {
	static NullBackend s_null_backend;
	return &s_null_backend;
}