#ifndef PORT_H
#define PORT_H 1

#include <string>
#include <list>
#include "AgentRef.h"

// never use this directly, kthx
struct Port {
	Port(int x, int y, std::string name, std::string desc);
	std::string name;
	std::string description;
	int x, y;
};

struct InputPort : public Port {
	InputPort(int x, int y, std::string name, std::string desc, int messageno);
	int messageno;
	AgentRef source;
	int sourceid;
};

typedef std::list<std::pair<AgentRef, unsigned int> > PortConnectionList;

struct OutputPort : public Port {
	OutputPort(int x, int y, std::string name, std::string desc);
	PortConnectionList dests; // dest agent + dest port id
};

#endif /* PORT_H */
