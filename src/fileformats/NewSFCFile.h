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

	void serialize(MFCReader& in) {
		in.size_u32(images);
		in.ascii_dword(filename);
		in(first_sprite);
		in(refcount);

		for (auto& image : images) {
			in(image.parent);
			in(image.status);
			in(image.width);
			in(image.height);
			in(image.offset);
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

	void serialize(MFCReader& in) {
		in(unused_is_wrappable);
		in(time_of_day);
		in(background);

		in.size_u32(rooms);
		for (auto& room : rooms) {
			in(room.left);
			in(room.top);
			in(room.right);
			in(room.bottom);
			in(room.type);
		}
		for (auto& g : groundlevel) {
			in(g);
		}
		for (auto& b : bacteria) {
			in(b.state);
			in(b.antigen);
			in(b.fatal_level);
			in(b.infect_level);
			in(b.toxin1);
			in(b.toxin2);
			in(b.toxin3);
			in(b.toxin4);
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

	void serialize(MFCReader& in) {
		in(sprite);
		in(current_sprite);
		in(image_offset);
		in(z_order);
		in(x);
		in(y);
		in(has_animation);
		if (has_animation) {
			in(animation_frame);
			// TODO: Assert real animation string?
			in.ascii_nullterminated(animation_string, 32);
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

	void serialize(MFCReader& in) {
		in(eventno);
		in(species);
		in(genus);
		in(family);
		in.ascii_mfcstring(text);
	}
};


struct ObjectV1 {
	virtual ~ObjectV1() {}

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

	void serialize(MFCReader& in) {
		uint8_t unused_eventno = 0;
		in(unused_eventno);

		in(species);
		in(genus);
		in(family);
		in(movement_status);
		in(attr);
		in(limit_left);
		in(limit_top);
		in(limit_right);
		in(limit_bottom);
		in(carrier);
		in(actv);
		in(sprite);
		in(tick_value);
		in(ticks_since_last_tick_event);
		in(objp);
		in.ascii_dword(current_sound);
		in(obv0);
		in(obv1);
		in(obv2);
		in.size_u32(scripts);
		for (auto& s : scripts) {
			s.serialize(in);
		}
	}
};

struct SimpleObjectV1 : ObjectV1 {
	EntityV1* part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	void serialize(MFCReader& in) {
		ObjectV1::serialize(in);

		in(part);
		in(z_order);
		in(click_bhvr[0]);
		in(click_bhvr[1]);
		in(click_bhvr[2]);
		in(touch_bhvr);
	}
};

struct BubbleV1 : SimpleObjectV1 {
	uint8_t life;
	ObjectV1* creator;
	std::string text;

	void serialize(MFCReader& in) {
		SimpleObjectV1::serialize(in);

		in(life);
		in(creator); // ???
		in.ascii_nullterminated(text, 25);
	}
};

struct PointerToolV1 : SimpleObjectV1 {
	int32_t relx;
	int32_t rely;
	BubbleV1* bubble;
	std::string text;

	void serialize(MFCReader& in) {
		SimpleObjectV1::serialize(in);

		in(relx);
		in(rely);
		in(bubble);
		in.ascii_nullterminated(text, 25);
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

	void serialize(MFCReader& in) {
		ObjectV1::serialize(in);

		in.size_u32(parts);
		for (auto& part : parts) {
			in(part.entity);
			if (part.entity == nullptr) {
				throw Exception("whoops, entity is null");
			}
			in(part.x);
			in(part.y);
		}
		for (auto& h : hotspots) {
			in(h.left);
			in(h.top);
			in(h.right);
			in(h.bottom);
		}
		for (auto& f : functions_to_hotspots) {
			in(f);
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

	void serialize(MFCReader& in) {
		CompoundObjectV1::serialize(in);
		in(xvel_times_256);
		in(yvel_times_256);
		in(x_times_256);
		in(y_times_256);
		in(cabin_left);
		in(cabin_top);
		in(cabin_right);
		in(cabin_bottom);
		in(bump);
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

	void serialize(MFCReader& in) {
		CompoundObjectV1::serialize(in);
		in(background_color);
		in(chalk_color);
		in(alias_color);
		in(text_x_position);
		in(text_y_position);
		for (auto& w : words) {
			in(w.value);
			in.ascii_nullterminated(w.text, 11);
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

	void serialize(MFCReader& in) {
		VehicleV1::serialize(in);
		in(num_floors);
		in(next_or_current_floor);
		in(current_call_button);
		in(delay_counter);
		for (auto& f : floors) {
			in(f.y);
			in(f.call_button);
		}
	}
};

struct CallButtonV1 : SimpleObjectV1 {
	LiftV1* lift;
	uint8_t floor;

	void serialize(MFCReader& in) {
		SimpleObjectV1::serialize(in);
		in(lift);
		in(floor);
	}
};

struct SceneryV1 : ObjectV1 {
	EntityV1* part;

	void serialize(MFCReader& in) {
		ObjectV1::serialize(in);
		in(part);
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

	void serialize(MFCReader& in) {
		in(selfdestruct);
		in(inst);
		in(script_length_maybe);
		in.ascii_mfcstring(script);
		in(ip);
		for (auto& s : stack) {
			in(s);
		}
		in(sp);
		for (auto& v : vars) {
			in(v);
		}
		in(ownr);
		in(from);
		in(exec);
		in(targ);
		in(_it_);
		in(part);
		in.ascii_dword(subroutine_label);
		in(subroutine_address);
		in(wait);
	}
};

struct BodyPartV1 : EntityV1 {
	uint32_t angle;
	uint32_t view;

	void serialize(MFCReader& in) {
		EntityV1::serialize(in);
		in(angle);
		in(view);
	}
};

struct PositionU8 {
	uint8_t x;
	uint8_t y;
};

struct BodyV1 : BodyPartV1 {
	std::array<std::array<PositionU8, 10>, 6> body_data;

	void serialize(MFCReader& in) {
		BodyPartV1::serialize(in);
		for (auto& pose : body_data) {
			for (auto& coords : pose) {
				in(coords.x);
				in(coords.y);
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

	void serialize(MFCReader& in) {
		BodyPartV1::serialize(in);
		for (auto& l : limb_data) {
			in(l.startx);
			in(l.starty);
			in(l.endx);
			in(l.endy);
		}
		in(next);
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

	void serialize(MFCReader& in) {
		in(source_lobe);
		in(min);
		in(max);
		in(spread);
		in(fanout);
		in(min_ltw);
		in(max_ltw);
		in(min_strength);
		in(max_strength);
		in(migration);
		in(relax_suscept);
		in(relax_stw);
		in(ltw_gain_rate);
		in(gain_rate);
		in(lose_rate);
		for (auto& i : gain_svrule) {
			in(i);
		}
		for (auto& i : lose_svrule) {
			in(i);
		}
		for (auto& i : suscept_svrule) {
			in(i);
		}
		for (auto& i : reinforce_svrule) {
			in(i);
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

	void serialize(MFCReader& in) {
		in(x);
		in(y);
		in(width);
		in(height);
		in(perceptible);
		in(activity);
		for (auto& c : chemicals) {
			in(c);
		}
		in(threshold);
		in(leakage);
		in(reststate);
		in(inputgain);
		for (auto& sv : svrule) {
			in(sv);
		}
		in(flags);
		d0_type.serialize(in);
		d1_type.serialize(in);
		in(num_neurons);
		in(num_dendrites);
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

	void serialize(MFCReader& in) {
		in(source_index);
		in(home_x);
		in(home_y);
		in(suscept);
		in(stw);
		in(ltw);
		in(strength);
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

	void serialize(MFCReader& in) {
		in(x);
		in(y);
		in(output);
		in(state);
		in(wta_disable);
		in(exclusive);
		in.size_u8(dendrites0);
		in(dendrite0_index);
		for (auto& d : dendrites0) {
			d.serialize(in);
		}
		in.size_u8(dendrites1);
		in(dendrite1_index);
		for (auto& d : dendrites1) {
			d.serialize(in);
		}
	}
};

struct CBrainV1 {
	std::vector<LobeV1> lobes;
	std::vector<NeuronV1> neurons;

	void serialize(MFCReader& in) {
		in.size_u32(lobes);
		for (auto& l : lobes) {
			l.serialize(in);
			neurons.resize(neurons.size() + l.num_neurons);
		}
		for (auto& n : neurons) {
			n.serialize(in);
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

	void serialize(MFCReader& in) {
		in(organ);
		in(tissue);
		in(locus);
		in(chemical);
		in(threshold);
		in(rate);
		in(gain);
		in(effect);
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

	void serialize(MFCReader& in) {
		in(organ);
		in(tissue);
		in(locus);
		in(chemical);
		in(threshold);
		in(nominal);
		in(gain);
		in(effect);
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

	void serialize(MFCReader& in) {
		in(r1_amount);
		in(r1_chem);
		in(r2_amount);
		in(r2_chem);
		in(rate);
		in(p1_amount);
		in(p1_chem);
		in(p2_amount);
		in(p2_chem);
	}
};

struct CBiochemistryV1 {
	CreatureV1* owner;
	std::array<ChemicalDataV1, 256> chemicals;
	std::vector<EmitterV1> emitters;
	std::vector<ReceptorV1> receptors;
	std::vector<ReactionV1> reactions;

	void serialize(MFCReader& in) {
		in(owner);
		in.size_u32(emitters);
		in.size_u32(receptors);
		in.size_u32(reactions);
		for (auto& c : chemicals) {
			in(c.concentration);
			in(c.decay);
		}
		for (auto& e : emitters) {
			e.serialize(in);
		}
		for (auto& r : receptors) {
			r.serialize(in);
		}
		for (auto& r : reactions) {
			r.serialize(in);
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

	void serialize(MFCReader& in) {
		for (auto& d : dendrites) {
			in(d.lobe);
			in(d.cell);
		}
		in(motor_decision);
		in(reinforcement_chemical);
		in(reinforcement_amount);
		in(phase);
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

	void serialize(MFCReader& in) {
		ObjectV1::serialize(in);
		in.ascii_dword(moniker);
		in.ascii_dword(mother);
		in.ascii_dword(father);
		in(body);
		in(head);
		in(left_thigh);
		in(right_thigh);
		in(left_arm);
		in(right_arm);
		in(tail);
		in(direction);
		in(downfoot);
		in(footx);
		in(footy);
		in(z_order);
		in.ascii_mfcstring(current_pose);
		in(expression);
		in(eyes_open);
		in(asleep);
		for (auto& p : poses) {
			in.ascii_mfcstring(p);
		}
		for (auto& g : gait_animations) {
			in.ascii_mfcstring(g);
		}
		for (auto& v : vocabulary) {
			in.ascii_mfcstring(v.in);
			in.ascii_mfcstring(v.out);
			in(v.strength);
		}
		for (auto& p : object_positions) {
			in(p.x);
			in(p.y);
		}
		for (auto& s : stimuli) {
			in(s.significance);
			in(s.input);
			in(s.intensity);
			in(s.flags);
			in(s.chem0);
			in(s.amount0);
			in(s.chem1);
			in(s.amount1);
			in(s.chem2);
			in(s.amount2);
			in(s.chem3);
			in(s.amount3);
		}

		in(brain);
		in(biochemistry);
		in(sex);
		in(age);
		in(biotick);
		in.ascii_dword(gamete);
		in.ascii_dword(zygote);
		in(dead);
		in(age_ticks);
		in.size_u32(instincts);
		in(dreaming);
		for (auto& i : instincts) {
			in(i);
		}
		for (auto& g : goals) {
			for (auto& drive : g) {
				in(drive);
			}
		}
		in(zzzz);
		for (auto& v : voices_lookup) {
			for (auto& s : v) {
				// TODO: is this the right order? how does voice data work again?
				in(s);
			}
		}
		for (auto& v : voices) {
			in.ascii_dword(v.name);
			in(v.delay_ticks);
		}

		// these monikers are super weird.. they're MFC strings that are the
		// reversed and hex-encoded version of the original string characters
		in.ascii_mfcstring(history_moniker);
		in.ascii_mfcstring(history_name);
		in.ascii_mfcstring(history_moms_moniker);
		in.ascii_mfcstring(history_dads_moniker);
		in.ascii_mfcstring(history_birthday);
		in.ascii_mfcstring(history_birthplace);
		in.ascii_mfcstring(history_owner_name);
		in.ascii_mfcstring(history_owner_phone);
		in.ascii_mfcstring(history_owner_address);
		in.ascii_mfcstring(history_owner_email);
	}
};

struct CGenomeV1 {
	std::string moniker;
	uint32_t sex;
	uint8_t life_stage;
	std::vector<uint8_t> data;
	// TODO: should we actually parse this? sometimes it has weird extra data
	// that doesn't all get parsed, might be useful to keep it around.

	void serialize(MFCReader& in) {
		in.size_u32(data);
		in.ascii_dword(moniker);
		in(sex);
		in(life_stage);
		in(data);
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
	std::array<FavoritePlaceV1, 6> favorite_places;
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
