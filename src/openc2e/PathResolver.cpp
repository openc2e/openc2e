/*
 *  PathResolver.cpp
 *  openc2e
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "PathResolver.h"

#include "Engine.h"
#include "World.h"
#include "caosValue.h"
#include "common/Ascii.h"
#include "common/Exception.h"
#include "common/case_insensitive_filesystem.h"
#include "common/wildcard_match.h"

#include <assert.h>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <pwd.h> // getpwuid
#include <sys/types.h> // passwd*
#endif

namespace fs = ghc::filesystem;

DataDirectory::DataDirectory(fs::path main_)
	: main(main_) {
}

std::vector<DataDirectory> data_directories;

std::vector<fs::path> getMainDirectories() {
	std::vector<fs::path> result;
	for (auto& d : data_directories) {
		result.push_back(d.main);
	}
	return result;
}

enum FileDirectory {
	DIRECTORY_MAIN,
	DIRECTORY_BACKGROUNDS,
	DIRECTORY_BODY_DATA,
	DIRECTORY_BOOTSTRAP,
	DIRECTORY_CATALOGUE,
	DIRECTORY_CREATURE_GALLERIES,
	DIRECTORY_EXPORTED_CREATURES,
	DIRECTORY_GENETICS,
	DIRECTORY_IMAGES,
	DIRECTORY_JOURNAL,
	DIRECTORY_OVERLAY_DATA,
	DIRECTORY_AGENTS,
	DIRECTORY_SOUNDS,
	DIRECTORY_USERS,
	DIRECTORY_WORLDS
};

static const fs::path& nonempty_or(const fs::path& first, const fs::path& second) {
	if (!first.empty()) {
		return first;
	}
	return second;
}

static fs::path getDirectory(const DataDirectory& d, FileDirectory type) {
	switch (type) {
		case DIRECTORY_MAIN: return d.main;
		case DIRECTORY_BACKGROUNDS: return nonempty_or(d.backgrounds, d.main / "Backgrounds");
		case DIRECTORY_BODY_DATA: return nonempty_or(d.body_data, d.main / "Body Data");
		case DIRECTORY_BOOTSTRAP: return nonempty_or(d.bootstrap, d.main / "Bootstrap");
		case DIRECTORY_CATALOGUE: return nonempty_or(d.catalogue, d.main / "Catalogue");
		case DIRECTORY_CREATURE_GALLERIES: return nonempty_or(d.creature_galleries, d.main / "Creature Galleries");
		case DIRECTORY_EXPORTED_CREATURES: {
			if (engine.gametype == "sm") {
				return nonempty_or(d.exported_creatures, d.main / "Exported Creatures");
			}
			return nonempty_or(d.exported_creatures, d.main / "My Creatures");
		}
		case DIRECTORY_GENETICS: return nonempty_or(d.genetics, d.main / "Genetics");
		case DIRECTORY_IMAGES: return nonempty_or(d.images, d.main / "Images");
		case DIRECTORY_JOURNAL: return nonempty_or(d.journal, d.main / "Journal");
		case DIRECTORY_OVERLAY_DATA: return nonempty_or(d.overlay_data, d.main / "Overlay Data");
		case DIRECTORY_AGENTS: {
			if (engine.gametype == "sm") {
				return nonempty_or(d.agents, d.main / "Resource Files");
			}
			return nonempty_or(d.agents, d.main / "My Agents");
		}
		case DIRECTORY_SOUNDS: return nonempty_or(d.sounds, d.main / "Sounds");
		case DIRECTORY_USERS: return nonempty_or(d.users, d.main / "Users");
		case DIRECTORY_WORLDS: return nonempty_or(d.worlds, d.main / "My Worlds");
	}
}

static fs::path findFile(FileDirectory type, fs::path name) {
	// TODO: check user directory before or after data directories?
	for (auto d : data_directories) {
		std::error_code err;
		fs::path resolved = case_insensitive_filesystem::canonical(getDirectory(d, type) / name, err);
		if (!err) {
			return resolved;
		}
	}
	return {};
}

static std::vector<fs::path> findByWildcard(FileDirectory type, std::string wild) {
	std::vector<fs::path> results;
	// TODO: check user directory before or after data directories?
	for (auto d : data_directories) {
		auto dirname = getDirectory(d, type);
		if (!fs::path(wild).parent_path().empty()) {
			dirname /= fs::path(wild).parent_path();
			wild = fs::path(wild).filename().string();
		}
		for (const auto& entry : case_insensitive_filesystem::directory_iterator(dirname)) {
			if (wildcard_match_ignore_case(wild, entry.lexically_relative(dirname).native())) {
				results.push_back(entry);
			}
		}
	}
	return results;
}

fs::path findMainDirectoryFile(fs::path path) {
	return findFile(DIRECTORY_MAIN, path);
}

fs::path findBackgroundFile(fs::path path) {
	return findFile(DIRECTORY_BACKGROUNDS, path);
}

fs::path findBodyDataFile(fs::path path) {
	return findFile(DIRECTORY_BODY_DATA, path);
}

fs::path findCatalogueFile(fs::path path) {
	return findFile(DIRECTORY_CATALOGUE, path);
}

fs::path findCobFile(fs::path path) {
	if (engine.version == 2) {
		return findFile(DIRECTORY_MAIN, fs::path("Objects") / path);
	} else {
		return findFile(DIRECTORY_MAIN, path);
	}
}

fs::path findGeneticsFile(fs::path path) {
	return findFile(DIRECTORY_GENETICS, path);
}

fs::path findImageFile(fs::path path) {
	return findFile(DIRECTORY_IMAGES, path);
}

fs::path findJournalFile(fs::path path) {
	return findFile(DIRECTORY_JOURNAL, path);
}

fs::path findOverlayDataFile(fs::path path) {
	return findFile(DIRECTORY_OVERLAY_DATA, path);
}

fs::path findSoundFile(fs::path path) {
	return findFile(DIRECTORY_SOUNDS, path);
}

fs::path getCurrentWorldJournalPath(fs::path filename) {
	if (!fs::exists(getUserDataDir() / "Journal")) {
		fs::create_directory(getUserDataDir() / "Journal");
	}
	return getUserDataDir() / "Journal" / filename;
}

fs::path getMainJournalPath(fs::path filename) {
	if (!fs::exists(getUserDataDir() / "Journal")) {
		fs::create_directory(getUserDataDir() / "Journal");
	}
	return getUserDataDir() / "Journal" / filename;
}

fs::path getOtherWorldJournalPath(fs::path) {
	// TODO
	throw Exception("getOtherWorldJournalPath not implemented");
}

std::ofstream createUserBackgroundFile(fs::path name) {
	fs::path dir = getUserDataDir() / "Backgrounds";
	fs::create_directory(dir);
	return case_insensitive_filesystem::ofstream(dir / name);
}

std::ofstream createUserBodyDataFile(fs::path name) {
	fs::path dir = getUserDataDir() / "Body Data";
	fs::create_directory(dir);
	return case_insensitive_filesystem::ofstream(dir / name);
}

std::ofstream createUserCatalogueFile(fs::path name) {
	fs::path dir = getUserDataDir() / "Catalogue";
	fs::create_directory(dir);
	return case_insensitive_filesystem::ofstream(dir / name);
}

std::ofstream createUserGeneticsFile(fs::path name) {
	fs::path dir = getUserDataDir() / "Genetics";
	fs::create_directory(dir);
	return case_insensitive_filesystem::ofstream(dir / name);
}

std::ofstream createUserImageFile(fs::path name) {
	fs::path dir = getUserDataDir() / "Images";
	fs::create_directory(dir);
	return case_insensitive_filesystem::ofstream(dir / name);
}

std::ofstream createUserOverlayDataFile(fs::path name) {
	fs::path dir = getUserDataDir() / "Overlay Data";
	fs::create_directory(dir);
	return case_insensitive_filesystem::ofstream(dir / name);
}

std::ofstream createUserSoundFile(fs::path name) {
	fs::path dir = getUserDataDir() / "Sounds";
	fs::create_directory(dir);
	return case_insensitive_filesystem::ofstream(dir / name);
}

std::vector<fs::path> findAgentFiles(std::string wild) {
	return findByWildcard(DIRECTORY_AGENTS, wild);
}

std::vector<fs::path> findCobFiles(std::string wild) {
	if (engine.version == 2) {
		return findByWildcard(DIRECTORY_MAIN, fs::path("Objects") / wild);
	} else {
		return findByWildcard(DIRECTORY_MAIN, wild);
	}
}

std::vector<fs::path> findGeneticsFiles(std::string wild) {
	return findByWildcard(DIRECTORY_GENETICS, wild);
}

std::vector<fs::path> findJournalFiles(std::string wild) {
	return findByWildcard(DIRECTORY_JOURNAL, wild);
}

std::vector<fs::path> findSoundFiles(std::string wild) {
	return findByWildcard(DIRECTORY_SOUNDS, wild);
}

fs::path getUserDataDir() {
	if (data_directories.size() == 0) {
		throw Exception("Can't get user data directory when there are no data directories");
	}
	return data_directories.back().main;
}

std::vector<fs::path> getCatalogueDirectories() {
	std::vector<fs::path> result;
	for (auto d : data_directories) {
		result.push_back(getDirectory(d, DIRECTORY_CATALOGUE));
	}
	return result;
}

fs::path getWorldSwitcherBootstrapDirectory() {
	assert(data_directories.size() > 0);
	auto directory = getDirectory(data_directories[0], DIRECTORY_BOOTSTRAP);

	std::error_code err;
	auto bootstrap = case_insensitive_filesystem::canonical(directory / "000 Switcher", err);
	if (!err) {
		return bootstrap;
	}
	bootstrap = case_insensitive_filesystem::canonical(directory / "000_Switcher", err);
	if (!err) {
		return bootstrap;
	}
	bootstrap = case_insensitive_filesystem::canonical(directory / "Startup", err);
	if (!err) {
		return bootstrap;
	}
	throw Exception("couldn't find '000 Switcher' or 'Startup' bootstrap directory");
}

std::vector<fs::path> getBootstrapDirectories() {
	std::vector<fs::path> result;

	for (size_t i = 0; i < data_directories.size(); ++i) {
		if (engine.eame_variables[fmt::format("engine_no_auxiliary_bootstrap_{}", i)] != caosValue(0)) {
			// skip this one! this is how Docking Station undocked worlds work
			continue;
		}
		auto bootstrap = getDirectory(data_directories[i], DIRECTORY_BOOTSTRAP);
		for (auto d : case_insensitive_filesystem::directory_iterator(bootstrap)) {
			fs::path s = to_ascii_lowercase(fs::path(d).filename().string());
			if (s == "000 switcher" || s == "000_switcher" || s == "startup") {
				continue;
			}
			if (!fs::is_directory(d)) {
				continue;
			}
			result.push_back(d);
		}
	}
	std::sort(result.begin(), result.end());
	return result;
}

fs::path homeDirectory() {
	fs::path p;

#ifndef _WIN32
	char* envhome = getenv("HOME");
	if (envhome)
		p = fs::path(envhome);
	if ((!envhome) || (!fs::is_directory(p)))
		p = fs::path(getpwuid(getuid())->pw_dir);
	if (!fs::is_directory(p)) {
		fmt::print(stderr, "Can't work out what your home directory is, giving up and using /tmp for now.\n");
		p = fs::path("/tmp"); // sigh
	}
#else
	TCHAR szPath[_MAX_PATH];
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, TRUE);

	p = fs::path(szPath);
	if (!fs::exists(p) || !fs::is_directory(p))
		throw Exception("Windows reported that your My Documents folder is at '" + p.string() + "' but there's no directory there!");
#endif

	return p;
}

fs::path storageDirectory() {
#ifdef _WIN32
	fs::path dirname = "My Games";
#elif defined(__APPLE__)
	fs::path dirname = "Documents/openc2e Data";
#else
	fs::path dirname = ".openc2e";
#endif

	// main storage dir
	fs::path p = homeDirectory() / dirname;
	if (!fs::exists(p))
		fs::create_directory(p);
	else if (!fs::is_directory(p))
		throw Exception("Your openc2e data directory " + p.string() + " is a file, not a directory. That's bad.");

	// game-specific storage dir
	p = p / engine.getGameName();
	if (!fs::exists(p))
		fs::create_directory(p);
	else if (!fs::is_directory(p))
		throw Exception("Your openc2e game data directory " + p.string() + " is a file, not a directory. That's bad.");

	return p;
}
