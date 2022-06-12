#pragma once

#include "Object.h"
#include "ObjectHandle.h"
#include "common/EntityPool.h"
#include "common/PointerView.h"

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
	// TODO: should we actually round these high-precision coordinates? This only
	// matters for vehicles.
	r.left = main_part->x_times_256 / 256;
	r.right = main_part->x_times_256 / 256 + main_part->width();
	r.top = main_part->y_times_256 / 256;
	r.bottom = main_part->y_times_256 / 256 + main_part->height();
	return r;
}

inline void move_object_to_high_precision(PointerView<Object> obj, PointerView<RenderableManager> renderables, int32_t x256, int32_t y256) {
	if (!obj) {
		throw_exception("Tried to move a null object");
	}

	auto* main_part = renderables->try_get(obj->get_part(0));
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*obj));
	}

	if (main_part->x_times_256 == x256 && main_part->y_times_256 == y256) {
		return;
	}

	main_part->x_times_256 = x256;
	main_part->y_times_256 = y256;

	if (auto* comp = dynamic_cast<CompoundObject*>(obj.get())) {
		for (size_t i = 1; i < comp->parts.size(); ++i) {
			auto& p = comp->parts[i];

			Renderable* part = renderables->try_get(obj->get_part(i));

			part->x_times_256 = x256 + p.x * 256;
			part->y_times_256 = y256 + p.y * 256;
		}
	}
}

inline void move_object_by_high_precision(PointerView<Object> obj, PointerView<RenderableManager> renderables, int32_t xdiff256, int32_t ydiff256) {
	if (!obj) {
		throw_exception("Tried to move a null object");
	}

	auto* main_part = renderables->try_get(obj->get_part(0));
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*obj));
	}

	if (xdiff256 == 0 && ydiff256 == 0) {
		return;
	}

	main_part->x_times_256 += xdiff256;
	main_part->y_times_256 += ydiff256;

	if (auto* comp = dynamic_cast<CompoundObject*>(obj.get())) {
		for (size_t i = 1; i < comp->parts.size(); ++i) {
			auto& p = comp->parts[i];

			Renderable* part = renderables->try_get(obj->get_part(i));

			part->x_times_256 = main_part->x_times_256 + p.x * 256;
			part->y_times_256 = main_part->y_times_256 + p.y * 256;
		}
	}
}

inline void move_object_to(PointerView<Object> obj, PointerView<RenderableManager> renderables, int32_t x, int32_t y) {
	// TODO: if the object's current x-position is 100.1 and the new x-position
	// is 100, should we actually move it? do we lose the high-precision part of
	// the current position? This is only relevant to Vehicles.
	move_object_to_high_precision(obj, renderables, x * 256, y * 256);
}

inline void move_object_by(PointerView<Object> obj, PointerView<RenderableManager> renderables, int32_t xdiff, int32_t ydiff) {
	// TODO: if the object's current x-position is 100.1 and the x-diff is 5, do
	// we move it to 105.1 or 105? e.g. should we lose the high-precision part
	// of the current position? This is only relevant to Vehicles.
	move_object_by_high_precision(obj, renderables, xdiff * 256, ydiff * 256);
}

class ObjectManager {
  private:
	std::shared_ptr<RenderableManager> renderables;
	EntityPool<std::unique_ptr<Object>>
		m_pool;
	using Id = decltype(m_pool)::Id;
	static_assert(sizeof(ObjectHandle) == sizeof(Id), "");

	ObjectHandle id_to_handle(Id id) {
		// TODO
		return *(ObjectHandle*)&id;
	}
	Id handle_to_id(ObjectHandle handle) {
		// TODO
		return *(Id*)&handle;
	}

  public:
	ObjectManager(std::shared_ptr<RenderableManager> renderables_)
		: renderables(renderables_) {}

	template <typename T>
	ObjectHandle add(T obj) {
		auto t = new T(std::move(obj));
		ObjectHandle handle = id_to_handle(m_pool.add(std::unique_ptr<Object>(t)));
		t->uid = handle;
		return handle;
	}

	template <typename T>
	T* try_get(ObjectHandle handle) {
		auto* obj = m_pool.try_get(handle_to_id(handle));
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
				result.push_back(id_to_handle(i.id));
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
				move_object_by_high_precision(vehicle, renderables, vehicle->xvel_times_256, vehicle->yvel_times_256);
			}
		}
	}
};