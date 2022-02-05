#pragma once

#include <stdint.h>

class ObjectHandle {
  public:
	ObjectHandle() {}
	uint32_t id() const {
		return m_id;
	}

  private:
	friend class ObjectManager;
	uint32_t m_id = ~0;
};