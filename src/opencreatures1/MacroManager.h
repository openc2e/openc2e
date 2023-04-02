#pragma once

#include "Macro.h"
#include "MacroContext.h"
#include "ObjectHandle.h"
#include "common/StaticVector.h"

#include <array>
#include <fmt/core.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

enum ScriptNumber {
	SCRIPT_DEACTIVATE = 0,
	SCRIPT_ACTIVATE1 = 1,
	SCRIPT_ACTIVATE2 = 2,
	SCRIPT_HIT = 3,
	SCRIPT_PICKUP = 4,
	SCRIPT_DROP = 5,
	SCRIPT_COLLISION = 6,
	SCRIPT_INITIALIZE = 7,
	SCRIPT_DEINITIALIZE = 8,
	SCRIPT_TIMER = 9,

	// creature
	SCRIPT_EXTRA_QUIESCENT = 16,
	SCRIPT_EXTRA_ACTIVATE1 = 17,
	SCRIPT_EXTRA_ACTIVATE2 = 18,
	SCRIPT_EXTRA_DEACTIVATE = 19,
	SCRIPT_EXTRA_SEEK = 20,
	SCRIPT_EXTRA_AVOID = 21,
	SCRIPT_EXTRA_PICKUP = 22,
	SCRIPT_EXTRA_DROP = 23,
	SCRIPT_EXTRA_NEED = 24,
	SCRIPT_EXTRA_REST = 25,
	SCRIPT_EXTRA_WEST = 26,
	SCRIPT_EXTRA_EAST = 27,
	SCRIPT_EXTRA_UNDEFINED1 = 28,
	SCRIPT_EXTRA_UNDEFINED2 = 29,
	SCRIPT_EXTRA_UNDEFINED3 = 30,
	SCRIPT_EXTRA_UNDEFINED4 = 31,

	// introspective
	SCRIPT_INTRO_QUIESCENT = 32,
	SCRIPT_INTRO_ACTIVATE1 = 33,
	SCRIPT_INTRO_ACTIVATE2 = 34,
	SCRIPT_INTRO_DEACTIVATE = 35,
	SCRIPT_INTRO_SEEK = 36,
	SCRIPT_INTRO_AVOID = 37,
	SCRIPT_INTRO_PICKUP = 38,
	SCRIPT_INTRO_DROP = 39,
	SCRIPT_INTRO_NEED = 40,
	SCRIPT_INTRO_REST = 41,
	SCRIPT_INTRO_WEST = 42,
	SCRIPT_INTRO_EAST = 43,
	SCRIPT_INTRO_UNDEFINED1 = 44,
	SCRIPT_INTRO_UNDEFINED2 = 45,
	SCRIPT_INTRO_UNDEFINED3 = 46,
	SCRIPT_INTRO_UNDEFINED4 = 47,

	// pointer
	SCRIPT_POINTER_ACTIVATE1 = 50,
	SCRIPT_POINTER_ACTIVATE2 = 51,
	SCRIPT_POINTER_DEACTIVATE = 52,
	SCRIPT_POINTER_PICKUP = 53,
	SCRIPT_POINTER_DROP = 54,

	// involuntary
	SCRIPT_INVOLUNTARY0 = 64,
	SCRIPT_INVOLUNTARY1 = 65,
	SCRIPT_INVOLUNTARY2 = 66,
	SCRIPT_INVOLUNTARY3 = 67,
	SCRIPT_INVOLUNTARY4 = 68,
	SCRIPT_INVOLUNTARY5 = 69,
	SCRIPT_INVOLUNTARY6 = 70,
	SCRIPT_INVOLUNTARY7 = 71,
	SCRIPT_DIE = 72,
};


std::string scriptnumber_to_string(ScriptNumber);

class MacroManager {
  public:
	void add(Macro macro);
	bool has_macro_owned_by(ObjectHandle ownr) const;
	void delete_macros_owned_by(ObjectHandle ownr);
	void tick();
	bool queue_script(ObjectHandle from_id, ObjectHandle to_id, ScriptNumber eventno, bool override_existing = true);
	bool queue_script(Object* from, Object* to, ScriptNumber eventno, bool override_existing = true);

	MacroContext ctx;

  private:
	std::vector<Macro> m_pool;
};