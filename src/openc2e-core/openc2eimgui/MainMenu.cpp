#include "MainMenu.h"

#include "AgentInjector.h"
#include "BrainViewer.h"
#include "CreatureGrapher.h"
#include "Engine.h"
#include "Hatchery.h"
#include "ImGuiUtils.h"
#include "MusicManager.h"
#include "PathResolver.h"
#include "PointerAgent.h"
#include "SoundManager.h"
#include "World.h"
#include "creatures/SkeletalCreature.h"
#include "creatures/c2eCreature.h"
#include "creatures/oldCreature.h"
#include "historyManager.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>
#include <imgui.h>

namespace fs = ghc::filesystem;

namespace Openc2eImgui {

static void NewDebugNorn() {
	if (engine.version == 3 && engine.gametype != "sm") {
		auto possibles = findGeneticsFiles("*.gen");
		if (possibles.size() == 0) {
			return;
		}
		auto genome_name = fs::path(possibles[rand() % possibles.size()]).stem().string();

		auto genome = world.loadGenome(genome_name);
		if (!genome) {
			return;
		}

		SkeletalCreature* a = new SkeletalCreature(4);
		bool is_female = rand() % 2 == 1;
		Creature* c = new c2eCreature(genome, is_female, 0, a);
		a->setCreature(c);
		a->finishInit();

		a->setSlot(0, genome);
		world.newMoniker(genome, genome_name, a);
		world.history->getMoniker(world.history->findMoniker(genome)).moveToCreature(a);

		// default values of c3_creature_accg, c3_creature_bhvr, c3_creature_attr, c3_creature_perm
		a->accg = 5;
		a->perm = 100;
		a->attr = 198;
		a->cr_can_push = true;
		a->cr_can_pull = true;
		a->cr_can_stop = true;
		a->cr_can_hit = true;

		c->born();

		world.hand()->addCarried(a);
		world.selectedcreature = a;
		return;

	} else if (engine.version <= 2) {
		std::string genomefile = "test";
		shared_ptr<genomeFile> genome;
		try {
			genome = world.loadGenome(genomefile);
		} catch (Exception& e) {
			printf("Couldn't load genome file: %s\n", e.prettyPrint().c_str());
			return;
		}

		if (!genome) {
			return;
		}

		SkeletalCreature* a = new SkeletalCreature(4);

		bool is_female = rand() % 2 == 1;
		oldCreature* c;
		try {
			if (engine.version == 1) {
				c = new c1Creature(genome, is_female, 0, a);
			} else {
				c = new c2Creature(genome, is_female, 0, a);
			}
		} catch (Exception& e) {
			delete a;
			printf("Couldn't create creature: %s\n", e.prettyPrint().c_str());
			return;
		}

		a->setCreature(c);
		a->finishInit();

		// if you make this work for c2e, you should probably set sane attributes here?

		a->setSlot(0, genome);
		world.newMoniker(genome, genomefile, a);
		world.history->getMoniker(world.history->findMoniker(genome)).moveToCreature(a);

		// TODO: set it dreaming

		c->born();

		world.hand()->addCarried(a);
	} else {
	}
}

static void NewRandomEgg() {
	std::string eggscript;
	/* create the egg obj */
	eggscript = fmt::format("new: simp eggs 8 {} 2000 0\n", ((rand() % 6) * 8));
	/* set the pose */
	eggscript += "pose 3\n";
	/* set the correct class/attributes */
	if (engine.version == 1)
		eggscript += "setv clas 33882624\nsetv attr 67\n";
	else if (engine.version == 2)
		eggscript += "setv cls2 2 5 2\nsetv attr 195\n";
	/* create the genome */
	if (engine.version == 1)
		eggscript += fmt::format("new: gene tokn dad{} tokn mum{} obv0\n", (1 + rand() % 6), (1 + rand() % 6));
	else if (engine.version == 2)
		eggscript += "new: gene tokn norn tokn norn obv0\n";
	/* set the gender */
	eggscript += "setv obv1 0\n";
	/* start the clock */
	eggscript += "tick 2400\n";

	/* move it into place */
	/* TODO: good positions? */
	if (engine.version == 1)
		eggscript += fmt::format("mvto {} 870\n", (2600 + rand() % 200));
	else if (engine.version == 2)
		eggscript += fmt::format("mvto {} 750\n", (4900 + rand() % 350));

	/* c2: enable gravity */
	if (engine.version == 2)
		eggscript += "setv grav 1\n";

	std::string err = engine.executeNetwork(eggscript);
	if (err.size()) {
		printf("Couldn't create egg: %s\n", err.c_str());
	}
}


void DrawMainMenu() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Quit")) {
			world.quitting = true;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View")) {
		ImGui::MenuItem("Show Scrollbars", nullptr, false, false);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Control")) {
		if (ImGui::MenuItem("Pause", nullptr, world.paused)) {
			world.paused = !world.paused;
		}
		if (ImGui::MenuItem("Mute", nullptr, soundmanager.isMuted() && engine.musicmanager->isMuted() && engine.musicmanager->isMIDIMuted())) {
			bool muted = soundmanager.isMuted() && engine.musicmanager->isMuted();
			soundmanager.setMuted(!muted);
			engine.musicmanager->setMuted(!muted);
			engine.musicmanager->setMIDIMuted(!muted);
			// hack to make C3/DS sound options panel update (also makes it slide back in, but whatever)
			for (auto& a : world.agents) {
				if (!a)
					continue;
				a->queueScript(123, 0); // window resized script
			}
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Fast speed", nullptr, engine.fastticks)) {
			engine.fastticks = !engine.fastticks;
		};
		ImGui::MenuItem("Slow display updates", nullptr, false, false);
		if (ImGui::MenuItem("Autokill", nullptr, world.autokill)) {
			world.autokill = !world.autokill;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Debug")) {
		if (ImGui::MenuItem("Show Map", nullptr, world.showrooms)) {
			world.showrooms = !world.showrooms;
		}
		if (ImGui::MenuItem("Create a new (debug) Norn", nullptr, false, engine.gametype != "sm")) {
			NewDebugNorn();
		}
		if (ImGui::MenuItem("Create a random egg", nullptr, false, engine.version <= 2)) {
			NewRandomEgg();
		};
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Tools")) {
		if (ImGui::MenuItem("Hatchery", nullptr, false, IsHatcheryEnabled())) {
			SetHatcheryOpen(true);
		}
		if (ImGui::MenuItem("Agent Injector", nullptr, false, IsAgentInjectorEnabled())) {
			SetAgentInjectorOpen(true);
		}
		if (ImGui::MenuItem("Brain Viewer")) {
			SetBrainViewerOpen(true);
		}
		if (ImGui::MenuItem("Creature Grapher")) {
			SetCreatureGrapherOpen(true);
		};
		ImGui::EndMenu();
	}
}

} // namespace Openc2eImgui
