#pragma once

#include "fileformats/MFCReader.h"

#include <array>
#include <iosfwd>
#include <string>
#include <vector>

namespace sfc {

struct CGalleryV1;

struct ImageV1 {
	// not CArchive serialized
	CGalleryV1* parent;
	uint8_t status;
	uint32_t width;
	uint32_t height;
	uint32_t offset;
};

struct CGalleryV1 {
	std::string filename;
	int32_t first_sprite;
	int32_t refcount;
	std::vector<ImageV1> images;

	void read_from(MFCReader& in) {
		uint32_t num_images = in.read32le();
		filename = in.read_ascii(4);
		first_sprite = in.reads32le();
		refcount = in.reads32le();

		for (size_t i = 0; i < num_images; ++i) {
			ImageV1 image;
			image.parent = in.read_type<CGalleryV1>();
			image.status = in.read8();
			image.width = in.read32le();
			image.height = in.read32le();
			image.offset = in.read32le();
			images.push_back(image);
		}
	}
};

struct RoomV1 {
	// not CArchive serialized
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
	int32_t type;
};


struct BacteriumV1 {
	// not CArchive serialized
	uint8_t state;
	uint8_t antigen;
	uint8_t fatal_level;
	uint8_t infect_level;
	uint8_t toxin1;
	uint8_t toxin2;
	uint8_t toxin3;
	uint8_t toxin4;
};

struct MapDataV1 {
	uint32_t unused_is_wrappable;
	uint32_t time_of_day;
	CGalleryV1* background;
	std::vector<RoomV1> rooms;
	std::array<uint32_t, 261> groundlevel;
	std::array<BacteriumV1, 100> bacteria;

	void read_from(MFCReader& in) {
		unused_is_wrappable = in.read32le();
		time_of_day = in.read32le();

		background = in.read_type<CGalleryV1>();

		uint32_t num_rooms = in.read32le();
		for (size_t i = 0; i < num_rooms; ++i) {
			RoomV1 room;
			room.left = in.reads32le();
			room.top = in.reads32le();
			room.right = in.reads32le();
			room.bottom = in.reads32le();
			room.type = in.reads32le();
			rooms.push_back(room);
		}
		for (size_t i = 0; i < 261; ++i) {
			groundlevel[i] = in.read32le();
		}
		for (size_t i = 0; i < 100; ++i) {
			bacteria[i].state = in.read8();
			bacteria[i].antigen = in.read8();
			bacteria[i].fatal_level = in.read8();
			bacteria[i].infect_level = in.read8();
			bacteria[i].toxin1 = in.read8();
			bacteria[i].toxin2 = in.read8();
			bacteria[i].toxin3 = in.read8();
			bacteria[i].toxin4 = in.read8();
		}
	}
};

struct EntityV1 {
	CGalleryV1* sprite;
	uint8_t current_sprite;
	uint8_t image_offset;
	int32_t z_order; // TODO: should be signed?
	int32_t x;
	int32_t y;
	uint8_t has_animation;
	uint8_t animation_frame; // only if has_animation is true
	std::string animation_string; // only if has_animation is true

	void read_from(MFCReader& in) {
		sprite = in.read_type<CGalleryV1>();
		current_sprite = in.read8();
		image_offset = in.read8();
		z_order = in.reads32le();
		x = in.reads32le();
		y = in.reads32le();
		has_animation = in.read8();
		if (has_animation) {
			animation_frame = in.read8();
			// TODO: Assert real animation string?
			animation_string = in.read_ascii_nullterminated(32);
		}
	}
};


struct ScriptV1 {
	// not CArchive serialized, though it likely should have been!
	uint8_t eventno;
	uint8_t species;
	uint8_t genus;
	uint8_t family;
	std::string text;

	void read_from(MFCReader& in) {
		eventno = in.read8();
		species = in.read8();
		genus = in.read8();
		family = in.read8();
		text = in.read_ascii_mfcstring();
	}
};


struct ObjectV1 {
	virtual ~ObjectV1(){};

	uint8_t species;
	uint8_t genus;
	uint8_t family;
	uint8_t movement_status;
	uint8_t attr;
	int32_t limit_left;
	int32_t limit_top;
	int32_t limit_right;
	int32_t limit_bottom;
	ObjectV1* carrier;
	uint8_t actv;
	CGalleryV1* sprite;
	int32_t tick_value;
	int32_t ticks_since_last_tick_event;
	ObjectV1* objp;
	std::string current_sound;
	int32_t obv0;
	int32_t obv1;
	int32_t obv2;
	std::vector<ScriptV1> scripts;

	void read_from(MFCReader& in) {
		(void)in.read8(); // eventno, unused
		species = in.read8();
		genus = in.read8();
		family = in.read8();
		movement_status = in.read8();
		attr = in.read8();
		limit_left = in.reads32le();
		limit_top = in.reads32le();
		limit_right = in.reads32le();
		limit_bottom = in.reads32le();
		carrier = in.read_type<ObjectV1>();
		actv = in.read8();
		sprite = in.read_type<CGalleryV1>();
		tick_value = in.reads32le();
		ticks_since_last_tick_event = in.reads32le();
		objp = in.read_type<ObjectV1>();
		current_sound = in.read_ascii_nullterminated(4);
		obv0 = in.reads32le();
		obv1 = in.reads32le();
		obv2 = in.reads32le();
		uint32_t num_scripts = in.read32le();
		for (size_t i = 0; i < num_scripts; ++i) {
			ScriptV1 script;
			script.read_from(in);
			scripts.push_back(script);
		}
	}
};

struct SimpleObjectV1 : ObjectV1 {
	EntityV1* part;
	int32_t z_order;
	std::array<uint8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	void read_from(MFCReader& in) {
		ObjectV1::read_from(in);

		part = in.read_type<EntityV1>();
		z_order = in.reads32le();
		click_bhvr[0] = in.read8();
		click_bhvr[1] = in.read8();
		click_bhvr[2] = in.read8();
		touch_bhvr = in.read8();
	}
};

struct BubbleV1 : SimpleObjectV1 {
	uint8_t life;
	int32_t creator;
	std::string text;

	void read_from(MFCReader& in) {
		SimpleObjectV1::read_from(in);

		life = in.read8();
		creator = in.reads32le(); // ???
		text = in.read_ascii_nullterminated(25);
	}
};

struct PointerToolV1 : SimpleObjectV1 {
	int32_t relx;
	int32_t rely;
	BubbleV1* bubble;
	std::string text;

	void read_from(MFCReader& in) {
		SimpleObjectV1::read_from(in);

		relx = in.reads32le();
		rely = in.reads32le();
		bubble = in.read_type<BubbleV1>();
		text = in.read_ascii_nullterminated(25);
	}
};

struct CompoundPartV1 {
	// not CArchive serialized
	EntityV1* entity;
	int32_t x;
	int32_t y;
};

struct HotSpotV1 {
	// not CArchive serialized
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
};

struct CompoundObjectV1 : ObjectV1 {
	std::vector<CompoundPartV1> parts;
	std::array<HotSpotV1, 6> hotspots;
	std::array<int32_t, 6> functions_to_hotspots;

	void read_from(MFCReader& in) {
		ObjectV1::read_from(in);

		uint32_t num_parts = in.read32le();
		for (size_t i = 0; i < num_parts; ++i) {
			CompoundPartV1 part;
			part.entity = in.read_type<EntityV1>();
			if (part.entity == nullptr) {
				throw Exception("whoops, entity is null");
			}
			part.x = in.reads32le();
			part.y = in.reads32le();
			parts.push_back(part);
		}
		for (size_t i = 0; i < 6; ++i) {
			hotspots[i].left = in.reads32le();
			hotspots[i].top = in.reads32le();
			hotspots[i].right = in.reads32le();
			hotspots[i].bottom = in.reads32le();
		}
		for (size_t i = 0; i < 6; ++i) {
			functions_to_hotspots[i] = in.reads32le();
		}
	}
};

struct VehicleV1 : CompoundObjectV1 {
	int32_t xvel_times_256;
	int32_t yvel_times_256;
	int32_t x_times_256;
	int32_t y_times_256;
	int32_t cabin_left;
	int32_t cabin_top;
	int32_t cabin_right;
	int32_t cabin_bottom;
	uint32_t bump;

	void read_from(MFCReader& in) {
		CompoundObjectV1::read_from(in);
		xvel_times_256 = in.reads32le();
		yvel_times_256 = in.reads32le();
		x_times_256 = in.reads32le();
		y_times_256 = in.reads32le();
		cabin_left = in.reads32le();
		cabin_top = in.reads32le();
		cabin_right = in.reads32le();
		cabin_bottom = in.reads32le();
		bump = in.read32le();
	}
};

struct BlackboardV1 : CompoundObjectV1 {
	struct BlackboardWord {
		// not CArchive serialized
		uint32_t value;
		std::string text;
	};

	uint8_t background_color;
	uint8_t chalk_color;
	uint8_t alias_color;
	int8_t text_x_position;
	int8_t text_y_position;
	std::array<BlackboardWord, 16> words;

	void read_from(MFCReader& in) {
		CompoundObjectV1::read_from(in);
		background_color = in.read8();
		chalk_color = in.read8();
		alias_color = in.read8();
		text_x_position = in.reads8();
		text_y_position = in.reads8();
		for (size_t i = 0; i < 16; ++i) {
			words[i].value = in.read32le();
			words[i].text = in.read_ascii_nullterminated(11);
		}
	}
};

struct CallButtonV1;

struct LiftV1 : VehicleV1 {
	struct LiftFloor {
		uint32_t y;
		CallButtonV1* call_button;
	};

	uint32_t num_floors;
	uint32_t next_or_current_floor;
	int32_t current_call_button;
	uint8_t delay_counter;
	std::array<LiftFloor, 8> floors;

	void read_from(MFCReader& in) {
		VehicleV1::read_from(in);
		num_floors = in.read32le();
		next_or_current_floor = in.read32le();
		current_call_button = in.reads32le();
		delay_counter = in.read8();
		for (size_t i = 0; i < 8; ++i) {
			floors[i].y = in.read32le();
			floors[i].call_button = in.read_type<CallButtonV1>();
		}
	}
};

struct CallButtonV1 : SimpleObjectV1 {
	LiftV1* lift;
	uint8_t button_id;

	void read_from(MFCReader& in) {
		SimpleObjectV1::read_from(in);
		lift = in.read_type<LiftV1>();
		button_id = in.read8();
	}
};

struct SceneryV1 : ObjectV1 {
	EntityV1* part;

	void read_from(MFCReader& in) {
		ObjectV1::read_from(in);
		part = in.read_type<EntityV1>();
	}
};

struct MacroV1 {
	uint32_t selfdestruct;
	uint32_t inst;
	uint32_t script_length_maybe;
	std::string script;
	uint32_t ip;
	std::array<int32_t, 20> stack;
	uint32_t sp;
	std::array<int32_t, 10> vars;
	ObjectV1* ownr;
	ObjectV1* from;
	ObjectV1* exec;
	ObjectV1* targ;
	ObjectV1* _it_;
	int32_t part;
	std::string subroutine_label;
	uint32_t subroutine_address;
	int32_t wait;

	void read_from(MFCReader& in) {
		selfdestruct = in.read32le();
		inst = in.read32le();
		script_length_maybe = in.read32le();
		script = in.read_ascii_mfcstring();
		ip = in.read32le();
		for (size_t i = 0; i < 20; ++i) {
			stack[i] = in.reads32le();
		}
		sp = in.read32le();
		for (size_t i = 0; i < 10; ++i) {
			vars[i] = in.reads32le();
		}
		ownr = in.read_type<ObjectV1>();
		from = in.read_type<ObjectV1>();
		exec = in.read_type<ObjectV1>();
		targ = in.read_type<ObjectV1>();
		_it_ = in.read_type<ObjectV1>();
		part = in.reads32le();
		std::string subroutine_label = in.read_ascii(4);
		subroutine_address = in.read32le();
		wait = in.reads32le();
	}
};

struct CreatureV1 {
	// not implemented
};

struct FavoritePlaceV1 {
	std::string name;
	int16_t x;
	int16_t y;
};

struct SFCFile {
	MapDataV1* map;
	std::vector<ObjectV1*> objects;
	std::vector<SceneryV1*> sceneries;
	std::vector<ScriptV1> scripts;

	int32_t scrollx;
	int32_t scrolly;

	CreatureV1* current_norn;
	std::vector<FavoritePlaceV1> favorite_places;
	std::vector<std::string> speech_history;
	std::vector<MacroV1*> macros;
	std::vector<ObjectV1*> death_row;
	std::vector<ObjectV1*> events;

	uint32_t current_score;
	uint32_t current_health;
	uint32_t hatchery_eggs;
	uint32_t natural_eggs;
	uint32_t dead_norns;
	uint32_t live_norns;
	uint32_t breeders_score;
	uint32_t tick;

	std::vector<CreatureV1*> stuffed_norns;

	std::vector<std::unique_ptr<MFCObject>> mfc_objects;
};

SFCFile read_sfc_v1_file(std::istream& in);
SFCFile read_sfc_v1_file(const std::string& path);

} // namespace sfc
