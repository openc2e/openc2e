/*
 *  sprImage.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Nov 19 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#pragma once

#include "common/Image.h"
#include "common/io/Reader.h"

#include <stdint.h>
#include <vector>

struct SprFileData {
	std::vector<Image> images;
	std::vector<int32_t> offsets;
};

MultiImage ReadSprFile(Reader& in);
SprFileData ReadSprFileWithMetadata(Reader& in, int32_t absolute_base, int32_t image_count);