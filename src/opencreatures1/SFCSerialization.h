#pragma once

#include "objects/ObjectHandle.h"

#include <memory>

class Object;
class Renderable;

namespace sfc {
struct CGalleryV1;
struct EntityV1;
struct ObjectV1;
struct SFCFile;
} // namespace sfc

class SFCContext {
  public:
	virtual ~SFCContext() = default;
	virtual bool is_loading() const = 0;
	virtual bool is_storing() const = 0;

	virtual ObjectHandle load_object(sfc::ObjectV1*) = 0;
	virtual std::shared_ptr<sfc::ObjectV1> dump_object(Object*) = 0;
};

void sfc_load_everything(const sfc::SFCFile&);
sfc::SFCFile sfc_dump_everything();
Renderable sfc_load_renderable(const sfc::EntityV1*);
std::shared_ptr<sfc::EntityV1> sfc_dump_renderable(const Renderable&, const std::shared_ptr<sfc::CGalleryV1>& = {});