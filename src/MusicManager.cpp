/*
 *  MusicManager.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Oct 31 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "MusicManager.h"
#include "World.h"
#include "AudioBackend.h"
#include "Engine.h"
#include <iostream> // for debug messages
using namespace boost;
#include <cmath> // for cos/sin
using namespace std;

// this is all for MusicManager::tick
#include "World.h"
#include "Camera.h"
#include "Room.h"
#include "MetaRoom.h"

MusicManager musicmanager;

struct MusicStream : public AudioStreamBase {
	virtual bool isStereo() const { return true; }
	virtual int sampleRate() const { return 22050; }
	virtual int latency() const { return 1000; }
	virtual bool reset() { return true; }
	virtual int bitDepth() const { return 16; }

	virtual size_t produce(void *data, size_t len) {
		musicmanager.render((signed short *)data, len / 2);
		return len;
	}
};

MusicManager::MusicManager() {
	current_latency = 0;
}

MusicManager::~MusicManager() {
	for (std::map<std::string, MNGFile *>::iterator i = files.begin(); i != files.end(); i++) {
		delete i->second;
	}
}

void MusicManager::tick() {
	if (current_latency) {
		current_latency--;
	}
	if (!current_latency) {
		// TODO: this behaviour is different in C2, and should probably be cleverer
		MetaRoom *m = world.camera->getMetaRoom();
		if (m) {
			shared_ptr<Room> r = m->roomAt(world.camera->getXCentre(), world.camera->getYCentre());
			if (r && r->music.size()) {
				playTrack(r->music, 0);
			} else if (m->music.size()) {
				playTrack(m->music, 0);
			}
		}
	}

	if (nexttrack && currenttrack->fadedOut()) {
		currenttrack = nexttrack;
		nexttrack.reset();
	}
}

void MusicManager::playTrack(std::string track, unsigned int latency) {
	std::string filename, trackname;

	// seconds -> world ticks
	latency = (latency * 1000) / world.ticktime;

	if (track == "Silence") {
		playing_silence = true;
		current_latency = latency;
		currenttrack->startFadeOut();
		nexttrack.reset();
		return;
	}

	std::string::size_type n = track.find("\\");
	if (n == std::string::npos) {
		filename = "Music.mng"; // TODO
		trackname = track;
	} else {
		filename = track.substr(0, n);
		trackname = track.substr(n + 1, std::string::npos);
	}

	std::string realfilename = world.findFile("Sounds/" + filename);
	if (!realfilename.size()) {
		std::cout << "Couldn't find MNG file '" << filename << "'!" << std::endl;
		return; // TODO: exception?
	}

	MNGFile *file;
	std::transform(filename.begin(), filename.end(), filename.begin(), (int(*)(int))tolower);
	if (files.find(filename) == files.end()) {
		file = new MNGFile(realfilename);
		files[filename] = file;
	} else {
		file = files[filename];
	}

	std::transform(trackname.begin(), trackname.end(), trackname.begin(), (int(*)(int))tolower);

	// TODO: these lowercase transformations are ridiculous, we should store inside MusicTrack
	if (nexttrack && nexttrack->getParent() == file) {
		std::string nextname = nexttrack->getName();
		std::transform(nextname.begin(), nextname.end(), nextname.begin(), (int(*)(int))tolower);
		if (nextname == trackname) {
			// already moving to this track
			return;
		}
	}
	if (currenttrack && currenttrack->getParent() == file) {
		std::string thisname = currenttrack->getName();
		std::transform(thisname.begin(), thisname.end(), thisname.begin(), (int(*)(int))tolower);
		if (thisname == trackname) {
			// already playing this track!
			if (!playing_silence && !nexttrack) return;
			nexttrack.reset();
			currenttrack->startFadeIn();
			return;
		}
	}

	if (file->tracks.find(trackname) == file->tracks.end()) {
		std::cout << "Couldn't find MNG track '" << trackname << "' ('" << track << "')!" << std::endl;
		return; // TODO: exception?
	}

	shared_ptr<MusicTrack> tracknode(new MusicTrack(file, file->tracks[trackname]));
	tracknode->init();
	playTrack(tracknode);
	current_latency = latency;
}

void MusicManager::playTrack(shared_ptr<MusicTrack> track) {
	playing_silence = false;
	track->startFadeIn();
	if (!currenttrack) {
		currenttrack = track;
	} else {
		nexttrack = track;
		currenttrack->startFadeOut();
	}

	startPlayback();
}

void MusicManager::startPlayback() {
	shared_ptr<AudioSource> src = engine.audio->getBGMSource();
	if (!src) return;

	if (!stream) {
		// we assume no-one else ever steals the BGM stream from
		// under us, but what are we meant to do then anyway?
		stream = shared_ptr<MusicStream>(new MusicStream);
		src->setStream(stream);
		src->play();
	}
}

void MusicManager::render(signed short *data, size_t len) {
	if (!currenttrack) {
		// silence
		memset((void *)data, 0, len * 2);
		return;
	}

	currenttrack->update(len);
	currenttrack->render(data, len);
}

float evaluateExpression(MNGExpression *e, MusicStage *stage = NULL, MusicVoice *voice = NULL, MusicLayer *layer = NULL) {
	MNGVariableNode *v = dynamic_cast<MNGVariableNode *>(e);
	if (v) {
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

	MNGConstantNode *c = dynamic_cast<MNGConstantNode *>(e);
	if (c) {
		return c->getValue();
	}

	MNGAddNode *add = dynamic_cast<MNGAddNode *>(e);
	if (add) {
		return evaluateExpression(add->first(), stage, voice, layer)
			+ evaluateExpression(add->second(), stage, voice, layer);
	}

	MNGSubtractNode *sub = dynamic_cast<MNGSubtractNode *>(e);
	if (sub) {
		return evaluateExpression(sub->first(), stage, voice, layer)
			- evaluateExpression(sub->second(), stage, voice, layer);
	}

	MNGMultiplyNode *mul = dynamic_cast<MNGMultiplyNode *>(e);
	if (mul) {
		return evaluateExpression(mul->first(), stage, voice, layer)
			* evaluateExpression(mul->second(), stage, voice, layer);
	}

	MNGDivideNode *div = dynamic_cast<MNGDivideNode *>(e);
	if (div) {
		return evaluateExpression(div->first(), stage, voice, layer)
			/ evaluateExpression(div->second(), stage, voice, layer);
	}

	MNGSineWaveNode *sinewave = dynamic_cast<MNGSineWaveNode *>(e);
	if (sinewave) {
		return sin(2 * M_PI * (evaluateExpression(sinewave->first(), stage, voice, layer)
			/ evaluateExpression(sinewave->second(), stage, voice, layer)));
	}

	MNGCosineWaveNode *cosinewave = dynamic_cast<MNGCosineWaveNode *>(e);
	if (cosinewave) {
		return cos(2 * M_PI * (evaluateExpression(cosinewave->first(), stage, voice, layer)
			/ evaluateExpression(cosinewave->second(), stage, voice, layer)));
	}

	MNGRandomNode *r = dynamic_cast<MNGRandomNode *>(e);
	if (r) {
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
	signed short *data = (signed short *)p->samples[sampleno].first;
	unsigned int length = (unsigned int)p->samples[sampleno].second;
	buffer = FloatAudioBuffer(new float[length], length);
	for (unsigned int i = 0; i < length / 2; i++) {
		buffer.data[i*2] = data[i];
		buffer.data[(i*2) + 1] = data[i];
	}
}

MusicWave::~MusicWave() {
	delete[] buffer.data;
}

MusicStage::MusicStage(MNGStageNode *n) {
	node = n;
	pan = NULL;
	volume = NULL;
	delay = NULL;
	tempodelay = NULL;

	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGPanNode *p = dynamic_cast<MNGPanNode *>(n);
		if (p) {
			pan = p->getExpression();
			continue;
		}

		MNGEffectVolumeNode *v = dynamic_cast<MNGEffectVolumeNode *>(n);
		if (v) {
			volume = v->getExpression();
			continue;
		}

		MNGDelayNode *d = dynamic_cast<MNGDelayNode *>(n);
		if (d) {
			delay = d->getExpression();
			continue;
		}

		MNGTempoDelayNode *td = dynamic_cast<MNGTempoDelayNode *>(n);
		if (td) {
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

	unsigned int offset_amt = 22050 * 2 * delay_value;
	std::vector<FloatAudioBuffer> buffers;
	for (std::vector<FloatAudioBuffer>::iterator i = sources.begin(); i != sources.end(); i++) {
		FloatAudioBuffer &src = *i;
		src.start_offset += offset_amt;
		volume_value *= src.volume;
		// TODO: better pan_value calculation
		if (src.pan != 0.0f) {
			if (pan_value) pan_value = (src.pan + pan_value) / 2.0f;
			else pan_value = src.pan;
		}
		buffers.push_back(FloatAudioBuffer(src.data, src.len, src.start_offset, volume_value, pan_value));
	}

	return buffers;
}

MusicEffect::MusicEffect(MNGEffectDecNode *n) {
	node = n;

	for (std::list<MNGStageNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		shared_ptr<MusicStage> stage(new MusicStage(*i));
		stages.push_back(stage);
	}
}

std::vector<FloatAudioBuffer> MusicEffect::applyEffect(shared_ptr<class MusicTrack> t, std::vector<FloatAudioBuffer> src, float beatlength) {
	std::vector<FloatAudioBuffer> buffers;

	for (std::vector<shared_ptr<MusicStage> >::iterator i = stages.begin(); i != stages.end(); i++) {
		std::vector<FloatAudioBuffer> newbuffers = (*i)->applyStage(src, beatlength);
		for (std::vector<FloatAudioBuffer>::iterator j = newbuffers.begin(); j != newbuffers.end(); j++) {
			buffers.push_back(*j);
		}
	}

	return buffers;
}

MusicVoice::MusicVoice(shared_ptr<MusicLayer> p, MNGVoiceNode *n) {
	node = n;
	parent = p;

	interval = 0.0f;
	interval_expression = NULL;
	volume = 1.0f;

	updatenode = NULL;

	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGWaveNode *e = dynamic_cast<MNGWaveNode *>(n);
		if (e) {
			// TODO: share duplicate MusicWaves
			wave = shared_ptr<MusicWave>(new MusicWave(p->getParent()->getParent(), e));
			continue;
		}

		MNGIntervalNode *in = dynamic_cast<MNGIntervalNode *>(n);
		if (in) {
			interval_expression = in->getExpression();
			continue;
		}

		MNGConditionNode *c = dynamic_cast<MNGConditionNode *>(n);
		if (c) {
			conditions.push_back(c);
			continue;
		}

		MNGUpdateNode *u = dynamic_cast<MNGUpdateNode *>(n);
		if (u) {
			updatenode = u;
			continue;
		}

		MNGEffectNode *eff = dynamic_cast<MNGEffectNode *>(n);
		if (eff) {
			if (effect)
				throw MNGFileException("got effect '" + eff->getName() + "' but we already have one!");

			// TODO: share effects
			std::map<std::string, class MNGEffectDecNode *> &effects = parent->getParent()->getParent()->effects;
			if (effects.find(eff->getName()) == effects.end())
				throw MNGFileException("couldn't find effect '" + eff->getName() + "'");

			MNGEffectDecNode *n = effects[eff->getName()];
			effect = shared_ptr<MusicEffect>(new MusicEffect(n));
			continue;
		}

		throw MNGFileException("unexpected node in Voice: " + n->dump());
	}
}

bool MusicVoice::shouldPlay() {
	for (std::vector<MNGConditionNode *>::iterator i = conditions.begin(); i != conditions.end(); i++) {
		MNGConditionNode *n = *i;

		float value = evaluateExpression(n->getVariable(), NULL, this);
		if (value < n->minimum() || value > n->maximum())
			return false;
	}
	return true;
}

MusicLayer::MusicLayer(shared_ptr<MusicTrack> p) {
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

	for (std::list<MNGAssignmentNode *>::iterator i = updatenode->children->begin(); i != updatenode->children->end(); i++) {
		MNGAssignmentNode *n = *i;

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

	for (std::list<MNGAssignmentNode *>::iterator i = updatenode->children->begin(); i != updatenode->children->end(); i++) {
		MNGAssignmentNode *n = *i;

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

MusicAleotoricLayer::MusicAleotoricLayer(MNGAleotoricLayerNode *n, shared_ptr<MusicTrack> p) : MusicLayer(p) {
	node = n;
}

void MusicAleotoricLayer::init() {
	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGEffectNode *e = dynamic_cast<MNGEffectNode *>(n);
		if (e) {
			if (effect)
				throw MNGFileException("got effect '" + e->getName() + "' but we already have one!");

			// TODO: share effects
			std::map<std::string, class MNGEffectDecNode *> &effects = parent->getParent()->effects;
			if (effects.find(e->getName()) == effects.end())
				throw MNGFileException("couldn't find effect '" + e->getName() + "'");

			MNGEffectDecNode *n = effects[e->getName()];
			effect = shared_ptr<MusicEffect>(new MusicEffect(n));
			continue;
		}

		MNGVoiceNode *v = dynamic_cast<MNGVoiceNode *>(n);
		if (v) {
			shared_ptr<MusicVoice> voice(new MusicVoice(shared_from_this(), v));
			voices.push_back(voice);
			continue;
		}

		MNGUpdateNode *u = dynamic_cast<MNGUpdateNode *>(n);
		if (u) {
			updatenode = u;
			continue;
		}

		MNGLayerVolumeNode *lv = dynamic_cast<MNGLayerVolumeNode *>(n);
		if (lv) {
			volume = evaluateExpression(lv->getExpression());
			continue;
		}

		MNGUpdateRateNode *ur = dynamic_cast<MNGUpdateRateNode *>(n);
		if (ur) {
			updaterate = evaluateExpression(ur->getExpression());
			continue;
		}

		MNGVariableDecNode *vd = dynamic_cast<MNGVariableDecNode *>(n);
		if (vd) {
			std::string name = vd->getName();
			float value = evaluateExpression(vd->getExpression());
			variables[name] = value;
			continue;
		}

		MNGBeatSynchNode *bs = dynamic_cast<MNGBeatSynchNode *>(n);
		if (bs) {
			beatsynch = evaluateExpression(bs->getExpression());
			continue;
		}

		MNGIntervalNode *in = dynamic_cast<MNGIntervalNode *>(n);
		if (in) {
			interval = evaluateExpression(in->getExpression());
			continue;
		}

		throw MNGFileException("unexpected node in AleotoricLayer: " + n->dump());
	}

	runUpdateBlock();
}

void MusicAleotoricLayer::update(unsigned int latency) {
	unsigned int parent_offset = parent->getCurrentOffset();

	if (next_offset > parent_offset + latency) return;
	unsigned int offset = next_offset;

	std::vector<FloatAudioBuffer> buffers;

	float our_volume = volume * parent->getVolume();
	for (std::vector<shared_ptr<MusicVoice> >::iterator i = voices.begin(); i != voices.end(); i++) {
		if (!(*i)->shouldPlay()) continue;

		if ((*i)->getWave()) {
			FloatAudioBuffer &data = (*i)->getWave()->getData();
			FloatAudioBuffer voicebuffer = FloatAudioBuffer(data.data, data.len, offset, our_volume, pan);
			shared_ptr<MusicEffect> voice_effect = (*i)->getEffect();
			if (voice_effect) {
				std::vector<FloatAudioBuffer> newbuffers;
				newbuffers.push_back(voicebuffer);
				newbuffers = voice_effect->applyEffect(parent, newbuffers, parent->getBeatLength());
				for (std::vector<FloatAudioBuffer>::iterator i = newbuffers.begin(); i != newbuffers.end(); i++) {
					buffers.push_back(*i);
				}
			} else {
				buffers.push_back(voicebuffer);
			}
		}

		/* not sure where this should be run exactly.. see C2's UpperTemple for odd example
		 * GR's source says "These take effect after playback of the voice has begun"
		 * so I try to run it in the same place that code does, for now */
		(*i)->runUpdateBlock();

		float our_interval = interval + (*i)->getInterval() + (beatsynch * parent->getBeatLength());
		offset += 22050 * 2 * our_interval;
	}

	if (effect) {
		buffers = effect->applyEffect(parent, buffers, parent->getBeatLength());
	}
	for (std::vector<FloatAudioBuffer>::iterator i = buffers.begin(); i != buffers.end(); i++) {
		parent->addBuffer(*i);
	}

	runUpdateBlock();

	next_offset = offset;
}

MusicLoopLayer::MusicLoopLayer(MNGLoopLayerNode *n, shared_ptr<MusicTrack> p) : MusicLayer(p) {
	node = n;
	update_period = 0;
}

void MusicLoopLayer::init() {
	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGWaveNode *e = dynamic_cast<MNGWaveNode *>(n);
		if (e) {
			// TODO: share duplicate MusicWaves
			wave = shared_ptr<MusicWave>(new MusicWave(parent->getParent(), e));
			continue;
		}

		MNGUpdateRateNode *ur = dynamic_cast<MNGUpdateRateNode *>(n);
		if (ur) {
			updaterate = evaluateExpression(ur->getExpression());
			continue;
		}

		MNGVariableDecNode *vd = dynamic_cast<MNGVariableDecNode *>(n);
		if (vd) {
			std::string name = vd->getName();
			float value = evaluateExpression(vd->getExpression());
			variables[name] = value;
			continue;
		}

		MNGUpdateNode *u = dynamic_cast<MNGUpdateNode *>(n);
		if (u) {
			updatenode = u;
			continue;
		}

		throw MNGFileException("unexpected node in LoopLayer: " + n->dump());
	}

	runUpdateBlock();
}

void MusicLoopLayer::update(unsigned int latency) {
	if (!wave) return;

	unsigned int parent_offset = parent->getCurrentOffset();

	if (next_offset > parent_offset + latency) return;

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
}

// shared_from_this
void MusicTrack::init() {
	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGAleotoricLayerNode *al = dynamic_cast<MNGAleotoricLayerNode *>(n);
		if (al) {
			MusicAleotoricLayer *ptr = new MusicAleotoricLayer(al, shared_from_this());
			shared_ptr<MusicLayer> mal(ptr);
			ptr->init();
			layers.push_back(mal);
			continue;
		}

		MNGLoopLayerNode *ll = dynamic_cast<MNGLoopLayerNode *>(n);
		if (ll) {
			MusicLoopLayer *ptr = new MusicLoopLayer(ll, shared_from_this());
			shared_ptr<MusicLayer> mll(ptr);
			ptr->init();
			layers.push_back(mll);
			continue;
		}

		MNGFadeInNode *fi = dynamic_cast<MNGFadeInNode *>(n);
		if (fi) {
			fadein = evaluateExpression(fi->getExpression());
			continue;
		}

		MNGFadeOutNode *fo = dynamic_cast<MNGFadeOutNode *>(n);
		if (fo) {
			fadeout = evaluateExpression(fo->getExpression());
			continue;
		}

		MNGBeatLengthNode *bl = dynamic_cast<MNGBeatLengthNode *>(n);
		if (bl) {
			beatlength = evaluateExpression(bl->getExpression());
			continue;
		}

		MNGLayerVolumeNode *lv = dynamic_cast<MNGLayerVolumeNode *>(n);
		if (lv) {
			volume = evaluateExpression(lv->getExpression());
			continue;
		}

		throw MNGFileException("unexpected node in Track: " + n->dump());
	}
}

MusicTrack::~MusicTrack() {
	for (int i = 0; i < (int)buffers.size(); i++) {
		delete[] buffers[i].data;
	}
}

void MusicTrack::update(unsigned int latency) {
	for (std::vector<shared_ptr<MusicLayer> >::iterator i = layers.begin(); i != layers.end(); i++) {
		(*i)->update(latency);
	}
}

void MusicTrack::startFadeIn() {
	if (fadein_count) return;
	if (fadeout_count) {
		fadein_count = (fadein * 22050 * 2) * (1.0 - (float)(fadeout_count / (fadeout * 22050 * 2)));
		fadeout_count = 0;
	} else fadein_count = fadein * 22050 * 2;
}

void MusicTrack::startFadeOut() {
	if (fadeout_count) return;
	if (fadein_count) {
		fadeout_count = (fadeout * 22050 * 2) * (1.0 - (float)(fadein_count / (fadein * 22050 * 2)));
		fadein_count = 0;
	} else fadeout_count = fadeout * 22050 * 2;
	if (!fadeout_count) fadeout_count = 1;
}

bool MusicTrack::fadedOut() {
	return fadeout_count == 1;
}

void MusicTrack::render(signed short *data, size_t len) {
	float *output = (float *)alloca(len * sizeof(float));
	for (unsigned int i = 0; i < len; i++) output[i] = 0.0f;

	// mix pending buffers, render
	//unsigned int numbuffers = 0;
	for (int i = 0; i < (int)buffers.size(); i++) {
		FloatAudioBuffer &buffer = buffers[i];
		unsigned int j = 0;
		if (buffer.start_offset > current_offset) {
			// buffer hasn't started (quite) yet
			if (buffer.start_offset > current_offset + len)
				continue;
			j = buffer.start_offset - current_offset;
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

	current_offset += len; // measuring offset in terms of samples*2 is horrid!
}

/* vim: set noet: */
