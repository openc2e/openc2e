#include "Backend.h"

#include "common/Exception.h"

static Backend* s_backend = nullptr;

Backend* get_backend() {
	return s_backend;
}
void set_backend(Backend* backend) {
	if (s_backend) {
		// TODO: ???
		throw Exception("set_backend: Backend has already been set!");
	}
	s_backend = backend;
}