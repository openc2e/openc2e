#include "MNGMusic.h"

#include "endianlove.h"
#include "utils/ascii_tolower.h"
#include "utils/find_if.h"
#include <algorithm>
#include <cmath> // for cos/sin
#include <iostream> // for debug messages

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

using namespace mngtoktype;

MNGMusic::MNGMusic(const std::shared_ptr<AudioBackend>& b) : backend(b) {}
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

	auto parsed_script = mngparse(file->script);
	auto track = find_if(parsed_script.tracks, [&](const auto &t){ return ascii_tolower(t.name) == trackname; });
	if (!track) {
		std::cout << "Couldn't find MNG track '" << trackname << "' ('" << file->name << "')!" << std::endl;
		return; // TODO: exception?
	}

	playTrack(std::make_shared<MusicTrack>(file, parsed_script, *track, backend.get()));
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
	switch (e.type) {
		case MNGExpression::MNGEXPRESSION_FLOAT:
			return e.getFloat();
		case MNGExpression::MNGEXPRESSION_STRING:
		{
			std::string variable = e.getString();
			if (layer == nullptr) {
				throw MNGFileException("Variable '" + variable + "' only valid inside Layer");
			}
			return layer->getVariable(variable);
		}
		case MNGExpression::MNGEXPRESSION_FUNCTION:
		{
			const auto& function = e.getFunction();
			switch (function.type) {
			case MNG_ADD:
				return evaluateExpression(function.first, layer)
					+ evaluateExpression(function.second, layer);
			case MNG_SUBTRACT:
				return evaluateExpression(function.first, layer)
					- evaluateExpression(function.second, layer);
			case MNG_MULTIPLY:
				return evaluateExpression(function.first, layer)
					* evaluateExpression(function.second, layer);
			case MNG_DIVIDE:
				return evaluateExpression(function.first, layer)
					/ evaluateExpression(function.second, layer);
			case MNG_SINEWAVE:
				return sin(2 * M_PI * (evaluateExpression(function.first, layer)
					/ evaluateExpression(function.second, layer)));
			case MNG_COSINEWAVE:
				return cos(2 * M_PI * (evaluateExpression(function.first, layer)
					/ evaluateExpression(function.second, layer)));
			case MNG_RANDOM:
			{
				float first = evaluateExpression(function.first, layer);
				float second = evaluateExpression(function.second, layer);
				return ((float)rand() / (float)RAND_MAX) * (second - first) + first;
			}
			default:
				// TODO: set up actual function names enum
				break;
			}
		}
	}
	// TODO: e->dump
	throw MNGFileException("couldn't evaluate expression");
}

static AudioChannel playSample(const std::string& name, MNGFile *file, AudioBackend* backend, bool looping=false) {
	unsigned int sampleno = file->getSampleForName(name);
	auto *data = file->samples[sampleno].data();
	auto length = file->samples[sampleno].size();
	std::vector<uint8_t> buf(length + 8);
	memcpy(buf.data(), "WAVEfmt ", 8);
	memcpy(buf.data() + 8, data, length);
	// TODO: prettify this
	return backend->playWavData(buf.data(), buf.size(), looping);
}

MusicStage::MusicStage(MNGStage node) {
	pan = node.pan.value_or(0.0);
	volume = node.volume.value_or(1.0);
	delay = node.delay.value_or(1.0); // TODO: default? error if doesn't have one of these or tempodelay?
	tempodelay = node.delay.value_or(1.0); // TODO: default?
}

MusicEffect::MusicEffect(MNGEffect node) {
	name = node.name;
	for (auto &s : node.stages) {
		stages.push_back(std::make_shared<MusicStage>(s));
	}
}

MusicVoice::MusicVoice(MusicLayer *p, MNGVoice node) {
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
	for (auto &n : conditions) {
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
		getVariable(update.variable) = evaluateExpression(update.value, this);
	}
}

void MusicVoice::runUpdateBlock() {
	for (auto kv : updates) {
		parent->getVariable(kv.variable) = evaluateExpression(kv.value, parent);
	}
}

MusicAleotoricLayer::MusicAleotoricLayer(MNGAleotoricLayer node, MusicTrack *p, AudioBackend *b) {
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

float& MusicAleotoricLayer::getVariable(std::string name) {
	if (name == "Volume") {
		return volume;
	}
	if (name == "Interval") {
		return interval;
	}
	if (name == "Pan") {
		throw creaturesException("'Pan' is not a valid variable inside AleotoricLayer");
	}
	return variables[name];
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

	for (auto pw = playing_waves.begin(); pw != playing_waves.end(); ) {
		if (backend->getChannelState(pw->channel) == AUDIO_PLAYING) {
			backend->setChannelVolume(pw->channel, pw->volume * current_volume);
			pw++;
		} else {
			pw = playing_waves.erase(pw);
		}
	}
	for (auto qw = queued_waves.begin(); qw != queued_waves.end(); ) {
		if (mngclock::now() >= qw->start_time) {
			auto channel = playSample(qw->wave_name, parent->parent, backend);
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
		for (auto &voice : voices) {
			if (!voice->shouldPlay()) continue;
			if (last_voice.get() == voice.get()) continue;
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
			for (auto &stage : our_effect->stages) {
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
			auto channel = playSample(voice->wave, parent->parent, backend);
			backend->setChannelVolume(channel, current_volume);
			playing_waves.push_back({channel, 1.0});
		}
		/* not sure where this should be run exactly.. see C2's UpperTemple for odd example
		 * GR's source says "These take effect after playback of the voice has begun"
		 * so I try to run it in the same place that code does, for now */
		voice->runUpdateBlock();
	}

	// Voices' Update blocks can alter the Layer Interval, so keep this after the Voice Update block
	float our_interval = [&]{
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

MusicLoopLayer::MusicLoopLayer(MNGLoopLayer node, MusicTrack *p, AudioBackend *b) {
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

float& MusicLoopLayer::getVariable(std::string name) {
	if (name == "Volume") {
		return volume;
	}
	if (name == "Interval") {
		throw creaturesException("'Interval' is not a valid variable inside LoopLayer");
	}
	if (name == "Pan") {
		return pan;
	}
	return variables[name];
}

void MusicLoopLayer::update(float track_volume) {
	if (!channel) {
		channel = playSample(wave, parent->parent, backend, true);
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

MusicTrack::MusicTrack(MNGFile *p, MNGScript script, MNGTrack n, AudioBackend *b) {
	node = n;
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
		switch (l.type) {
			case MNGLayer::LOOPING:
				looplayers.push_back(std::make_shared<MusicLoopLayer>(l.looplayer, this, b));
				break;
			case MNGLayer::ALEOTORIC:
				aleotoriclayers.push_back(std::make_shared<MusicAleotoricLayer>(l.aleotoriclayer, this, b));
				break;
		}
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