#include "Agent.h"
#include "AgentRef.h"
#include "CompoundAgent.h"
#include "PointerAgent.h"
#include "SimpleAgent.h"
#include "caosVM.h"
#include "caosValue.h"

#include <memory>
#include <string>
std::string datapath; // because some files link to it

int main() {
#define PSIZE(t) printf("Size of (%s): %zu\n", #t, sizeof(t))

	PSIZE(Agent);
	PSIZE(CompoundAgent);
	PSIZE(PointerAgent);
	PSIZE(SimpleAgent);

	PSIZE(AgentRef);
	PSIZE(std::string);
	PSIZE(caosValue);
	PSIZE(caosVM);
	PSIZE(std::weak_ptr<Agent>);
	return 0;
}
