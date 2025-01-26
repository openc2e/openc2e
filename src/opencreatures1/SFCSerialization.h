#pragma once

#include <memory>

class Renderable;

namespace sfc {
struct CGalleryV1;
struct EntityV1;
struct SFCFile;
} // namespace sfc


void sfc_load_everything(const sfc::SFCFile&);
sfc::SFCFile sfc_dump_everything();