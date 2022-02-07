#pragma once

#include <stdint.h>

class ObjectHandle {
  public:
	ObjectHandle() {}

	uint32_t id() const {
		return m_id;
	}

	bool operator==(const ObjectHandle& other) const {
		return m_id == other.m_id;
	}

	bool operator!=(const ObjectHandle& other) const {
		return !(*this == other);
	}

  private:
	friend class ObjectManager;
	uint32_t m_id = ~0;
};