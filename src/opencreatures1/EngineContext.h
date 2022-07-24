#pragma once

#include <memory>

class EngineContext {
  public:
	std::shared_ptr<class AudioBackend> audio_backend;
	std::shared_ptr<class SDLBackend> backend;
	std::shared_ptr<class EventManager> events;
	std::shared_ptr<class ImageManager> images;
	std::shared_ptr<class MacroManager> macros;
	std::shared_ptr<class MapManager> map;
	std::shared_ptr<class C1MusicManager> music;
	std::shared_ptr<class ObjectManager> objects;
	std::shared_ptr<class PathManager> paths;
	std::shared_ptr<class PointerManager> pointer;
	std::shared_ptr<class RenderableManager> renderables;
	std::shared_ptr<class Scriptorium> scriptorium;
	std::shared_ptr<class C1SoundManager> sounds;
	std::shared_ptr<class ViewportManager> viewport;
	std::shared_ptr<class TimerSystem> timers;
};