#pragma once

#include "ObjectManager.h"
#include "RenderableManager.h"
#include "ViewportManager.h"
#include "common/backend/BackendEvent.h"

class PointerManager {
  public:
	ObjectHandle m_pointer_tool;

	int m_screenx;
	int m_screeny;

	PointerManager() {}

	void update() {
		PointerTool* pointer = g_engine_context.objects->try_get<PointerTool>(m_pointer_tool);
		Renderable* r = g_engine_context.renderables->try_get(pointer->part);
		r->x = m_screenx + g_engine_context.viewport->scrollx - pointer->relx;
		r->y = m_screeny + g_engine_context.viewport->scrolly - pointer->rely;
	}

	void handle_event(const BackendEvent& event) {
		if (event.type == eventmousemove) {
			m_screenx = event.x;
			m_screeny = event.y;
		}
	}
};