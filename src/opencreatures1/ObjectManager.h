#pragma once

#include "Object.h"
#include "ObjectHandle.h"
#include "common/PointerView.h"
#include "common/SlotMap.h"

#include <memory>

static inline Renderable* get_main_part(PointerView<Object> obj, PointerView<RenderableManager> renderables) {
	if (!obj) {
		throw_exception("Can't get main part of null object");
	}

	auto* main_part = renderables->try_get(obj->get_part(0));
	if (!main_part) {
		throw_exception("Can't get main part of object without any parts: {}", repr(*obj));
	}

	return main_part;
}

inline Rect get_object_bbox(PointerView<Object> obj, PointerView<RenderableManager> renderables) {
	Renderable* main_part = get_main_part(obj, renderables);

	Rect r;
	// TODO: should we actually trunc these high-precision coordinates? This only
	// matters for vehicles.
	r.left = main_part->x.trunc();
	r.right = main_part->x.trunc() + main_part->width();
	r.top = main_part->y.trunc();
	r.bottom = main_part->y.trunc() + main_part->height();
	return r;
}

inline void move_object_to(PointerView<Object> obj, PointerView<RenderableManager> renderables, fixed24_8_t x, fixed24_8_t y) {
	// TODO: if the object's current x-position is 100.1 and the new x-position
	// is 100 (low-precision), should we actually move it? do we lose the
	// high-precision part of the current position? This is only relevant to Vehicles.

	if (!obj) {
		throw_exception("Tried to move a null object");
	}

	auto* main_part = renderables->try_get(obj->get_part(0));
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*obj));
	}

	if (main_part->x == x && main_part->y == y) {
		return;
	}

	main_part->x = x;
	main_part->y = y;

	if (auto* comp = dynamic_cast<CompoundObject*>(obj.get())) {
		for (size_t i = 1; i < comp->parts.size(); ++i) {
			auto& p = comp->parts[i];

			Renderable* part = renderables->try_get(obj->get_part(numeric_cast<int32_t>(i)));

			part->x = x + p.x;
			part->y = y + p.y;
		}
	}
}

inline void move_object_by(PointerView<Object> obj, PointerView<RenderableManager> renderables, fixed24_8_t xdiff, fixed24_8_t ydiff) {
	// TODO: if the object's current x-position is 100.1 and the x-diff is 5 (low-
	// precision), do we move it to 105.1 or 105? e.g. should we lose the
	// high-precision part of the current position? This is only relevant to Vehicles.

	if (!obj) {
		throw_exception("Tried to move a null object");
	}

	auto* main_part = renderables->try_get(obj->get_part(0));
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*obj));
	}

	if (xdiff == 0 && ydiff == 0) {
		return;
	}

	main_part->x += xdiff;
	main_part->y += ydiff;

	if (auto* comp = dynamic_cast<CompoundObject*>(obj.get())) {
		for (size_t i = 1; i < comp->parts.size(); ++i) {
			auto& p = comp->parts[i];

			Renderable* part = renderables->try_get(obj->get_part(numeric_cast<int32_t>(i)));

			part->x = main_part->x + p.x;
			part->y = main_part->y + p.y;
		}
	}
}

class ObjectManager {
  private:
	std::shared_ptr<RenderableManager> renderables;
	DenseSlotMap<std::unique_ptr<Object>> m_pool;

  public:
	ObjectManager(std::shared_ptr<RenderableManager> renderables_)
		: renderables(renderables_) {}

	template <typename T>
	ObjectHandle add(T obj) {
		auto t = new T(std::move(obj));
		ObjectHandle handle = m_pool.add(std::unique_ptr<Object>(t));
		t->uid = handle;
		return handle;
	}

	template <typename T>
	T* try_get(ObjectHandle handle) {
		auto* obj = m_pool.try_get(handle);
		if (!obj) {
			return nullptr;
		}
		return dynamic_cast<T*>(obj->get());
	}

	template <typename T>
	std::vector<ObjectHandle> find_all() {
		// TODO: make this const
		std::vector<ObjectHandle> result;
		for (auto i : m_pool.enumerate()) {
			if (dynamic_cast<T*>(i.value->get())) {
				result.push_back(i.id);
			}
		}
		return result;
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
				auto bbox = get_object_bbox(o, renderables);
				o->current_sound.set_position(bbox.left, bbox.top, bbox.width(), bbox.height());
			}

			if (auto* vehicle = dynamic_cast<Vehicle*>(o.get())) {
				move_object_by(vehicle, renderables, vehicle->xvel, vehicle->yvel);
			}
		}
	}
};