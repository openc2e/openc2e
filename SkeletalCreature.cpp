/* 20th feb 2005, by fuzzie
   with thanks to vadim for helping out

   TODO:
	* hair tidiness/untidiness (offset by 0 to 2 multiplied by 16)
	* render hairs/ears .. at the moment we avoid them, we lack zorder sanity too
*/

#include "SkeletalCreature.h"
#include "World.h"

#include <typeinfo> // TODO: remove when genome system is fixed

struct bodypartinfo {
	char letter;
	int parent;
	int attorder;
};

bodypartinfo cee_bodyparts[17] = {
	{ 'b', -1, -1 }, // body
	{ 'a', 0, 0 }, // head - attached to body
	{ 'c', 0, 1 }, // left thigh - attached to body
	{ 'f', 0, 2 }, // right thigh - attached to body
	{ 'i', 0, 3 }, // left humerus - attached to body
	{ 'k', 0, 4 }, // right humerus - attached to body
	{ 'm', 0, 5 }, // tail root - attached to body
	{ 'd', 2, 1 }, // left shin - attached to left thigh
	{ 'g', 3, 1 }, // right shin - attached to right thigh
	{ 'j', 4, 1 }, // left radius - attached to left humerus
	{ 'l', 5, 1 }, // right radius - attached to right humerus
	{ 'e', 7, 1 }, // left foot - attached to left shin
	{ 'h', 8, 1 }, // right foot - attached to right shin
	{ 'n', 6, 1 }, // tail tip - attached to tail root
	// Creatures Village only:
	{ 'o', 1, -1 }, // left ear - attached to head
	{ 'p', 1, -1 }, // right ear - attached to head
	{ 'q', 1, -1 } // hair - attached to head
};

unsigned int cee_zorder[4][14] = {
	{ 6, 13, 2, 7, 11, 4, 9, 0, 1, 5, 3, 8, 10, 12 },
	{ 6, 13, 3, 8, 12, 5, 10, 0, 1, 2, 7, 11, 4, 9 },
	{ 6, 13, 2, 4, 9, 5, 3, 7, 8, 10, 0, 11, 12, 1 },
	{ 2, 4, 9, 1, 5, 3, 7, 8, 10, 11, 12, 0, 6, 13 }
};

// needed for setPose(string) at least .. maybe cee_bodyparts should be indexed by letter
unsigned int cee_lookup[17] = { 1, 0, 2, 7, 11, 3, 8, 12, 4, 9, 5, 10, 6, 13, 14, 15, 16 };

std::string SkeletalCreature::dataString(unsigned int _stage, bool sprite) {
	char _postfix[4] = "XXX";
	_postfix[0] = '0' + species + ((sprite && female) ? 4 : 0);
	_postfix[1] = '0' + _stage;
	_postfix[2] = 'a' + breed;
	return _postfix;
}

SkeletalCreature::SkeletalCreature(genomeFile *g, unsigned char _family, bool is_female, unsigned char _variant, unsigned int s, unsigned int b, lifestage t)
 : Creature(g, _family, is_female, _variant) {
	species = s;
	assert(species < 4);
	breed = b;
	assert(breed < 26);
	stage = t;
	// todo: check lifestage
	facialexpression = 0;
	pregnancy = 0;
	eyesclosed = false;

	imageGallery gallery;

	for (int i = 0; i < 14; i++) {
		// try this stage and the stages below it to find data which worksforus
		images[i] = 0;
		char x = cee_bodyparts[i].letter;
		int stage_to_try = stage;
		while (stage_to_try > -1 && images[i] == 0) {
			images[i] = gallery.getImage(x + dataString(stage_to_try, true));
			if (images[i] == 0) stage_to_try--;
		}
		assert(images[i] != 0);
		std::ifstream in(std::string(world.findFile(std::string("/Body Data/") + x + dataString(stage_to_try, false) + ".att")).c_str());
		in >> att[i];
	}

	setPose(0);
}

/*void SkeletalCreature::render(SDLBackend *renderer, int xoffset, int yoffset) {
	for (int j = 0; j < 14; j++) {
		int i = cee_zorder[direction][j];

		bodypartinfo *part = &cee_bodyparts[i];

		unsigned int ourpose;
		if (part->parent == -1) // body
			ourpose = pose[i] + (pregnancy * 16);
		else if (i == 1) // head
			ourpose = pose[i] + (eyesclosed ? 16 : 0) + (facialexpression * 32);
		else // everything else
			ourpose = pose[i];

		renderer->render(images[i], ourpose, partx[i] + adjustx + xoffset + x, party[i] + adjusty + yoffset + y, false, 0);
	}
}*/

void SkeletalCreature::recalculateSkeleton() {
	int lowestx = 0, lowesty = 0, highestx = 0, highesty = 0;

	for (int i = 0; i < 14; i++) {
		bodypartinfo *part = &cee_bodyparts[i];

		if (part->parent == -1) {
			partx[i] = 0; party[i] = 0;
		} else {
			attFile &bodyattinfo = att[0];
			attFile &attinfo = att[i];

			int attachx = att[i].attachments[pose[i]][0];
			int attachy = att[i].attachments[pose[i]][1];
			int x, y;

			if (part->parent == 0) { // linking to body
				int bodyx = bodyattinfo.attachments[pose[0]][part->attorder * 2];
				int bodyy = bodyattinfo.attachments[pose[0]][(part->attorder * 2) + 1];

				x = bodyx - attachx;
				y = bodyy - attachy;
			} else { // extra limb
				attFile &parentattinfo = att[part->parent];

				x = partx[part->parent] + parentattinfo.attachments[pose[part->parent]][part->attorder * 2] - attachx; 
				y = party[part->parent] + parentattinfo.attachments[pose[part->parent]][(part->attorder * 2) + 1] - attachy;
			}

			partx[i] = x; party[i] = y;

			if (x < lowestx) { lowestx = x; }
			if (y < lowesty) { lowesty = y; }
			if (x + images[i]->width(pose[i]) > highestx) { highestx = x + images[i]->width(pose[i]); }
			if (y + images[i]->height(pose[i]) > highesty) { highesty = y + images[i]->height(pose[i]); }
		}
	}

	adjustx = -lowestx;
	adjusty = -lowesty;
	width = highestx - lowestx;
	height = highesty - lowesty;
}

void SkeletalCreature::setPose(unsigned int p) {
	direction = 0;
	for (int i = 0; i < 14; i++)
		pose[i] = p;
	recalculateSkeleton();
}

void SkeletalCreature::setPose(std::string s) {
	switch (s[0]) {
		case '?': direction = 0; break; // hack
		case '!': direction = 1; break; // hack
		case '0': direction = 3; break;
		case '1': direction = 2; break;
		case '2': direction = 0; break;
		case '3': direction = 1; break;
		default: assert(false);
	}

	for (int i = 0; i < 14; i++) {
		switch (s[i + 1]) {
			case '0': pose[cee_lookup[i]] = 0 + (direction * 4); break;
			case '1': pose[cee_lookup[i]] = 1 + (direction * 4); break;
			case '2': pose[cee_lookup[i]] = 2 + (direction * 4); break;
			case '3': pose[cee_lookup[i]] = 3 + (direction * 4); break;
			case '?': assert(i == 0); pose[1] = 0 + (direction * 4); break; // hack
			case 'X': break; // do nothing
			default: assert(false); 
		}
	}
		
	recalculateSkeleton();
}

void SkeletalCreature::setPoseGene(unsigned int poseno) {
	/* TODO: this sets by sequence, now, not the 'poseno' inside the gene.
	 * this is what the POSE caos command does. is this right? - fuzzie */
	creaturePose *g = (creaturePose *)genome->getGene(2, 3, poseno);
	assert(g); // TODO: -> caos_assert

	gaitgene = 0;
}

void SkeletalCreature::setGaitGene(unsigned int gaitdrive) { // TODO: not sure if this is *useful*
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(creatureGait)) {
			creatureGait *g = (creatureGait *)(*i);
			if (g->drive == gaitdrive) {
				gaitgene = g;
				gaiti = 0;
			}
		}
	}

	// explode!
	gaitgene = 0;
	gaitTick();
}

void SkeletalCreature::gaitTick() {
	if (!gaitgene) return;
	uint8 pose = gaitgene->pose[gaiti];
	creaturePose *poseg = 0;
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(creaturePose)) {
			creaturePose *g = (creaturePose *)(*i);
			if (g->poseno == pose)
				poseg = g;
			
		}
	}
	assert(poseg); // TODO: don't assert. caos_assert? but this means a bad genome file, always.
	setPose(poseg->getPoseString());
	gaiti++; if (gaiti > 7) gaiti = 0;
}
		
/* vim: set noet: */
