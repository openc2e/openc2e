#include "creaturesImage.h"
#include "attFile.h"

// for enum lifestage (0 to 6)
#include "genome.h"

#include "openc2e.h"
#include "Creature.h"

class SkeletalCreature : public Creature {
private:
	unsigned int species;
	unsigned int breed;
	lifestage stage;

	unsigned int direction;
	unsigned int pose[14];
	unsigned int facialexpression;
	unsigned int pregnancy;
	bool eyesclosed;

	creaturesImage *images[14];
	attFile att[14];

	int width, height, adjustx, adjusty;
	int partx[14], party[14];

	std::string dataString(unsigned int _stage, bool sprite);

	unsigned int gaiti;
	creatureGait *gaitgene;

public:
	SkeletalCreature(genomeFile *g, unsigned char _family, bool is_female, unsigned char _variant, unsigned int s, unsigned int b, lifestage t);
	void render(SDLBackend *renderer, int xoffset, int yoffset);
	void recalculateSkeleton();
	unsigned int getPose(unsigned int i) { return pose[i]; }
	void setPose(unsigned int p);
	void setPose(std::string s);
	void setPoseGene(unsigned int p);
	void setGaitGene(unsigned int g);
	void gaitTick();
	unsigned int getPregnancy() { return pregnancy; }
	void setPregnancy(unsigned int p) { assert(p < 4); pregnancy = p; }
	bool getEyesClosed() { return eyesclosed; }
	void setEyesClosed(bool e) { eyesclosed = e; }
	unsigned int getFacialExpression() { return facialexpression; }
	void setFacialExpression(unsigned int f) { assert (f < 6); facialexpression = f; }
	unsigned int getWidth() { return width; }
	unsigned int getHeight() { return height; }
};

/* vim: set noet: */
