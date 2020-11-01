/*
 *  caosVM_files.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Aug 13 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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


#include "caos_assert.h"
#include "caosVM.h"
#include "World.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

std::string calculateJournalFilename(int directory, std::string filename, bool writable) {
	// sanitise string: remove leading dots, replace slashes with underscores
	// todo: check DS behaviour for backslashes (a problem on Windows)
	std::string::size_type r;
	while ((r = filename.find("/", 0)) != std::string::npos)
		filename.replace(r, 1, "_");
	for (unsigned int i = 0; i < filename.size(); i++) {
		if (filename[i] == '.') {
			filename.erase(i, 1);
			i--;
		} else break;
	}
	
	std::string fullfilename;
	
	// TODO: point at the correct journal directories!
	
	if (!writable) {
		// search all directories for a readable file
		fullfilename = world.findFile("Journal/" + filename);

		// if we found one, return that
		if (fullfilename.size()) return fullfilename;
	}

	// otherwise, we should always write to the journal directory
	switch (directory) {
		case 0: fullfilename = world.getUserDataDir() + "/Journal/"; break;
		case 1: fullfilename = world.getUserDataDir() + "/Journal/"; break;
		case 2: fullfilename = world.getUserDataDir() + "/Journal/"; break;
		default: throw caosException("unknown Journal directory");
	}
	
	fs::path dir = fs::path(fullfilename);
	if (!fs::exists(dir))
		fs::create_directory(dir);
	caos_assert(fs::exists(dir) && fs::is_directory(dir));

	return fullfilename + filename;
}

/**
 FILE GLOB (command) directory (integer) filespec (string)
 %status maybe

 Globs the given journal directory (pass 0 for the world directory, or 1 for the main directory) for the 
 given filespec (you can use wildcards, and descend into subdirectories using '..').
 
 The result is placed on the input stream for reading with standard input commands (eg, INNI and INNL);
 this consists of the number of results on the first line, and then the full filename paths to the matched
 files on the remaining lines.
*/
void caosVM::c_FILE_GLOB() {
	VM_PARAM_STRING(filespec)
	VM_PARAM_INTEGER(directory)

	std::string::size_type n = filespec.find_last_of("/\\") + 1;
	std::string dirportion; dirportion.assign(filespec, 0, n);
	std::string specportion; specportion.assign(filespec, n, filespec.size() - n);

	if (directory == 1)
		dirportion = "Journal/" + dirportion;
	else
		throw creaturesException("whoops, openc2e doesn't support FILE GLOB in world journal directory yet, bug fuzzie");

	std::vector<std::string> possiblefiles = world.findFiles(dirportion, specportion);

	std::string str = fmt::format("{}\n", possiblefiles.size());
	for (std::vector<std::string>::iterator i = possiblefiles.begin(); i != possiblefiles.end(); i++) {
		str += *i + "\n";
	}

	inputstream = new std::istringstream(str);
}

/**
 FILE ICLO (command)
 %status maybe

 Disconnects everything from the input stream.
*/
void caosVM::c_FILE_ICLO() {
	if (inputstream) {
		delete inputstream;
		inputstream = 0;
	}
}

/**
 FILE IOPE (command) directory (integer) filename (string)
 %status maybe

 Puts the given filename in the given directory (pass 1 for the world directory, or 0 for the main 
 directory) on the current VM's input stream, for use by INOK, INNL, INNI and INNF.
 If a file is already open, it will be closed first.
*/
void caosVM::c_FILE_IOPE() {
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)

	c_FILE_ICLO();
	
	std::string fullfilename = calculateJournalFilename(directory, filename, false);
	inputstream = new std::ifstream(fullfilename.c_str());

	if (inputstream->fail()) {
		delete inputstream;
		inputstream = 0;
	}
}

/**
 FILE JDEL (command) directory (integer) filename (string)
 %status stub

 Removes the given file in the given directory (pass 1 for the world directory, or 0 for the main 
 directory) immediately.
*/
void caosVM::c_FILE_JDEL() {
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)
	
	std::string fullfilename = calculateJournalFilename(directory, filename, true);

	// TODO
}

/**
 FILE OCLO (command)
 %status maybe

 Disconnects everything from the output stream.
*/
void caosVM::c_FILE_OCLO() {
	if (outputstream) {
		delete outputstream;
		outputstream = 0;
	}
}

/**
 FILE OFLU (command)
 %status maybe

 Flushes the current output stream; if this is a file, all data in the buffer will be written to it.
*/
void caosVM::c_FILE_OFLU() {
	if (outputstream)
		outputstream->flush();
}

/**
 FILE OOPE (command) directory (integer) filename (string) append (integer)
 %status maybe

 Puts the given filename in the given directory (pass 1 for the world directory, or 0 for the main
 directory) on the current VM's output stream.
 If a file is already open, it will be closed first. 
*/
void caosVM::c_FILE_OOPE() {
	VM_PARAM_INTEGER(append)
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)

	c_FILE_OCLO();

	std::string fullfilename = calculateJournalFilename(directory, filename, true);

	if (append)
		outputstream = new std::ofstream(fullfilename.c_str(), std::ios::app);
	else	
		outputstream = new std::ofstream(fullfilename.c_str(), std::ios::trunc);

	if (outputstream->fail()) {
		outputstream = 0;
		throw caosException(fmt::format("FILE OOPE failed to open {}", fullfilename));
	}
}

/**
 FVWM (string) name (string)
 %status stub

 Returns an safe (not-in-use) filename for naming worlds and other saved files.
*/
void caosVM::v_FVWM() {
	VM_PARAM_STRING(name)

	result.setString(name + "_something"); // TODO
}

/**
 INNF (float)
 %status maybe

 Fetches a float from the current input stream, or 0.0 if there is no data.
*/
void caosVM::v_INNF() {
	if (!inputstream)
		throw caosException("no input stream in INNF!");

	float f = 0.0f;
	*inputstream >> f;
	result.setFloat(f);
}

/**
 INNI (integer)
 %status maybe

 Fetches an integer from the current input stream, or 0 if there is no data.
*/
void caosVM::v_INNI() {
	if (!inputstream)
		throw caosException("no input stream in INNI!");

	int i = 0;
	*inputstream >> i;
	result.setInt(i);
}

/**
 INNL (string)
 %status maybe

 Fetches a string of text from the input stream.
*/
void caosVM::v_INNL() {
	if (!inputstream)
		throw caosException("no input stream in INNL!");
	std::string str;
	std::getline(*inputstream, str);
	result.setString(str);
}

/**
 INOK (integer)
 %status maybe

 Determines whether the current input stream is usable (0 or 1).
*/
void caosVM::v_INOK() {
	if (!inputstream)
		result.setInt(0);
	else if (inputstream->fail())
		result.setInt(0);
	else
		result.setInt(1);
}

/**
 WEBB (command) url (string)
 %status stub

 Launches the specified URL, prepended with 'http://' (so you'd only specify, for example, 'example.com/foo.html'), in the user's browser.
*/
void caosVM::c_WEBB() {
	VM_PARAM_STRING(url)

	// TODO
	std::string to_use = std::string("http://") + url;
}

/* vim: set noet: */
