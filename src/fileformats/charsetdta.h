#pragma once

#include "Image.h"
#include "utils/shared_array.h"
#include "utils/span.h"

#include <cstdint>
#include <vector>

MultiImage ReadCharsetDtaFile(std::istream& in);