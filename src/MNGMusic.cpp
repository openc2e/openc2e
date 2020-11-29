#include "MNGMusic.h"

#include "endianlove.h"
#include "utils/ascii_tolower.h"
#include <algorithm>
#include <cmath> // for cos/sin
#include <iostream> // for debug messages

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

MNGMusic::MNGMusic(const std::shared_ptr<AudioBackend>& b) : backend(b) {}
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
	if (nexttrack && nexttrack->parent == file) {
		std::string nextname = ascii_tolower(nexttrack->getName());
		if (nextname == trackname) {
			// already moving to this track
			return;
		}
	}
	if (currenttrack && currenttrack->parent == file) {
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

	playTrack(std::make_shared<MusicTrack>(file, file->tracks[trackname], backend.get()));
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

void MNGMusic::update() {
	if (!currenttrack) {
		return;
	}
	currenttrack->update();
	if (nexttrack && currenttrack->fadedOut()) {
		currenttrack = nexttrack;
		nexttrack.reset();
	}
}

void MNGMusic::setVolume(float volume) {
	// TODO
}

void MNGMusic::stop() {
	// TODO
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
	parent = p;
	sampleno = n->getSampleNumber();
	if (sampleno >= parent->samples.size())
		throw MNGFileException("sample not present");
}

MusicWave::~MusicWave() {
}

AudioChannel MusicWave::play(AudioBackend *backend, bool looping) {
	char *data = parent->samples[sampleno].first;
	unsigned int length = (unsigned int)parent->samples[sampleno].second;
	std::vector<uint8_t> buf(length + 8);
	memcpy(buf.data(), "WAVEfmt ", 8);
	memcpy(buf.data() + 8, data, length);
	// TODO: prettify this, don't depend on SDLMixerBackend
	return backend->playWavData(buf.data(), buf.size(), looping);
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

MusicEffect::MusicEffect(MNGEffectDecNode *node) {
	for (auto &c : *node->children) {
		stages.push_back(std::make_shared<MusicStage>(c));
	}
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
			wave = std::make_shared<MusicWave>(p->getParent()->parent, e);
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
			auto &effects = parent->getParent()->parent->effects;
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

MusicAleotoricLayer::MusicAleotoricLayer(MNGAleotoricLayerNode *node, MusicTrack *p, AudioBackend *b) : MusicLayer(p) {
	backend = b;
	for (auto &n : *node->children) {
		if (MNGEffectNode *e = dynamic_cast<MNGEffectNode *>(n)) {
			if (effect)
				throw MNGFileException("got effect '" + e->getName() + "' but we already have one!");

			// TODO: share effects
			std::map<std::string, class MNGEffectDecNode *> &effects = parent->parent->effects;
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

void MusicAleotoricLayer::update(float track_volume, float track_beatlength) {
	float current_volume = volume * track_volume;

	// TODO: handle updaterate correctly
	// TODO: keep track of playing voices and update them?

	for (auto qw = queued_waves.begin(); qw != queued_waves.end(); ) {
		if (mngclock::now() >= qw->start_time) {
			auto channel = qw->wave->play(backend);
			backend->setChannelVolume(channel, qw->volume * current_volume);
			backend->setChannelPan(channel, qw->pan);
			qw = queued_waves.erase(qw);
		} else {
			qw++;
		}
	}

	if (mngclock::now() < next_update_at) {
		return;
	}

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
		auto voice_effect = voice->getEffect() ? voice->getEffect() : effect;
		// TODO: do effects play the original voice, and then each stage? or just each stage?
		if (voice_effect) {
			mngtimepoint start_offset = mngclock::now();
			for (auto &stage : voice_effect->stages) {
				float volume_value = stage->volume ? evaluateExpression(stage->volume, stage.get()) : 1.0f;

				float pan_value = stage->pan ? evaluateExpression(stage->pan, stage.get()) : 0.0f;
				// TODO: better pan_value calculation
				if (pan != 0.0f) {
					pan_value = pan_value ? (pan + pan_value) / 2.0f : pan;
				}

				float delay_value = stage->delay ? evaluateExpression(stage->delay, stage.get()) : 0.0f;
				if (stage->tempodelay) {
					delay_value += evaluateExpression(stage->tempodelay, stage.get()) * track_beatlength;
				}

				// first as float and then as whatever internal representation...
				start_offset += dseconds(delay_value);
				queued_waves.push_back({voice->getWave(), start_offset, volume_value, pan_value});
			}
		} else {
			auto channel = voice->getWave()->play(backend); // TODO: hold onto this to change params later as layer params update
			backend->setChannelVolume(channel, current_volume);
			backend->setChannelPan(channel, pan);
		}

		/* not sure where this should be run exactly.. see C2's UpperTemple for odd example
		 * GR's source says "These take effect after playback of the voice has begun"
		 * so I try to run it in the same place that code does, for now */
		voice->runUpdateBlock();

		// TODO: this isn't right. we should update offset even if a voice doesn't play,
		// TODO: and also the intervals aren't added, the voice interval overrides the track interval.
		float our_interval = interval + voice->getInterval() + (beatsynch * track_beatlength);
		next_update_at = mngclock::now() + dseconds(our_interval);
	}
}

MusicLoopLayer::MusicLoopLayer(MNGLoopLayerNode *node, MusicTrack *p, AudioBackend *b) : MusicLayer(p) {
	backend = b;
	
	for (auto &n : *node->children) {
		if (MNGWaveNode *e = dynamic_cast<MNGWaveNode *>(n)) {
			wave = std::make_shared<MusicWave>(p->parent, e);
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
}

void MusicLoopLayer::update(float track_volume, float track_beatlength) {
	if (!channel) {
		channel = wave->play(backend, true);
	}

	float current_volume = volume * track_volume;
	backend->setChannelVolume(channel, current_volume);
	backend->setChannelPan(channel, pan);

	if (mngclock::now() >= next_update_at) {
		runUpdateBlock();
		next_update_at = mngclock::now() + dseconds(updaterate);
	}
}

MusicTrack::MusicTrack(MNGFile *p, MNGTrackDecNode *n, AudioBackend *b) {
	node = n;
	parent = p;

	volume = 1.0f;
	// TODO: what's the default fadein/fadeout?
	// for now, changed this from 0.0f to 1.0f because otherwise c3 sounds silly
	fadein = fadeout = 1.0f;
	beatlength = 0.0f;

	for (auto &n : *node->children) {
		if (MNGAleotoricLayerNode *al = dynamic_cast<MNGAleotoricLayerNode *>(n)) {
			layers.push_back(std::dynamic_pointer_cast<MusicLayer>(
				std::make_shared<MusicAleotoricLayer>(al, this, b)
			));
			continue;
		}

		if (MNGLoopLayerNode *ll = dynamic_cast<MNGLoopLayerNode *>(n)) {
			layers.push_back(std::dynamic_pointer_cast<MusicLayer>(
				std::make_shared<MusicLoopLayer>(ll, this, b)
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

float MusicTrack::getCurrentFadeMultiplier() {
	if (fadein_start && fadeout_start) {
		throw creaturesException("Track is both fading in and fading out, this shouldn't happen");
	}
	if (fadein_start) {
		const float time_since_fadein_start = dseconds(mngclock::now() - *fadein_start).count();
		if (time_since_fadein_start < fadein) {
			return time_since_fadein_start / fadein;
		}
		fadein_start = {};
	}
	if (fadeout_start) {
		const float time_since_fadeout_start = dseconds(mngclock::now() - *fadeout_start).count();
		if (time_since_fadeout_start < fadeout) {
			return 1.0 - time_since_fadeout_start / fadeout;
		}
		return 0.0;
	}
	return 1.0;
}

void MusicTrack::update() {
	float our_volume = volume * getCurrentFadeMultiplier();
	for (auto &l : layers) {
		l->update(our_volume, beatlength);
	}
}

void MusicTrack::startFadeIn() {
	if (fadein_start) {
		return;
	}

	// backdate fadein_start so the volume change is seamless
	if (fadeout_start) {
		fadein_start = mngclock::now() - getCurrentFadeMultiplier() * dseconds(fadein);
		fadeout_start = {};
	} else {
		fadein_start = mngclock::now();
	}
}

void MusicTrack::startFadeOut() {
	if (fadeout_start) {
		return;
	}

	// backdate fadeout_start so the volume change is seamless
	if (fadein_start) {
		fadeout_start = mngclock::now() - (1 - getCurrentFadeMultiplier()) * dseconds(fadeout);
		fadein_start = {};
	} else {
		fadeout_start = mngclock::now();
	}
}

bool MusicTrack::fadedOut() {
	return fadeout_start && mngclock::now() >= *fadeout_start + dseconds(fadeout);
}