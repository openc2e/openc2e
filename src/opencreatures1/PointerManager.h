#pragma once

#include "objects/ObjectHandle.h"

struct BackendEvent;

class PointerManager {
  public:
	ObjectHandle m_pointer_tool;

	float m_screenx;
	float m_screeny;

	PointerManager() {}
	void update();
	void handle_event(const BackendEvent& event);
};