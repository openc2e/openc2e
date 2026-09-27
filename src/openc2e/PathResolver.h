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

#include <filesystem>
#include <string>
#include <vector>

class FileReader;
class FileWriter;

class DataDirectory {
  public:
	DataDirectory(std::filesystem::path main);
	std::filesystem::path main;
	std::filesystem::path backgrounds;
	std::filesystem::path body_data;
	std::filesystem::path bootstrap;
	std::filesystem::path catalogue;
	std::filesystem::path creature_galleries;
	std::filesystem::path exported_creatures;
	std::filesystem::path genetics;
	std::filesystem::path images;
	std::filesystem::path journal;
	std::filesystem::path overlay_data;
	std::filesystem::path agents;
	std::filesystem::path sounds;
	std::filesystem::path users;
	std::filesystem::path worlds;
};

extern std::vector<DataDirectory> data_directories;

std::vector<std::filesystem::path> getMainDirectories();

std::filesystem::path findMainDirectoryFile(std::filesystem::path name);
std::filesystem::path findBackgroundFile(std::filesystem::path name);
std::filesystem::path findBodyDataFile(std::filesystem::path name);
std::filesystem::path findCatalogueFile(std::filesystem::path name);
std::filesystem::path findCobFile(std::filesystem::path name);
std::filesystem::path findGeneticsFile(std::filesystem::path name);
std::filesystem::path findImageFile(std::filesystem::path name);
std::filesystem::path findOverlayDataFile(std::filesystem::path name);
std::filesystem::path findSoundFile(std::filesystem::path name);

FileWriter createUserBackgroundFile(std::filesystem::path name);
FileWriter createUserBodyDataFile(std::filesystem::path name);
FileWriter createUserCatalogueFile(std::filesystem::path name);
FileWriter createUserCobFile(std::filesystem::path name);
FileWriter createUserGeneticsFile(std::filesystem::path name);
FileWriter createUserImageFile(std::filesystem::path name);
FileWriter createUserOverlayDataFile(std::filesystem::path name);
FileWriter createUserSoundFile(std::filesystem::path name);

std::filesystem::path getCurrentWorldJournalPath(std::filesystem::path name);
std::filesystem::path getMainJournalPath(std::filesystem::path name);
std::filesystem::path getOtherWorldJournalPath(std::filesystem::path name);

std::vector<std::filesystem::path> findAgentFiles(std::string wild);
std::vector<std::filesystem::path> findCobFiles(std::string wild);
std::vector<std::filesystem::path> findGeneticsFiles(std::string wild);
std::vector<std::filesystem::path> findJournalFiles(std::string wild);
std::vector<std::filesystem::path> findSoundFiles(std::string wild);

std::filesystem::path getUserDataDir();

std::filesystem::path getWorldSwitcherBootstrapDirectory();
std::vector<std::filesystem::path> getBootstrapDirectories();
std::vector<std::filesystem::path> getCatalogueDirectories();

std::filesystem::path homeDirectory();
std::filesystem::path storageDirectory();