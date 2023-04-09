#include "EngineContext.h"

#include "C1MusicManager.h"
#include "C1SoundManager.h"
#include "ImageManager.h"
#include "MacroCommands.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "MessageManager.h"
#include "ObjectManager.h"
#include "PathManager.h"
#include "PointerManager.h"
#include "Scriptorium.h"
#include "ViewportManager.h"

struct EngineContextConstructTag {};

EngineContext g_engine_context(EngineContextConstructTag{});

EngineContext::EngineContext(const EngineContextConstructTag&) {
	MacroCommands::install_default_commands(macros->ctx);
}

void EngineContext::reset() {
	this->~EngineContext();
	new (this) EngineContext(EngineContextConstructTag{});
}