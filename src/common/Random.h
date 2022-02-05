#pragma once

#include <stdlib.h>

namespace Random {

template <typename T>
T randrange(T low, T high) {
	// TODO: this is not a uniform distribution. or seeded. welp.
	return rand() % (high - low) + low;
}

} // namespace Random