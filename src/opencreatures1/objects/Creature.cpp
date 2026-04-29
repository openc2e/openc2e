#include "Creature.h"

#include "SFCSerialization.h"
#include "common/Color.h"
#include "fileformats/sfc/CBiochemistry.h"

#include <fmt/core.h>

inline auto attachment_for_pose_and_part(const std::array<std::array<sfc::Vector2i8, 10>, 6>& body_data, int32_t pose, int32_t part) {
	// TODO: part should be an enum class!
	return body_data[numeric_cast<size_t>(part)][numeric_cast<size_t>(pose)];
}

class PoseStringView {
  public:
	PoseStringView(const std::string& p_)
		: p{p_} {
		if (p.size() != 15) {
			throw Exception("bad pose string");
		}
	}

	// ???() { return p[0]; }
	int32_t head() {
		if (p[1] == '?') {
			// look up or down at _IT_
			// TODO
			return 1;
		}
		return p[1] - '0';
	}
	int32_t body() { return p[2] - '0'; }
	int32_t left_thigh() { return p[3] - '0'; }
	int32_t left_shin() { return p[4] - '0'; }
	int32_t left_foot() { return p[5] - '0'; }
	int32_t right_thigh() { return p[6] - '0'; }
	int32_t right_shin() { return p[7] - '0'; }
	int32_t right_foot() { return p[8] - '0'; }
	int32_t left_arm() { return p[9] - '0'; }
	int32_t left_hand() { return p[10] - '0'; }
	int32_t right_arm() { return p[11] - '0'; }
	int32_t right_hand() { return p[12] - '0'; }
	// int32_t tail_root() { return p[13] - '0'; }
	// int32_t tail_tip() { return p[14] - '0'; }

  private:
	const std::string& p;
};


static void set_creature_pose(Creature* creature, const std::string& p) {
	// we want to change the current creature pose to a new pose, given by a string. we'll
	// change the actual sprites, then recalculate positions based on body data, then see
	// if we need to change the current downfoot.
	// a pose string is 15 characters long:
	// 0: ???
	// 1: head
	// 2: body
	// 3–5: left thigh, left shin, left foot
	// 6–8: right thigh, right shin, right foot
	// 9–10: left arm, left hand
	// 11–12: right arm, right hand
	// 13: ???
	// 14: ???

	PoseStringView pose{p};

	// Step 1: first change the sprites for each body part according to the new pose.
	//   this is pretty straightforward, though we might need to take into account current
	//   direction, mood, angle, etc.
	// TODO: actually take them into account rather than hardcoding

	creature->head->set_pose(pose.head());
	printf("body pose %i\n", pose.body());
	creature->body->set_pose(pose.body());
	creature->left_thigh->set_pose(pose.left_thigh());
	creature->left_shin()->set_pose(pose.left_shin());
	creature->left_foot()->set_pose(pose.left_foot());
	creature->right_thigh->set_pose(pose.right_thigh());
	creature->right_shin()->set_pose(pose.right_shin());
	creature->right_foot()->set_pose(pose.right_foot());
	creature->left_arm->set_pose(pose.left_arm());
	creature->left_hand()->set_pose(pose.left_hand());
	creature->right_arm->set_pose(pose.right_arm());
	creature->right_hand()->set_pose(pose.right_hand());
	if (!(p[13] == 'X' && p[14] == 'X')) {
		throw Exception(fmt::format("Pose string with tail poses not implemented: {}", p));
	}

	// Step 2: recalculate the current body part positions based on the creature's body data.
	// we recalculate based off of the current "downfoot", i.e. the foot that is making contact
	// with the ground. TODO: actually use the current downfoot rather than assuming left!
	// first we find the body position based off the downfoot, then reposition everything else
	// from the body
	auto calculate_positions = [&] {
		float offsetx = 0;
		float offsety = 0;

		// TODO: this feels overly complicated
		if (creature->downfoot_left) {
			offsetx += creature->left_thigh->endx() - creature->left_thigh->startx();
			offsety += creature->left_thigh->endy() - creature->left_thigh->starty();

			offsetx += creature->left_shin()->endx() - creature->left_shin()->startx();
			offsety += creature->left_shin()->endy() - creature->left_shin()->starty();

			offsetx += creature->left_foot()->endx() - creature->left_foot()->startx();
			offsety += creature->left_foot()->endy() - creature->left_foot()->starty();

			offsetx += attachment_for_pose_and_part(creature->body->body_data, creature->body->pose(), 1).x;
			offsety += attachment_for_pose_and_part(creature->body->body_data, creature->body->pose(), 1).y;

		} else {
			offsetx += creature->right_thigh->endx() - creature->right_thigh->startx();
			offsety += creature->right_thigh->endy() - creature->right_thigh->starty();

			offsetx += creature->right_shin()->endx() - creature->right_shin()->startx();
			offsety += creature->right_shin()->endy() - creature->right_shin()->starty();

			offsetx += creature->right_foot()->endx() - creature->right_foot()->startx();
			offsety += creature->right_foot()->endy() - creature->right_foot()->starty();

			offsetx += attachment_for_pose_and_part(creature->body->body_data, creature->body->pose(), 2).x;
			offsety += attachment_for_pose_and_part(creature->body->body_data, creature->body->pose(), 2).y;
		}

		creature->body->set_position(
			creature->footx - offsetx,
			creature->footy - offsety);


		// g_points.clear();

		auto position_limb = [creature](auto* limb, int32_t trunk_part) {
			float offsetx = attachment_for_pose_and_part(creature->body->body_data, creature->body->pose(), trunk_part).x;
			float offsety = attachment_for_pose_and_part(creature->body->body_data, creature->body->pose(), trunk_part).y;

			// auto renderitem = get_rendersystem()->render_item_create(LAYER_DEBUG);
			// get_rendersystem()->render_item_set_unfilled_rect(renderitem,
			//     creature->body->x() + offsetx,
			//     creature->body->y() + offsety, 2, 2, Color{255, 255, 255, 255});
			// g_points.push_back(std::move(renderitem));

			while (limb != nullptr) {
				printf("position trunk chain %i x %i y %i\n", (int)trunk_part, (int)offsetx, (int)offsety);
				limb->set_position(
					creature->body->x() + offsetx - limb->startx(),
					creature->body->y() + offsety - limb->starty());
				offsetx += limb->endx() - limb->startx();
				offsety += limb->endy() - limb->starty();
				printf("limb data %i %i, %i %i\n", limb->startx(), limb->starty(), limb->endx(), limb->endy());

				limb = limb->next_limb.get();
				// if (limb) {
				//     auto renderitem = get_rendersystem()->render_item_create(LAYER_DEBUG);
				//     get_rendersystem()->render_item_set_unfilled_rect(renderitem,
				//         creature->body->x() + offsetx,
				//         creature->body->y() + offsety, 2, 2, Color{0, 255, 0, 255});
				//     g_points.push_back(std::move(renderitem));
				// }
			}

			printf("tip %i %i for %i\n", (int)offsetx, (int)offsety, (int)trunk_part);

			// {
			//     auto renderitem = get_rendersystem()->render_item_create(LAYER_DEBUG);
			//     get_rendersystem()->render_item_set_unfilled_rect(renderitem,
			//         creature->body->x() + offsetx,
			//         creature->body->y() + offsety, 2, 2, Color{0, 0, 255, 255});
			//     g_points.push_back(std::move(renderitem));
			// }
		};
		position_limb(creature->head.get(), 0);
		position_limb(creature->left_thigh.get(), 1);
		position_limb(creature->right_thigh.get(), 2);
		position_limb(creature->left_arm.get(), 3);
		position_limb(creature->right_arm.get(), 4);
		position_limb(creature->tail.get(), 5);
	};
	calculate_positions();


	// Step 3: check for a different downfoot! here's the meat of creature movement.
	// we need to check if the downfoot has changed.
	float nondownfoot_x = 0;
	float nondownfoot_y = 0;
	float downfoot_y = 0;
	if (creature->downfoot_left) {
		downfoot_y = creature->left_foot()->y() + creature->left_foot()->endy();
		nondownfoot_x = creature->right_foot()->x() + creature->right_foot()->endx();
		nondownfoot_y = creature->right_foot()->y() + creature->right_foot()->endy();
	} else {
		downfoot_y = creature->right_foot()->y() + creature->right_foot()->endy();
		nondownfoot_x = creature->left_foot()->x() + creature->left_foot()->endx();
		nondownfoot_y = creature->left_foot()->y() + creature->left_foot()->endy();
	}
	printf("downfoot %s\n", creature->downfoot_left ? "left" : "right");
	printf("footx %i\n", creature->footx);
	printf("footy %i\n", creature->footy);
	printf("leftfoot x %f startx %i startndx %i tipy %f\n", creature->left_foot()->x(),
		creature->left_foot()->startx(),
		creature->left_foot()->endx(), creature->left_foot()->y() + creature->left_foot()->endy());
	printf("rightfoot x %f startx %i endx %i tipy %f\n", creature->right_foot()->x(),
		creature->right_foot()->startx(),
		creature->right_foot()->endx(), creature->right_foot()->y() + creature->right_foot()->endy());

	printf("nondownfoot x %f y %f\n", nondownfoot_x, nondownfoot_y);
	if ((float)(int32_t)nondownfoot_x != nondownfoot_x) {
		throw Exception(fmt::format("nondownfoot_x {} isn't integral. how did this happen? are we in a vehicle?", nondownfoot_x));
	}
	if (nondownfoot_y > downfoot_y) {
		// TODO: need to check direction in case both feet are at same height? we could actually
		// just check if the forward foot is at the same height or below the behind/down foot?
		printf("switch feet!!!\n");
		// int32_t diffy = creature->footy - (int32_t)nondownfoot_y;
		// printf("diffy %i\n", diffy);
		creature->footx = (int32_t)nondownfoot_x;
		// keep footy the same, since rooms are flat and we can't walk to another room
		creature->downfoot_left = !creature->downfoot_left;
		// printf("bumping up %i\n", -diffy);

		calculate_positions();
		// shift up to adjust for new foot position
		// auto part_adjust_y = [](DullPart& r, float adjusty) {
		// 	r.set_position(r.x(), r.y() + adjusty);
		// };
		// part_adjust_y(creature->body, diffy);
		// part_adjust_y(creature->head, diffy);
		// part_adjust_y(creature->left_thigh, diffy);
		// part_adjust_y(creature->left_shin(), diffy);
		// part_adjust_y(creature->left_foot(), diffy);
		// part_adjust_y(creature->right_thigh, diffy);
		// part_adjust_y(creature->right_shin(), diffy);
		// part_adjust_y(creature->right_foot(), diffy);
		// part_adjust_y(creature->left_arm, diffy);
		// part_adjust_y(creature->left_hand(), diffy);
		// part_adjust_y(creature->right_arm, diffy);
		// part_adjust_y(creature->right_hand(), diffy);
		// if (creature->tail) {
		// 	part_adjust_y(creature->tail, diffy);
		// 	if (creature->tail->next_limb) {
		// 		part_adjust_y(creature->tail->next_limb, diffy);
		// 	}
		// }
	}

	{
		int bodyx = (int)creature->body->x();
		int bodyy = (int)creature->body->y();

		printf("\n");
		printf("body %i %i\n", bodyx, bodyy);
		printf("head %i %i\n", (int)creature->head->x() - bodyx, (int)creature->head->y() - bodyy);
		printf("left_thigh %i %i\n", (int)creature->left_thigh->x() - bodyx, (int)creature->left_thigh->y() - bodyy);
		printf("left_shin %i %i\n", (int)creature->left_shin()->x() - bodyx, (int)creature->left_shin()->y() - bodyy);
		printf("left_foot %i %i\n", (int)creature->left_foot()->x() - bodyx, (int)creature->left_foot()->y() - bodyy);
		printf("right_thigh %i %i\n", (int)creature->right_thigh->x() - bodyx, (int)creature->right_thigh->y() - bodyy);
		printf("right_shin %i %i\n", (int)creature->right_shin()->x() - bodyx, (int)creature->right_shin()->y() - bodyy);
		printf("right_foot %i %i\n", (int)creature->right_foot()->x() - bodyx, (int)creature->right_foot()->y() - bodyy);
		printf("left_arm %i %i\n", (int)creature->left_arm->x() - bodyx, (int)creature->left_arm->y() - bodyy);
		printf("left_hand %i %i\n", (int)creature->left_hand()->x() - bodyx, (int)creature->left_hand()->y() - bodyy);
		printf("right_arm %i %i\n", (int)creature->right_arm->x() - bodyx, (int)creature->right_arm->y() - bodyy);
		printf("right_hand %i %i\n", (int)creature->right_hand()->x() - bodyx, (int)creature->right_hand()->y() - bodyy);
		printf("\n");
	}
}


static void creature_biotick(Creature* creature) {
	creature->biotick++;

	const bool IS_WALKING = true; // TODO:
	if (IS_WALKING) {
		// broken:
		// lot of things are weird
		const size_t STRONGEST_GAIT_LOCUS = 0; // TODO:

		auto gait_string = creature->gait_animations[STRONGEST_GAIT_LOCUS];
		if (gait_string.back() != 'R') {
			throw Exception(fmt::format("gait string {} does not end with 'R'", gait_string));
		}
		if (gait_string.size() % 2 != 1) {
			throw Exception(fmt::format("gait string {} should be even aside from final 'R'", gait_string));
		}
		auto gait_size = (gait_string.size() - 1) / 2;
		fmt::print("gait_string {}\n", gait_string);

		auto gait_pose_index = creature->biotick % gait_size;
		fmt::print("gait_pose_index {}\n", gait_pose_index);

		// TODO: this is stupid, but I don't want to create a new string just to use std::stoi
		size_t pose_num = numeric_cast<size_t>(gait_string[gait_pose_index * 2] - '0') * 10 + numeric_cast<size_t>(gait_string[gait_pose_index * 2 + 1] - '0');
		fmt::print("pose_num {}\n", pose_num);
		fmt::print("pose string {}\n", creature->poses[pose_num]);

		set_creature_pose(creature, creature->poses[pose_num]);
	}
}

void Creature::creature_tick() {
	age_ticks++;

	// offset calls to biotick, so 1/4 of Creatures in the world get a biotick every tick.
	if (age_ticks % 4 == reinterpret_cast<uintptr_t>(this) % 4) {
		creature_biotick(this);
	}
}

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
		for (auto x : crea->gait_animations) {
			printf("%s\n", x.c_str());
		}
		for (size_t i = 0; i < crea->poses.size(); ++i) {
			printf("%i: %s\n", (int)i, crea->poses[i].c_str());
		}

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
