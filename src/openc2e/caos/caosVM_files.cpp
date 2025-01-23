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


#include "PathResolver.h"
#include "World.h"
#include "caosVM.h"
#include "common/io/FileReader.h"
#include "common/io/FileWriter.h"
#include "common/io/Getline.h"
#include "common/io/IOException.h"
#include "common/io/Reader.h"
#include "common/io/Scanf.h"
#include "common/io/SpanReader.h"
#include "common/io/WriterFmt.h"
#include "common/throw_ifnot.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

static fs::path calculateJournalFilename(int directory, std::string filename) {
	// sanitise string: remove leading dots, replace slashes with underscores
	// todo: check DS behaviour for backslashes (a problem on Windows)
	std::string::size_type r;
	while ((r = filename.find("/", 0)) != std::string::npos)
		filename.replace(r, 1, "_");

	for (unsigned int i = 0; i < filename.size(); i++) {
		if (filename[i] == '.') {
			filename.erase(i, 1);
			i--;
		} else
			break;
	}

	switch (directory) {
		case 0: return getCurrentWorldJournalPath(filename);
		case 1: return getMainJournalPath(filename);
		case 2: return getOtherWorldJournalPath(filename);
		default: throw caosException("unknown Journal directory");
	}
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
void c_FILE_GLOB(caosVM* vm) {
	VM_PARAM_STRING(filespec)
	VM_PARAM_INTEGER(directory)

	if (directory != 1) {
		throw Exception("whoops, openc2e doesn't support FILE GLOB in world journal directory yet, bug fuzzie");
	}

	std::vector<fs::path> possiblefiles = findJournalFiles(filespec);

	std::string str = fmt::format("{}\n", possiblefiles.size());
	for (auto& possiblefile : possiblefiles) {
		str += possiblefile.string() + "\n";
	}

	class OwningStringReader : public SpanReader {
	  public:
		OwningStringReader(std::string s) {
			s_ = s;
			static_cast<SpanReader&>(*this) = SpanReader(s_);
		}
		// can't move because SpanReader is pointing at ourself
		OwningStringReader(const OwningStringReader&) = delete;
		OwningStringReader(OwningStringReader&&) = delete;
		OwningStringReader& operator=(const OwningStringReader&) = delete;
		OwningStringReader& operator=(OwningStringReader&&) = delete;

	  private:
		std::string s_;
	};

	vm->inputstream = new OwningStringReader(str);
}

/**
 FILE ICLO (command)
 %status maybe

 Disconnects everything from the input stream.
*/
void c_FILE_ICLO(caosVM* vm) {
	if (vm->inputstream) {
		delete vm->inputstream;
		vm->inputstream = 0;
	}
}

/**
 FILE IOPE (command) directory (integer) filename (string)
 %status maybe

 Puts the given filename in the given directory (pass 1 for the world directory, or 0 for the main 
 directory) on the current VM's input stream, for use by INOK, INNL, INNI and INNF.
 If a file is already open, it will be closed first.
*/
void c_FILE_IOPE(caosVM* vm) {
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)

	c_FILE_ICLO(vm);

	std::string fullfilename = calculateJournalFilename(directory, filename);
	try {
		vm->inputstream = new FileReader(fullfilename);
	} catch (const IOException&) {
		return;
	}
}

/**
 FILE JDEL (command) directory (integer) filename (string)
 %status stub

 Removes the given file in the given directory (pass 1 for the world directory, or 0 for the main 
 directory) immediately.
*/
void c_FILE_JDEL(caosVM* vm) {
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)

	std::string fullfilename = calculateJournalFilename(directory, filename);

	// TODO
}

/**
 FILE OCLO (command)
 %status maybe

 Disconnects everything from the output stream.
*/
void c_FILE_OCLO(caosVM* vm) {
	if (vm->outputstream) {
		delete vm->outputstream;
		vm->outputstream = 0;
	}
}

/**
 FILE OFLU (command)
 %status maybe

 Flushes the current output stream; if this is a file, all data in the buffer will be written to it.
*/
void c_FILE_OFLU(caosVM* vm) {
	if (vm->outputstream)
		vm->outputstream->flush();
}

/**
 FILE OOPE (command) directory (integer) filename (string) append (integer)
 %status maybe

 Puts the given filename in the given directory (pass 1 for the world directory, or 0 for the main
 directory) on the current VM's output stream.
 If a file is already open, it will be closed first. 
*/
void c_FILE_OOPE(caosVM* vm) {
	VM_PARAM_INTEGER(append)
	VM_PARAM_STRING(filename)
	VM_PARAM_INTEGER(directory)

	c_FILE_OCLO(vm);

	std::string fullfilename = calculateJournalFilename(directory, filename);

	try {
		if (append)
			vm->outputstream = new FileWriter(fullfilename, FileWriter::option_append);
		else
			vm->outputstream = new FileWriter(fullfilename);
	} catch (const IOException&) {
		throw caosException(fmt::format("FILE OOPE failed to open {}", fullfilename));
	}
}

/**
 FVWM (string) name (string)
 %status stub

 Returns an safe (not-in-use) filename for naming worlds and other saved files.
*/
void v_FVWM(caosVM* vm) {
	VM_PARAM_STRING(name)

	vm->result.setString(name + "_something"); // TODO
}

/**
 INNF (float)
 %status maybe

 Fetches a float from the current input stream, or 0.0 if there is no data.
*/
void v_INNF(caosVM* vm) {
	if (!vm->inputstream)
		throw caosException("no input stream in INNF!");

	float f = 0.0;
	f = scan_float(*vm->inputstream);
	vm->result.setFloat(f);
}

/**
 INNI (integer)
 %status maybe

 Fetches an integer from the current input stream, or 0 if there is no data.
*/
void v_INNI(caosVM* vm) {
	if (!vm->inputstream)
		throw caosException("no input stream in INNI!");

	int i = 0;
	i = scan_int(*vm->inputstream);
	vm->result.setInt(i);
}

/**
 INNL (string)
 %status maybe

 Fetches a string of text from the input stream.
*/
void v_INNL(caosVM* vm) {
	if (!vm->inputstream)
		throw caosException("no input stream in INNL!");
	std::string str = getline(*vm->inputstream);
	vm->result.setString(str);
}

/**
 INOK (integer)
 %status maybe

 Determines whether the current input stream is usable (0 or 1).
*/
void v_INOK(caosVM* vm) {
	if (!vm->inputstream)
		vm->result.setInt(0);
	else
		vm->result.setInt(1);
}

/**
 WEBB (command) url (string)
 %status stub

 Launches the specified URL, prepended with 'http://' (so you'd only specify, for example, 'example.com/foo.html'), in the user's browser.
*/
void c_WEBB(caosVM* vm) {
	VM_PARAM_STRING(url)

	// TODO
	std::string to_use = std::string("http://") + url;
}

/* vim: set noet: */
