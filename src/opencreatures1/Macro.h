#pragma once

#include "common/StaticVector.h"
#include "objects/ObjectHandle.h"

#include <array>
#include <stdint.h>
#include <string>

struct Macro {
	Macro() = default;
	explicit Macro(std::string script_)
		: script(std::move(script_)) {}

	bool selfdestruct = false;
	bool inst = false;
	std::string script;
	uint32_t ip = 0;
	StaticVector<int32_t, 20> stack;
	std::array<int32_t, 10> vars{};
	ObjectHandle ownr;
	ObjectHandle from;
	ObjectHandle exec;
	ObjectHandle targ;
	ObjectHandle _it_;
	int32_t part = 0;
	std::string subroutine_label;
	uint32_t subroutine_address = 0;
	int32_t wait = 0;
	bool destroy_as_soon_as_possible = false;

	// TODO: oof.
	std::vector<ObjectHandle> enum_result;
};