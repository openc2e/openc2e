#include "Creature.h"

#include "SFCSerialization.h"
#include "common/render/RenderSystem.h"
#include "fileformats/sfc/CBiochemistry.h"

void Creature::handle_left_click(float, float) {
	// When an object is the subject of a left click event, we queue up a message
	// to ACTIVATE1, ACTIVATE2, or DEACTIVATE. But how do we know which message
	// to send?
}

const DullPart* Creature::get_part(int32_t partno) const {
	// TODO: no idea
	if (partno == 0) {
		return body.get();
	}
	return nullptr;
}

static std::unique_ptr<Limb> sfc_load_limb(sfc::LimbV1* p) {
	if (!p) {
		return nullptr;
	}
	auto limb = std::make_unique<Limb>();
	static_cast<DullPart&>(*limb) = sfc_load_entity(p);
	limb->angle = p->angle;
	limb->view = p->view;
	limb->limb_data = p->limb_data;
	limb->next_limb = sfc_load_limb(p->next.get());
	return limb;
}

static std::shared_ptr<sfc::LimbV1> sfc_dump_limb(const Limb* limb, const std::shared_ptr<sfc::CGalleryV1>& gallery) {
	if (!limb) {
		return nullptr;
	}
	// sorta implemented
	auto p = std::make_shared<sfc::LimbV1>();
	*static_cast<sfc::EntityV1*>(p.get()) = *sfc_dump_entity(*limb, gallery);
	p->limb_data = limb->limb_data;
	p->next = sfc_dump_limb(limb->next_limb.get(), gallery);

	// not implemented
	p->angle = limb->angle;
	p->view = limb->view;

	return p;
};

void Creature::serialize(SFCContext& ctx, sfc::CreatureV1* crea) {
	if (ctx.is_loading()) {
		// not implemented
		moniker = crea->moniker;
		mother = crea->mother;
		father = crea->father;

		// sorta implemented
		body = std::make_unique<Body>();
		static_cast<DullPart&>(*body) = sfc_load_entity(crea->body.get());
		body->body_data = crea->body->body_data;
		body->angle = crea->body->angle;
		body->view = crea->body->view;

		head = sfc_load_limb(crea->head.get());
		left_thigh = sfc_load_limb(crea->left_thigh.get());
		right_thigh = sfc_load_limb(crea->right_thigh.get());
		left_arm = sfc_load_limb(crea->left_arm.get());
		right_arm = sfc_load_limb(crea->right_arm.get());
		tail = sfc_load_limb(crea->tail.get());

		// sorta implemented
		direction = crea->direction;
		downfoot_left = crea->downfoot_left;
		footx = crea->footx;
		footy = crea->footy;

		// not implemented
		z_order = crea->z_order;
		current_pose = crea->current_pose;
		expression = crea->expression;
		eyes_open = crea->eyes_open;
		asleep = crea->asleep;

		// sorta implemented
		poses = crea->poses;
		gait_animations = crea->gait_animations;

		// not implemented
		vocabulary = crea->vocabulary;
		object_positions = crea->object_positions;
		stimuli = crea->stimuli;
		brain = crea->brain;
		biochemistry = crea->biochemistry;
		sex = crea->sex;
		age = crea->age;

		// sorta implemented
		biotick = crea->biotick;

		// not implemented
		gamete = crea->gamete;
		zygote = crea->zygote;
		dead = crea->dead;

		// sorta implemented
		age_ticks = crea->age_ticks;

		// not implemented
		dreaming = crea->dreaming;
		instincts = crea->instincts;
		goals = crea->goals;
		zzzz = crea->zzzz;
		voices_lookup = crea->voices_lookup;
		voices = crea->voices;
		history_moniker = crea->history_moniker;
		history_name = crea->history_name;
		history_moms_moniker = crea->history_moms_moniker;
		history_dads_moniker = crea->history_dads_moniker;
		history_birthday = crea->history_birthday;
		history_birthplace = crea->history_birthplace;
		history_owner_name = crea->history_owner_name;
		history_owner_phone = crea->history_owner_phone;
		history_owner_address = crea->history_owner_address;
		history_owner_email = crea->history_owner_email;
	} else {
		// not implemented
		crea->moniker = moniker;
		crea->mother = mother;
		crea->father = father;

		// sorta implemented
		crea->body = std::make_shared<sfc::BodyV1>();
		*static_cast<sfc::EntityV1*>(crea->body.get()) = *sfc_dump_entity(*body);
		auto gallery = static_cast<sfc::ObjectV1*>(crea)->gallery = crea->body->gallery;
		crea->body->body_data = body->body_data;

		// not implemented
		crea->body->angle = body->angle;
		crea->body->view = body->view;

		crea->head = sfc_dump_limb(head.get(), gallery);
		crea->left_thigh = sfc_dump_limb(left_thigh.get(), gallery);
		crea->right_thigh = sfc_dump_limb(right_thigh.get(), gallery);
		crea->left_arm = sfc_dump_limb(left_arm.get(), gallery);
		crea->right_arm = sfc_dump_limb(right_arm.get(), gallery);
		crea->tail = sfc_dump_limb(tail.get(), gallery);

		// sorta implemented
		crea->direction = direction;
		crea->downfoot_left = downfoot_left;
		crea->footx = footx;
		crea->footy = footy;

		// not implemented
		crea->z_order = z_order;
		crea->current_pose = current_pose;
		crea->expression = expression;
		crea->eyes_open = eyes_open;
		crea->asleep = asleep;

		// sorta implemented
		crea->poses = poses;
		crea->gait_animations = gait_animations;

		// not implemented
		crea->vocabulary = vocabulary;
		crea->object_positions = object_positions;
		crea->stimuli = stimuli;
		crea->brain = brain;
		crea->biochemistry = biochemistry;
		crea->biochemistry->owner = crea; // danger!!!
		crea->sex = sex;
		crea->age = age;

		// sorta implemented
		crea->biotick = biotick;

		// not implemented
		crea->gamete = gamete;
		crea->zygote = zygote;
		crea->dead = dead;

		// sorta implemented
		crea->age_ticks = age_ticks;

		// not implemented
		crea->dreaming = dreaming;
		crea->instincts = instincts;
		crea->goals = goals;
		crea->zzzz = zzzz;
		crea->voices_lookup = voices_lookup;
		crea->voices = voices;
		crea->history_moniker = history_moniker;
		crea->history_name = history_name;
		crea->history_moms_moniker = history_moms_moniker;
		crea->history_dads_moniker = history_dads_moniker;
		crea->history_birthday = history_birthday;
		crea->history_birthplace = history_birthplace;
		crea->history_owner_name = history_owner_name;
		crea->history_owner_phone = history_owner_phone;
		crea->history_owner_address = history_owner_address;
		crea->history_owner_email = history_owner_email;
	}

	Object::serialize(ctx, crea);
}
