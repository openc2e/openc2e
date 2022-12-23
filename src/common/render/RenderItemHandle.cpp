#include "RenderItemHandle.h"

#include "RenderSystem.h"

RenderItemHandle::RenderItemHandle(RenderItemHandle&& other)
	: RenderItemHandle() {
	*this = std::move(other);
}

RenderItemHandle& RenderItemHandle::operator=(RenderItemHandle&& other) {
	reset();
	parent = other.parent;
	key = other.key;
	other.parent = nullptr;
	other.key = {};
	return *this;
}

RenderItemHandle::~RenderItemHandle() {
	reset();
}

void RenderItemHandle::reset() {
	if (!parent) {
		return;
	}
	parent->m_render_items.erase(key);
	parent = nullptr;
	key = {};
}

RenderItemHandle::operator bool() const {
	return parent != nullptr && key;
}