#pragma once

#include "audiobackend/AudioBackend.h"
#include "fileformats/mngfile.h"
#include "fileformats/mngparser.h"
#include "utils/optional.h"

#include <chrono>
#include <memory>

// use times as floating-point numbers because that's how MNG files are written,
// and this makes calculations much easier (avoiding duration_cast and manual
// arithmetic)
using dseconds = std::chrono::duration<double>;

using mngclock = std::chrono::steady_clock;
using mngtimepoint = std::chrono::time_point<mngclock, dseconds>;

class MNGMusic {
  private:
	void playTrack(std::shared_ptr<class MusicTrack> track);
	std::shared_ptr<class MusicTrack> currenttrack, nexttrack;
	std::shared_ptr<AudioBackend> backend;
	float volume = 1.0;

  public:
	MNGMusic(const std::shared_ptr<AudioBackend>& backend);
	~MNGMusic();
	void update();
	void setVolume(float);
	void stop();

	void playTrack(MNGFile* file, std::string trackname);
	void playSilence();

	bool playing_silence = true;
};

class MusicStage {
  public:
	MusicStage(MNGStage);
	optional<MNGExpression> pan, volume, delay, tempodelay;
};

class MusicEffect {
  public:
	MusicEffect(MNGEffect);
	std::string name;
	std::vector<std::shared_ptr<MusicStage>> stages;
};

class MusicVoice {
  public:
	class MusicLayer* parent;
	std::string wave;
	std::vector<MNGCondition> conditions;
	std::shared_ptr<MusicEffect> effect;
	optional<MNGExpression> interval;
	std::vector<MNGUpdate> updates;

	MusicVoice(class MusicLayer* p, MNGVoice n);
	bool shouldPlay();
	void runUpdateBlock();
};

class MusicLayer {
  public:
	MusicTrack* parent = nullptr;
	std::unordered_map<std::string, float> variables;
	std::vector<MNGUpdate> updates;
	float updaterate, volume;

	virtual ~MusicLayer() = default;
	void runUpdateBlock();
	MusicTrack* getParent() { return parent; }
	virtual float& getVariable(std::string name) = 0;
};

class MusicAleotoricLayer : public MusicLayer {
  public:
	struct QueuedWave {
		std::string wave_name;
		mngtimepoint start_time;
		float volume;
		float pan;
	};
	struct PlayingWave {
		AudioChannel channel;
		float volume;
	};

	std::string name;
	std::shared_ptr<MusicEffect> effect;
	std::vector<std::shared_ptr<MusicVoice>> voices;
	mngtimepoint next_update_at;
	mngtimepoint next_voice_at;
	std::shared_ptr<MusicVoice> last_voice;
	std::vector<QueuedWave> queued_waves;
	std::vector<PlayingWave> playing_waves;
	AudioBackend* backend;
	float interval;
	optional<float> beatsynch;

	MusicAleotoricLayer(MNGAleotoricLayer n, MusicTrack* p, AudioBackend* b);
	void update(float track_volume, float track_beatlength);
	void stop();
	float& getVariable(std::string name);
};

class MusicLoopLayer : public MusicLayer {
  public:
	std::string wave;
	AudioChannel channel;
	mngtimepoint next_update_at;
	AudioBackend* backend;
	float pan = 0.0;

	MusicLoopLayer(MNGLoopLayer n, MusicTrack* p, AudioBackend* b);
	void update(float track_volume);
	void stop();
	float& getVariable(std::string name);
};

class MusicTrack {
  public:
	MNGTrack node;
	MNGFile* parent;

	std::vector<std::shared_ptr<MusicAleotoricLayer>> aleotoriclayers;
	std::vector<std::shared_ptr<MusicLoopLayer>> looplayers;
	std::vector<std::shared_ptr<MusicEffect>> effects;

	float fadein, fadeout, beatlength, volume;

	MusicTrack(MNGFile* p, MNGScript s, MNGTrack n, AudioBackend* b);
	void update(float system_volume);
	void stop();
	void startFadeIn();
	void startFadeOut();
	bool fadedOut();
	float getCurrentFadeMultiplier();

	std::string getName() { return node.name; }

	optional<mngtimepoint> fadein_start;
	optional<mngtimepoint> fadeout_start;
};