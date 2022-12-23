#pragma once

#include "EngineContext.h"
#include "Object.h"
#include "ObjectHandle.h"
#include "common/PointerView.h"
#include "common/SlotMap.h"

#include <memory>

static inline Renderable* get_main_part(Object* obj) {
	if (!obj) {
		throw_exception("Can't get main part of null object");
	}

	auto* main_part = obj->get_renderable_for_part(0);
	if (!main_part) {
		throw_exception("Can't get main part of object without any parts: {}", repr(*obj));
	}

	return main_part;
}

inline Rect get_object_bbox(Object* obj) {
	Renderable* main_part = get_main_part(obj);
	return main_part->get_bbox();
}

inline void move_object_to(Object* obj, fixed24_8_t x, fixed24_8_t y) {
	// TODO: if the object's current x-position is 100.1 and the new x-position
	// is 100 (low-precision), should we actually move it? do we lose the
	// high-precision part of the current position? This is only relevant to Vehicles.

	if (!obj) {
		throw_exception("Tried to move a null object");
	}

	auto* main_part = obj->get_renderable_for_part(0);
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*obj));
	}

	// TODO: replace this with get_position and a Vector2?
	if (main_part->get_x() == x && main_part->get_y() == y) {
		return;
	}

	main_part->set_position(x, y);

	if (auto* comp = obj->compound_data.get()) {
		for (size_t i = 1; i < comp->parts.size(); ++i) {
			Renderable& p = comp->parts[i].renderable;

			p.set_position(x + p.get_x(), y + p.get_y());
		}
	}
}

inline void move_object_by(Object* obj, fixed24_8_t xdiff, fixed24_8_t ydiff) {
	// TODO: if the object's current x-position is 100.1 and the x-diff is 5 (low-
	// precision), do we move it to 105.1 or 105? e.g. should we lose the
	// high-precision part of the current position? This is only relevant to Vehicles.

	if (!obj) {
		throw_exception("Tried to move a null object");
	}

	auto* main_part = obj->get_renderable_for_part(0);
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*obj));
	}

	if (xdiff == 0 && ydiff == 0) {
		return;
	}

	main_part->set_position(main_part->get_x() + xdiff, main_part->get_y() + ydiff);

	if (auto* comp = obj->compound_data.get()) {
		for (size_t i = 1; i < comp->parts.size(); ++i) {
			Renderable& p = comp->parts[i].renderable;

			p.set_position(main_part->get_x() + p.get_x(), main_part->get_y() + p.get_y());
		}
	}
}

class ObjectManager {
  private:
	DenseSlotMap<std::unique_ptr<Object>> m_pool;

  public:
	ObjectManager() {}

	ObjectHandle add() {
		auto t = new Object();
		ObjectHandle handle = m_pool.add(std::unique_ptr<Object>(t));
		t->uid = handle;
		return handle;
	}

	Object* try_get(ObjectHandle handle) {
		auto* obj = m_pool.try_get(handle);
		if (!obj) {
			return nullptr;
		}
		return obj->get();
	}

	auto begin() {
		return m_pool.begin();
	}

	auto end() {
		return m_pool.end();
	}

	void tick() {
		// do something for objects?

		for (auto& o : m_pool) {
			if (o->current_sound) {
				auto bbox = get_object_bbox(o.get());
				o->current_sound.set_position(bbox.x, bbox.y, bbox.width, bbox.height);
			}

			if (auto* vehicle = o->vehicle_data.get()) {
				move_object_by(o.get(), vehicle->xvel, vehicle->yvel);
			}
		}
	}

	int32_t count_classifier(int32_t family, int32_t genus, int32_t species) {
		int32_t result = 0;
		for (auto& o : *this) {
			if (
				(o->family == family || family == 0) && (o->genus == genus || genus == 0) && (o->species == species || species == 0)) {
				result++;
			}
		}
		return result;
	}
};