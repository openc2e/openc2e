#include "PointerManager.h"

#include "ViewportManager.h"
#include "common/backend/BackendEvent.h"
#include "objects/ObjectManager.h"
#include "objects/PointerTool.h"

#include <fmt/format.h>

void PointerManager::update() {
	Object* obj = g_engine_context.objects->try_get(m_pointer_tool);
	if (!obj) {
		return;
	}

	auto worldx = g_engine_context.viewport->window_x_to_world_x(m_screenx);
	auto worldy = g_engine_context.viewport->window_y_to_world_y(m_screeny);

	constexpr const int32_t too_big = (1 << 20);
	if (worldx >= too_big || worldx <= -too_big || worldy >= too_big || worldy <= -too_big) {
		// this can happen before the viewport manager is properly initialized. it causes
		// issues later because we try to convert some floats back to int32_t but the floats
		// are too big and would lose precision! these aren't real coordinates anyways so
		// just skip and wait until the viewport manager is correctly returning information.
		// TODO: properly make sure viewport manager is initialized instead of this hack
		fmt::print("warning: pointermanager got nonsense world coordinates {} {} from viewportmanager, skipping update\n",
			worldx, worldy);
		return;
	}

	obj->set_position(worldx - obj->as_pointer_tool()->relx, worldy - obj->as_pointer_tool()->rely);
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

		int worldx = g_engine_context.viewport->window_x_to_world_x(event.x);
		int worldy = g_engine_context.viewport->window_y_to_world_y(event.y);

		Object* pntr = g_engine_context.objects->try_get(m_pointer_tool);
		if (pntr) {
			worldx += pntr->as_pointer_tool()->relx;
			worldy += pntr->as_pointer_tool()->rely;
		}

		fmt::print("click @ {} {}\n", worldx, worldy);

		Object* best_object = nullptr;

		// TODO: move this into some sort of generic spatial query system. we
		// can start by just iterating all objects, but later on we can optimize
		// this, ENUM, ESEE, collisions, etc. if we have all of the similar logic
		// in the same place.
		for (auto* obj : *g_engine_context.objects) {
			if (obj->as_pointer_tool()) {
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
				fmt::print("found {}\n", format_as(obj));
				best_object = obj;
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