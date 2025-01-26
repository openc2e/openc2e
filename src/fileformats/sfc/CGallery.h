#pragma once

#include "fileformats/MFCObject.h"

#include <stdint.h>
#include <string>
#include <vector>

namespace sfc {

struct CGalleryV1;

struct ImageV1 {
	// not CArchive serialized
	CGalleryV1* parent = nullptr;
	uint8_t status;
	int32_t width;
	int32_t height;
	int32_t offset;
};

struct CGalleryV1 : MFCObject {
	std::string filename;
	int32_t absolute_base;
	int32_t refcount;
	std::vector<ImageV1> images;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar.size_u32(images);
		ar.ascii_dword(filename);
		ar(absolute_base);
		ar(refcount);

		for (auto& image : images) {
			ar(image.parent);
			ar(image.status);
			ar(image.width);
			ar(image.height);
			ar(image.offset);
		}
	}
};

} // namespace sfc