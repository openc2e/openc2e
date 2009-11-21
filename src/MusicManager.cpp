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
}

MusicManager::~MusicManager() {
	for (std::map<std::string, MNGFile *>::iterator i = files.begin(); i != files.end(); i++) {
		delete i->second;
	}
}

void MusicManager::tick() {
}

void MusicManager::playTrack(std::string track, unsigned int latency) {
	std::string filename, trackname;

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
	if (files.find(realfilename) == files.end()) {
		file = new MNGFile(realfilename);
		files[filename] = file;
	} else {
		file = files[realfilename];
	}

	if (file->tracks.find(trackname) == file->tracks.end()) {
		std::cout << "Couldn't find MNG track '" << trackname << "' ('" << track << "')!" << std::endl;
		return; // TODO: exception?
	}

	shared_ptr<MusicTrack> tracknode(new MusicTrack(file, file->tracks[trackname]));
	tracknode->init();
	playTrack(tracknode);
}

void MusicManager::playTrack(shared_ptr<MusicTrack> track) {
	currenttrack = track;

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

	currenttrack->update();
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
	data = (unsigned char *)p->samples[sampleno].first;
	length = (unsigned int)p->samples[sampleno].second;
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

FloatAudioBuffer MusicStage::applyStage(FloatAudioBuffer src, float beatlength) {
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

	float left_pan = 1.0f - pan_value;
	float right_pan = 1.0f + pan_value;

	float *data = new float[src.len];
	for (unsigned int i = 0; i < src.len / 2; i++) {
		data[i*2] = src.data[i*2] * left_pan * volume_value;
		data[(i*2)+1] = src.data[(i*2)+1] * right_pan * volume_value;
	}

	FloatAudioBuffer buffer(data, src.len, src.start_offset + (22050 * 2 * delay_value));
	return buffer;
}

MusicEffect::MusicEffect(MNGEffectDecNode *n) {
	node = n;

	for (std::list<MNGStageNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		shared_ptr<MusicStage> stage(new MusicStage(*i));
		stages.push_back(stage);
	}
}

void MusicEffect::applyEffect(shared_ptr<class MusicTrack> t, FloatAudioBuffer src, float beatlength) {
	for (std::vector<shared_ptr<MusicStage> >::iterator i = stages.begin(); i != stages.end(); i++) {
		FloatAudioBuffer buffer = (*i)->applyStage(src, beatlength);
		t->addBuffer(buffer);

		// stagger delays
		src.start_offset = buffer.start_offset;
	}
}

MusicVoice::MusicVoice(shared_ptr<MusicLayer> p, MNGVoiceNode *n) {
	node = n;
	parent = p;

	interval = NULL;

	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGWaveNode *e = dynamic_cast<MNGWaveNode *>(n);
		if (e) {
			wave = shared_ptr<MusicWave>(new MusicWave(p->getParent()->getParent(), e));
			continue;
		}

		MNGIntervalNode *in = dynamic_cast<MNGIntervalNode *>(n);
		if (in) {
			interval = in->getExpression();
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

float MusicVoice::getInterval() {
	if (!interval) return 0.0f;

	return evaluateExpression(interval, NULL, this);
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
			case VOLUME:
			case PAN:
				throw MNGFileException("panic"); // TODO?
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

void MusicAleotoricLayer::update() {
	unsigned int parent_offset = parent->getCurrentOffset();

	// TODO: adjust for buffering
	if (next_offset > parent_offset) return;

	unsigned int min_time = 0, max_time = 0;
	for (std::vector<shared_ptr<MusicVoice> >::iterator i = voices.begin(); i != voices.end(); i++) {
		float our_interval = interval + (*i)->getInterval() + (beatsynch * parent->getBeatLength());
		unsigned int voice_interval = 22050 * 2 * our_interval;
		unsigned int wave_len = 0;
		if ((*i)->getWave()) wave_len = (*i)->getWave()->getLength();

		if (wave_len > voice_interval) {
			if (max_time < min_time + wave_len)
				max_time = min_time + wave_len;
		} else {
			if (max_time < min_time + voice_interval)
				max_time = min_time + voice_interval;
		}
		min_time += voice_interval;
	}

	FloatAudioBuffer buffer(new float[max_time * 2], max_time * 2, parent_offset);
	memset(buffer.data, 0, buffer.len * 2);
	unsigned int offset = 0;
	float our_volume = volume * parent->getVolume();
	// TODO: perhaps we shouldn't be applying pan on top of effects?
	float left_pan = (1.0f - pan) * our_volume;
	float right_pan = (1.0f + pan) * our_volume;
	for (std::vector<shared_ptr<MusicVoice> >::iterator i = voices.begin(); i != voices.end(); i++) {
		if (!(*i)->shouldPlay()) continue;

		if ((*i)->getWave()) {
			signed short *data = (signed short *)(*i)->getWave()->getData();
			unsigned int len = (*i)->getWave()->getLength();
			for (unsigned int j = 0; j < len / 2; j++) {
				buffer.data[offset + j*2] += (float)data[j] * left_pan;
				buffer.data[offset + (j*2)+1] += (float)data[j] * right_pan;
			}
		}
		float our_interval = interval + (*i)->getInterval() + (beatsynch * parent->getBeatLength());
		offset += 22050 * 2 * our_interval;
		assert(offset <= max_time * 2);

		/* not sure where this should be run exactly.. see C2's UpperTemple for odd example
		 * GR's source says "These take effect after playback of the voice has begun"
		 * so I try to run it in the same place that code does, for now */
		(*i)->runUpdateBlock();
	}

	if (!effect) {
		parent->addBuffer(buffer);
	} else {
		effect->applyEffect(parent, buffer, parent->getBeatLength());
		delete[] buffer.data;
	}

	runUpdateBlock();

	next_offset = parent_offset + offset;
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

void MusicLoopLayer::update() {
	if (!wave) return;

	unsigned int parent_offset = parent->getCurrentOffset();

	// TODO: adjust for buffering
	if (next_offset > parent_offset) return;

	float our_volume = volume * parent->getVolume();
	float left_pan = (1.0f - pan) * our_volume;
	float right_pan = (1.0f + pan) * our_volume;

	unsigned int len = wave->getLength();
	signed short *data = (signed short *)wave->getData();
	FloatAudioBuffer buffer(new float[len], len, parent_offset);
	for (unsigned int j = 0; j < len / 2; j++) {
		buffer.data[j*2] += (float)data[j] * left_pan;
		buffer.data[(j*2)+1] += (float)data[j] * right_pan;
	}
	parent->addBuffer(buffer);

	next_offset = parent_offset + len;

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
	fadein = fadeout = 0.0f;
	beatlength = 0.0f;
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
}

void MusicTrack::update() {
	for (std::vector<shared_ptr<MusicLayer> >::iterator i = layers.begin(); i != layers.end(); i++) {
		(*i)->update();
	}
}

void MusicTrack::render(signed short *data, size_t len) {
	float output[len];
	for (unsigned int i = 0; i < len; i++) output[i] = 0.0f;

	// mix pending buffers, render
	unsigned int numbuffers = 0;
	for (int i = 0; i < (int)buffers.size(); i++) {
		FloatAudioBuffer &buffer = buffers[i];
		unsigned int j = 0;
		numbuffers++; // we include queued buffers for now..
		if (buffer.start_offset > current_offset) {
			// buffer hasn't started (quite) yet
			if (buffer.start_offset + len > current_offset)
				continue;
			j = (buffer.start_offset - current_offset) * 2;
		}
		for (; j < len && buffer.position < buffer.len; j++) {
			output[j] += buffer.data[buffer.position];
			buffer.position++;
		}
		if (buffer.position == buffer.len) {
			delete[] buffer.data;
			buffers.erase(buffers.begin() + i);
			i--;
		}
	}
	float mul = (1.0f/numbuffers) * 0.8f; // TODO: this is a hack to try and avoid clipping
	for (unsigned int i = 0; i < len; i++) {
		output[i] *= mul;
	}
	for (unsigned int i = 0; i < len; i++) {
		data[i] = (signed short)output[i];
	}

	current_offset += len; // measuring offset in terms of samples*2 is horrid!
}

/* vim: set noet: */
