#pragma once

#include "common/Image.h"
#include "common/shared_array.h"
#include "common/span.h"

#include <cstdint>
#include <vector>

MultiImage ReadCharsetDtaFile(std::istream& in);