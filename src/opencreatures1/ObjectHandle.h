#pragma once

#include "common/SlotMap.h"

#include <memory>
#include <stdint.h>

class Object;

using ObjectHandle = DenseSlotMap<std::unique_ptr<Object>>::Key;
