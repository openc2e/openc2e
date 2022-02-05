#pragma once

#include <map>
#include <string>
#include <utility>

class Scriptorium {
  public:
	void add(int family, int genus, int species, int eventno, std::string text) {
		m_scripts[std::make_tuple(family, genus, species, eventno)] = text;
	}

	std::string get(int family, int genus, int species, int eventno) {
		auto it = m_scripts.find(std::make_tuple(family, genus, species, eventno));
		if (it == m_scripts.end()) {
			return "";
		}
		return it->second;
	}

  private:
	std::map<std::tuple<int, int, int, int>, std::string> m_scripts;
};