/*
 *  PathResolver.h
 *  openc2e
 *
 *  Created by Bryan Donlan
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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

#pragma once

#include <ghc/filesystem.hpp>
#include <string>
#include <vector>

class FileReader;
class FileWriter;

class DataDirectory {
  public:
	DataDirectory(ghc::filesystem::path main);
	ghc::filesystem::path main;
	ghc::filesystem::path backgrounds;
	ghc::filesystem::path body_data;
	ghc::filesystem::path bootstrap;
	ghc::filesystem::path catalogue;
	ghc::filesystem::path creature_galleries;
	ghc::filesystem::path exported_creatures;
	ghc::filesystem::path genetics;
	ghc::filesystem::path images;
	ghc::filesystem::path journal;
	ghc::filesystem::path overlay_data;
	ghc::filesystem::path agents;
	ghc::filesystem::path sounds;
	ghc::filesystem::path users;
	ghc::filesystem::path worlds;
};

extern std::vector<DataDirectory> data_directories;

std::vector<ghc::filesystem::path> getMainDirectories();

ghc::filesystem::path findMainDirectoryFile(ghc::filesystem::path name);
ghc::filesystem::path findBackgroundFile(ghc::filesystem::path name);
ghc::filesystem::path findBodyDataFile(ghc::filesystem::path name);
ghc::filesystem::path findCatalogueFile(ghc::filesystem::path name);
ghc::filesystem::path findCobFile(ghc::filesystem::path name);
ghc::filesystem::path findGeneticsFile(ghc::filesystem::path name);
ghc::filesystem::path findImageFile(ghc::filesystem::path name);
ghc::filesystem::path findOverlayDataFile(ghc::filesystem::path name);
ghc::filesystem::path findSoundFile(ghc::filesystem::path name);

FileWriter createUserBackgroundFile(ghc::filesystem::path name);
FileWriter createUserBodyDataFile(ghc::filesystem::path name);
FileWriter createUserCatalogueFile(ghc::filesystem::path name);
FileWriter createUserCobFile(ghc::filesystem::path name);
FileWriter createUserGeneticsFile(ghc::filesystem::path name);
FileWriter createUserImageFile(ghc::filesystem::path name);
FileWriter createUserOverlayDataFile(ghc::filesystem::path name);
FileWriter createUserSoundFile(ghc::filesystem::path name);

ghc::filesystem::path getCurrentWorldJournalPath(ghc::filesystem::path name);
ghc::filesystem::path getMainJournalPath(ghc::filesystem::path name);
ghc::filesystem::path getOtherWorldJournalPath(ghc::filesystem::path name);

std::vector<ghc::filesystem::path> findAgentFiles(std::string wild);
std::vector<ghc::filesystem::path> findCobFiles(std::string wild);
std::vector<ghc::filesystem::path> findGeneticsFiles(std::string wild);
std::vector<ghc::filesystem::path> findJournalFiles(std::string wild);
std::vector<ghc::filesystem::path> findSoundFiles(std::string wild);

ghc::filesystem::path getUserDataDir();

ghc::filesystem::path getWorldSwitcherBootstrapDirectory();
std::vector<ghc::filesystem::path> getBootstrapDirectories();
std::vector<ghc::filesystem::path> getCatalogueDirectories();

ghc::filesystem::path homeDirectory();
ghc::filesystem::path storageDirectory();