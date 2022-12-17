#include "PointerManager.h"

#include "ObjectManager.h"
#include "ViewportManager.h"
#include "common/backend/BackendEvent.h"


void PointerManager::update() {
	Object* obj = g_engine_context.objects->try_get(m_pointer_tool);
	Renderable* r = obj->get_renderable_for_part(0);
	r->x = m_screenx + g_engine_context.viewport->scrollx - obj->pointer_data->relx;
	r->y = m_screeny + g_engine_context.viewport->scrolly - obj->pointer_data->rely;
}

void PointerManager::handle_event(const BackendEvent& event) {
	if (event.type == eventmousemove) {
		m_screenx = event.x;
		m_screeny = event.y;
	}
	if (event.type == eventmousebuttondown && event.button == buttonleft) {
		Object* obj = g_engine_context.objects->try_get(m_pointer_tool);

		int x = g_engine_context.viewport->scrollx + event.x + obj->pointer_data->relx;
		int y = g_engine_context.viewport->scrolly + event.y + obj->pointer_data->rely;

		fmt::print("click @ {} {}\n", x, y);

		Object* last_object = nullptr;

		for (auto& obj : *g_engine_context.objects) {
			if (obj->pointer_data || obj->scenery_data) {
				continue;
			}

			auto bbox = get_object_bbox(obj.get());
			if (x >= bbox.left && x <= bbox.right && y >= bbox.top && y <= bbox.bottom) {
				fmt::print("found {}\n", repr(obj.get()));
				if (obj->simple_data && obj->attr & ATTR_ACTIVATEABLE) {
					// TODO: handle clicks on compound objects
					if (last_object != nullptr) {
						fmt::print("ERRO [PointerManager] found multiple objects for click\n");
					}
					last_object = obj.get();
				}
			}
		}

		if (last_object) {
			last_object->handle_click();
		}
	}
}