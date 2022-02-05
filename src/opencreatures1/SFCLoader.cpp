#include "SFCLoader.h"

#include "ImageManager.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "ObjectManager.h"
#include "RenderableManager.h"
#include "Scriptorium.h"
#include "ViewportManager.h"

static Renderable renderable_from_sfc_entity(ImageManager& images, sfc::EntityV1& part) {
	if (part.x >= CREATURES1_WORLD_WIDTH) {
		throw Exception(fmt::format("Expected x to be between [0, {}), but got {}", CREATURES1_WORLD_WIDTH, part.x));
	}
	if (part.y >= CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected y to be between [0, {}), but got {}", CREATURES1_WORLD_HEIGHT, part.y));
	}

	Renderable r;
	r.x = part.x;
	r.y = part.y;
	r.z = part.z_order;
	r.object_sprite_base = part.sprite->first_sprite;
	r.part_sprite_base = part.image_offset;
	r.sprite_index = part.current_sprite - part.image_offset;
	r.sprite = images.get_image(part.sprite->filename, ImageManager::IMAGE_SPR);
	r.has_animation = part.has_animation;
	if (part.has_animation) {
		r.animation_frame = part.animation_frame;
		r.animation_string = part.animation_string;
	}
	return r;
}

static Object object_without_refs_from_sfc(const sfc::ObjectV1& p) {
	Object obj;
	obj.species = p.species;
	obj.genus = p.genus;
	obj.family = p.family;
	obj.movement_status = static_cast<MovementStatus>(p.movement_status);
	obj.attr = p.attr;
	obj.limit.left = p.limit_left;
	obj.limit.top = p.limit_top;
	obj.limit.right = p.limit_right;
	obj.limit.bottom = p.limit_bottom;
	// obj.carrier = sfc_object_mapping.at(p.carrier);
	obj.actv = p.actv;
	// creaturesImage sprite;
	obj.tick_value = p.tick_value;
	obj.ticks_since_last_tick_event = p.ticks_since_last_tick_event;
	// obj.objp = sfc_object_mapping.at(p.objp);
	// std::string current_sound;
	obj.obv0 = p.obv0;
	obj.obv1 = p.obv1;
	obj.obv2 = p.obv2;
	return obj;
}

static SimpleObject simple_object_without_refs_from_sfc(RenderableManager& renderables, ImageManager& images, const sfc::SimpleObjectV1& p) {
	SimpleObject obj;
	static_cast<Object&>(obj) = object_without_refs_from_sfc(p);
	obj.part = renderables.add(renderable_from_sfc_entity(images, *p.part));
	obj.z_order = p.z_order;
	obj.click_bhvr = p.click_bhvr;
	obj.touch_bhvr = p.touch_bhvr;
	return obj;
}

SFCLoader::SFCLoader(const sfc::SFCFile& sfc_)
	: sfc(sfc_) {
	sfc_object_mapping[nullptr] = {};
}

void SFCLoader::load_viewport(ViewportManager& viewport) {
	viewport.scrollx = sfc.scrollx;
	viewport.scrolly = sfc.scrolly;
}
void SFCLoader::load_map(MapManager& map) {
	for (auto& r : sfc.map->rooms) {
		Room room;
		room.left = r.left;
		room.top = r.top;
		room.right = r.right;
		room.bottom = r.bottom;
		room.type = r.type;
		map.rooms.push_back(room);
	}
}
void SFCLoader::load_objects(ObjectManager& objects, RenderableManager& renderables, ImageManager& images) {
	// first load toplevel objects
	for (auto* p : sfc.objects) {
		if (auto* pt = dynamic_cast<sfc::PointerToolV1*>(p)) {
			PointerTool obj;
			static_cast<SimpleObject&>(obj) = simple_object_without_refs_from_sfc(renderables, images, *pt);
			obj.relx = pt->relx;
			obj.rely = pt->rely;
			// obj.bubble = pt->bubble;
			obj.text = pt->text;
			auto handle = objects.add(obj);
			sfc_object_mapping[p] = handle;

		} else if (auto* simp = dynamic_cast<sfc::SimpleObjectV1*>(p)) {
			SimpleObject obj = simple_object_without_refs_from_sfc(renderables, images, *simp);
			sfc_object_mapping[p] = objects.add(obj);

		} else if (auto* comp = dynamic_cast<sfc::CompoundObjectV1*>(p)) {
			for (auto& part : comp->parts) {
				renderables.add(renderable_from_sfc_entity(images, *part.entity));
			}
			CompoundObject obj;
			static_cast<Object&>(obj) = object_without_refs_from_sfc(*p);

			for (auto& cp : comp->parts) {
				CompoundPart part;
				part.renderable = renderables.add(renderable_from_sfc_entity(images, *cp.entity));
				part.x = cp.x;
				part.y = cp.y;
				obj.parts.push_back(part);
			}
			for (size_t i = 0; i < obj.hotspots.size(); ++i) {
				obj.hotspots[i].left = comp->hotspots[i].left;
				obj.hotspots[i].top = comp->hotspots[i].top;
				obj.hotspots[i].right = comp->hotspots[i].right;
				obj.hotspots[i].bottom = comp->hotspots[i].bottom;
			}
			for (size_t i = 0; i < obj.functions_to_hotspots.size(); ++i) {
				obj.functions_to_hotspots[i] = comp->functions_to_hotspots[i];
			}

			sfc_object_mapping[p] = objects.add(obj);

		} else {
			Object obj = object_without_refs_from_sfc(*p);
			sfc_object_mapping[p] = objects.add(obj);
		}
	}
	for (auto* p : sfc.sceneries) {
		Scenery scen;
		static_cast<Object&>(scen) = object_without_refs_from_sfc(*p);
		scen.part = renderables.add(renderable_from_sfc_entity(images, *p->part));
		sfc_object_mapping[p] = objects.add(scen);
	}
	// patch up object references
	for (auto* p : sfc.objects) {
		ObjectHandle handle = sfc_object_mapping[p];
		Object* obj = objects.try_get<Object>(handle);
		obj->carrier = sfc_object_mapping[p->carrier];
		obj->objp = sfc_object_mapping[p->objp];

		if (auto* pt = dynamic_cast<sfc::PointerToolV1*>(p)) {
			auto pointertool = objects.try_get<PointerTool>(handle);
			pointertool->bubble = sfc_object_mapping[pt->bubble];
		}
	}
	for (auto* p : sfc.sceneries) {
		Object* obj = objects.try_get<Object>(sfc_object_mapping[p]);
		obj->carrier = sfc_object_mapping[p->carrier];
		obj->objp = sfc_object_mapping[p->objp];
	}
}

void SFCLoader::load_scripts(Scriptorium& scriptorium) {
	for (auto& s : sfc.scripts) {
		scriptorium.add(s.family, s.genus, s.species, s.eventno, s.text);
	}
}

void SFCLoader::load_macros(MacroManager& macros) {
	for (auto* m : sfc.macros) {
		Macro macro;
		macro.selfdestruct = m->selfdestruct;
		macro.inst = m->inst;
		macro.script = m->script;
		macro.ip = m->ip;
		for (size_t i = 0; i < m->sp; ++i) {
			macro.stack.push_back(m->stack[i]);
		}
		macro.vars = m->vars;
		macro.ownr = sfc_object_mapping[m->ownr];
		macro.from = sfc_object_mapping[m->from];
		macro.exec = sfc_object_mapping[m->exec];
		macro.targ = sfc_object_mapping[m->targ];
		macro._it_ = sfc_object_mapping[m->_it_];
		macro.part = m->part;
		macro.subroutine_label = m->subroutine_label;
		macro.subroutine_address = m->subroutine_address;
		macro.wait = m->wait;

		macros.add(macro);
	}
}