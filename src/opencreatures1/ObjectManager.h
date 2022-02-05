#pragma once

#include "Object.h"
#include "ObjectHandle.h"
#include "common/EntityPool.h"

#include <memory>

class ObjectManager {
  private:
	EntityPool<std::unique_ptr<Object>> m_pool;
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
	}
};