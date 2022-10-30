#pragma once

#include "common/HeapValue.h"

#include <memory>

class EngineContext {
  public:
	// singleton, instantiated at bottom of file
	EngineContext(const struct EngineContextConstructTag&);
	EngineContext(const EngineContext&) = delete;
	EngineContext(EngineContext&&) = delete;
	EngineContext& operator=(const EngineContext&) = delete;
	EngineContext& operator=(EngineContext&&) = delete;
	void reset();

	// all the parts
	std::shared_ptr<class AudioBackend> audio_backend;
	std::shared_ptr<class Backend> backend;
	HeapValue<class EventManager> events;
	HeapValue<class ImageManager> images;
	HeapValue<class MacroManager> macros;
	HeapValue<class MapManager> map;
	HeapValue<class C1MusicManager> music;
	HeapValue<class ObjectManager> objects;
	HeapValue<class PathManager> paths;
	HeapValue<class PointerManager> pointer;
	HeapValue<class RenderableManager> renderables;
	HeapValue<class Scriptorium> scriptorium;
	HeapValue<class C1SoundManager> sounds;
	HeapValue<class ViewportManager> viewport;
	HeapValue<class TimerSystem> timers;
};

extern EngineContext g_engine_context;