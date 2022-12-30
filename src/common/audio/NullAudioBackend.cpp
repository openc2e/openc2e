#include "NullAudioBackend.h"

AudioBackend* NullAudioBackend::get_instance() {
	static NullAudioBackend s_null_audio_backend;
	return &s_null_audio_backend;
}