#ifndef LobeNames_H
#define LobeNames_H

#include "../../engine/Creature/Brain/BrainAccess.h"

class LobeNames : public BrainAccess
{
public:
	LobeNames(Lobes lobes);
	~LobeNames();
	const char *GetLobeFullName(int l){return myLobeNames[l];};
	const char *GetNeuronName(int l, int n){return myNeuronNames[l][n];};

private:
	char **myLobeNames;
	char ***myNeuronNames;

	Lobes myLobes;

	void HandleNoLobeDefinition(int l);
};

#endif

