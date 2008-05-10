#include "Port.h"
#include <string>

Port::Port(int _x, int _y, std::string _name, std::string _desc) {
	x = _x; y = _y; name = _name; description = _desc;
}

InputPort::InputPort(int x, int y, std::string name, std::string desc, int _messageno):
	Port(x, y, name, desc) {
	messageno = _messageno;
	sourceid = 0;
}

OutputPort::OutputPort(int x, int y, std::string name, std::string desc):
	Port(x, y, name, desc) {
}
