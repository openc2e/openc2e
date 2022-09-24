#pragma once

#include "common/Exception.h"

#include <map>
#include <string>
#include <utility>

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

		auto classifier = std::make_tuple(family, genus, species, eventno);
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

		auto it = m_scripts.find(std::make_tuple(family, genus, species, eventno));
		if (it == m_scripts.end()) {
			return "";
		}
		return it->second;
	}

  private:
	std::map<std::tuple<int, int, int, int>, std::string> m_scripts;
};