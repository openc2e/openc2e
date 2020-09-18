#pragma once

#include <string>

enum eventtype {
	eventquit,
	eventtextinput,
	eventrawkeyup,
	eventrawkeydown,
	eventmousebuttondown,
	eventmousebuttonup,
	eventmousemove,
	eventresizewindow,
};
enum eventbuttons { buttonleft=0x1, buttonright=0x2, buttonmiddle=0x4, buttonwheeldown=0x8, buttonwheelup=0x10 };

struct BackendEvent {
	eventtype type;
	int x, y, xrel, yrel;
	int key;
	unsigned int button;
	std::string text;
};