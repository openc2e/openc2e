/*
 *  World.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

#include "World.h"
#include "Engine.h"
#include "caosVM.h" // for setupCommandPointers()
#include "PointerAgent.h"
#include "CompoundAgent.h" // for setFocus
#include <limits.h> // for MAXINT
#include "creaturesImage.h"
#include "CreatureAgent.h"
#include "Backend.h"
#include "SFCFile.h"
#include "Room.h"
#include "MetaRoom.h"

#include <boost/format.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

World world;

World::World() {
	ticktime = 50;
	tickcount = 0;
	worldtickcount = 0;
	race = 50; // sensible default?
	pace = 0.0f; // sensible default?
	quitting = saving = false;
	theHand = 0;
	showrooms = false;
	autokill = false;
}

World::~World() {
	agents.clear();
}

// annoyingly, if we put this in the constructor, the catalogue isn't available yet
void World::init() {
	// First, try initialising the mouse cursor from the catalogue tag.
	if (catalogue.hasTag("Pointer Information")) {
		const std::vector<std::string> &pointerinfo = catalogue.getTag("Pointer Information");
		if (pointerinfo.size() >= 3) {
			shared_ptr<creaturesImage> img = gallery.getImage(pointerinfo[2]);
			if (img) {
				theHand = new PointerAgent(pointerinfo[2]);
				theHand->finishInit();
				// TODO: set family/genus/species based on the first entry (normally "2 1 1")
				// TODO: work out what second entry is ("2 2" normally?! "7 7" in CV)
			}
		}
	}
	
	// If for some reason we failed to do that (missing/bad catalogue tag? missing file?), try falling back to a sane default.
	if (!theHand) {
		shared_ptr<creaturesImage> img;
		if (gametype == "c3")
			img = gallery.getImage("hand"); // as used in C3 and DS
		else
			img = gallery.getImage("syst"); // as used in C1, C2 and CV
		if (!img)
			throw creaturesException("no valid \"Pointer Information\" catalogue tag, and fallback failed");
		theHand = new PointerAgent(img->name);
		theHand->finishInit();
		std::cout << "Warning: No valid \"Pointer Information\" catalogue tag, defaulting to '" << img->name << "'." << std::endl;
	}

	// *** set defaults for non-zero GAME engine variables
	// TODO: this should be doing during world init, rather than global init
	// TODO: not complete
	caosVar v;
	variables.clear();

	// core engine bits
	v.setInt(1); variables["engine_debug_keys"] = v;
	v.setInt(1); variables["engine_full_screen_toggle"] = v;
	v.setInt(9998); variables["engine_plane_for_lines"] = v;
	v.setInt(6); variables["engine_zlib_compression"] = v;

	// creature pregnancy
	v.setInt(1); variables["engine_multiple_birth_maximum"] = v;
	v.setFloat(0.5f); variables["engine_multiple_birth_identical_chance"] = v;
	
	// port lines
	v.setFloat(600.0f); variables["engine_distance_before_port_line_warns"] = v;
	v.setFloat(800.0f); variables["engine_distance_before_port_line_snaps"] = v;

	// set engine version
	// TODO: move this somewhere more sensible?
	if (gametype == "c1")
		engine.version = 1;
	else if (gametype == "c2")
		engine.version = 2;
	else if (gametype == "c3" || gametype == "cv")
		engine.version = 3;
	else
		throw creaturesException(boost::str(boost::format("unknown gametype '%s'!") % gametype));

	// adjust to default tick rate for C1/C2 if necessary
	if (engine.version < 3)
		ticktime = 100;
}

void World::shutdown() {
	map.Reset();
}

caosVM *World::getVM(Agent *a) {
	if (vmpool.empty()) {
		return new caosVM(a);
	} else {
		caosVM *x = vmpool.back();
		vmpool.pop_back();
		x->setOwner(a);
		return x;
	}
}

void World::freeVM(caosVM *v) {
	v->setOwner(0);
	vmpool.push_back(v);
}

void World::queueScript(unsigned short event, AgentRef agent, AgentRef from, caosVar p0, caosVar p1) {
	scriptevent e;

	assert(agent);

	e.scriptno = event;
	e.agent = agent;
	e.from = from;
	e.p[0] = p0;
	e.p[1] = p1;

	scriptqueue.push_back(e);
}

// TODO: eventually, the part should be referenced via a weak_ptr, maaaaybe?
void World::setFocus(TextEntryPart *p) {
	// Unfocus the current agent. Not sure if c2e always does this (what if the agent/part is bad?).
        if (focusagent) {
		CompoundAgent *c = dynamic_cast<CompoundAgent *>(focusagent.get());
		assert(c);
		TextEntryPart *p = dynamic_cast<TextEntryPart *>(c->part(focuspart));
		if (p)
			p->loseFocus();
	}

	if (!p)
		focusagent.clear();
	else {
		p->gainFocus();
		focusagent = p->getParent();
		focuspart = p->id;
	}
}

void World::tick() {
	if (saving) {} // TODO: save
	if (quitting) {
		// due to destruction ordering we must explicitly destroy all agents here
		agents.clear();
		exit(0);
	}

	// Tick all agents, deleting as necessary.	
	std::list<boost::shared_ptr<Agent> >::iterator i = agents.begin();
	while (i != agents.end()) {
		boost::shared_ptr<Agent> a = *i;
		if (!a) {
			std::list<boost::shared_ptr<Agent> >::iterator i2 = i;
			i2++;
			agents.erase(i);
			i = i2;
			continue;
		}
		i++;
		a->tick();
	}
	
	// Process the script queue.
	for (std::list<scriptevent>::iterator i = scriptqueue.begin(); i != scriptqueue.end(); i++) {
		boost::shared_ptr<Agent> agent = i->agent.lock();
		if (agent && agent->fireScript(i->scriptno, i->from)) {
			assert(agent->vm);
			agent->vm->setVariables(i->p[0], i->p[1]);
			agent->vmTick();
		}
	}
	scriptqueue.clear();

	tickcount++;
	worldtickcount++;

	world.map.tick();

	// TODO: correct behaviour? hrm :/
	world.hand()->velx.setFloat(world.hand()->velx.getFloat() / 2.0f);
	world.hand()->vely.setFloat(world.hand()->vely.getFloat() / 2.0f);
}

Agent *World::agentAt(unsigned int x, unsigned int y, bool obey_all_transparency, bool needs_mouseable) {
	CompoundPart *p = partAt(x, y, obey_all_transparency, needs_mouseable);
	if (p)
		return p->getParent();
	else
		return 0;
}

CompoundPart *World::partAt(unsigned int x, unsigned int y, bool obey_all_transparency, bool needs_mouseable) {
	Agent *transagent = 0;
	if (!obey_all_transparency)
		transagent = agentAt(x, y, true, needs_mouseable);

	for (std::multiset<CompoundPart *, partzorder>::iterator i = zorder.begin(); i != zorder.end(); i++) {
		int ax = (int)(x - (*i)->getParent()->x);
		int ay = (int)(y - (*i)->getParent()->y);
		if ((*i)->x <= ax) if ((*i)->y <= ay) if (((*i) -> x + (int)(*i)->getWidth()) >= ax) if (((*i) -> y + (int)(*i)->getHeight()) >= ay)
			if ((*i)->getParent() != theHand) {
				SpritePart *s = dynamic_cast<SpritePart *>(*i);
				if (s && s->isTransparent() && obey_all_transparency)
					if (s->transparentAt(ax - s->x, ay - s->y))
						continue;
				if (needs_mouseable && !((*i)->getParent()->mouseable()))
					continue;
				if (!obey_all_transparency)
					if ((*i)->getParent() != transagent)
						continue;
				return *i;
			}
	}
	
	return 0;
}

int World::getUNID(Agent *whofor) {
	do {
		int unid = rand();
		if (unid && unidmap[unid].expired()) {
			unidmap[unid] = whofor->shared_from_this();
			return unid;
		}
	} while (1);
}

void World::freeUNID(int unid) {
	unidmap.erase(unid);
}

Agent *World::lookupUNID(int unid) {
	return unidmap[unid].lock().get();
}

void World::drawWorld() {
	drawWorld(&camera, engine.backend->getMainSurface());
}

void World::drawWorld(Camera *cam, Surface *surface) {
	assert(surface);

	MetaRoom *m = cam->getMetaRoom();
	if (!m) {
		// Whoops - the room we're in vanished, or maybe we were never in one?
		// Try to get a new one ...
		m = map.getFallbackMetaroom();
		if (!m)
			throw creaturesException("drawWorld() couldn't find any metarooms");
		cam->goToMetaRoom(m->id);
	}
	int adjustx = cam->getX();
	int adjusty = cam->getY();
	shared_ptr<creaturesImage> bkgd = m->getBackground(""); // TODO

	// TODO: work out what c2e does when it doesn't have a background..
	if (!bkgd) return;

	assert(bkgd->numframes() > 0);

	int sprwidth = bkgd->width(0);
	int sprheight = bkgd->height(0);

	// draw the blk
	for (unsigned int i = 0; i < (m->fullheight() / sprheight); i++) {
		for (unsigned int j = 0; j < (m->fullwidth() / sprwidth); j++) {
			// figure out which block number to use
			unsigned int whereweare = j * (m->fullheight() / sprheight) + i;
			
			// make one pass for non-wraparound rooms, or two passes for wraparound ones
			// TODO: implement this in a more sensible way, or at least optimise it
			for (unsigned int z = 0; z < (m->wraparound() ? 2 : 1); z++) {
				int destx = (j * sprwidth) - adjustx + m->x();
				int desty = (i * sprheight) - adjusty + m->y();

				// if we're on the second pass, render to the *right* of the normal area
				if (z == 1) destx += m->width();

				// if the block's on screen, render it.
				if ((destx >= -sprwidth) && (desty >= -sprheight) &&
						(destx - sprwidth <= (int)surface->getWidth()) &&
						(desty - sprheight <= (int)surface->getHeight()))
					surface->render(bkgd, whereweare, destx, desty, false, 0, false, true);
			}
		}
	}

	// render all the agents
	for (std::multiset<renderable *, renderablezorder>::iterator i = renders.begin(); i != renders.end(); i++) {
		if ((*i)->showOnRemoteCameras() || cam == &camera) {
			// three-pass for wraparound rooms, the third since agents often straddle the boundary
			// TODO: same as above with background rendering
			for (unsigned int z = 0; z < (m->wraparound() ? 3 : 1); z++) {
				int newx = -adjustx, newy = -adjusty;
				if (z == 1) newx += m->width();
				else if (z == 2) newx -= m->width();
				(*i)->render(surface, newx, newy);
			}
		}
	}

	if (showrooms) {
		Room *r = map.roomAt(hand()->x, hand()->y);
		for (std::vector<Room *>::iterator i = cam->getMetaRoom()->rooms.begin();
				 i != cam->getMetaRoom()->rooms.end(); i++) {
			unsigned int col = 0xFFFF00CC;
			if (*i == r) col = 0xFF00FFCC;
			else if (r) {
				if ((**i).doors.find(r) != (**i).doors.end())
					col = 0x00FFFFCC;
			}

			// rooms don't wrap over the boundary, so just draw twice
			for (unsigned int z = 0; z < (m->wraparound() ? 2 : 1); z++) {
				int newx = adjustx;
				if (z == 1)
					newx -= m->width();

				(*i)->renderBorders(surface, newx, adjusty, col);
			}
		}
	}

	surface->renderDone();
}

void World::executeInitScript(fs::path p) {
	assert(fs::exists(p));
	assert(!fs::is_directory(p));

	std::string x = p.native_file_string();
	std::ifstream s(x.c_str());
	assert(s.is_open());
	//std::cout << "executing script " << x << "...\n";
	//std::cout.flush(); std::cerr.flush();
	try {
		caosScript script(gametype, x);
		script.parse(s);
		caosVM vm(0);
		script.installScripts();
		vm.runEntirely(script.installer);
	} catch (std::exception &e) {
		std::cerr << "exec of \"" << p.leaf() << "\" failed due to exception " << e.what() << std::endl;
	}
	std::cout.flush(); std::cerr.flush();
}

void World::executeBootstrap(fs::path p) {
	if (!fs::is_directory(p)) {
		executeInitScript(p);
		return;
	}

	std::vector<fs::path> scripts;
	
	fs::directory_iterator fsend;
	for (fs::directory_iterator d(p); d != fsend; ++d) {
		if ((!fs::is_directory(*d)) && (fs::extension(*d) == ".cos"))
			scripts.push_back(*d);
	}

	std::sort(scripts.begin(), scripts.end());
	for (std::vector<fs::path>::iterator i = scripts.begin(); i != scripts.end(); i++)
		executeInitScript(*i);
}

void World::executeBootstrap(bool switcher) {
	if (engine.version < 3) {
		// read from Eden.sfc
		
		if (data_directories.size() == 0)
			throw creaturesException("C1/2 can't run without data directories!");

		// TODO: case-sensitivity for the lose
		fs::path edenpath(data_directories[0] / "/Eden.sfc");
		if (fs::exists(edenpath) && !fs::is_directory(edenpath)) {
			SFCFile sfc;
			std::ifstream f(edenpath.native_directory_string().c_str(), std::ios::binary);
			f >> std::noskipws;
			sfc.read(&f);
			sfc.copyToWorld();
			return;
		} else
			throw creaturesException("couldn't find file Eden.sfc, required for C1/2");
	}

	// TODO: this code is possibly wrong with multiple bootstrap directories
	std::multimap<std::string, fs::path> bootstraps;

	for (std::vector<fs::path>::iterator i = data_directories.begin(); i != data_directories.end(); i++) {
		assert(fs::exists(*i));
		assert(fs::is_directory(*i));
		fs::path b(*i / "/Bootstrap/");
		if (fs::exists(b) && fs::is_directory(b)) {
			fs::directory_iterator fsend;
			// iterate through each bootstrap directory
			for (fs::directory_iterator d(b); d != fsend; ++d) {
				if (fs::exists(*d) && fs::is_directory(*d)) {
					std::string s = (*d).leaf();
					if (s == "000 Switcher") {
						if (!switcher) continue;
					} else {
						if (switcher) continue;
					}
					
					bootstraps.insert(std::pair<std::string, fs::path>(s, *d));
				}
			}
		}
	}

	for (std::multimap<std::string, fs::path>::iterator i = bootstraps.begin(); i != bootstraps.end(); i++) {
		executeBootstrap(i->second);
	}
}

void World::initCatalogue() {
	for (std::vector<fs::path>::iterator i = data_directories.begin(); i != data_directories.end(); i++) {
		assert(fs::exists(*i));
		assert(fs::is_directory(*i));

		fs::path c(*i / "/Catalogue/");
		if (fs::exists(c) && fs::is_directory(c))
			catalogue.initFrom(c);
	}
}

#include "PathResolver.h"
std::string World::findFile(std::string name) {
	// Go backwards, so we find files in more 'modern' directories first..
	for (int i = data_directories.size() - 1; i != -1; i--) {
		fs::path p = data_directories[i];
		std::string r = (p / fs::path(name, fs::native)).native_directory_string();
		if (resolveFile(r))
			return r;
	}
	
	return "";
}

std::vector<std::string> World::findFiles(std::string dir, std::string wild) {
	std::vector<std::string> possibles;
	
	// Go backwards, so we find files in more 'modern' directories first..
	for (int i = data_directories.size() - 1; i != -1; i--) {
		fs::path p = data_directories[i];
		std::string r = (p / fs::path(dir, fs::native)).native_directory_string();
		std::vector<std::string> results = findByWildcard(r, wild);
		possibles.insert(possibles.end(), results.begin(), results.end()); // merge results
	}

	return possibles;
}

std::string World::getUserDataDir() {
	return (data_directories.end() - 1)->native_directory_string();
}

void World::selectCreature(boost::shared_ptr<Agent> a) {
	if (a) {
		CreatureAgent *c = dynamic_cast<CreatureAgent *>(a.get());
		caos_assert(c);
	}
	selectedcreature = a;

	// TODO: send script 120 (selected creature changed) as needed
}

shared_ptr<genomeFile> World::loadGenome(std::string &genefile) {
	std::vector<std::string> possibles = findFiles("/Genetics/", genefile + ".gen");
	if (possibles.empty()) return shared_ptr<genomeFile>();
	std::string gfilename = possibles[(int)((float)possibles.size() * (rand() / (RAND_MAX + 1.0)))];

	// TODO: change genefile to be the file we selected

	shared_ptr<genomeFile> p(new genomeFile());
	std::ifstream gfile(gfilename.c_str(), std::ios::binary);
	caos_assert(gfile.is_open());
	gfile >> std::noskipws;
	gfile >> *(p.get());

	return p;
}

void World::newMoniker(shared_ptr<genomeFile> g, std::string genefile, AgentRef agent) {
	std::string d = history.newMoniker(g);
	world.history.getMoniker(d).addEvent(2, "", genefile);
	world.history.getMoniker(d).moveToAgent(agent);
}

std::string World::generateMoniker(std::string basename) {
	// TODO: is there a better way to handle this? incoming basename is from catalogue files..
	if (basename.size() != 4) {
		std::cout << "World::generateMoniker got passed '" << basename << "' as a basename which isn't 4 characters, so ignoring it" << std::endl;
		basename = "xxxx";
	}

	std::string x = basename;
	for (unsigned int i = 0; i < 4; i++) {
		unsigned int n = (unsigned int) (0xfffff * (rand() / (RAND_MAX + 1.0)));
		x = x + "-" + boost::str(boost::format("%05x") % n);
	}
	
	return x;
}

/* vim: set noet: */
