#include "AudioBackend.h"

#include "NullAudioBackend.h"
#include "common/Exception.h"

static AudioBackend* s_audio_backend = NullAudioBackend::get_instance();

AudioBackend* get_audio_backend() {
	return s_audio_backend;
}
void set_audio_backend(AudioBackend* audio_backend) {
	if (s_audio_backend != NullAudioBackend::get_instance()) {
		// TODO: ???
		throw Exception("set_audio_backend: AudioBackend has already been set!");
	}
	s_audio_backend = audio_backend;
}