/*
 *  caosVM_resources.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jun 13 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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
#include "caosScript.h" // PRAY INJT
#include "Engine.h"
#include "World.h"
#include "Catalogue.h"

#include "prayManager.h"
#include <fmt/printf.h>
#include <fstream>
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

bool prayInstall(std::string name, unsigned int type, bool actually_install) {
	std::string directory = world.praymanager->getResourceDir(type);
	caos_assert(!directory.empty());

	fs::path dir = fs::path(world.getUserDataDir()) / fs::path(directory);
	if (!fs::exists(dir))
		fs::create_directory(dir);
	caos_assert(fs::exists(dir) && fs::is_directory(dir));

	fs::path outputfile = dir / fs::path(name);
	if (fs::exists(outputfile)) {
		// TODO: update file if necessary? check it's not a directory :P
		return true;
	}

	fs::path possiblefile = fs::path(directory) / fs::path(name);
	if (!world.findFile(possiblefile.string()).empty()) {
		// TODO: we need to return 'okay' if the file exists anywhere, but someone needs to work out update behaviour (see other comment above, also)
		return true;
	}

	std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i = world.praymanager->blocks.find(name);
	if (i == world.praymanager->blocks.end()) {
		std::cout << "PRAY FILE: couldn't find block " << name << std::endl;
		return false;
	}

	PrayBlock* p = i->second.get();
	if (p->type != "FILE") {
		std::cout << "PRAY FILE: block " << name << " is " << p->type << " not FILE" << std::endl;
		// TODO: correct behaviour? possibly not..
		return false;
	}

	if (!actually_install) {
		// TODO: work out if we've tested enough
		return true;
	}

	p->load();
	std::ofstream output(outputfile.string().c_str(), std::ios::binary);
	output.write((char *)p->getBuffer(), p->getSize());
	// p->unload();

	if (type == 7) {
		output.flush(); output.close();
		// TODO: verify it is a catalogue file first, perhaps?
		catalogue.addFile(outputfile);
	}

	return true;
}

int prayInstallDeps(std::string name, bool actually_install) {
	std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i = world.praymanager->blocks.find(name);
	caos_assert(i != world.praymanager->blocks.end());

	PrayBlock *p = i->second.get();
	p->parseTags();

	std::map<std::string, uint32_t>::iterator j;
	j = p->integerValues.find("Agent Type");
	// previously errored when this didn't exist, but some community-created
	// agent files don't have it
	if (j != p->integerValues.end()) {
		// I have no idea what this is, so let's just error out when it's not zero, pending fix. - fuzzie
		caos_assert(j->second == 0);
	}

	j = p->integerValues.find("Dependency Count");
	if (j == p->integerValues.end()) {
		return -2;
	}
	int nodeps = j->second; caos_assert(nodeps >= 0);

	for (int z = 1; z <= nodeps; z++) {
		std::string depcatname = fmt::sprintf("Dependency Category %d", z);
		std::string depname = fmt::sprintf("Dependency %d", z);
		j = p->integerValues.find(depcatname);
		if (j == p->integerValues.end()) {
			return (-2 - nodeps - z);
		}
		int depcat = j->second; caos_assert(depcat >= 0 && depcat <= 11);
		std::map<std::string, std::string>::iterator k = p->stringValues.find(depname);
		if (k == p->stringValues.end()) {
			return (-2 - z);
		}
		std::string dep = k->second;

		// TODO: CL docs say 2*count to 3*count is the category ID for that dependency being invalid
		if (!prayInstall(dep, depcat, actually_install)) {
			return z;
		}
	}

	return 0;
}

//used by PRAY BACK, PRAY FORE, PRAY NEXT and PRAY PREV to implement their functionality
std::string findBlock(std::string type, std::string last, bool forward, bool loop) {
	PrayBlock *firstblock = 0, *currblock = 0;
	bool foundblock = false;

	if (world.praymanager->blocks.size() == 0) return ""; // We definitely can't find anything in that case!

	// Where do we start?
	std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i;
	if (forward)
		i = world.praymanager->blocks.begin();
	else {
		i = world.praymanager->blocks.end();
		i--;
	}
	
	// Loop through all the blocks.
	while (true) {
		if (i->second->type == type) {
			currblock = i->second.get();

			// Store the first block if we didn't already find one, for possible use later.
			if (!firstblock)
				firstblock = currblock;
			
			// If this is the resource we want, grab it!
			if (foundblock)
				return currblock->name;

			// If this is the resource we're looking for, make a note to grab the next one.
			if (last == currblock->name)
				foundblock = true;
		}
		
		// Step through the list. Break if we need to.
		if (!forward && i == world.praymanager->blocks.begin()) break;
		if (forward) i++; else i--;
		if (forward && i == world.praymanager->blocks.end()) break;
	}

	if (foundblock && loop) return firstblock->name; // loop around to first-found block
	else if (!foundblock && currblock) return firstblock->name; // default to first-found block (XXX this is in direct opposition to what CAOS docs say!)
	
	return ""; // yarr, failure.
}

/**
 PRAY AGTI (integer) resource (string) tag (string) default (integer)
 %status maybe

 using the specified resource, returns the integer value associated with the given tag,
 or default if the tag doesn't exist
*/
void caosVM::v_PRAY_AGTI() {
	VM_PARAM_INTEGER(_default)
	VM_PARAM_STRING(tag)
	VM_PARAM_STRING(resource)

	std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i = world.praymanager->blocks.find(resource);
	caos_assert(i != world.praymanager->blocks.end());

	PrayBlock *p = i->second.get();
	p->parseTags();
	if (p->integerValues.find(tag) == p->integerValues.end())
		result.setInt(_default);
	else
		result.setInt(p->integerValues[tag]);
}

/**
 PRAY AGTS (string) resource (string) tag (string) default (string)
 %status maybe

 using the specified resource, returns the string value associated with the given tag,
 or default if the tag doesn't exist
*/
void caosVM::v_PRAY_AGTS() {
	VM_PARAM_STRING(_default)
	VM_PARAM_STRING(tag)
	VM_PARAM_STRING(resource)

	std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i = world.praymanager->blocks.find(resource);
	caos_assert(i != world.praymanager->blocks.end());

	PrayBlock *p = i->second.get();
	p->parseTags();
	if (p->stringValues.find(tag) == p->stringValues.end())
		result.setString(_default);
	else
		result.setString(p->stringValues[tag]);
}

/**
 PRAY BACK (string) type (string) last (string)
 %status maybe

 returns the name of the resource of the specified type which is immediately previous to last
 see PRAY PREV if you want to loop around
*/
void caosVM::v_PRAY_BACK() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)

	result.setString(findBlock(type, last, false, false));
}

/**
 PRAY COUN (integer) type (string)
 %status maybe

 return the number of resources of the specified type available
*/
void caosVM::v_PRAY_COUN() {
	VM_PARAM_STRING(type)

	unsigned int count = 0;
	for (std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i = world.praymanager->blocks.begin(); i != world.praymanager->blocks.end(); i++)
		if (i->second->type == type)
			count++;
	
	result.setInt(count);
}

/**
 PRAY DEPS (integer) name (string) install (integer)
 %status maybe
*/
void caosVM::v_PRAY_DEPS() {
	VM_PARAM_INTEGER(install)
	VM_PARAM_STRING(name)

	result.setInt(prayInstallDeps(name, install != 0));
}

/**
 PRAY EXPO (integer) type (string)
 %status stub
*/
void caosVM::v_PRAY_EXPO() {
	VM_PARAM_STRING(type)

	result.setInt(0); // TODO
}

/**
 PRAY FILE (integer) name (integer) type (integer) install (integer)
 %status maybe

 install a file with given resource name and type
 if install is 0, the install doesn't actually happen, it's just tested
 returns 0 on success, 1 on failure
*/
void caosVM::v_PRAY_FILE() {
	VM_PARAM_INTEGER(install)
	VM_PARAM_INTEGER(type)
	VM_PARAM_STRING(name)

	if (prayInstall(name, type, (install != 0)))
		result.setInt(0);
	else
		result.setInt(1);
}

/**
 PRAY FORE (string) type (string) last (string)
 %status maybe

 returns the name of the resource of the specified type which is immediately after last
 see PRAY NEXT if you want to loop around
*/
void caosVM::v_PRAY_FORE() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)

	result.setString(findBlock(type, last, true, false));
}

/**
 PRAY GARB (command) force (integer)
 %status stub

 if force is 0, make the pray manager garbage-collect resources
 otherwise, make the pray manager empty its cache entirely

 recommended to be called after intensive PRAY usage, eg agent installation
*/
void caosVM::c_PRAY_GARB() {
	VM_PARAM_INTEGER(force)

	// TODO
}

/**
 PRAY IMPO (integer) moniker (string) doit (integer) keepfile (integer)
 %status stub
*/
void caosVM::v_PRAY_IMPO() {
	VM_PARAM_INTEGER(keepfile)
	VM_PARAM_INTEGER(doit)
	VM_PARAM_STRING(moniker)

	result.setInt(4); // TODO
}

/**
 PRAY INJT (integer) name (string) install (integer) report (variable)
 %status maybe
*/
void caosVM::v_PRAY_INJT() {
	VM_PARAM_VARIABLE(report)
	VM_PARAM_INTEGER(install)
	VM_PARAM_STRING(name)

	// Try installing the dependencies.
	int r = prayInstallDeps(name, install != 0);
	if (r != 0) {
		result.setInt(-3);
		report->setInt(r);
		return;
	}
	
	// Now grab the relevant block..
	std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i = world.praymanager->blocks.find(name);
	caos_assert(i != world.praymanager->blocks.end());
	PrayBlock *p = i->second.get();
	p->parseTags();
	
	// .. grab the script count ..
	std::map<std::string, uint32_t>::iterator j = p->integerValues.find("Script Count");
	if (j == p->integerValues.end()) {
		result.setInt(-3); // TODO: this isn't really a dependency fail, what do I do here?
		return;
	}
	int noscripts = j->second; caos_assert(noscripts >= 0);

	// .. and iterate over the scripts.
	for (int z = 1; z <= noscripts; z++) {
		// First, retrieve the script.
		std::string scriptname = fmt::sprintf("Script %d", z);
		std::map<std::string, std::string>::iterator k = p->stringValues.find(scriptname);
		if (k == p->stringValues.end()) {
			result.setInt(-1);
			report->setString(scriptname);
			return;
		}
		std::string script = k->second;

		if (!install) continue;
		
		// Then, execute it.
		caosVM *vm = world.getVM(NULL);
		try {
			std::istringstream iss(script);
			caosScript script(engine.gametype, name + " - PRAY " + scriptname);
			script.parse(iss);
			script.installScripts();
			vm->resetCore();
			vm->runEntirely(script.installer);
		} catch (caosException &e) {
			world.freeVM(vm);
			result.setInt(-2);
			report->setString(scriptname + " error: " + e.what());
			std::cerr << "PRAY INJT caught exception trying to inject " << name << " - PRAY " << scriptname << ": " << e.prettyPrint() << std::endl;
			return;
		} catch (std::exception &e) {
			world.freeVM(vm);
			result.setInt(-2);
			report->setString(scriptname + " error: " + e.what());
			std::cerr << "PRAY INJT caught exception trying to inject " << name << " - PRAY " << scriptname << ": " << e.what() << std::endl;
			return;
		}
		world.freeVM(vm);
	}
	
	result.setInt(0);
}

/**
 PRAY KILL (integer) resource (string)
 %status stub

 deletes from disk the file containing the given resource

 returns 1 upon success, or 0 upon failure (typically no such resource)
*/
void caosVM::v_PRAY_KILL() {
	VM_PARAM_STRING(resource)

	result.setInt(0); // TODO
}

/**
 PRAY MAKE (integer) journalspot (integer) journalname (string) prayspot (integer) name (string) report (variable)
 %status stub
*/
void caosVM::v_PRAY_MAKE() {
	VM_PARAM_VARIABLE(report)
	VM_PARAM_STRING(name)
	VM_PARAM_INTEGER(prayspot)
	VM_PARAM_STRING(journalname)
	VM_PARAM_INTEGER(journalspot)

	result.setInt(1); // TODO
	report->setString("hat u");
}

/**
 NET: MAKE (integer) journalspot (integer) journalname (integer) user (string) report (variable)
 %status stub

 Networking is not supported in openc2e, so conveniently fails.
*/
void caosVM::v_NET_MAKE() {
	VM_PARAM_VARIABLE(report)
	VM_PARAM_STRING(user)
	VM_PARAM_STRING(journalname)
	VM_PARAM_INTEGER(journalspot)

	result.setInt(1);
	report->setString("Networking unsupported.");
}

/**
 PRAY NEXT (string) type (string) last (string)
 %status maybe

 returns the name of the resource of the specified type which is immediately after last
 see PRAY FORE if you don't want to loop around
*/
void caosVM::v_PRAY_NEXT() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)
	
	result.setString(findBlock(type, last, true, true));
}

/**
 PRAY PREV (string) type (string) last (string)
 %status maybe

 returns the name of the resource of the specified type which is immediately previous to last
 see PRAY BACK if you don't want to loop around
*/
void caosVM::v_PRAY_PREV() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)

	result.setString(findBlock(type, last, false, true));
}

/**
 PRAY REFR (command)
 %status maybe

 make the pray manager check for deleted/new files in the resource directory
*/
void caosVM::c_PRAY_REFR() {
	world.praymanager->update();
}

/**
 PRAY TEST (integer) name (string)
 %status maybe
*/
void caosVM::v_PRAY_TEST() {
	VM_PARAM_STRING(name)

	std::map<std::string, std::unique_ptr<PrayBlock> >::iterator i = world.praymanager->blocks.find(name);
	if (i == world.praymanager->blocks.end())
		result.setInt(0);
	else {
		PrayBlock *p = i->second.get();
		if (p->isLoaded())
			result.setInt(1);
		else if (p->isCompressed())
			result.setInt(3);
		else
			result.setInt(2);
	}
}

/* vim: set noet: */
