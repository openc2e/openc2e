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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar.size_u32(images);
		ar.ascii_dword(filename);
		ar(first_sprite);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(unused_is_wrappable);
		ar(time_of_day);
		ar(background);

		ar.size_u32(rooms);
		for (auto& room : rooms) {
			ar(room.left);
			ar(room.top);
			ar(room.right);
			ar(room.bottom);
			ar(room.type);
		}
		for (auto& g : groundlevel) {
			ar(g);
		}
		for (auto& b : bacteria) {
			ar(b.state);
			ar(b.antigen);
			ar(b.fatal_level);
			ar(b.infect_level);
			ar(b.toxin1);
			ar(b.toxin2);
			ar(b.toxin3);
			ar(b.toxin4);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(sprite);
		ar(current_sprite);
		ar(image_offset);
		ar(z_order);
		ar(x);
		ar(y);
		ar(has_animation);
		if (has_animation) {
			ar(animation_frame);
			// TODO: Assert real animation string?
			ar.ascii_nullterminated(animation_string, 32);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(eventno);
		ar(species);
		ar(genus);
		ar(family);
		ar.ascii_mfcstring(text);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		uint8_t unused_eventno = 0;
		ar(unused_eventno);

		ar(species);
		ar(genus);
		ar(family);
		ar(movement_status);
		ar(attr);
		ar(limit_left);
		ar(limit_top);
		ar(limit_right);
		ar(limit_bottom);
		ar(carrier);
		ar(actv);
		ar(sprite);
		ar(tick_value);
		ar(ticks_since_last_tick_event);
		ar(objp);
		ar.ascii_dword(current_sound);
		ar(obv0);
		ar(obv1);
		ar(obv2);
		ar.size_u32(scripts);
		for (auto& s : scripts) {
			s.serialize(ar);
		}
	}
};

struct SimpleObjectV1 : ObjectV1 {
	EntityV1* part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	template <typename Archive>
	void serialize(Archive& ar) {
		ObjectV1::serialize(ar);

		ar(part);
		ar(z_order);
		ar(click_bhvr[0]);
		ar(click_bhvr[1]);
		ar(click_bhvr[2]);
		ar(touch_bhvr);
	}
};

struct BubbleV1 : SimpleObjectV1 {
	uint8_t life;
	ObjectV1* creator;
	std::string text;

	template <typename Archive>
	void serialize(Archive& ar) {
		SimpleObjectV1::serialize(ar);

		ar(life);
		ar(creator); // ???
		ar.ascii_nullterminated(text, 25);
	}
};

struct PointerToolV1 : SimpleObjectV1 {
	int32_t relx;
	int32_t rely;
	BubbleV1* bubble;
	std::string text;

	template <typename Archive>
	void serialize(Archive& ar) {
		SimpleObjectV1::serialize(ar);

		ar(relx);
		ar(rely);
		ar(bubble);
		ar.ascii_nullterminated(text, 25);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ObjectV1::serialize(ar);

		ar.size_u32(parts);
		for (auto& part : parts) {
			ar(part.entity);
			if (part.entity == nullptr) {
				throw Exception("whoops, entity is null");
			}
			ar(part.x);
			ar(part.y);
		}
		for (auto& h : hotspots) {
			ar(h.left);
			ar(h.top);
			ar(h.right);
			ar(h.bottom);
		}
		for (auto& f : functions_to_hotspots) {
			ar(f);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		CompoundObjectV1::serialize(ar);
		ar(xvel_times_256);
		ar(yvel_times_256);
		ar(x_times_256);
		ar(y_times_256);
		ar(cabin_left);
		ar(cabin_top);
		ar(cabin_right);
		ar(cabin_bottom);
		ar(bump);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		CompoundObjectV1::serialize(ar);
		ar(background_color);
		ar(chalk_color);
		ar(alias_color);
		ar(text_x_position);
		ar(text_y_position);
		for (auto& w : words) {
			ar(w.value);
			ar.ascii_nullterminated(w.text, 11);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		VehicleV1::serialize(ar);
		ar(num_floors);
		ar(next_or_current_floor);
		ar(current_call_button);
		ar(delay_counter);
		for (auto& f : floors) {
			ar(f.y);
			ar(f.call_button);
		}
	}
};

struct CallButtonV1 : SimpleObjectV1 {
	LiftV1* lift;
	uint8_t floor;

	template <typename Archive>
	void serialize(Archive& ar) {
		SimpleObjectV1::serialize(ar);
		ar(lift);
		ar(floor);
	}
};

struct SceneryV1 : ObjectV1 {
	EntityV1* part;

	template <typename Archive>
	void serialize(Archive& ar) {
		ObjectV1::serialize(ar);
		ar(part);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(selfdestruct);
		ar(inst);
		ar(script_length_maybe);
		ar.ascii_mfcstring(script);
		ar(ip);
		for (auto& s : stack) {
			ar(s);
		}
		ar(sp);
		for (auto& v : vars) {
			ar(v);
		}
		ar(ownr);
		ar(from);
		ar(exec);
		ar(targ);
		ar(_it_);
		ar(part);
		ar.ascii_dword(subroutine_label);
		ar(subroutine_address);
		ar(wait);
	}
};

struct BodyPartV1 : EntityV1 {
	uint32_t angle;
	uint32_t view;

	template <typename Archive>
	void serialize(Archive& ar) {
		EntityV1::serialize(ar);
		ar(angle);
		ar(view);
	}
};

struct PositionU8 {
	uint8_t x;
	uint8_t y;
};

struct BodyV1 : BodyPartV1 {
	std::array<std::array<PositionU8, 10>, 6> body_data;

	template <typename Archive>
	void serialize(Archive& ar) {
		BodyPartV1::serialize(ar);
		for (auto& pose : body_data) {
			for (auto& coords : pose) {
				ar(coords.x);
				ar(coords.y);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		BodyPartV1::serialize(ar);
		for (auto& l : limb_data) {
			ar(l.startx);
			ar(l.starty);
			ar(l.endx);
			ar(l.endy);
		}
		ar(next);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(source_lobe);
		ar(min);
		ar(max);
		ar(spread);
		ar(fanout);
		ar(min_ltw);
		ar(max_ltw);
		ar(min_strength);
		ar(max_strength);
		ar(migration);
		ar(relax_suscept);
		ar(relax_stw);
		ar(ltw_gain_rate);
		ar(gain_rate);
		ar(lose_rate);
		for (auto& i : gain_svrule) {
			ar(i);
		}
		for (auto& i : lose_svrule) {
			ar(i);
		}
		for (auto& i : suscept_svrule) {
			ar(i);
		}
		for (auto& i : reinforce_svrule) {
			ar(i);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(x);
		ar(y);
		ar(width);
		ar(height);
		ar(perceptible);
		ar(activity);
		for (auto& c : chemicals) {
			ar(c);
		}
		ar(threshold);
		ar(leakage);
		ar(reststate);
		ar(inputgain);
		for (auto& sv : svrule) {
			ar(sv);
		}
		ar(flags);
		d0_type.serialize(ar);
		d1_type.serialize(ar);
		ar(num_neurons);
		ar(num_dendrites);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(source_index);
		ar(home_x);
		ar(home_y);
		ar(suscept);
		ar(stw);
		ar(ltw);
		ar(strength);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(x);
		ar(y);
		ar(output);
		ar(state);
		ar(wta_disable);
		ar(exclusive);
		ar.size_u8(dendrites0);
		ar(dendrite0_index);
		for (auto& d : dendrites0) {
			d.serialize(ar);
		}
		ar.size_u8(dendrites1);
		ar(dendrite1_index);
		for (auto& d : dendrites1) {
			d.serialize(ar);
		}
	}
};

struct CBrainV1 {
	std::vector<LobeV1> lobes;
	std::vector<NeuronV1> neurons;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar.size_u32(lobes);
		for (auto& l : lobes) {
			l.serialize(ar);
			neurons.resize(neurons.size() + l.num_neurons);
		}
		for (auto& n : neurons) {
			n.serialize(ar);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(organ);
		ar(tissue);
		ar(locus);
		ar(chemical);
		ar(threshold);
		ar(rate);
		ar(gain);
		ar(effect);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(organ);
		ar(tissue);
		ar(locus);
		ar(chemical);
		ar(threshold);
		ar(nominal);
		ar(gain);
		ar(effect);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(r1_amount);
		ar(r1_chem);
		ar(r2_amount);
		ar(r2_chem);
		ar(rate);
		ar(p1_amount);
		ar(p1_chem);
		ar(p2_amount);
		ar(p2_chem);
	}
};

struct CBiochemistryV1 {
	CreatureV1* owner;
	std::array<ChemicalDataV1, 256> chemicals;
	std::vector<EmitterV1> emitters;
	std::vector<ReceptorV1> receptors;
	std::vector<ReactionV1> reactions;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(owner);
		ar.size_u32(emitters);
		ar.size_u32(receptors);
		ar.size_u32(reactions);
		for (auto& c : chemicals) {
			ar(c.concentration);
			ar(c.decay);
		}
		for (auto& e : emitters) {
			e.serialize(ar);
		}
		for (auto& r : receptors) {
			r.serialize(ar);
		}
		for (auto& r : reactions) {
			r.serialize(ar);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		for (auto& d : dendrites) {
			ar(d.lobe);
			ar(d.cell);
		}
		ar(motor_decision);
		ar(reinforcement_chemical);
		ar(reinforcement_amount);
		ar(phase);
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

	template <typename Archive>
	void serialize(Archive& ar) {
		ObjectV1::serialize(ar);
		ar.ascii_dword(moniker);
		ar.ascii_dword(mother);
		ar.ascii_dword(father);
		ar(body);
		ar(head);
		ar(left_thigh);
		ar(right_thigh);
		ar(left_arm);
		ar(right_arm);
		ar(tail);
		ar(direction);
		ar(downfoot);
		ar(footx);
		ar(footy);
		ar(z_order);
		ar.ascii_mfcstring(current_pose);
		ar(expression);
		ar(eyes_open);
		ar(asleep);
		for (auto& p : poses) {
			ar.ascii_mfcstring(p);
		}
		for (auto& g : gait_animations) {
			ar.ascii_mfcstring(g);
		}
		for (auto& v : vocabulary) {
			ar.ascii_mfcstring(v.in);
			ar.ascii_mfcstring(v.out);
			ar(v.strength);
		}
		for (auto& p : object_positions) {
			ar(p.x);
			ar(p.y);
		}
		for (auto& s : stimuli) {
			ar(s.significance);
			ar(s.input);
			ar(s.intensity);
			ar(s.flags);
			ar(s.chem0);
			ar(s.amount0);
			ar(s.chem1);
			ar(s.amount1);
			ar(s.chem2);
			ar(s.amount2);
			ar(s.chem3);
			ar(s.amount3);
		}

		ar(brain);
		ar(biochemistry);
		ar(sex);
		ar(age);
		ar(biotick);
		ar.ascii_dword(gamete);
		ar.ascii_dword(zygote);
		ar(dead);
		ar(age_ticks);
		ar.size_u32(instincts);
		ar(dreaming);
		for (auto& i : instincts) {
			ar(i);
		}
		for (auto& g : goals) {
			for (auto& drive : g) {
				ar(drive);
			}
		}
		ar(zzzz);
		for (auto& v : voices_lookup) {
			for (auto& s : v) {
				// TODO: is this the right order? how does voice data work again?
				ar(s);
			}
		}
		for (auto& v : voices) {
			ar.ascii_dword(v.name);
			ar(v.delay_ticks);
		}

		// these monikers are super weird.. they're MFC strings that are the
		// reversed and hex-encoded version of the original string characters
		ar.ascii_mfcstring(history_moniker);
		ar.ascii_mfcstring(history_name);
		ar.ascii_mfcstring(history_moms_moniker);
		ar.ascii_mfcstring(history_dads_moniker);
		ar.ascii_mfcstring(history_birthday);
		ar.ascii_mfcstring(history_birthplace);
		ar.ascii_mfcstring(history_owner_name);
		ar.ascii_mfcstring(history_owner_phone);
		ar.ascii_mfcstring(history_owner_address);
		ar.ascii_mfcstring(history_owner_email);
	}
};

struct CGenomeV1 {
	std::string moniker;
	uint32_t sex;
	uint8_t life_stage;
	std::vector<uint8_t> data;
	// TODO: should we actually parse this? sometimes it has weird extra data
	// that doesn't all get parsed, might be useful to keep it around.

	template <typename Archive>
	void serialize(Archive& ar) {
		ar.size_u32(data);
		ar.ascii_dword(moniker);
		ar(sex);
		ar(life_stage);
		ar(data);
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
