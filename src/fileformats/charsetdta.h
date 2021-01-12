#pragma once

#include "Image.h"
#include "shared_array.h"
#include "span.h"

#include <cstdint>
#include <vector>

MultiImage ReadCharsetDtaFile(std::istream& in);