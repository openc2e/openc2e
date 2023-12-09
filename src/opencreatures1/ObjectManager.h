#pragma once

#include "EngineContext.h"
#include "Object.h"
#include "ObjectHandle.h"
#include "common/Ranges.h"
#include "common/SlotMap.h"

#include <memory>

class ObjectManager {
  private:
	DenseSlotMap<std::unique_ptr<Object>> m_pool;

	auto as_ptr_range() {
		return make_transform_view(m_pool, [](auto&& p) { return p.get(); });
	}

  public:
	ObjectManager() {}

	template <typename T>
	auto add() -> std::enable_if_t<std::is_base_of<Object, T>::value, ObjectHandle> {
		auto t = new T();
		ObjectHandle handle = m_pool.add(std::unique_ptr<Object>(t));
		t->uid = handle;
		return handle;
	}

	template <typename T>
	auto add() -> std::enable_if_t<std::is_pointer<T>::value, ObjectHandle> {
		return add<std::remove_pointer_t<T>>();
	}

	Object* try_get(ObjectHandle handle) {
		auto* obj = m_pool.try_get(handle);
		if (!obj) {
			return nullptr;
		}
		return obj->get();
	}

	auto begin() {
		return as_ptr_range().begin();
	}

	auto end() {
		return as_ptr_range().end();
	}

	void tick() {
		for (auto* o : *this) {
			o->tick();
		}
	}

	int32_t count_classifier(int32_t family, int32_t genus, int32_t species) {
		int32_t result = 0;
		for (auto* o : *this) {
			if (
				(o->family == family || family == 0) && (o->genus == genus || genus == 0) && (o->species == species || species == 0)) {
				result++;
			}
		}
		return result;
	}
};