#pragma once

#include "Object.h"

#include <array>
#include <memory>
#include <stdint.h>
#include <string>

namespace sfc {

struct BodyV1;
struct CBiochemistryV1;
struct CBrainV1;
struct CInstinctV1;
struct LimbV1;
struct SimpleObjectV1;

struct Vector2i {
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

struct VocabWordV1 {
	// not CArchive serialized
	std::string in;
	std::string out;
	uint32_t strength;
};

struct VoiceV1 {
	// not CArchive serialized
	std::array<uint8_t, 4> name; // either a four-character ASCII token, "\0\0\0\0", or "\0\0\0\1"
	uint32_t delay_ticks;
};

struct CreatureV1 : ObjectV1 {
	std::string moniker;
	std::string mother;
	std::string father;
	std::shared_ptr<BodyV1> body;
	std::shared_ptr<LimbV1> head;
	std::shared_ptr<LimbV1> left_thigh;
	std::shared_ptr<LimbV1> right_thigh;
	std::shared_ptr<LimbV1> left_arm;
	std::shared_ptr<LimbV1> right_arm;
	std::shared_ptr<LimbV1> tail;
	uint8_t direction;
	uint8_t downfoot_left;
	int32_t footx;
	int32_t footy;
	uint32_t z_order;
	std::string current_pose;
	uint8_t expression;
	uint8_t eyes_open;
	uint8_t asleep;
	std::array<std::string, 100> poses;
	std::array<std::string, 8> gait_animations;
	std::array<VocabWordV1, 80> vocabulary;
	std::array<Vector2i, 40> object_positions;
	std::array<StimulusV1, 36> stimuli;
	std::shared_ptr<CBrainV1> brain;
	std::shared_ptr<CBiochemistryV1> biochemistry;
	uint8_t sex;
	uint8_t age;
	uint32_t biotick;
	std::string gamete;
	std::string zygote;
	uint8_t dead;
	uint32_t age_ticks;
	uint32_t dreaming;
	std::vector<std::shared_ptr<CInstinctV1>> instincts;
	std::array<std::array<uint32_t, 16>, 40> goals;
	std::shared_ptr<SimpleObjectV1> zzzz;
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
		ar(downfoot_left);
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
			ar(v.name);
			ar(v.delay_ticks);
		}

		// these monikers are super weird.. they're MFC strings that are the
		// reversed and hex-encoded version of the original string characters
		ar.ascii_mfcstring(history_moniker);
		ar.ascii_mfcstring(history_name);
		ar.ascii_mfcstring(history_moms_moniker);
		ar.ascii_mfcstring(history_dads_moniker);
		ar.ascii_mfcstring(history_birthday);
		ar.win1252_mfcstring(history_birthplace);
		ar.win1252_mfcstring(history_owner_name);
		ar.win1252_mfcstring(history_owner_phone);
		ar.win1252_mfcstring(history_owner_address);
		ar.win1252_mfcstring(history_owner_email);
	}
};

} // namespace sfc