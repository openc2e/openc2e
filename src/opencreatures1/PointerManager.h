#pragma once

#include "ObjectHandle.h"
#include "common/backend/BackendEvent.h"

class PointerManager {
  public:
	ObjectHandle m_pointer_tool;

	int m_screenx;
	int m_screeny;

	PointerManager() {}
	void update();
	void handle_event(const BackendEvent& event);
};