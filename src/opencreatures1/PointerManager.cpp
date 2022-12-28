#include "PointerManager.h"

#include "ObjectManager.h"
#include "ViewportManager.h"
#include "common/backend/BackendEvent.h"


void PointerManager::update() {
	Object* obj = g_engine_context.objects->try_get(m_pointer_tool);
	Renderable* r = obj->get_renderable_for_part(0);
	r->set_position(
		g_engine_context.viewport->window_x_to_world_x(m_screenx) - obj->pointer_data->relx,
		g_engine_context.viewport->window_y_to_world_y(m_screeny) - obj->pointer_data->rely);
}

void PointerManager::handle_event(const BackendEvent& event) {
	if (event.type == eventmousemove) {
		m_screenx = event.x;
		m_screeny = event.y;
	}
	if (event.type == eventmousebuttondown && event.button == buttonleft) {
		// Left-button clicks
		// What happens here? We look over all objects whose bounding boxes contain
		// the click location, and find the topmost (largest z-order) one that
		// has ATTR & Activateable.
		// Note that we look for Objects, not Parts - in Creatures 1, CompoundObjects
		// must have all of their parts w/in the bounding box of the first/main part.
		// We use the main part's z-order as the object overall z-order.

		Object* pntr = g_engine_context.objects->try_get(m_pointer_tool);

		int worldx = g_engine_context.viewport->window_x_to_world_x(event.x) + pntr->pointer_data->relx;
		int worldy = g_engine_context.viewport->window_y_to_world_y(event.y) + pntr->pointer_data->rely;

		fmt::print("click @ {} {}\n", worldx, worldy);

		Object* best_object = nullptr;

		// TODO: move this into some sort of generic spatial query system. we
		// can start by just iterating all objects, but later on we can optimize
		// this, ENUM, ESEE, collisions, etc. if we have all of the similar logic
		// in the same place.
		for (auto& obj : *g_engine_context.objects) {
			if (obj->pointer_data) {
				// pointer can't click on itself
				continue;
			}
			if (!(obj->attr & ATTR_ACTIVATEABLE)) {
				// only ACTIVATEABLE objects
				continue;
			}

			// TODO: better way to handle world wrap?
			auto bbox = obj->get_bbox();
			bool contains_click = (bbox.has_point(worldx, worldy) || bbox.has_point(worldx + CREATURES1_WORLD_WIDTH, worldy) || bbox.has_point(worldx - CREATURES1_WORLD_WIDTH, worldy));
			bool topmost = (best_object == nullptr || obj->get_z_order() > best_object->get_z_order());
			if (contains_click && topmost) {
				fmt::print("found {}\n", repr(obj.get()));
				best_object = obj.get();
			}
		}

		if (best_object) {
			auto bbox = best_object->get_bbox();
			// TODO: better way to handle world wrap?
			int32_t relx = worldx - bbox.x;
			int32_t rely = worldy - bbox.y;
			if (relx < 0) {
				relx += CREATURES1_WORLD_WIDTH;
			}
			printf("clickrel %i %i\n", relx, rely);
			best_object->handle_left_click(relx, rely);
		}
	}
}