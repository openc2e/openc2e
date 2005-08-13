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
#include <fstream>
#include <iostream>

std::string calculateFilename(int directory, std::string filename) {
	std::string fullfilename;
	switch (directory) {
		// TODO: point at the correct journal directories
		case 0: fullfilename = "data/Journal/"; break;
		case 1: fullfilename = "data/Journal/"; break;
		case 2: fullfilename = "data/Journal/"; break;
		default: throw caosException("unknown directory in FILE OOPE");
	}

	// sanitise string: remove leading dots, replace slashes with underscores
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
*/
void caosVM::c_FILE_GLOB() {
	VM_PARAM_STRING(filespec)
	VM_PARAM_STRING(directory)
	// TODO
}

/**
 FILE ICLO (command)
 %status probablyok
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
 %status probablyok
*/
void caosVM::c_FILE_IOPE() {
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)

	std::string fullfilename = calculateFilename(directory, filename);
	inputstream = new std::ifstream(fullfilename.c_str());
}

/**
 FILE JDEL (command) directory (integer) filename (string)
 %status stub
*/
void caosVM::c_FILE_JDEL() {
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)
	
	std::string fullfilename = calculateFilename(directory, filename);

	// TODO
}

/**
 FILE OCLO (command)
 %status probablyok
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
		outputstream = &std::cout; // always restore to default..
	}
}

/**
 FILE OFLU (command)
 %status probablyok
*/
void caosVM::c_FILE_OFLU() {
	if (outputstream)
		outputstream->flush();
}

/**
 FILE OOPE (command) directory (integer) filename (string) append (integer)
 %status probablyok
*/
void caosVM::c_FILE_OOPE() {
	VM_PARAM_INTEGER(append)
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)

	std::string fullfilename = calculateFilename(directory, filename);

	if (append)
		outputstream = new std::ofstream(fullfilename.c_str(), std::ios::app);
	else	
		outputstream = new std::ofstream(fullfilename.c_str(), std::ios::trunc);
}

/**
 FVWM (string) name (string)
 %status stub
*/
void caosVM::v_FVWM() {
	VM_PARAM_STRING(name)

	result.setString(name + "_something"); // TODO
}

/**
 INNF (float)
 %status stub
*/
void caosVM::v_INNF() {
	// TODO
}

/**
 INNI (integer)
 %status stub
*/
void caosVM::v_INNI() {
	// TODO
}

/**
 INNL (string)
 %status probablyok
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
 %status probablyok
*/
void caosVM::v_INOK() {
	if (!inputstream)
		throw caosException("inputstream not valid in INOK");

	if (inputstream->fail())
		result.setInt(0);
	else
		result.setInt(1);
}

/**
 WEBB (command) url (string)
 %status stub

 launches the specified URL, prepended with http:// (so you'd only specify, say, 'hostname/blah.html'), in the user's browser.
*/
void caosVM::c_WEBB() {
	VM_PARAM_STRING(url)

	// TODO
	std::string to_use = std::string("http://") + url;
}

/* vim: set noet: */
