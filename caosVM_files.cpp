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


#include "caosVM.h"
#include "World.h"
#include <fstream>
#include <iostream>
#include <boost/format.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

std::string calculateFilename(int directory, std::string filename) {
	std::string fullfilename;
	switch (directory) {
		// TODO: point at the correct journal directories
		case 0: fullfilename = world.getUserDataDir() + "/Journal/"; break;
		case 1: fullfilename = world.getUserDataDir() + "/Journal/"; break;
		case 2: fullfilename = world.getUserDataDir() + "/Journal/"; break;
		default: throw caosException("unknown directory in FILE OOPE");
	}

	fs::path dir = fs::path(fullfilename, fs::native);
	if (!fs::exists(dir))
		fs::create_directory(dir);
	caos_assert(fs::exists(dir) && fs::is_directory(dir));

	// sanitise string: remove leading dots, replace slashes with underscores
	// todo: check DS behaviour for backslashes (a problem on Windows)
	std::string::size_type r;
	while ((r = filename.find("/", 0)) != std::string::npos)
		filename.replace(r, 1, "_");
	for (unsigned int i = 0; i < filename.size(); i++)
		if (filename[i] == '.') { filename.erase(i, 1); i--; }
		else break;
	
	return fullfilename + filename;
}

/**
 FILE GLOB (command) directory (integer) filespec (string)
 %status stub

 Globs the given directory (pass 1 for the world directory, or 0 for the main directory) for the 
 given filespec, and puts it on the input stream for use by INNI, INOK, or INNL.
*/
void caosVM::c_FILE_GLOB() {
	VM_PARAM_STRING(filespec)
	VM_PARAM_STRING(directory)
	// TODO
}

/**
 FILE ICLO (command)
 %status maybe

 Disconnects everything from the input stream.
*/
void caosVM::c_FILE_ICLO() {
	if (inputstream) {
		std::ifstream *iftest = dynamic_cast<std::ifstream *>(inputstream);
		if (iftest) {
			// goodbye...
			iftest->close();
			delete iftest;
			inputstream = 0;
		} else
			std::cout << "don't know what's attached to inputstream at FILE ICLO.." << std::endl;
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
	
	std::string fullfilename = calculateFilename(directory, filename);
	inputstream = new std::ifstream(fullfilename.c_str());

	if (inputstream->fail()) {
		inputstream = 0;
		throw caosException(boost::str(boost::format("FILE IOPE failed to open %s") % fullfilename));
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
	
	std::string fullfilename = calculateFilename(directory, filename);

	// TODO
}

/**
 FILE OCLO (command)
 %status maybe

 Disconnects everything from the output stream.
*/
void caosVM::c_FILE_OCLO() {
	// TODO: right now, cout is hooked up to outputstream by default, someday when this
	// changes, this function will need revising

	if (outputstream) {
		std::ofstream *oftest = dynamic_cast<std::ofstream *>(outputstream);
		if (oftest) {
			oftest->close();
			delete oftest;
		}
		outputstream = &std::cout; // always restore to default.. TODO, should be null?
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

	std::string fullfilename = calculateFilename(directory, filename);

	if (append)
		outputstream = new std::ofstream(fullfilename.c_str(), std::ios::app);
	else	
		outputstream = new std::ofstream(fullfilename.c_str(), std::ios::trunc);

	if (outputstream->fail()) {
		outputstream = &std::cout; // TODO: probably not the right fallback, should null?
		throw caosException(boost::str(boost::format("FILE OOPE failed to open %s") % fullfilename));
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
		//throw caosException("inputstream not valid in INOK");
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
