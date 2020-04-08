#include "prayfile/Caos2PrayParser.h"
#include "prayfile/PraySourceParser.h"
#include "prayfile/PrayFileWriter.h"

#include <iostream>
#include <map>
#include <string>

template <class... Fs> struct overloaded;

template <class F0, class... Frest>
struct overloaded<F0, Frest...> : F0, overloaded<Frest...> {
  overloaded(F0 f0, Frest... rest) : F0(f0), overloaded<Frest...>(rest...) {}
  using F0::operator();
  using overloaded<Frest...>::operator();
};

template <class F0> struct overloaded<F0> : F0 {
  overloaded(F0 f0) : F0(f0) {}
  using F0::operator();
};

template <typename... Fs>
overloaded<Fs...> make_overloaded_function(Fs... args) {
  return overloaded<Fs...>(args...);
};

template <typename V, typename... Fs> void visit_overloads(V v, Fs... fs) {
  return visit(make_overloaded_function(fs...), v);
}

#include <fstream>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

int main(int argc, char**argv) {
  {
      if (!(argc == 2 || argc == 3)) {
          printf("USAGE: %s INPUT [OUTPUT]\n", argv[0]);
          return 1;
      }
      
    std::ifstream f(argv[1]);
    std::string str((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());

    fs::path parent_path = fs::path(argv[1]).parent_path();

    std::string output_filename;
    if (argc == 3) {
        output_filename = argv[2];
    }
    std::vector<PraySourceParser::Event> events;
    if (fs::path(argv[1]).extension() == ".txt") {
        events = PraySourceParser::parse(str);
    } else if (fs::path(argv[1]).extension() == ".cos") {
        events = Caos2PrayParser::parse(str, output_filename.size() ? nullptr : &output_filename);
    } else {
        std::cout << "Don't know how to handle input file \"" << argv[1] << "\"" << std::endl;
        exit(1);
    }
    if (!output_filename.size()) {
        output_filename = fs::path(argv[1]).stem().string() + ".agents";
    }
    
    if (mpark::holds_alternative<PraySourceParser::Error>(events[0])) {
        std::cout << "Error: "
                  << mpark::get<PraySourceParser::Error>(events[0]).message << "\n";
        return 1;
    }

    std::cout << "Writing output to \"" << output_filename << "\"" << std::endl;
    PrayFileWriter writer(output_filename);

    std::map<std::string, std::string> string_tags;
    std::map<std::string, int> int_tags;
    
    for (auto res : events) {
      visit_overloads(
          res, [](PraySourceParser::Error) {
              /* handled already */
          },
          [&](PraySourceParser::ClearAccumulatedTags) {
            string_tags = {};
            int_tags = {};
          },
          [&](PraySourceParser::GroupBlock event) {
            writer.writeBlockTags(event.type, event.name, int_tags, string_tags);
            std::cout << "Tag block " << event.type << " \""
                      << event.name << "\"\n";
          },
          [&](PraySourceParser::InlineBlock event) {
              std::cout << "Inline block " << event.type << " \""
                      << event.name << "\" from file \"" << event.filename
                      << "\"\n";
            
            // TODO: check in same directory
          std::ifstream in((parent_path / event.filename).string());
          if (!in) {
              std::cout << "Couldn't open file \""
                        << (parent_path / event.filename).string() << "\""
                        << std::endl;
              exit(1);
          }
          std::vector<char> data((std::istreambuf_iterator<char>(in)),
                          std::istreambuf_iterator<char>());
                          
            writer.writeBlockRawData(event.type, event.name, data.data(), data.size());
            
          },
          [&](PraySourceParser::StringTag event) {
            string_tags[event.key] = event.value;
          },
          [&](PraySourceParser::StringTagFromFile event) {
            // TODO: check in same directory
            std::ifstream in((parent_path / event.filename).string());
            if (!in) {
                std::cout << "Couldn't open file \""
                          << (parent_path / event.filename).string() << "\""
                          << std::endl;
                exit(1);
            }
            std::string val((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
                            
            string_tags[event.key] = val;
          },
          [&](PraySourceParser::IntegerTag event) {
            int_tags[event.key] = event.value;
          });
    }

    std::cout << "Done!" << std::endl;
  }
}
