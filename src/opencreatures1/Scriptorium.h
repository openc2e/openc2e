#pragma once

#include "common/Exception.h"
#include "common/NumericCast.h"

#include <map>
#include <string>
#include <utility>

struct ScriptClassifier {
	ScriptClassifier(int family_, int genus_, int species_, int eventno_)
		: family(numeric_cast<uint8_t>(family_)),
		  genus(numeric_cast<uint8_t>(genus_)),
		  species(numeric_cast<uint8_t>(species_)),
		  eventno(numeric_cast<uint8_t>(eventno_)) {}
	uint8_t family;
	uint8_t genus;
	uint8_t species;
	uint8_t eventno;

	bool operator<(const ScriptClassifier& other) const {
		if (family != other.family)
			return family < other.family;
		if (genus != other.genus)
			return genus < other.genus;
		if (species != other.species)
			return species < other.species;
		return eventno < other.eventno;
	}
};

class Scriptorium {
  public:
	void add(int family, int genus, int species, int eventno, std::string text) {
		if (family == 1 && genus == 0 && species == 0) {
			throw Exception("Can't register a script for Scenery");
		}
		if (family >= 5) {
			throw Exception("Can't register a script using an unknown family number");
		}
		if (genus >= 256) {
			throw Exception("Can't register a script using a genus number >= 256");
		}
		if (species >= 256) {
			throw Exception("Can't request a script using a species number >= 256");
		}
		if (eventno > 72) {
			throw Exception("Can't register a script using an unknown event number");
		}
		if (m_scripts.size() >= 600) {
			throw Exception("Scriptorium can only hold 600 scripts");
		}

		ScriptClassifier classifier{family, genus, species, eventno};
		auto it = m_scripts.find(classifier);
		if (it != m_scripts.end()) {
			throw Exception("Can't register a duplicate script");
		}

		m_scripts[classifier] = text;
	}

	std::string get(int family, int genus, int species, int eventno) {
		if (family >= 5) {
			throw Exception("Can't request a script using an unknown family number");
		}
		if (genus >= 256) {
			throw Exception("Can't request a script using a genus number >= 256");
		}
		if (species >= 256) {
			throw Exception("Can't request a script using a species number >= 256");
		}
		if (eventno > 72) {
			throw Exception("Can't request a script using an unknown event number");
		}

		auto it = m_scripts.find(ScriptClassifier{family, genus, species, eventno});
		if (it == m_scripts.end()) {
			return "";
		}
		return it->second;
	}

	auto begin() const {
		return m_scripts.begin();
	}

	auto end() const {
		return m_scripts.end();
	}

  private:
	std::map<ScriptClassifier, std::string> m_scripts;
};