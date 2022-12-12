#pragma once

#include "ObjectManager.h"
#include "ViewportManager.h"
#include "common/backend/BackendEvent.h"

class PointerManager {
  public:
	ObjectHandle m_pointer_tool;

	int m_screenx;
	int m_screeny;

	PointerManager() {}

	void update() {
		Object* obj = g_engine_context.objects->try_get(m_pointer_tool);
		Renderable* r = obj->get_renderable_for_part(0);
		r->x = m_screenx + g_engine_context.viewport->scrollx - obj->pointer_data->relx;
		r->y = m_screeny + g_engine_context.viewport->scrolly - obj->pointer_data->rely;
	}

	void handle_event(const BackendEvent& event) {
		if (event.type == eventmousemove) {
			m_screenx = event.x;
			m_screeny = event.y;
		}
	}
};