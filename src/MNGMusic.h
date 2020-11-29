#pragma once

#include "audiobackend/AudioBackend.h"
#include "fileformats/mngfile.h"
#include "optional.h"
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

class MusicWave {
protected:
	unsigned int sampleno;
	MNGFile *parent;

public:
	MusicWave(MNGFile *p, MNGWaveNode *w);
	~MusicWave();
	AudioChannel play(AudioBackend *b, bool looping=false);
};

class MusicStage {
public:
	MusicStage(MNGStageNode *n);
	MNGExpression *pan, *volume, *delay, *tempodelay;
};

class MusicEffect {
public:
	MusicEffect(MNGEffectDecNode *n);
	std::vector<std::shared_ptr<MusicStage> > stages;
};

class MusicVoice {
protected:
	MNGVoiceNode *node;
	MNGUpdateNode *updatenode;
	class MusicLayer *parent;
	std::shared_ptr<MusicWave> wave;
	std::shared_ptr<MusicEffect> effect;

	std::vector<MNGConditionNode *> conditions;

	MNGExpression *interval_expression;
	float interval, volume;

public:
	MusicVoice(class MusicLayer *p, MNGVoiceNode *n);
	std::shared_ptr<MusicWave> getWave() { return wave; }
	float getInterval() { return interval; }
	float getVolume() { return volume; }
	std::shared_ptr<MusicEffect> getEffect() { return effect; }
	bool shouldPlay();
	void runUpdateBlock();
	MusicLayer *getParent() { return parent; }
};

class MusicLayer {
protected:
	MNGUpdateNode *updatenode;
	MusicTrack *parent;

	std::map<std::string, float> variables;
	float updaterate, volume, interval, beatsynch, pan;

	MusicLayer(MusicTrack *p);
	void runUpdateBlock();

public:
	virtual ~MusicLayer() = default;
	MusicTrack *getParent() { return parent; }
	float &getVariable(std::string name) { return variables[name]; }
	virtual void update(float track_volume, float track_beatlength) = 0;
	float getVolume() { return volume; }
	float getInterval() { return interval; }
	float getPan() { return pan; }
};

class MusicAleotoricLayer : public MusicLayer {
protected:
	struct QueuedWave {
		std::shared_ptr<MusicWave> wave;
		mngtimepoint start_time;
		float volume;
		float pan;
	};

	std::shared_ptr<MusicEffect> effect;
	std::vector<std::shared_ptr<MusicVoice> > voices;
	mngtimepoint next_update_at;
	std::shared_ptr<MusicVoice> last_voice;
	std::vector<QueuedWave> queued_waves;
	AudioBackend* backend;

public:
	MusicAleotoricLayer(MNGAleotoricLayerNode *n, MusicTrack *p, AudioBackend *b);
	void update(float track_volume, float track_beatlength);
};

class MusicLoopLayer : public MusicLayer {
protected:
	std::shared_ptr<MusicWave> wave;
	AudioChannel channel;
	mngtimepoint next_update_at;
	AudioBackend* backend;

public:
	MusicLoopLayer(MNGLoopLayerNode *n, MusicTrack *p, AudioBackend *b);
	void update(float track_volume, float track_beatlength);
};

class MusicTrack {
public:
	MNGTrackDecNode *node;
	MNGFile *parent;

	std::vector<std::shared_ptr<MusicLayer> > layers;

	float fadein, fadeout, beatlength, volume;

	MusicTrack(MNGFile *p, MNGTrackDecNode *n, AudioBackend *b);
	void update();

	void startFadeIn();
	void startFadeOut();
	bool fadedOut();
	float getCurrentFadeMultiplier();

	std::string getName() { return node->getName(); }

	optional<mngtimepoint> fadein_start;
	optional<mngtimepoint> fadeout_start;
};