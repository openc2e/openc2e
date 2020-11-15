#pragma once

#include "audiobackend/AudioState.h"

class Sound {
public:
	Sound();
	operator bool();
	void fadeOut();
	void stop();
	void setPosition(float x, float y, float width, float height);
	void setMuted(bool);
	AudioState getState();
	void setVolume(float);
private:
	friend class SoundManager;
	int index = -1;
	int generation = -1;
};