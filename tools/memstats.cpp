#include "caosVar.h"
#include "AgentRef.h"
#include "Agent.h"
#include "CompoundAgent.h"
#include "PointerAgent.h"
#include "SimpleAgent.h"
#include "caosVM.h"

#include <string>

int main() {
#define PSIZE(t) printf("Size of (%s): %zu\n", #t, sizeof (t))
    PSIZE(Agent);
    PSIZE(CompoundAgent);
    PSIZE(PointerAgent);
    PSIZE(SimpleAgent);
    
    PSIZE(AgentRef);
    PSIZE(std::string);
    PSIZE(caosVar);
    PSIZE(caosVM);
    return 0;
}
