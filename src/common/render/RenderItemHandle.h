#pragma once

#include "common/SlotMap.h"

class RenderSystem;

struct RenderItemHandle {
	RenderItemHandle() = default;
	RenderItemHandle(const RenderItemHandle&) = delete;
	RenderItemHandle& operator=(const RenderItemHandle&) = delete;
	RenderItemHandle(RenderItemHandle&&);
	RenderItemHandle& operator=(RenderItemHandle&&);
	~RenderItemHandle();
	explicit operator bool() const;

  private:
	friend RenderSystem;
	void reset();
	SlotMapKey key;
	// TODO: optimize by having a single global RenderServer?
	RenderSystem* parent = nullptr;
};