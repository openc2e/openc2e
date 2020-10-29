#include "audiobackend/SDLMixerBackend.h"
#include "MNGMusic.h"
#include "fileformats/mngfile.h"
#include "utils/ascii_tolower.h"

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <fmt/format.h>
#include <ghc/filesystem.hpp>
#include <mutex>
#include <thread>

namespace fs = ghc::filesystem;

class Event {
public:
  Event() {
      value = false;
  }
  void set() {
      std::lock_guard<std::mutex> lk(mutex);
      value = true;
      condvar.notify_all();
  }
  
  void wait() {
      std::unique_lock<std::mutex> lk(mutex);
      condvar.wait(lk, [&] { return (bool)value; });
  }
  
  std::atomic<bool> value;
  std::mutex mutex;
  std::condition_variable condvar;
};

int main (int argc, char **argv) {
    if (argc != 2 && argc != 3) {
        fmt::print(stderr, "USAGE: {} filename [trackname]\n", argv[0]);
        return 1;
    }

    std::string filename = argv[1];
    if (!fs::exists(filename)) {
        fmt::print(stderr, "File '{}' doesn't exist\n", filename);
        return 1;
    }

    std::string ext = ascii_tolower(fs::path(filename).extension());
    if (ext == ".mng") {
      MNGFile *file = new MNGFile(filename);

      if (argc == 2) {
          fmt::print("Tracks in {}:\n", filename);
          for (auto kv : file->tracks) {
              fmt::print("{}\n", kv.first);
          }
          return 0;
      }

      std::string trackname = argv[2];

      SDLMixerBackend backend;
      backend.init();

      MNGMusic mng_music;
      mng_music.startPlayback(backend);
      mng_music.playTrack(file, trackname);
      if (mng_music.playing_silence) {
          // If the track doesn't exist
          // TODO: better way to check this
          return 1;
      }

      Event sleep_forever;
      sleep_forever.wait();

    } else if (ext == ".mid" || ext == ".midi") {
      SDLMixerBackend backend;
      backend.init();
      backend.setBackgroundMusic(filename);

      Event sleep_forever;
      sleep_forever.wait();

    } else {
      fmt::print(stderr, "Don't know how to play file '{}'\n", filename);
      return 1;
    }
}