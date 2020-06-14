#include "audiobackend/SDLMixerBackend.h"
#include "Catalogue.h"
#include "VoiceData.h"

#include <fstream>
#include <fmt/format.h>
#include <ghc/filesystem.hpp>
#include <thread>

namespace fs = ghc::filesystem;

int main (int argc, char **argv) {
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
    
    SDLMixerBackend backend;
    backend.init();
    
    fmt::print("{} ->\n", sentence);
    std::vector<std::shared_ptr<AudioSource>> clips;
    auto syllables = voices.GetSyllablesFor(sentence);
    for (auto entry : syllables) {
        unsigned int delay_ms = entry.delay_ticks * (is_c2e ? 50 : 100);
        fmt::print("{} {}ms\n", entry.name.size() > 0 ? entry.name : "(silence)", delay_ms);
        
        if (entry.name.size() > 0) {
            auto clip = backend.loadClip(fs::path(datadirectory) / "Sounds" / (entry.name + ".wav"));
            clip->play();
            clips.push_back(clip); // don't cut off the clip before it's done
        }
            
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // don't cut off any clips
}