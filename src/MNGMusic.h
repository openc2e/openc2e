#pragma once

#include "audiobackend/AudioBackend.h"
#include "mngfile/mngfile.h"
#include <memory>

class MNGMusic {
private:
	void playTrack(std::shared_ptr<class MusicTrack> track);

	std::shared_ptr<class MNGStream> stream;
	std::shared_ptr<class MusicTrack> currenttrack, nexttrack;

public:
	MNGMusic();
	~MNGMusic();

	void startPlayback(AudioBackend &audio);
	void playTrack(MNGFile* file, std::string trackname);
	void playSilence();

	void render(signed short *data, size_t len);
	
	bool playing_silence = true;
};

struct FloatAudioBuffer {
	unsigned int start_offset;
	size_t len, position;
	float *data;
	float volume, pan;

	FloatAudioBuffer() { data = NULL; len = 0; position = 0; start_offset = 0; volume = 1.0f; pan = 0.0f; }
	FloatAudioBuffer(float *d, size_t l, unsigned int o = 0, float v = 1.0f, float p = 0.0f) {
		position = 0;
		len = l;
		data = d;
		start_offset = o;
		volume = v;
		pan = p;
	}
};

class MusicWave {
protected:
	FloatAudioBuffer buffer;

public:
	MusicWave(MNGFile *p, MNGWaveNode *w);
	~MusicWave();
	FloatAudioBuffer &getData() { return buffer; }
};

class MusicStage {
protected:
	MNGStageNode *node;

	MNGExpression *pan, *volume, *delay, *tempodelay;

public:
	MusicStage(MNGStageNode *n);
	std::vector<FloatAudioBuffer> applyStage(std::vector<FloatAudioBuffer> &sources, float beatlength);
};

class MusicEffect {
protected:
	MNGEffectDecNode *node;
	std::vector<std::shared_ptr<MusicStage> > stages;

public:
	MusicEffect(MNGEffectDecNode *n);
	std::vector<FloatAudioBuffer> applyEffect(class MusicTrack *t, std::vector<FloatAudioBuffer> src, float beatlength);
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
	MusicVoice(std::shared_ptr<class MusicLayer> p, MNGVoiceNode *n);
	std::shared_ptr<MusicWave> getWave() { return wave; }
	float getInterval() { return interval; }
	float getVolume() { return volume; }
	std::shared_ptr<MusicEffect> getEffect() { return effect; }
	bool shouldPlay();
	void runUpdateBlock();
	MusicLayer *getParent() { return parent; }
};

class MusicLayer : public std::enable_shared_from_this<class MusicLayer> {
protected:
	MNGUpdateNode *updatenode;

	MusicTrack *parent;
	unsigned int next_offset;

	std::map<std::string, float> variables;
	float updaterate, volume, interval, beatsynch, pan;

	MusicLayer(std::shared_ptr<MusicTrack> p);
	void runUpdateBlock();

public:
	virtual ~MusicLayer() = default;
	MusicTrack *getParent() { return parent; }
	float &getVariable(std::string name) { return variables[name]; }
	virtual void update(unsigned int latency_in_frames) = 0;
	float getVolume() { return volume; }
	float getInterval() { return interval; }
	float getPan() { return pan; }
};

class MusicAleotoricLayer : public MusicLayer {
protected:
	MNGAleotoricLayerNode *node;

	std::shared_ptr<MusicEffect> effect;
	std::vector<std::shared_ptr<MusicVoice> > voices;

public:
	MusicAleotoricLayer(MNGAleotoricLayerNode *n, std::shared_ptr<MusicTrack> p);
	void init();
	void update(unsigned int latency_in_frames);
};

class MusicLoopLayer : public MusicLayer {
protected:
	MNGLoopLayerNode *node;

	unsigned int update_period;
	std::shared_ptr<MusicWave> wave;

public:
	MusicLoopLayer(MNGLoopLayerNode *n, std::shared_ptr<MusicTrack> p);
	void init();
	void update(unsigned int latency_in_frames);
};

class MusicTrack : public std::enable_shared_from_this<class MusicTrack> {
protected:
	MNGTrackDecNode *node;
	MNGFile *parent;

	std::vector<std::shared_ptr<MusicLayer> > layers;

	float fadein, fadeout, beatlength, volume;

	unsigned int fadein_count, fadeout_count;

	unsigned int current_offset;
	std::vector<FloatAudioBuffer> buffers;

public:
	MusicTrack(MNGFile *p, MNGTrackDecNode *n);
	void init();
	virtual ~MusicTrack();
	void render(signed short *data, size_t len);
	void addBuffer(FloatAudioBuffer buf) { buffers.push_back(buf); }
	unsigned int getCurrentOffset() { return current_offset; }
	void update(unsigned int latency_in_frames);
	float getVolume() { return volume; }
	float getBeatLength() { return beatlength; }

	void startFadeIn();
	void startFadeOut();
	bool fadedOut();

	MNGFile *getParent() { return parent; }
	std::string getName() { return node->getName(); }
};