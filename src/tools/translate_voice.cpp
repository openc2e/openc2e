#include "openc2e/Catalogue.h"
#include "openc2e/VoiceData.h"
#include "sdlbackend/SDLMixerBackend.h"

#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <thread>

namespace fs = ghc::filesystem;

int main(int argc, char** argv) {
	if (argc != 4) {
		fmt::print(stderr, "USAGE: {} datadirectory voice sentence\n", argv[0]);
		return 1;
	}

	std::string datadirectory = argv[1];
	std::string voice = argv[2];
	std::string sentence = argv[3];

	if (!fs::exists(datadirectory)) {
		fmt::print(stderr, "Data directory '{}' doesn't exist\n", datadirectory);
		return 1;
	}

	VoiceData voices;
	bool is_c2e = false;
	if (fs::exists(fs::path(datadirectory) / "Catalogue")) {
		is_c2e = true;
		catalogue.initFrom(fs::path(datadirectory) / "Catalogue", "en");
		voices = VoiceData(voice);
	} else {
		std::ifstream in(fs::path(datadirectory) / (voice + ".vce"));
		voices = VoiceData(in);
	}
	if (!voices) {
		fmt::print(stderr, "Couldn't find voice\n");
		return 1;
	}

	auto backend = SDLMixerBackend::getInstance();
	backend->init();

	fmt::print("{} ->\n", sentence);
	std::vector<AudioChannel> channels;
	auto syllables = voices.GetSyllablesFor(sentence);
	for (auto entry : syllables) {
		unsigned int delay_ms = entry.delay_ticks * (is_c2e ? 50 : 100);
		fmt::print("{} {}ms\n", entry.name.size() > 0 ? entry.name : "(silence)", delay_ms);

		if (entry.name.size() > 0) {
			channels.push_back(backend->play_clip(fs::path(datadirectory) / "Sounds" / (entry.name + ".wav")));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	}
	while (true) {
		if (std::all_of(channels.begin(), channels.end(), [&](auto c) {
				return backend->audio_channel_get_state(c) == AUDIO_STOPPED;
			})) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}