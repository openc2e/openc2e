#pragma once

#include "ObjectHandle.h"
#include "SimpleObject.h"

#include <stdint.h>

struct CallButton : SimpleObject {
	ObjectHandle lift;
	uint8_t floor;
};