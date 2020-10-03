#pragma once

#include <cstdint>
#include <vector>

#include "Image.h"
#include "shared_array.h"
#include "span.h"

MultiImage ReadCharsetDtaFile(std::istream &in);