#include "prayfile/PraySourceParser.h"
#include "prayfile/PrayFileWriter.h"

#include <iostream>
#include <map>
#include <string>

const std::string EXAMPLE1 = R"(

"en-GB"

group EGGS "Fire Norn"
"Agent Type" 0
"Script Count" 0

"Genetics File" "norn.fire*"
"Egg Glyph File" "firemale.c16"
"Egg Glyph File 2" "firefemale.c16"
"Egg Gallery male" "firemale"
"Egg Gallery female" "firefemale"
"Egg Animation String" "0"

"Dependency Count" 4
"Dependency 1" "firefemale.c16"
"Dependency Category 1" 2
"Dependency 2" "firemale.c16"
"Dependency Category 2" 2
"Dependency 3" "norn.fire.gen"
"Dependency Category 3" 3
"Dependency 4" "norn.fire.gno"
"Dependency Category 4" 3

inline FILE "firefemale.c16" "firefemale.c16"
inline FILE "firemale.c16" "firemale.c16"
inline FILE "norn.fire.gen" "norn.fire.gen"
inline FILE "norn.fire.gno" "norn.fire.gno"

)";

const std::string EXAMPLE2 = R"(

    (- comment -)

    "en-GB"

    group AGNT "Bubble Blower <C3>"
    "Agent Type" 0
    "Agent Description" "Let your norns blow bubbles all over!"

    "Agent Animation File" "bubble blower.c16"
    "Agent Animation Gallery" "bubble blower"
    "Agent Animation String" "1 2 3 4 3 2 255"

    "Remove script" "enum 2 21 999 kill targ next enum 2 21 1000 kill targ next"

    "Script Count" 1
    "Script 1" @ "bubble blower.cos"

    "Dependency Count" 2
    "Dependency 1" "bubble blower.c16"
    "Dependency Category 1" 2
    "Dependency 2" "bubble blower.catalogue"
    "Dependency Category 2" 7

    group DSAG "Bubble Blower <DS>"
    "Agent Type" 0
    "Agent Description" "Let your norns blow bubbles all over!"

    "Agent Animation File" "bubble blower.c16"
    "Agent Animation Gallery" "bubble blower"
    "Agent Animation String" "1 2 3 4 3 2 255"
    "Agent Sprite First Image" 0

    "Remove script" "enum 2 21 999 kill targ next enum 2 21 1000 kill targ next"

    "Script Count" 1
    "Script 1" @ "bubble blower.cos"

    "Dependency Count" 2
    "Dependency 1" "bubble blower.c16"
    "Dependency Category 1" 2
    "Dependency 2" "bubble blower.catalogue"
    "Dependency Category 2" 7

    inline FILE "bubble blower.c16" "bubble blower.c16"
    inline FILE "bubble blower.catalogue" "bubble blower.catalogue"
)";

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
      if (argc != 3) {
          printf("USAGE: %s INPUT OUTPUT\n", argv[0]);
          return 1;
      }
      
    std::ifstream f(argv[1]);
    std::string str((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());
      
    PraySourceParser parser(str.c_str());

    fs::path parent_path = fs::path(argv[1]).parent_path();

    std::map<std::string, std::string> string_tags;
    std::map<std::string, int> int_tags;

    PrayFileWriter writer((std::string(argv[2])));

    while (true) {
      auto res = parser.next();

      if (mpark::holds_alternative<PraySourceParser::Error>(res)) {
        std::cout << "Error: "
                  << mpark::get<PraySourceParser::Error>(res).message << "\n";
        return 1;
      }

      if (mpark::holds_alternative<PraySourceParser::EndOfInput>(res)) {
        break;
      }

      visit_overloads(
          res, [](PraySourceParser::Error) {},
          [](PraySourceParser::EndOfInput) {},
          [&](PraySourceParser::GroupBlockStart) {
            string_tags = {};
            int_tags = {};
          },
          [&](PraySourceParser::GroupBlockEnd event) {
              writer.writeBlockTags(event.type, event.name, int_tags, string_tags);
            std::cout << "Group of type " << event.type << ", name \""
                      << event.name << "\"\n";
            for (auto kv : int_tags) {
              std::cout << "  Integer tag \"" << kv.first << "\" -> "
                        << kv.second << "\n";
            }
            for (auto kv : string_tags) {
              std::cout << "  String tag \"" << kv.first << "\" -> "
                        << kv.second << "\n";
            }
          },
          [&](PraySourceParser::InlineBlock event) {
            std::cout << "Inlining file data into block " << event.type << " \""
                      << event.name << "\" from file \"" << event.filename
                      << "\"\n";
            
            // TODO: check in same directory
          std::ifstream in((parent_path / event.filename).string());
          printf("open \"%s\"\n", (parent_path / event.filename).string().c_str());
          if (!in) abort();
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
            if (!in) abort();
            std::string val((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
                            
            string_tags[event.key] = val;
          },
          [&](PraySourceParser::IntegerTag event) {
            int_tags[event.key] = event.value;
          });
    }
  }
}
