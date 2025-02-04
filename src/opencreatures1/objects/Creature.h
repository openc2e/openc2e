#pragma once

#include "DullPart.h"
#include "fileformats/sfc/Body.h"
#include "fileformats/sfc/Creature.h"
#include "fileformats/sfc/Limb.h"
#include "objects/Object.h"

#include <array>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

class SFCContext;
namespace sfc {
struct CBiochemistryV1;
struct CBrainV1;
struct CInstinctV1;
} // namespace sfc

struct BodyPart : DullPart {
	// not implemented
	int32_t angle;
	int32_t view;
};

struct Body : BodyPart {
	// store body data in column-major order, like SFC files but unlike
	// ATT files which are in row-major order
	std::array<std::array<sfc::Vector2i8, 10>, 6> body_data;
};

struct Limb : BodyPart {
	std::array<sfc::LimbData, 10> limb_data;
	std::unique_ptr<Limb> next_limb;
};

struct Creature : Object {
	// TODO: lots of things!

	// not implemented
	std::string moniker;
	std::string mother;
	std::string father;

	// sorta implemented
	std::unique_ptr<Body> body;
	std::unique_ptr<Limb> head;
	std::unique_ptr<Limb> left_thigh;
	std::unique_ptr<Limb> right_thigh;
	std::unique_ptr<Limb> left_arm;
	std::unique_ptr<Limb> right_arm;
	std::unique_ptr<Limb> tail;

	void handle_left_click(float, float) override;
	const DullPart* get_part(int32_t) const override;

	uint8_t direction;
	bool downfoot_left;
	int32_t footx;
	int32_t footy;

	// not implemented
	uint32_t z_order;
	std::string current_pose;
	uint8_t expression;
	uint8_t eyes_open;
	uint8_t asleep;

	// sorta implemented
	std::array<std::string, 100> poses;
	std::array<std::string, 8> gait_animations;

	// not implemented
	std::array<sfc::VocabWordV1, 80> vocabulary;
	std::array<sfc::Vector2i, 40> object_positions;
	std::array<sfc::StimulusV1, 36> stimuli;
	// std::unique_ptr<Brain> brain;
	// std::unique_ptr<Biochemistry> biochemistry;
	std::shared_ptr<sfc::CBrainV1> brain;
	std::shared_ptr<sfc::CBiochemistryV1> biochemistry;
	uint8_t sex;
	uint8_t age;

	// sorta implemented
	uint32_t biotick;

	// not implemented
	std::string gamete;
	std::string zygote;
	uint8_t dead;

	// sorta implemented
	uint32_t age_ticks;

	// not implemented
	uint32_t dreaming;
	std::vector<std::shared_ptr<sfc::CInstinctV1>> instincts;
	std::array<std::array<uint32_t, 16>, 40> goals;
	std::shared_ptr<sfc::SimpleObjectV1> zzzz;
	std::array<std::array<uint32_t, 3>, 27> voices_lookup;
	std::array<sfc::VoiceV1, 32> voices;
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

	void serialize(SFCContext&, sfc::CreatureV1*);
	void creature_tick();
};
