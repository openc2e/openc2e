#pragma once

#include "EngineContext.h"
#include "Object.h"
#include "ObjectHandle.h"
#include "common/PointerView.h"
#include "common/SlotMap.h"

#include <memory>


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
		for (auto& o : m_pool) {
			o->tick();
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