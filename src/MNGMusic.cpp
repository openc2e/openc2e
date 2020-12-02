#include "MNGMusic.h"

#include "endianlove.h"
#include "utils/ascii_tolower.h"
#include <algorithm>
#include <cmath> // for cos/sin
#include <iostream> // for debug messages

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif


MNGMusic::MNGMusic() = default;
MNGMusic::~MNGMusic() = default;

void MNGMusic::playSilence() {
	playing_silence = true;
	if (currenttrack) {
		currenttrack->startFadeOut();
	}
	nexttrack.reset();
}

void MNGMusic::playTrack(MNGFile *file, std::string trackname) {
	trackname = ascii_tolower(trackname);

	// TODO: these lowercase transformations are ridiculous, we should store inside MusicTrack
	if (nexttrack && nexttrack->getParent() == file) {
		std::string nextname = ascii_tolower(nexttrack->getName());
		if (nextname == trackname) {
			// already moving to this track
			return;
		}
	}
	if (currenttrack && currenttrack->getParent() == file) {
		std::string thisname = ascii_tolower(currenttrack->getName());
		if (thisname == trackname) {
			// already playing this track!
			if (!playing_silence && !nexttrack) return;
			nexttrack.reset();
			currenttrack->startFadeIn();
			return;
		}
	}

	if (file->tracks.find(trackname) == file->tracks.end()) {
		std::cout << "Couldn't find MNG track '" << trackname << "' ('" << file->name << "')!" << std::endl;
		return; // TODO: exception?
	}

	playTrack(std::make_shared<MusicTrack>(file, file->tracks[trackname]));
}

void MNGMusic::playTrack(std::shared_ptr<MusicTrack> track) {
	playing_silence = false;
	track->startFadeIn();
	if (currenttrack) {
		currenttrack->startFadeOut();
		nexttrack = track;
	} else {
		currenttrack = track;
	}
}

void MNGMusic::render(signed short *data, size_t len) {
	if (!currenttrack) {
		// silence
		memset((void *)data, 0, len * 2);
		return;
	}

	currenttrack->update(len / 2);
	currenttrack->render(data, len);
	
	if (nexttrack && currenttrack->fadedOut()) {
		currenttrack = nexttrack;
		nexttrack.reset(); // TODO: do this not in the audio thread?
	}
}

float evaluateExpression(MNGExpression *e, MusicStage *stage = NULL, MusicVoice *voice = NULL, MusicLayer *layer = NULL) {
	if (MNGVariableNode *v = dynamic_cast<MNGVariableNode *>(e)) {
		if (stage) switch (v->getType()) {
			case NAMED:
			case INTERVAL:
				throw MNGFileException("expression " + e->dump() + " invalid in Stage");

			case VOLUME:
			case PAN:
				throw MNGFileException(e->dump() + " not evaluatable in Stage yet"); // TODO
		}
		else if (voice) switch (v->getType()) {
			case NAMED:
				return voice->getParent()->getVariable(v->getName());

			case PAN:
				throw MNGFileException("expression " + e->dump() + " invalid in Voice");

			case INTERVAL:
			case VOLUME:
				throw MNGFileException(e->dump() + " not evaluatable in Voice yet"); // TODO
		}
		else if (layer) switch (v->getType()) {
			case NAMED:
				return layer->getVariable(v->getName());

			case VOLUME:
				return layer->getVolume();

			case INTERVAL:
				return layer->getInterval();

			case PAN:
				return layer->getPan();
		}
	}

	if (MNGConstantNode *c = dynamic_cast<MNGConstantNode *>(e)) {
		return c->getValue();
	}

	if (MNGAddNode *add = dynamic_cast<MNGAddNode *>(e)) {
		return evaluateExpression(add->first(), stage, voice, layer)
			+ evaluateExpression(add->second(), stage, voice, layer);
	}

	if (MNGSubtractNode *sub = dynamic_cast<MNGSubtractNode *>(e)) {
		return evaluateExpression(sub->first(), stage, voice, layer)
			- evaluateExpression(sub->second(), stage, voice, layer);
	}

	if (MNGMultiplyNode *mul = dynamic_cast<MNGMultiplyNode *>(e)) {
		return evaluateExpression(mul->first(), stage, voice, layer)
			* evaluateExpression(mul->second(), stage, voice, layer);
	}

	if (MNGDivideNode *div = dynamic_cast<MNGDivideNode *>(e)) {
		return evaluateExpression(div->first(), stage, voice, layer)
			/ evaluateExpression(div->second(), stage, voice, layer);
	}

	if (MNGSineWaveNode *sinewave = dynamic_cast<MNGSineWaveNode *>(e)) {
		return sin(2 * M_PI * (evaluateExpression(sinewave->first(), stage, voice, layer)
			/ evaluateExpression(sinewave->second(), stage, voice, layer)));
	}

	if (MNGCosineWaveNode *cosinewave = dynamic_cast<MNGCosineWaveNode *>(e)) {
		return cos(2 * M_PI * (evaluateExpression(cosinewave->first(), stage, voice, layer)
			/ evaluateExpression(cosinewave->second(), stage, voice, layer)));
	}

	if (MNGRandomNode *r = dynamic_cast<MNGRandomNode *>(e)) {
		float first = evaluateExpression(r->first(), stage, voice, layer);
		float second = evaluateExpression(r->second(), stage, voice, layer);
		return ((float)rand() / (float)RAND_MAX) * (second - first) + first;
	}

	throw MNGFileException("couldn't evaluate expression " + e->dump());
}

MusicWave::MusicWave(MNGFile *p, MNGWaveNode *n) {
	unsigned int sampleno = n->getSampleNumber();
	if (sampleno >= p->samples.size())
		throw MNGFileException("sample not present");
	// TODO: someday, fix these casts at their source
	char *data = p->samples[sampleno].first;
	unsigned int length = (unsigned int)p->samples[sampleno].second;

	// skip fmt chunk and go straight to data
	data += 20;
	length -= 20;

	buffer = FloatAudioBuffer(new float[length], length);
	for (unsigned int i = 0; i < length / 2; i++) {
		buffer.data[i*2] = (signed short)read16le(data + i * 2);
		buffer.data[(i*2) + 1] = (signed short)read16le(data + i * 2);
	}
}

MusicWave::~MusicWave() {
	delete[] buffer.data;
}

MusicStage::MusicStage(MNGStageNode *node) {
	pan = NULL;
	volume = NULL;
	delay = NULL;
	tempodelay = NULL;

	for (auto &n : *node->children) {
		if (MNGPanNode *p = dynamic_cast<MNGPanNode *>(n)) {
			pan = p->getExpression();
			continue;
		}

		if (MNGEffectVolumeNode *v = dynamic_cast<MNGEffectVolumeNode *>(n)) {
			volume = v->getExpression();
			continue;
		}

		if (MNGDelayNode *d = dynamic_cast<MNGDelayNode *>(n)) {
			delay = d->getExpression();
			continue;
		}

		if (MNGTempoDelayNode *td = dynamic_cast<MNGTempoDelayNode *>(n)) {
			tempodelay = td->getExpression();
			continue;
		}

		throw MNGFileException("unexpected node in Stage: " + n->dump());
	}
}

std::vector<FloatAudioBuffer> MusicStage::applyStage(std::vector<FloatAudioBuffer> &sources, float beatlength) {
	float pan_value = 0.0f, volume_value = 1.0f, delay_value = 0.0f;

	if (pan) {
		pan_value = evaluateExpression(pan, this);
	}

	if (volume) {
		volume_value = evaluateExpression(volume, this);
	}

	if (delay) {
		delay_value = evaluateExpression(delay, this);
	}

	if (tempodelay) {
		delay_value += evaluateExpression(tempodelay, this) * beatlength;
	}

	unsigned int offset_amt = 22050 * delay_value;
	std::vector<FloatAudioBuffer> buffers;
	for (auto &src : sources) {
		src.start_offset += offset_amt;
		volume_value *= src.volume;
		// TODO: better pan_value calculation
		if (src.pan != 0.0f) {
			pan_value = pan_value ? (src.pan + pan_value) / 2.0f : src.pan;
		}
		buffers.push_back(FloatAudioBuffer(src.data, src.len, src.start_offset, volume_value, pan_value));
	}

	return buffers;
}

MusicEffect::MusicEffect(MNGEffectDecNode *node) {
	for (auto &c : *node->children) {
		stages.push_back(std::make_shared<MusicStage>(c));
	}
}

std::vector<FloatAudioBuffer> MusicEffect::applyEffect(class MusicTrack *t, std::vector<FloatAudioBuffer> src, float beatlength) {
	std::vector<FloatAudioBuffer> buffers;

	for (auto &stage : stages) {
		std::vector<FloatAudioBuffer> newbuffers = stage->applyStage(src, beatlength);
		buffers.insert(buffers.end(), newbuffers.begin(), newbuffers.end());
	}

	return buffers;
}

MusicVoice::MusicVoice(MusicLayer *p, MNGVoiceNode *n) {
	node = n;
	parent = p;

	interval = 0.0f;
	interval_expression = NULL;
	volume = 1.0f;

	updatenode = NULL;

	for (auto &n : *node->children) {
		if (MNGWaveNode *e = dynamic_cast<MNGWaveNode *>(n)) {
			// TODO: share duplicate MusicWaves
			wave = std::make_shared<MusicWave>(p->getParent()->getParent(), e);
			continue;
		}

		if (MNGIntervalNode *in = dynamic_cast<MNGIntervalNode *>(n)) {
			interval_expression = in->getExpression();
			continue;
		}

		if (MNGConditionNode *c = dynamic_cast<MNGConditionNode *>(n)) {
			conditions.push_back(c);
			continue;
		}

		if (MNGUpdateNode *u = dynamic_cast<MNGUpdateNode *>(n)) {
			updatenode = u;
			continue;
		}

		if (MNGEffectNode *eff = dynamic_cast<MNGEffectNode *>(n)) {
			if (effect)
				throw MNGFileException("got effect '" + eff->getName() + "' but we already have one!");

			// TODO: share effects
			auto &effects = parent->getParent()->getParent()->effects;
			if (effects.find(eff->getName()) == effects.end())
				throw MNGFileException("couldn't find effect '" + eff->getName() + "'");

			MNGEffectDecNode *n = effects[eff->getName()];
			effect = std::make_shared<MusicEffect>(n);
			continue;
		}

		throw MNGFileException("unexpected node in Voice: " + n->dump());
	}

	if (!wave) {
		throw MNGFileException("Voice must have a Wave node: " + n->dump());
	}
}

bool MusicVoice::shouldPlay() {
	for (auto &n : conditions) {
		float value = evaluateExpression(n->getVariable(), NULL, this);
		if (value < n->minimum() || value > n->maximum())
			return false;
	}
	return true;
}

MusicLayer::MusicLayer(MusicTrack *p) {
	parent = p;

	updaterate = 1.0f;
	volume = 1.0f;
	interval = 0.0f;
	beatsynch = 0.0f;
	pan = 0.0f;

	next_offset = 0;

	updatenode = NULL;

	// TODO: hack
	variables["Mood"] = 1.0f;
	variables["Threat"] = 0.5f;
}

void MusicLayer::runUpdateBlock() {
	if (!updatenode) return;

	for (auto &n : *updatenode->children) {
		float value = evaluateExpression(n->getExpression(), NULL, NULL, this);
		MNGVariableNode *var = n->getVariable();
		switch (var->getType()) {
			case NAMED:
				variables[var->getName()] = value;
				break;

			case INTERVAL:
				interval = value;
				break;

			case VOLUME:
				volume = value;
				break;

			case PAN:
				pan = value;
				break;
		}
	}
}

void MusicVoice::runUpdateBlock() {
	if (interval_expression) interval = evaluateExpression(interval_expression, NULL, this);

	if (!updatenode) return;

	for (auto &n : *updatenode->children) {
		float value = evaluateExpression(n->getExpression(), NULL, this);
		MNGVariableNode *var = n->getVariable();
		switch (var->getType()) {
			case NAMED:
				parent->getVariable(var->getName()) = value;
				break;

			case INTERVAL:
				interval = value;
				break;

			case VOLUME:
				volume = value;
				break;

			case PAN:
				throw MNGFileException("panic: attempt to set Pan inside Voice update"); // TODO?
		}
	}
}

MusicAleotoricLayer::MusicAleotoricLayer(MNGAleotoricLayerNode *node, MusicTrack *p) : MusicLayer(p) {
	for (auto &n : *node->children) {
		if (MNGEffectNode *e = dynamic_cast<MNGEffectNode *>(n)) {
			if (effect)
				throw MNGFileException("got effect '" + e->getName() + "' but we already have one!");

			// TODO: share effects
			std::map<std::string, class MNGEffectDecNode *> &effects = parent->getParent()->effects;
			if (effects.find(e->getName()) == effects.end())
				throw MNGFileException("couldn't find effect '" + e->getName() + "'");

			MNGEffectDecNode *n = effects[e->getName()];
			effect = std::make_shared<MusicEffect>(n);
			continue;
		}

		if (MNGVoiceNode *v = dynamic_cast<MNGVoiceNode *>(n)) {
			voices.push_back(std::make_shared<MusicVoice>(this, v));
			continue;
		}

		if (MNGUpdateNode *u = dynamic_cast<MNGUpdateNode *>(n)) {
			updatenode = u;
			continue;
		}

		if (MNGLayerVolumeNode *lv = dynamic_cast<MNGLayerVolumeNode *>(n)) {
			volume = evaluateExpression(lv->getExpression());
			continue;
		}

		if (MNGUpdateRateNode *ur = dynamic_cast<MNGUpdateRateNode *>(n)) {
			updaterate = evaluateExpression(ur->getExpression());
			continue;
		}

		if (MNGVariableDecNode *vd = dynamic_cast<MNGVariableDecNode *>(n)) {
			std::string name = vd->getName();
			float value = evaluateExpression(vd->getExpression());
			variables[name] = value;
			continue;
		}

		if (MNGBeatSynchNode *bs = dynamic_cast<MNGBeatSynchNode *>(n)) {
			beatsynch = evaluateExpression(bs->getExpression());
			continue;
		}

		if (MNGIntervalNode *in = dynamic_cast<MNGIntervalNode *>(n)) {
			interval = evaluateExpression(in->getExpression());
			continue;
		}

		throw MNGFileException("unexpected node in AleotoricLayer: " + n->dump());
	}

	runUpdateBlock();
}

void MusicAleotoricLayer::update(unsigned int latency_in_frames) {
	unsigned int parent_offset = parent->getCurrentOffset();

	if (next_offset > parent_offset + latency_in_frames) return;
	unsigned int offset = next_offset;

	runUpdateBlock();

	decltype(voices) available_voices;
	for (auto &voice : voices) {
		if (!voice->shouldPlay()) continue;
		if (last_voice.get() == voice.get()) continue;
		available_voices.push_back(voice);
	}
	// try to avoid playing the same voice twice in a row, it sounds awful and
	// doesn't seem to happen in the real engine
	if (!available_voices.size() && last_voice->shouldPlay()) {
		available_voices.push_back(last_voice);
	}

	if (available_voices.size()) {
		auto voice = last_voice = available_voices[rand() % available_voices.size()];

		FloatAudioBuffer &data = voice->getWave()->getData();
		std::vector<FloatAudioBuffer> buffers{{
			FloatAudioBuffer(data.data, data.len, offset, volume * parent->getVolume(), pan)
		}};
		if (voice->getEffect()) {
			buffers = voice->getEffect()->applyEffect(parent, buffers, parent->getBeatLength());
		}
		if (effect) {
			buffers = effect->applyEffect(parent, buffers, parent->getBeatLength());
		}

		/* not sure where this should be run exactly.. see C2's UpperTemple for odd example
		 * GR's source says "These take effect after playback of the voice has begun"
		 * so I try to run it in the same place that code does, for now */
		voice->runUpdateBlock();

		float our_interval = interval + voice->getInterval() + (beatsynch * parent->getBeatLength());
		offset += 22050 * our_interval;
		
		for (auto &b : buffers) {
			parent->addBuffer(b);
		}
	}

	next_offset = offset;
}

MusicLoopLayer::MusicLoopLayer(MNGLoopLayerNode *node, MusicTrack *p) : MusicLayer(p) {
	update_period = 0;
	
	for (auto &n : *node->children) {
		if (MNGWaveNode *e = dynamic_cast<MNGWaveNode *>(n)) {
			// TODO: share duplicate MusicWaves
			wave = std::shared_ptr<MusicWave>(new MusicWave(parent->getParent(), e));
			continue;
		}

		if (MNGUpdateRateNode *ur = dynamic_cast<MNGUpdateRateNode *>(n)) {
			updaterate = evaluateExpression(ur->getExpression());
			continue;
		}

		if (MNGVariableDecNode *vd = dynamic_cast<MNGVariableDecNode *>(n)) {
			std::string name = vd->getName();
			float value = evaluateExpression(vd->getExpression());
			variables[name] = value;
			continue;
		}

		if (MNGUpdateNode *u = dynamic_cast<MNGUpdateNode *>(n)) {
			updatenode = u;
			continue;
		}

		throw MNGFileException("unexpected node in LoopLayer: " + n->dump());
	}

	runUpdateBlock();
}

void MusicLoopLayer::update(unsigned int latency_in_frames) {
	if (!wave) return;

	unsigned int parent_offset = parent->getCurrentOffset();

	if (next_offset > parent_offset + latency_in_frames) return;

	float our_volume = volume * parent->getVolume();

	FloatAudioBuffer &data = wave->getData();
	parent->addBuffer(FloatAudioBuffer(data.data, data.len, next_offset, our_volume, pan));

	next_offset += data.len;

	update_period += updaterate;
	if (update_period > 1.0f) {
		runUpdateBlock();
		update_period -= 1.0f;
	}
}

MusicTrack::MusicTrack(MNGFile *p, MNGTrackDecNode *n) {
	node = n;
	parent = p;
	current_offset = 0;

	volume = 1.0f;
	// TODO: what's the default fadein/fadeout?
	// for now, changed this from 0.0f to 1.0f because otherwise c3 sounds silly
	fadein = fadeout = 1.0f;
	beatlength = 0.0f;

	fadein_count = fadeout_count = 0;

	for (auto &n : *node->children) {
		if (MNGAleotoricLayerNode *al = dynamic_cast<MNGAleotoricLayerNode *>(n)) {
			layers.push_back(std::dynamic_pointer_cast<MusicLayer>(
				std::make_shared<MusicAleotoricLayer>(al, this)
			));
			continue;
		}

		if (MNGLoopLayerNode *ll = dynamic_cast<MNGLoopLayerNode *>(n)) {
			layers.push_back(std::dynamic_pointer_cast<MusicLayer>(
				std::make_shared<MusicLoopLayer>(ll, this)
			));
			continue;
		}

		if (MNGFadeInNode *fi = dynamic_cast<MNGFadeInNode *>(n)) {
			fadein = evaluateExpression(fi->getExpression());
			continue;
		}

		if (MNGFadeOutNode *fo = dynamic_cast<MNGFadeOutNode *>(n)) {
			fadeout = evaluateExpression(fo->getExpression());
			continue;
		}

		if (MNGBeatLengthNode *bl = dynamic_cast<MNGBeatLengthNode *>(n)) {
			beatlength = evaluateExpression(bl->getExpression());
			continue;
		}

		if (MNGLayerVolumeNode *lv = dynamic_cast<MNGLayerVolumeNode *>(n)) {
			volume = evaluateExpression(lv->getExpression());
			continue;
		}

		throw MNGFileException("unexpected node in Track: " + n->dump());
	}
}

void MusicTrack::update(unsigned int latency_in_frames) {
	for (auto &l : layers) {
		l->update(latency_in_frames);
	}
}

void MusicTrack::startFadeIn() {
	if (fadein_count) {
		return;
	}
	if (fadeout_count) {
		fadein_count = (fadein * 22050 * 2) * (1.0 - (float)(fadeout_count / (fadeout * 22050 * 2)));
		fadeout_count = 0;
	} else {
		fadein_count = fadein * 22050 * 2;
	}
}

void MusicTrack::startFadeOut() {
	if (fadeout_count) {
		return;
	}
	if (fadein_count) {
		fadeout_count = (fadeout * 22050 * 2) * (1.0 - (float)(fadein_count / (fadein * 22050 * 2)));
		fadein_count = 0;
	} else {
		fadeout_count = fadeout * 22050 * 2;
	}
	if (!fadeout_count) {
		fadeout_count = 1;
	}
}

bool MusicTrack::fadedOut() {
	return fadeout_count == 1;
}

void MusicTrack::render(signed short *data, size_t len) {
	float *output = (float *)alloca(len * sizeof(float));
	memset(output, 0.0f, len * sizeof(float));

	// mix pending buffers, render
	//unsigned int numbuffers = 0;
	for (int i = 0; i < (int)buffers.size(); i++) {
		FloatAudioBuffer &buffer = buffers[i];
		unsigned int j = 0;
		if (buffer.start_offset > current_offset) {
			// buffer hasn't started (quite) yet
			if (buffer.start_offset > current_offset + len)
				continue;
			j = 2 * (buffer.start_offset - current_offset);
		}
		//numbuffers++;
		float left_pan = 1.0f - buffer.pan;
		float right_pan = 1.0f + buffer.pan;
		for (; j < len && buffer.position < buffer.len; j++) {
			output[j] += buffer.data[buffer.position] * buffer.volume * left_pan;
			buffer.position++;
			j++;
			output[j] += buffer.data[buffer.position] * buffer.volume * right_pan;
			buffer.position++;
		}
		if (buffer.position == buffer.len) {
			buffers.erase(buffers.begin() + i);
			i--;
		}
	}
	//float mul = (1.0f/numbuffers) * 0.8f; // TODO: this is a hack to try and avoid clipping
	float mul = 0.3f;
	if (fadein_count) {
		mul *= 1.0 - (fadein_count / (fadein * 22050 * 2));
		if (fadein_count >= len) fadein_count -= len; else fadein_count = 0;
	} else if (fadeout_count) {
		mul *= (fadeout_count / (fadeout * 22050 * 2));
		if (fadeout_count > len) fadeout_count -= len; else fadeout_count = 1;
	}
	for (unsigned int i = 0; i < len; i++) {
		output[i] *= mul;
	}
	for (unsigned int i = 0; i < len; i++) {
		data[i] = (signed short)output[i];
	}

	current_offset += len / 2;
}