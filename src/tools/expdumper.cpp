#include "common/Repr.h"
#include "common/backtrace.h"
#include "fileformats/NewSFCFile.h"

#include <fmt/core.h>
#include <fstream>


static void print_limb(std::shared_ptr<sfc::LimbV1> limb, const std::string& indent) {
	if (!limb) {
		fmt::print("null\n");
		return;
	}
	fmt::print("Limb {{\n");
	// fmt::print("{}  sprite = \"{}\"\n", indent, limb->sprite->filename);
	fmt::print("{}  sprite_base = {}\n", indent, limb->sprite_base);
	fmt::print("{}  position = ({},{})\n", indent, limb->x, limb->y);
	fmt::print("{}  z_order = {}\n", indent, limb->z_order);
	fmt::print("{}  angle = {}\n", indent, limb->angle);
	fmt::print("{}  view = {}\n", indent, limb->view);
	fmt::print("{}  limb_data = [\n", indent);
	for (auto& d : limb->limb_data) {
		fmt::print("{}    {:2d} {:2d} {:2d} {:2d}\n", indent, d.startx, d.starty, d.endx, d.endy);
	}
	fmt::print("{}  ]\n", indent);
	if (limb->next) {
		fmt::print("{}  next = ", indent);
		print_limb(limb->next, indent + "  ");
	}
	fmt::print("{}}}\n", indent);
}

static void print_genome(std::shared_ptr<sfc::CGenomeV1> genome) {
	if (!genome) {
		fmt::print("null\n");
		return;
	}
	fmt::print("Genome {{\n");
	fmt::print("  moniker = \"{}\"\n", genome->moniker);
	fmt::print("  sex = {}\n", genome->sex);
	fmt::print("  life_stage = {}\n", genome->life_stage);
	fmt::print("  data = [ {} bytes ]\n", genome->data.size());
	fmt::print("}}\n");
}

static void print_dendrite_type(const sfc::DendriteTypeV1& dt, const std::string& indent) {
	fmt::print("DendriteType {{\n");
	fmt::print("{}  source_lobe = {}\n", indent, dt.source_lobe);
	fmt::print("{}  min = {}\n", indent, dt.min);
	fmt::print("{}  max = {}\n", indent, dt.max);
	fmt::print("{}  spread = {}\n", indent, dt.spread);
	fmt::print("{}  fanout = {}\n", indent, dt.fanout);
	fmt::print("{}  min_ltw = {}\n", indent, dt.min_ltw);
	fmt::print("{}  max_ltw = {}\n", indent, dt.max_ltw);
	fmt::print("{}  min_strength = {}\n", indent, dt.min_strength);
	fmt::print("{}  max_strength = {}\n", indent, dt.max_strength);
	fmt::print("{}  migration = {}\n", indent, dt.migration);
	fmt::print("{}  relax_suscept = {}\n", indent, dt.relax_suscept);
	fmt::print("{}  relax_stw = {}\n", indent, dt.relax_stw);
	fmt::print("{}  ltw_gain_rate = {}\n", indent, dt.ltw_gain_rate);
	fmt::print("{}  gain_rate = {}\n", indent, dt.gain_rate);
	fmt::print("{}  lose_rate = {}\n", indent, dt.lose_rate);
	fmt::print("{}  gain_svrule = SVRule {{ ... }}\n", indent);
	fmt::print("{}  lose_svrule = SVRule {{ ... }}\n", indent);
	fmt::print("{}  suscept_svrule = SVRule {{ ... }}\n", indent);
	fmt::print("{}  reinforce_svrule = SVRule {{ ... }}\n", indent);
	fmt::print("{}}}\n", indent);
}

static void print_dendrite(const sfc::DendriteV1& dendrite) {
	fmt::print("Dendrite {{ source={} home=({}, {}) suscept={} stw={} ltw={} strength={} }}\n", dendrite.source_index, dendrite.home_x, dendrite.home_y, dendrite.suscept, dendrite.stw, dendrite.ltw, dendrite.strength);
}

int main(int argc, char** argv) {
	install_backtrace_printer();

	if (argc != 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		fmt::print(stderr, "USAGE: {} FILE\n", argv[0]);
		return 1;
	}

	auto input_filename = argv[1];

	std::ifstream in(input_filename, std::ios_base::binary);
	auto exp = sfc::read_exp_v1_file(in);

	fmt::print("creature = Creature {{\n");
	fmt::print("  moniker = \"{}\"\n", exp.creature->moniker);
	fmt::print("  mother = \"{}\"\n", exp.creature->mother);
	fmt::print("  father = \"{}\"\n", exp.creature->father);

	fmt::print("  body = Body {{\n");
	// fmt::print("    sprite = \"{}\"\n", exp.creature->body->sprite->filename);
	// fmt::print("    frame = \"{}\"\n", exp.creature->body->current_sprite);
	fmt::print("    position = ({},{})\n", exp.creature->body->x, exp.creature->body->y);
	fmt::print("    z_order = {}\n", exp.creature->body->z_order);
	fmt::print("    angle = {}\n", exp.creature->body->angle);
	fmt::print("    view = {}\n", exp.creature->body->view);
	fmt::print("    body_data = [\n");
	for (auto& row : exp.creature->body->body_data) {
		fmt::print("      ");
		for (auto& col : row) {
			fmt::print("{:2d} {:2d} ", col.x, col.y);
		}
		fmt::print("\n");
	}
	fmt::print("    ]\n");
	fmt::print("  }}\n");

	fmt::print("  head = ");
	print_limb(exp.creature->head, "  ");
	fmt::print("  left_thigh = ");
	print_limb(exp.creature->left_thigh, "  ");
	fmt::print("  right_thigh = ");
	print_limb(exp.creature->right_thigh, "  ");
	fmt::print("  left_arm = ");
	print_limb(exp.creature->left_arm, "  ");
	fmt::print("  right_arm = ");
	print_limb(exp.creature->right_arm, "  ");
	fmt::print("  tail = ");
	print_limb(exp.creature->tail, "  ");

	fmt::print("  direction = {}\n", exp.creature->direction);
	fmt::print("  downfoot = {}\n", exp.creature->downfoot);
	fmt::print("  foot_position = ({},{})\n", exp.creature->footx, exp.creature->footy);
	fmt::print("  z_order = {}\n", exp.creature->z_order);
	fmt::print("  current_pose = \"{}\"\n", exp.creature->current_pose);
	fmt::print("  expression = {}\n", exp.creature->expression);
	fmt::print("  eyes_open = {}\n", exp.creature->eyes_open);
	fmt::print("  asleep = {}\n", exp.creature->asleep);

	fmt::print("  poses = [\n");
	for (size_t i = 0; i < exp.creature->poses.size(); ++i) {
		auto& p = exp.creature->poses[i];
		fmt::print("    {}: \"{}\"\n", i, p);
	}
	fmt::print("  ]\n");

	fmt::print("  gait_animations = [\n");
	for (size_t i = 0; i < exp.creature->gait_animations.size(); ++i) {
		auto& g = exp.creature->gait_animations[i];
		fmt::print("    {}: \"{}\"\n", i, g);
	}
	fmt::print("  ]\n");

	fmt::print("  vocabulary = [\n");
	for (size_t i = 0; i < exp.creature->vocabulary.size(); ++i) {
		auto& v = exp.creature->vocabulary[i];
		fmt::print("    {}: in=\"{}\" out=\"{}\" strength={}\n", i, v.in, v.out, v.strength);
	}
	fmt::print("  ]\n");

	fmt::print("  object_positions = [\n");
	for (size_t i = 0; i < exp.creature->object_positions.size(); ++i) {
		auto& p = exp.creature->object_positions[i];
		if (p.x == -1 && p.y == -1) {
			fmt::print("    {}: ()\n", i);
		} else {
			fmt::print("    {}: ({},{})\n", i, p.x, p.y);
		}
	}
	fmt::print("  ]\n");

	fmt::print("  stimuli = [\n");
	for (size_t i = 0; i < exp.creature->stimuli.size(); ++i) {
		auto& s = exp.creature->stimuli[i];

		if (s.significance == 0 && s.input == 255 && s.intensity == 0 && s.amount0 == 0 && s.amount1 == 0 && s.amount2 == 0 && s.amount3 == 0) {
			fmt::print("    {}: Stimulus {{}}\n", i);
			continue;
		}
		fmt::print("    {}: Stimulus {{\n", i);
		if (s.significance) {
			fmt::print("      significance={}\n", s.significance);
		}
		if (s.input != 255) {
			fmt::print("      input={}\n", s.input);
		}
		if (s.intensity) {
			fmt::print("      intensity={}\n", s.intensity);
		}
		if (s.flags) {
			fmt::print("      flags={}\n", s.flags);
		}
		if (s.amount0) {
			fmt::print("      chem0={}\n", s.chem0);
			fmt::print("      amount0={}\n", s.amount0);
		}
		if (s.amount1) {
			fmt::print("      chem1={}\n", s.chem1);
			fmt::print("      amount1={}\n", s.amount1);
		}
		if (s.amount2) {
			fmt::print("      chem2={}\n", s.chem2);
			fmt::print("      amount2={}\n", s.amount2);
		}
		if (s.amount3) {
			fmt::print("      chem3={}\n", s.chem3);
			fmt::print("      amount3={}\n", s.amount3);
		}
		fmt::print("    }}\n");
	}
	fmt::print("  ]\n");

	fmt::print("  brain = Brain {{\n");
	fmt::print("    lobes = [\n");
	for (size_t i = 0; i < exp.creature->brain->lobes.size(); ++i) {
		auto& lobe = exp.creature->brain->lobes[i];
		fmt::print("      {}: Lobe {{\n", i);
		fmt::print("        x = {}\n", lobe.x);
		fmt::print("        y = {}\n", lobe.y);
		fmt::print("        width = {}\n", lobe.width);
		fmt::print("        height = {}\n", lobe.height);
		fmt::print("        perceptible = {}\n", lobe.perceptible);
		fmt::print("        activity = {}\n", lobe.activity);
		fmt::print("        chemicals = [{} {} {} {}]\n", lobe.chemicals[0], lobe.chemicals[1], lobe.chemicals[2], lobe.chemicals[3]);
		fmt::print("        threshold = {}\n", lobe.threshold);
		fmt::print("        leakage = {}\n", lobe.leakage);
		fmt::print("        reststate = {}\n", lobe.reststate);
		fmt::print("        inputgain = {}\n", lobe.inputgain);
		fmt::print("        svrule = SVRule {{ ... }}\n");
		fmt::print("        flags = {}\n", lobe.flags);
		fmt::print("        d0_type = ");
		print_dendrite_type(lobe.d0_type, "        ");
		fmt::print("        d1_type = ");
		print_dendrite_type(lobe.d1_type, "        ");
		fmt::print("        num_neurons = {}\n", lobe.num_neurons);
		fmt::print("        num_dendrites = {}\n", lobe.num_dendrites);
		fmt::print("      }}\n");
	}
	fmt::print("    ]\n");
	fmt::print("    neurons = [\n");
	for (size_t i = 0; i < exp.creature->brain->neurons.size(); ++i) {
		auto& neuron = exp.creature->brain->neurons[i];
		fmt::print("      {}: Neuron {{\n", i);
		fmt::print("        pos=({},{}) output={} state={} wtadisable={} exclusive={}\n", neuron.x, neuron.y, neuron.output, neuron.state, neuron.wta_disable, neuron.exclusive);
		// fmt::print("        dendrites0_index = {}\n", neuron.dendrite0_index);
		// fmt::print("        dendrites1_index = {}\n", neuron.dendrite1_index);
		if (neuron.dendrites0.size()) {
			fmt::print("        dendrites0 = [\n");
			for (size_t j = 0; j < neuron.dendrites0.size(); ++j) {
				fmt::print("          {}: ", neuron.dendrite0_index + j);
				print_dendrite(neuron.dendrites0[j]);
			}
			fmt::print("        ]\n");
		} else {
			fmt::print("        dendrites0 = []\n");
		}
		if (neuron.dendrites1.size()) {
			fmt::print("        dendrites1 = [\n");
			for (size_t j = 0; j < neuron.dendrites1.size(); ++j) {
				fmt::print("          {}: ", neuron.dendrite1_index + j);
				print_dendrite(neuron.dendrites1[j]);
			}
		} else {
			fmt::print("        dendrites1 = []\n");
		}
		fmt::print("      }}\n");
	}
	fmt::print("    ]\n");
	fmt::print("  }}\n");

	fmt::print("  biochemistry = Biochemistry {{\n");
	fmt::print("    owner = {}\n", (void*)exp.creature->biochemistry->owner);
	fmt::print("    chemicals = [\n");
	for (size_t i = 0; i < exp.creature->biochemistry->chemicals.size(); ++i) {
		auto& chem = exp.creature->biochemistry->chemicals[i];
		fmt::print("      {}: ChemicalState {{ concentration={} decay={} }}\n", i, chem.concentration, chem.decay);
	}
	fmt::print("    ]\n");
	fmt::print("    emitters = [\n");
	for (size_t i = 0; i < exp.creature->biochemistry->emitters.size(); ++i) {
		auto& e = exp.creature->biochemistry->emitters[i];
		fmt::print("      {}: Emitter {{ organ={} tissue={} locus={} chemical={} threshold={} rate={} gain={} effect={} }}\n", i, e.organ, e.tissue, e.locus, e.chemical, e.threshold, e.rate, e.gain, e.effect);
	}
	fmt::print("    ]\n");
	fmt::print("    receptors = [\n");
	for (size_t i = 0; i < exp.creature->biochemistry->receptors.size(); ++i) {
		auto& r = exp.creature->biochemistry->receptors[i];
		fmt::print("      {}: Receptor {{ organ={} tissue={} locus={} chemical={} threshold={} nominal={} gain={} effect={} }}\n", i, r.organ, r.tissue, r.locus, r.chemical, r.threshold, r.nominal, r.gain, r.effect);
	}
	fmt::print("    ]\n");
	fmt::print("    reactions = [\n");
	for (size_t i = 0; i < exp.creature->biochemistry->reactions.size(); ++i) {
		auto& r = exp.creature->biochemistry->reactions[i];
		fmt::print("      {}: Reaction {{ r1_amount={} r1_chem={} r2_amount={} r2_chem={} rate={} p1_amount={} p1_chem={} p2_amount={} p2_chem={} }}\n",
			i, r.r1_amount, r.r1_chem, r.r2_amount, r.r2_chem, r.rate, r.p1_amount, r.p1_chem, r.p2_amount, r.p2_chem);
	}
	fmt::print("    ]\n");
	fmt::print("  }}\n");

	fmt::print("  sex = {}\n", exp.creature->sex);
	fmt::print("  age = {}\n", exp.creature->age);
	fmt::print("  biotick = {}\n", exp.creature->biotick);
	fmt::print("  gamete = \"{}\"\n", exp.creature->gamete);
	fmt::print("  zygote = \"{}\"\n", exp.creature->zygote);
	fmt::print("  dead = {}\n", exp.creature->dead);
	fmt::print("  age_ticks = {}\n", exp.creature->age_ticks);
	fmt::print("  dreaming = {}\n", exp.creature->dreaming);

	if (exp.creature->instincts.size()) {
		fmt::print("  instincts = [\n");
		for (size_t i = 0; i < exp.creature->instincts.size(); ++i) {
			auto& instinct = exp.creature->instincts[i];
			fmt::print("    {}: Instinct {{\n", i);
			fmt::print("      dendrites = [\n");
			for (size_t j = 0; j < instinct->dendrites.size(); ++j) {
				auto& d = instinct->dendrites[j];
				fmt::print("        {}: lobe={} cell={}\n", j, d.lobe, d.cell);
			}
			fmt::print("      ]\n");
			fmt::print("      motor_decision = {}\n", instinct->motor_decision);
			fmt::print("      reinforcement_chemical = {}\n", instinct->reinforcement_chemical);
			fmt::print("      reinforcement_amount = {}\n", instinct->reinforcement_amount);
			fmt::print("      phase = {}\n", instinct->phase);
			fmt::print("    }}\n");
		}
		fmt::print("  ]\n");
	} else {
		fmt::print("  instincts = []\n");
	}

	fmt::print("  goals = [ ... ]\n");
	if (exp.creature->zzzz) {
		fmt::print("  zzzz = SimpleObject { ... }\n");
	} else {
		fmt::print("  zzzz = null\n");
	}

	fmt::print("  voice_lookup_table = [ ... ]\n");
	fmt::print("  voices = [\n");
	for (size_t i = 0; i < exp.creature->voices.size(); ++i) {
		auto& voice = exp.creature->voices[i];

		auto voicename = [&]() -> std::string {
			if (voice.name == std::array<uint8_t, 4>{0, 0, 0, 0}) {
				return "null";
			}
			if (voice.name == std::array<uint8_t, 4>{0, 0, 0, 1}) {
				return "\"\\0\\0\\0\\1\"";
			}
			return std::string("\"") + std::string((char*)&voice.name[0], 4) + "\"";
		}();

		fmt::print("    {}: Voice name={} delay={}\n", i, voicename, voice.delay_ticks);
	}
	fmt::print("  )]\n");

	fmt::print("  history_moniker = \"{}\"\n", exp.creature->history_moniker);
	fmt::print("  history_name = \"{}\"\n", exp.creature->history_name);
	fmt::print("  history_moms_moniker = \"{}\"\n", exp.creature->history_moms_moniker);
	fmt::print("  history_dads_moniker = \"{}\"\n", exp.creature->history_dads_moniker);
	fmt::print("  history_birthday = \"{}\"\n", exp.creature->history_birthday);
	fmt::print("  history_birthplace = \"{}\"\n", exp.creature->history_birthplace);
	fmt::print("  history_owner_name = \"{}\"\n", exp.creature->history_owner_name);
	fmt::print("  history_owner_phone = \"{}\"\n", exp.creature->history_owner_phone);
	fmt::print("  history_owner_address = \"{}\"\n", exp.creature->history_owner_address);
	fmt::print("  history_owner_email = \"{}\"\n", exp.creature->history_owner_email);

	fmt::print("}}\n");

	fmt::print("genome = ");
	print_genome(exp.genome);

	fmt::print("child_genome = ");
	print_genome(exp.child_genome);

	// check if we read all of it
	if (!in.eof()) {
		// stupid iostreams throw an error if you try to peek a file that's at EOF,
		// _after_ you've already peeked once to tell if it's at EOF. stupid stupid stupid.
		in.peek();
	}
	fmt::print("read entire file? {}\n", in.eof());
}
