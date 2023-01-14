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
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	void read_from(MFCReader& in) {
		ObjectV1::read_from(in);

		part = in.read_type<EntityV1>();
		z_order = in.reads32le();
		click_bhvr[0] = in.reads8();
		click_bhvr[1] = in.reads8();
		click_bhvr[2] = in.reads8();
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
		int32_t y;
		CallButtonV1* call_button;
	};

	int32_t num_floors;
	int32_t next_or_current_floor;
	int32_t current_call_button;
	uint8_t delay_counter;
	std::array<LiftFloor, 8> floors;

	void read_from(MFCReader& in) {
		VehicleV1::read_from(in);
		num_floors = in.reads32le();
		next_or_current_floor = in.reads32le();
		current_call_button = in.reads32le();
		delay_counter = in.read8();
		for (size_t i = 0; i < 8; ++i) {
			floors[i].y = in.reads32le();
			floors[i].call_button = in.read_type<CallButtonV1>();
		}
	}
};

struct CallButtonV1 : SimpleObjectV1 {
	LiftV1* lift;
	uint8_t floor;

	void read_from(MFCReader& in) {
		SimpleObjectV1::read_from(in);
		lift = in.read_type<LiftV1>();
		floor = in.read8();
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
		subroutine_label = in.read_ascii(4);
		subroutine_address = in.read32le();
		wait = in.reads32le();
	}
};

struct BodyPartV1 : EntityV1 {
	uint32_t angle;
	uint32_t view;

	void read_from(MFCReader& in) {
		EntityV1::read_from(in);
		angle = in.read32le();
		view = in.read32le();
	}
};

struct PositionU8 {
	uint8_t x;
	uint8_t y;
};

struct BodyV1 : BodyPartV1 {
	std::array<std::array<PositionU8, 10>, 6> body_data;

	void read_from(MFCReader& in) {
		BodyPartV1::read_from(in);
		for (auto& pose : body_data) {
			for (auto& coords : pose) {
				coords.x = in.read8();
				coords.y = in.read8();
			}
		}
	}
};

struct LimbData {
	uint8_t startx;
	uint8_t starty;
	uint8_t endx;
	uint8_t endy;
};

struct LimbV1 : BodyPartV1 {
	std::array<LimbData, 10> limb_data;
	LimbV1* next = nullptr;

	void read_from(MFCReader& in) {
		BodyPartV1::read_from(in);
		for (auto& l : limb_data) {
			l.startx = in.read8();
			l.starty = in.read8();
			l.endx = in.read8();
			l.endy = in.read8();
		}
		next = in.read_type<LimbV1>();
	}
};

struct VocabWordV1 {
	// not CArchive serialized
	std::string in;
	std::string out;
	uint32_t strength;
};

struct PositionV1 {
	// not CArchive serialized
	int32_t x;
	int32_t y;
};

struct StimulusV1 {
	// not CArchive serialized
	uint8_t significance;
	uint8_t input;
	uint8_t intensity;
	uint8_t flags;
	uint8_t chem0;
	uint8_t amount0;
	uint8_t chem1;
	uint8_t amount1;
	uint8_t chem2;
	uint8_t amount2;
	uint8_t chem3;
	uint8_t amount3;
};

struct DendriteTypeV1 {
	// not CArchive serialized
	uint32_t source_lobe;
	uint8_t min;
	uint8_t max;
	uint8_t spread;
	uint8_t fanout;
	uint8_t min_ltw;
	uint8_t max_ltw;
	uint8_t min_strength;
	uint8_t max_strength;
	uint8_t migration;
	uint8_t relax_suscept;
	uint8_t relax_stw;
	uint8_t ltw_gain_rate;
	uint8_t gain_rate;
	uint8_t lose_rate;
	std::array<uint8_t, 10> gain_svrule;
	std::array<uint8_t, 10> lose_svrule;
	std::array<uint8_t, 10> suscept_svrule;
	std::array<uint8_t, 10> reinforce_svrule;

	void read_from(MFCReader& in) {
		source_lobe = in.read32le();
		min = in.read8();
		max = in.read8();
		spread = in.read8();
		fanout = in.read8();
		min_ltw = in.read8();
		max_ltw = in.read8();
		min_strength = in.read8();
		max_strength = in.read8();
		migration = in.read8();
		relax_suscept = in.read8();
		relax_stw = in.read8();
		ltw_gain_rate = in.read8();
		gain_rate = in.read8();
		lose_rate = in.read8();
		for (auto& i : gain_svrule) {
			i = in.read8();
		}
		for (auto& i : lose_svrule) {
			i = in.read8();
		}
		for (auto& i : suscept_svrule) {
			i = in.read8();
		}
		for (auto& i : reinforce_svrule) {
			i = in.read8();
		}
	}
};

struct LobeV1 {
	// not CArchive serialized
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t perceptible;
	uint8_t activity;
	std::array<uint8_t, 4> chemicals;
	uint8_t threshold;
	uint8_t leakage;
	uint8_t reststate;
	uint8_t inputgain;
	std::array<uint8_t, 10> svrule;
	uint8_t flags;
	DendriteTypeV1 d0_type;
	DendriteTypeV1 d1_type;
	uint32_t num_neurons;
	uint32_t num_dendrites;

	void read_from(MFCReader& in) {
		x = in.read32le();
		y = in.read32le();
		width = in.read32le();
		height = in.read32le();
		perceptible = in.read32le();
		activity = in.read8();
		for (auto& c : chemicals) {
			c = in.read8();
		}
		threshold = in.read8();
		leakage = in.read8();
		reststate = in.read8();
		inputgain = in.read8();
		for (auto& sv : svrule) {
			sv = in.read8();
		}
		flags = in.read8();
		d0_type.read_from(in);
		d1_type.read_from(in);
		num_neurons = in.read32le();
		num_dendrites = in.read32le();
	}
};

struct DendriteV1 {
	// not CArchive serialized
	uint32_t source_index;
	uint8_t home_x;
	uint8_t home_y;
	uint8_t suscept;
	uint8_t stw;
	uint8_t ltw;
	uint8_t strength;

	void read_from(MFCReader& in) {
		source_index = in.read32le();
		home_x = in.read8();
		home_y = in.read8();
		suscept = in.read8();
		stw = in.read8();
		ltw = in.read8();
		strength = in.read8();
	}
};

struct NeuronV1 {
	// not CArchive serialized
	uint8_t x;
	uint8_t y;
	uint8_t output;
	uint8_t state;
	uint8_t wta_disable;
	uint8_t exclusive;
	uint32_t dendrite0_index;
	std::vector<DendriteV1> dendrites0;
	uint32_t dendrite1_index;
	std::vector<DendriteV1> dendrites1;

	void read_from(MFCReader& in) {
		x = in.read8();
		y = in.read8();
		output = in.read8();
		state = in.read8();
		wta_disable = in.read8();
		exclusive = in.read8();
		uint8_t dendrites0_size = in.read8();
		dendrite0_index = in.read32le();
		dendrites0.resize(dendrites0_size);
		for (auto& d : dendrites0) {
			d.read_from(in);
		}
		uint8_t dendrites1_size = in.read8();
		dendrite1_index = in.read32le();
		dendrites1.resize(dendrites1_size);
		for (auto& d : dendrites1) {
			d.read_from(in);
		}
	}
};

struct CBrainV1 {
	std::vector<LobeV1> lobes;
	std::vector<NeuronV1> neurons;

	void read_from(MFCReader& in) {
		uint32_t num_lobes = in.read32le();
		lobes.resize(num_lobes);
		for (auto& l : lobes) {
			l.read_from(in);
			neurons.resize(neurons.size() + l.num_neurons);
		}
		for (auto& n : neurons) {
			n.read_from(in);
		}
	}
};

struct CreatureV1;

struct ChemicalDataV1 {
	// not CArchive serialized
	uint8_t concentration;
	uint8_t decay;
};

struct EmitterV1 {
	// not CArchive serialized
	uint8_t organ;
	uint8_t tissue;
	uint8_t locus;
	uint8_t chemical;
	uint8_t threshold;
	uint8_t rate;
	uint8_t gain;
	uint8_t effect;

	void read_from(MFCReader& in) {
		organ = in.read8();
		tissue = in.read8();
		locus = in.read8();
		chemical = in.read8();
		threshold = in.read8();
		rate = in.read8();
		gain = in.read8();
		effect = in.read8();
	}
};

struct ReceptorV1 {
	// not CArchive serialized
	uint8_t organ;
	uint8_t tissue;
	uint8_t locus;
	uint8_t chemical;
	uint8_t threshold;
	uint8_t nominal;
	uint8_t gain;
	uint8_t effect;

	void read_from(MFCReader& in) {
		organ = in.read8();
		tissue = in.read8();
		locus = in.read8();
		chemical = in.read8();
		threshold = in.read8();
		nominal = in.read8();
		gain = in.read8();
		effect = in.read8();
	}
};

struct ReactionV1 {
	// not CArchive serialized
	uint8_t r1_amount;
	uint8_t r1_chem;
	uint8_t r2_amount;
	uint8_t r2_chem;
	uint8_t rate;
	uint8_t p1_amount;
	uint8_t p1_chem;
	uint8_t p2_amount;
	uint8_t p2_chem;

	void read_from(MFCReader& in) {
		r1_amount = in.read8();
		r1_chem = in.read8();
		r2_amount = in.read8();
		r2_chem = in.read8();
		rate = in.read8();
		p1_amount = in.read8();
		p1_chem = in.read8();
		p2_amount = in.read8();
		p2_chem = in.read8();
	}
};

struct CBiochemistryV1 {
	CreatureV1* owner;
	std::array<ChemicalDataV1, 256> chemicals;
	std::vector<EmitterV1> emitters;
	std::vector<ReceptorV1> receptors;
	std::vector<ReactionV1> reactions;

	void read_from(MFCReader& in) {
		owner = in.read_type<CreatureV1>();
		emitters.resize(in.read32le());
		receptors.resize(in.read32le());
		reactions.resize(in.read32le());
		for (auto& c : chemicals) {
			c.concentration = in.read8();
			c.decay = in.read8();
		}
		for (auto& e : emitters) {
			e.read_from(in);
		}
		for (auto& r : receptors) {
			r.read_from(in);
		}
		for (auto& r : reactions) {
			r.read_from(in);
		}
	}
};

struct InstinctDendriteV1 {
	// not CArchive serialized
	uint32_t lobe;
	uint32_t cell;
};

struct CInstinctV1 {
	std::array<InstinctDendriteV1, 3> dendrites;
	uint32_t motor_decision;
	uint32_t reinforcement_chemical;
	uint32_t reinforcement_amount;
	uint32_t phase;

	void read_from(MFCReader& in) {
		for (auto& d : dendrites) {
			d.lobe = in.read32le();
			d.cell = in.read32le();
		}
		motor_decision = in.read32le();
		reinforcement_chemical = in.read32le();
		reinforcement_amount = in.read32le();
		phase = in.read32le();
	}
};

struct VoiceV1 {
	// not CArchive serialized
	std::string name;
	uint32_t delay_ticks;
};

struct CreatureV1 : ObjectV1 {
	std::string moniker;
	std::string mother;
	std::string father;
	BodyV1* body;
	LimbV1* head;
	LimbV1* left_thigh;
	LimbV1* right_thigh;
	LimbV1* left_arm;
	LimbV1* right_arm;
	LimbV1* tail;
	uint8_t direction;
	uint8_t downfoot;
	uint32_t footx;
	uint32_t footy;
	uint32_t z_order;
	std::string current_pose;
	uint8_t expression;
	uint8_t eyes_open;
	uint8_t asleep;
	std::array<std::string, 100> poses;
	std::array<std::string, 8> gait_animations;
	std::array<VocabWordV1, 80> vocabulary;
	std::array<PositionV1, 40> object_positions;
	std::array<StimulusV1, 36> stimuli;
	CBrainV1* brain;
	CBiochemistryV1* biochemistry;
	uint8_t sex;
	uint8_t age;
	uint32_t biotick;
	std::string gamete;
	std::string zygote;
	uint8_t dead;
	uint32_t age_ticks;
	uint32_t dreaming;
	std::vector<CInstinctV1*> instincts;
	std::array<std::array<uint32_t, 16>, 40> goals;
	SimpleObjectV1* zzzz;
	std::array<std::array<uint32_t, 3>, 27> voices_lookup;
	std::array<VoiceV1, 32> voices;
	std::string history_moniker;
	std::string history_name;
	std::string history_moms_moniker;
	std::string history_dads_moniker;
	std::string history_birthday;
	std::string history_birthplace;
	std::string history_owner_name;
	std::string history_owner_phone;
	std::string history_owner_address;
	std::string history_owner_email;

	void read_from(MFCReader& in) {
		ObjectV1::read_from(in);
		moniker = in.read_ascii(4);
		mother = in.read_ascii(4);
		father = in.read_ascii(4);
		body = in.read_type<BodyV1>();
		head = in.read_type<LimbV1>();
		left_thigh = in.read_type<LimbV1>();
		right_thigh = in.read_type<LimbV1>();
		left_arm = in.read_type<LimbV1>();
		right_arm = in.read_type<LimbV1>();
		tail = in.read_type<LimbV1>();
		direction = in.read8();
		downfoot = in.read8();
		footx = in.read32le();
		footy = in.read32le();
		z_order = in.read32le();
		current_pose = in.read_ascii_mfcstring();
		expression = in.read8();
		eyes_open = in.read8();
		asleep = in.read8();
		for (auto& p : poses) {
			p = in.read_ascii_mfcstring();
		}
		for (auto& g : gait_animations) {
			g = in.read_ascii_mfcstring();
		}
		for (auto& v : vocabulary) {
			v.in = in.read_ascii_mfcstring();
			v.out = in.read_ascii_mfcstring();
			v.strength = in.read32le();
		}
		for (auto& p : object_positions) {
			p.x = in.reads32le();
			p.y = in.reads32le();
		}
		for (auto& s : stimuli) {
			s.significance = in.read8();
			s.input = in.read8();
			s.intensity = in.read8();
			s.flags = in.read8();
			s.chem0 = in.read8();
			s.amount0 = in.read8();
			s.chem1 = in.read8();
			s.amount1 = in.read8();
			s.chem2 = in.read8();
			s.amount2 = in.read8();
			s.chem3 = in.read8();
			s.amount3 = in.read8();
		}

		brain = in.read_type<CBrainV1>();
		biochemistry = in.read_type<CBiochemistryV1>();
		sex = in.read8();
		age = in.read8();
		biotick = in.read32le();
		gamete = in.read_ascii(4);
		zygote = in.read_ascii(4);
		dead = in.read8();
		age_ticks = in.read32le();
		uint32_t num_instincts = in.read32le();
		instincts.resize(num_instincts);
		dreaming = in.read32le();
		for (auto& i : instincts) {
			i = in.read_type<CInstinctV1>();
		}
		for (auto& g : goals) {
			for (auto& drive : g) {
				drive = in.read32le();
			}
		}
		zzzz = in.read_type<SimpleObjectV1>();
		for (auto& v : voices_lookup) {
			for (auto& s : v) {
				// TODO: is this the right order? how does voice data work again?
				s = in.read32le();
			}
		}
		for (auto& v : voices) {
			v.name = in.read_ascii(4);
			v.delay_ticks = in.read32le();
		}

		// these monikers are super weird.. they're MFC strings that are the
		// reversed and hex-encoded version of the original string characters
		history_moniker = in.read_ascii_mfcstring();
		history_name = in.read_ascii_mfcstring();
		history_moms_moniker = in.read_ascii_mfcstring();
		history_dads_moniker = in.read_ascii_mfcstring();
		history_birthday = in.read_ascii_mfcstring();
		history_birthplace = in.read_ascii_mfcstring();
		history_owner_name = in.read_ascii_mfcstring();
		history_owner_phone = in.read_ascii_mfcstring();
		history_owner_address = in.read_ascii_mfcstring();
		history_owner_email = in.read_ascii_mfcstring();
	}
};

struct CGenomeV1 {
	std::string moniker;
	uint32_t sex;
	uint8_t life_stage;
	std::vector<uint8_t> data;
	// TODO: should we actually parse this? sometimes it has weird extra data
	// that doesn't all get parsed, might be useful to keep it around.

	void read_from(MFCReader& in) {
		uint32_t data_length = in.read32le();
		moniker = in.read_ascii(4);
		sex = in.read32le();
		life_stage = in.read8();
		data.resize(data_length);
		in.read_exact(data.data(), data_length);
	}
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

struct EXPFile {
	CreatureV1* creature;
	CGenomeV1* genome;
	CGenomeV1* child_genome;

	std::vector<std::unique_ptr<MFCObject>> mfc_objects;
};

SFCFile read_sfc_v1_file(std::istream& in);
SFCFile read_sfc_v1_file(const std::string& path);

EXPFile read_exp_v1_file(std::istream& in);
EXPFile read_exp_v1_file(const std::string& path);

} // namespace sfc
