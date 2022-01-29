#include "MNGMusic.h"

#include "common/ascii_tolower.h"
#include "common/endianlove.h"
#include "common/find_if.h"

#include <algorithm>
#include <cmath> // for cos/sin
#include <iostream> // for debug messages

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

using namespace mngtoktype;

MNGMusic::MNGMusic(const std::shared_ptr<AudioBackend>& b)
	: backend(b) {
}
MNGMusic::~MNGMusic() {
	stop();
}

void MNGMusic::playSilence() {
	playing_silence = true;
	if (currenttrack) {
		currenttrack->startFadeOut();
	}
	nexttrack.reset();
}

void MNGMusic::playTrack(MNGFile* file, std::string trackname) {
	trackname = ascii_tolower(trackname);

	// TODO: these lowercase transformations are ridiculous, we should store inside MusicTrack
	if (nexttrack && nexttrack->file == file) {
		std::string nextname = ascii_tolower(nexttrack->getName());
		if (nextname == trackname) {
			// already moving to this track
			return;
		}
	}
	if (currenttrack && currenttrack->file == file) {
		std::string thisname = ascii_tolower(currenttrack->getName());
		if (thisname == trackname) {
			// already playing this track!
			if (!playing_silence && !nexttrack)
				return;
			nexttrack.reset();
			currenttrack->startFadeIn();
			return;
		}
	}

	auto parsed_script = mngparse(file->script);
	auto track = find_if(parsed_script.tracks, [&](const auto& t) { return ascii_tolower(t.name) == trackname; });
	if (!track) {
		std::cout << "Couldn't find MNG track '" << trackname << "' ('" << file->name << "')!" << std::endl;
		return; // TODO: exception?
	}

	playTrack(std::make_shared<MusicTrack>(this, file, parsed_script, *track, backend.get()));
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
	currenttrack->update(volume);
	if (nexttrack && currenttrack->fadedOut()) {
		currenttrack = nexttrack;
		nexttrack.reset();
	}
}

void MNGMusic::setVolume(float volume_) {
	if (volume_ != volume) {
		volume = volume_;
		update();
	}
}

void MNGMusic::setMood(float mood_) {
	// TODO: adjust slowly?
	if (mood_ != mood) {
		mood = mood_;
	}
}

void MNGMusic::setThreat(float threat_) {
	// TODO: adjust slowly?
	if (threat_ != threat) {
		threat = threat_;
	}
}

void MNGMusic::stop() {
	if (currenttrack) {
		currenttrack->stop();
		currenttrack.reset();
	}
	if (nexttrack) {
		nexttrack.reset();
	}
}

static float evaluateExpression(const MNGExpression& e, MusicLayer* layer = nullptr) {
	if (auto* float_value = mpark::get_if<float>(&e)) {
		return *float_value;
	} else if (auto* variable = mpark::get_if<std::string>(&e)) {
		if (layer == nullptr) {
			throw MNGFileException("Variable '" + *variable + "' only valid inside Layer");
		}
		return layer->getVariable(*variable);
	} else if (auto* function_p = mpark::get_if<heap_value<MNGFunction>>(&e)) {
		auto function = *function_p;
		switch (function->type) {
			case MNG_ADD:
				return evaluateExpression(function->first, layer) + evaluateExpression(function->second, layer);
			case MNG_SUBTRACT:
				return evaluateExpression(function->first, layer) - evaluateExpression(function->second, layer);
			case MNG_MULTIPLY:
				return evaluateExpression(function->first, layer) * evaluateExpression(function->second, layer);
			case MNG_DIVIDE:
				return evaluateExpression(function->first, layer) / evaluateExpression(function->second, layer);
			case MNG_SINEWAVE:
				return sin(2 * M_PI * (evaluateExpression(function->first, layer) / evaluateExpression(function->second, layer)));
			case MNG_COSINEWAVE:
				return cos(2 * M_PI * (evaluateExpression(function->first, layer) / evaluateExpression(function->second, layer)));
			case MNG_RANDOM: {
				float first = evaluateExpression(function->first, layer);
				float second = evaluateExpression(function->second, layer);
				return ((float)rand() / (float)RAND_MAX) * (second - first) + first;
			}
			default:
				// TODO: set up actual function names enum
				// TODO: e->dump
				throw MNGFileException("couldn't evaluate expression");
				break;
		}
	} else {
		std::terminate();
	}
}

static AudioChannel playSample(const std::string& name, MNGFile* file, AudioBackend* backend, bool looping = false) {
	const auto& sample = file->samples[file->getSampleForName(name)];
	return backend->playWavData(sample.data(), sample.size(), looping);
}

MusicStage::MusicStage(MNGStage node) {
	pan = node.pan.value_or(0.0);
	volume = node.volume.value_or(1.0);
	delay = node.delay.value_or(1.0); // TODO: default? error if doesn't have one of these or tempodelay?
	tempodelay = node.delay.value_or(1.0); // TODO: default?
}

MusicEffect::MusicEffect(MNGEffect node) {
	name = node.name;
	for (auto& s : node.stages) {
		stages.push_back(std::make_shared<MusicStage>(s));
	}
}

MusicVoice::MusicVoice(MusicLayer* p, MNGVoice node) {
	parent = p;

	wave = node.wave;
	conditions = node.conditions;

	if (node.effect) {
		auto toplevel_effect = find_if(parent->parent->effects,
			[&](auto e) { return ascii_tolower(e->name) == ascii_tolower(*node.effect); });
		if (!toplevel_effect) {
			throw MNGFileException("couldn't find effect '" + *node.effect + "'");
		}
		effect = *toplevel_effect;
	}
	interval = node.interval;
	updates = node.updates;
}

bool MusicVoice::shouldPlay() {
	for (auto& n : conditions) {
		float value = parent->getVariable(n.variable);
		float minimum = evaluateExpression(n.minimum, parent);
		float maximum = evaluateExpression(n.maximum, parent);
		if (value < minimum || value > maximum) {
			return false;
		}
	}
	return true;
}

void MusicLayer::runUpdateBlock() {
	for (auto update : updates) {
		setVariable(update.variable, evaluateExpression(update.value, this));
	}
}

void MusicVoice::runUpdateBlock() {
	for (auto kv : updates) {
		parent->setVariable(kv.variable, evaluateExpression(kv.value, parent));
	}
}

MusicAleotoricLayer::MusicAleotoricLayer(MNGAleotoricLayer node, MusicTrack* p, AudioBackend* b) {
	parent = p;
	backend = b;
	name = node.name;

	volume = node.volume.value_or(1.0);
	if (node.effect) {
		auto toplevel_effect = find_if(parent->effects,
			[&](auto e) { return ascii_tolower(e->name) == ascii_tolower(*node.effect); });
		if (!toplevel_effect) {
			throw MNGFileException("couldn't find effect '" + *node.effect + "'");
		}
		effect = *toplevel_effect;
	}
	beatsynch = node.beatsynch; // TODO: default?
	updaterate = node.updaterate.value_or(0.0); // TODO: default?
	interval = node.interval.value_or(1.0); // TODO: default?
	variables = node.variables;
	updates = node.updates;
	for (auto v : node.voices) {
		voices.push_back(std::make_shared<MusicVoice>(this, v));
	}

	// TODO: hack
	variables["Mood"] = 1.0f;
	variables["Threat"] = 0.5f;

	runUpdateBlock();
}

float MusicAleotoricLayer::getVariable(std::string name) {
	if (name == "Volume") {
		return volume;
	} else if (name == "Interval") {
		return interval;
	} else if (name == "Pan") {
		throw Exception("'Pan' is not a valid variable inside AleotoricLayer");
	} else if (name == "Mood") {
		return parent->parent->mood;
	} else if (name == "Threat") {
		return parent->parent->threat;
	} else {
		return variables[name];
	}
}

void MusicAleotoricLayer::setVariable(std::string name, float value) {
	if (name == "Volume") {
		volume = value;
	} else if (name == "Interval") {
		interval = value;
	} else if (name == "Pan") {
		throw Exception("'Pan' is not a valid variable inside AleotoricLayer");
	} else if (name == "Mood") {
		throw Exception("'Mood' cannot be set inside AleotoricLayer");
	} else if (name == "Threat") {
		throw Exception("'Threat' cannot be set inside AleotoricLayer");
	} else {
		variables[name] = value;
	}
}

void MusicAleotoricLayer::stop() {
	for (auto pw : playing_waves) {
		backend->stopChannel(pw.channel);
	}
	playing_waves = {};
	queued_waves = {};
}

void MusicAleotoricLayer::update(float track_volume, float track_beatlength) {
	float current_volume = volume * track_volume;

	for (auto pw = playing_waves.begin(); pw != playing_waves.end();) {
		if (backend->getChannelState(pw->channel) == AUDIO_PLAYING) {
			backend->setChannelVolume(pw->channel, pw->volume * current_volume);
			pw++;
		} else {
			pw = playing_waves.erase(pw);
		}
	}
	for (auto qw = queued_waves.begin(); qw != queued_waves.end();) {
		if (mngclock::now() >= qw->start_time) {
			auto channel = playSample(qw->wave_name, parent->file, backend);
			backend->setChannelVolume(channel, qw->volume * current_volume);
			backend->setChannelPan(channel, qw->pan);
			playing_waves.push_back({channel, qw->volume});
			qw = queued_waves.erase(qw);
		} else {
			qw++;
		}
	}

	if (mngclock::now() >= next_update_at) {
		// I think this is how it works.. otherwise why allow UpdateRate in AleotoricLayer?
		runUpdateBlock();
		next_update_at = mngclock::now() + dseconds(updaterate);
	}

	if (mngclock::now() < next_voice_at) {
		return;
	}

	auto voice = [&] {
		decltype(voices) available_voices;
		for (auto& voice : voices) {
			if (!voice->shouldPlay())
				continue;
			if (last_voice.get() == voice.get())
				continue;
			available_voices.push_back(voice);
		}
		if (available_voices.size()) {
			return last_voice = available_voices[rand() % available_voices.size()];
		}
		// try to avoid playing the same voice twice in a row, it sounds awful and
		// doesn't seem to happen in the real engine
		if (last_voice && last_voice->shouldPlay()) {
			return last_voice;
		}
		return std::shared_ptr<MusicVoice>();
	}();

	if (voice) {
		auto our_effect = voice->effect ? voice->effect : effect;
		// TODO: do effects play the original voice, and then each stage? or just each stage?
		if (our_effect) {
			mngtimepoint start_offset = mngclock::now();
			for (auto& stage : our_effect->stages) {
				float volume_value = stage->volume ? evaluateExpression(*stage->volume) : 1.0f;

				float pan_value = stage->pan ? evaluateExpression(*stage->pan) : 0.0f;

				float delay_value = stage->delay ? evaluateExpression(*stage->delay) : 0.0f;
				if (stage->tempodelay) {
					delay_value += evaluateExpression(*stage->tempodelay) * track_beatlength;
				}

				// first as float and then as whatever internal representation...
				start_offset += dseconds(delay_value);
				queued_waves.push_back({voice->wave, start_offset, volume_value, pan_value});
			}
		} else {
			auto channel = playSample(voice->wave, parent->file, backend);
			backend->setChannelVolume(channel, current_volume);
			playing_waves.push_back({channel, 1.0});
		}
		/* not sure where this should be run exactly.. see C2's UpperTemple for odd example
		 * GR's source says "These take effect after playback of the voice has begun"
		 * so I try to run it in the same place that code does, for now */
		voice->runUpdateBlock();
	}

	// Voices' Update blocks can alter the Layer Interval, so keep this after the Voice Update block
	float our_interval = [&] {
		if (voice && voice->interval) {
			return evaluateExpression(*voice->interval, this);
		}
		if (beatsynch) {
			return *beatsynch * track_beatlength;
		}
		return interval;
	}();
	next_voice_at = mngclock::now() + dseconds(our_interval);
}

MusicLoopLayer::MusicLoopLayer(MNGLoopLayer node, MusicTrack* p, AudioBackend* b) {
	parent = p;
	backend = b;

	wave = node.wave;
	volume = node.volume.value_or(1.0);
	updaterate = node.updaterate.value_or(0); // TODO: what should the default be?
	variables = node.variables;
	updates = node.updates;

	// TODO: hack
	variables["Mood"] = 1.0f;
	variables["Threat"] = 0.5f;
}

float MusicLoopLayer::getVariable(std::string name) {
	if (name == "Volume") {
		return volume;
	} else if (name == "Interval") {
		throw Exception("'Interval' is not a valid variable inside LoopLayer");
	} else if (name == "Pan") {
		return pan;
	} else if (name == "Mood") {
		return parent->parent->mood;
	} else if (name == "Threat") {
		return parent->parent->threat;
	} else {
		return variables[name];
	}
}

void MusicLoopLayer::setVariable(std::string name, float value) {
	if (name == "Volume") {
		volume = value;
	} else if (name == "Interval") {
		throw Exception("'Interval' is not a valid variable inside LoopLayer");
	} else if (name == "Pan") {
		pan = value;
	} else if (name == "Mood") {
		throw Exception("'Mood' cannot be set inside LoopLayer");
	} else if (name == "Threat") {
		throw Exception("'Threat' cannot be set inside LoopLayer");
	} else {
		variables[name] = value;
	}
}

void MusicLoopLayer::update(float track_volume) {
	if (!channel) {
		channel = playSample(wave, parent->file, backend, true);
	}

	float current_volume = volume * track_volume;
	backend->setChannelVolume(channel, current_volume);
	backend->setChannelPan(channel, pan);

	if (mngclock::now() >= next_update_at) {
		runUpdateBlock();
		next_update_at = mngclock::now() + dseconds(updaterate);
	}
}

void MusicLoopLayer::stop() {
	backend->stopChannel(channel);
	channel = {};
}

MusicTrack::MusicTrack(MNGMusic* p, MNGFile* f, MNGScript script, MNGTrack n, AudioBackend* b) {
	node = n;
	file = f;
	parent = p;

	volume = n.volume.value_or(1.0);
	// TODO: what's the default fadein/fadeout?
	// for now, changed this from 0.0f to 1.0f because otherwise c3 sounds silly
	fadein = n.fadein.value_or(1.0);
	fadeout = n.fadeout.value_or(1.0);
	beatlength = n.beatlength.value_or(0.0f);

	for (auto e : script.effects) {
		effects.push_back(std::make_shared<MusicEffect>(e));
	}
	for (auto l : n.layers) {
		if (MNGLoopLayer* ll = mpark::get_if<MNGLoopLayer>(&l)) {
			looplayers.push_back(std::make_shared<MusicLoopLayer>(*ll, this, b));
		} else if (MNGAleotoricLayer* al = mpark::get_if<MNGAleotoricLayer>(&l)) {
			aleotoriclayers.push_back(std::make_shared<MusicAleotoricLayer>(*al, this, b));
		} else {
			std::terminate();
		}
	}
}

float MusicTrack::getCurrentFadeMultiplier() {
	if (fadein_start && fadeout_start) {
		throw Exception("Track is both fading in and fading out, this shouldn't happen");
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

void MusicTrack::update(float system_volume) {
	float our_volume = system_volume * volume * getCurrentFadeMultiplier();
	for (auto ll : looplayers) {
		ll->update(our_volume);
	}
	for (auto al : aleotoriclayers) {
		al->update(our_volume, beatlength);
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

void MusicTrack::stop() {
	for (auto ll : looplayers) {
		ll->stop();
	}
	for (auto al : aleotoriclayers) {
		al->stop();
	}
}