#include "AudioBackend.h"

#include "common/Exception.h"

static AudioBackend* s_audio_backend = nullptr;

AudioBackend* get_audio_backend() {
	return s_audio_backend;
}
void set_audio_backend(AudioBackend* audio_backend) {
	if (s_audio_backend) {
		// TODO: ???
		throw Exception("set_audio_backend: AudioBackend has already been set!");
	}
	s_audio_backend = audio_backend;
}