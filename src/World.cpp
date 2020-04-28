/*
 *  World.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
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
#include "caosScript.h"
#include "PointerAgent.h"
#include "CompoundAgent.h" // for setFocus
#include <cassert>
#include <limits.h> // for MAXINT
#include <memory>
#include "creaturesImage.h"
#include "creatures/CreatureAgent.h"
#include "Backend.h"
#include "AudioBackend.h"
#include "SFCFile.h"
#include "Room.h"
#include "MetaRoom.h"
#include "Catalogue.h"
#include "Camera.h"
#include "historyManager.h"
#include "imageManager.h"
#include "Map.h"
#include "MusicManager.h"
#include "prayManager.h"
#include "Scriptorium.h"

#include <fmt/printf.h>
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

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
	autostop = false;

	camera.reset(new MainCamera());
	gallery.reset(new imageManager());
	history.reset(new historyManager());
	map.reset(new Map());
	praymanager.reset(new prayManager());
	scriptorium.reset(new Scriptorium());
}

World::~World() {
	agents.clear();
	for (std::vector<caosVM *>::iterator i = vmpool.begin(); i != vmpool.end(); i++)
		delete *i;
}

#include "images/bmpImage.h"

// annoyingly, if we put this in the constructor, the catalogue isn't available yet
void World::init() {
	// First, try initialising the mouse cursor from the catalogue tag.
	if (engine.version > 2 && catalogue.hasTag("Pointer Information")) {
		const std::vector<std::string> &pointerinfo = catalogue.getTag("Pointer Information");
		if (pointerinfo.size() >= 3) {
			shared_ptr<creaturesImage> img = gallery->getImage(pointerinfo[2]);
			if (img) {
				theHand = new PointerAgent(pointerinfo[2]);
				int family, genus, species;
				if (sscanf(pointerinfo[0].c_str(), "%d %d %d", &family, &genus, &species) == 3)
					theHand->setClassifier(family, genus, species);
				int hotspotx, hotspoty;
				if (sscanf(pointerinfo[1].c_str(), "%d %d", &hotspotx, &hotspoty) == 2)
					theHand->setHotspot(hotspotx, hotspoty);
				if (gametype == "sm" && pointerinfo.size() >= 5) {
					// TODO: seamonkeys has 'numImages baseImage' too
					int blockwidth, blockheight;
					if (sscanf(pointerinfo[4].c_str(), "%d %d", &blockwidth, &blockheight) == 2) {
						bmpImage *bmpimg = dynamic_cast<bmpImage *>(img.get());
						if (bmpimg) bmpimg->setBlockSize(blockwidth, blockheight);
					}
				}
				theHand->finishInit();
			} else {
				std::cout << "There was a seemingly-useful \"Pointer Information\" catalogue tag provided, but sprite file '" << pointerinfo[2] << "' doesn't exist!" << std::endl;
			}
		}
	}
	
	// If for some reason we failed to do that (missing/bad catalogue tag? missing file?), try falling back to a sane default.
	if (!theHand) {
		shared_ptr<creaturesImage> img;
		if (gametype == "c3")
			img = gallery->getImage("hand"); // as used in C3 and DS
		else
			img = gallery->getImage("syst"); // as used in C1, C2 and CV
		if (img) {
			theHand = new PointerAgent(img->getName());
			if (engine.version > 2) {
				theHand->finishInit(); // SFCFile handles this for c1/c2
				std::cout << "Warning: No valid \"Pointer Information\" catalogue tag, defaulting to '" << img->getName() << "'." << std::endl;
			}
		} else {
			if (engine.version > 2)
				std::cout << "Couldn't find a valid \"Pointer Information\" catalogue tag, and c";
			else
				std::cout << "C";
			std::cout << "ouldn't find a pointer sprite, so not creating the pointer agent." << std::endl;
		}
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

	// adjust to default tick rate for C1/C2 if necessary
	if (engine.version < 3)
		ticktime = 100;
	
	timeofday = dayofseason = season = year = 0;
}

void World::shutdown() {
	agents.clear();
	uncontrolled_sounds.clear();
	map->Reset();
}

caosVM *World::getVM(Agent *a) {
	if (vmpool.empty()) {
		return new caosVM(a);
	} else {
		caosVM *x = vmpool.back();
		vmpool.pop_back();
		x->setOwner(a);
		x->resetScriptState();
		return x;
	}
}

void World::freeVM(caosVM *v) {
	// we don't reset the script state here because caosVM might be in our call stack and we don't want to reset the VM from under itself
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
void World::setFocus(CompoundPart *p) {
	assert(!p || p->canGainFocus());

	// Unfocus the current agent. Not sure if c2e always does this (what if the agent/part is bad?).
	if (focusagent) {
		CompoundAgent *c = dynamic_cast<CompoundAgent *>(focusagent.get());
		assert(c);
		CompoundPart *p = c->part(focuspart);
		if (p && p->canGainFocus())
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
		engine.done = true;
	}

	// Notify the audio backend about our current viewpoint center.
	engine.audio->setViewpointCenter(camera->getXCentre(), camera->getYCentre());

	std::list<std::pair<std::shared_ptr<AudioSource>, bool> >::iterator si = uncontrolled_sounds.begin();
	while (si != uncontrolled_sounds.end()) {
		std::list<std::pair<std::shared_ptr<AudioSource>, bool> >::iterator next = si; next++;
		if (si->first->getState() != SS_PLAY) {
			// sound is stopped, so release our reference
			uncontrolled_sounds.erase(si);
		} else {
			if (si->second) {
				// follow viewport
				si->first->setPos(camera->getXCentre(), camera->getYCentre(), 0);
			} else {
				// mute/unmute off-screen uncontrolled audio if necessary
				float x, y, z;
				si->first->getPos(x, y, z);
				if (engine.version > 2) // TODO: this is because of wrap issues, but we need a better fix
					si->first->setMute(camera->getMetaRoom() != world.map->metaRoomAt(x, y));
			}
		}

		si = next;
	}

	musicmanager.tick();

	// Tick all agents, deleting as necessary.
	std::list<std::shared_ptr<Agent> >::iterator i = agents.begin();
	while (i != agents.end()) {
		std::shared_ptr<Agent> a = *i;
		if (!a) {
			std::list<std::shared_ptr<Agent> >::iterator i2 = i;
			i2++;
			agents.erase(i);
			i = i2;
			continue;
		}
		i++;
		a->tick();
	}
	
	// Process the script queue.
	std::list<scriptevent> newqueue;
	for (std::list<scriptevent>::iterator i = scriptqueue.begin(); i != scriptqueue.end(); i++) {
		std::shared_ptr<Agent> agent = i->agent.lock();
		if (agent) {
			if (engine.version < 3) {
				// only try running a collision script if the agent doesn't have a running script
				// TODO: we don't really understand how script interruption in c1/c2 works
				if (agent->vm && !agent->vm->stopped() && i->scriptno == 6) {
					continue;
				}
			}
			agent->fireScript(i->scriptno, i->from, i->p[0], i->p[1]);
		}
	}
	scriptqueue.clear();
	scriptqueue = newqueue;

	tickcount++;
	worldtickcount++;

	if (engine.version == 2) {
		if (worldtickcount % 3600 == 0) {
			timeofday++;
			if (timeofday == 5) { // 5 parts of the day
				timeofday = 0;
				dayofseason++;
			}
			if (dayofseason == 4) { // 4 days per season
				dayofseason = 0;
				season++;
			}
			if (season == 4) { // 4 seasons per year
				season = 0;
				year++;
			}
		}
	}

	world.map->tick();

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

CompoundPart *World::partAt(unsigned int x, unsigned int y, bool obey_all_transparency, bool needs_mouseable, bool needs_clickable) {
	Agent *transagent = 0;
	if (!obey_all_transparency)
		transagent = agentAt(x, y, true, needs_mouseable);

	MetaRoom *m = world.map->metaRoomAt(x, y); // for wraparound checking

	for (std::multiset<CompoundPart *, partzorder>::iterator i = zorder.begin(); i != zorder.end(); i++) {
		CompoundPart *p = *i;
		if (p->getParent() == theHand) continue;

		int ax = (int)(x - p->getParent()->x);
		int ay = (int)(y - p->getParent()->y);

		// we check the wrap too..
		if (!m || !m->wraparound() || p->x > ax + (int)m->width() || p->x + (int)p->getWidth() < ax + (int)m->width()) {
			if (p->x > ax) continue;
			if (p->x + (int)p->getWidth() < ax) continue;
		} else {
			// wrapped!
			ax += m->width();
		}

		if (p->y > ay) continue;
		if (p->y + (int)p->getHeight() < ay) continue;

		SpritePart *s = dynamic_cast<SpritePart *>(p);
		if (s && s->isTransparent() && obey_all_transparency) {
			// transparent parts in C1/C2 are scenery
			// TODO: always true? you can't sekritly set parts to be transparent in C2?
			if (engine.version < 3 || s->transparentAt(ax - s->x, ay - s->y))
				continue;
		}

		if (needs_mouseable && !(p->getParent()->mouseable()))
			continue;

		if (needs_clickable && !(p->canClick()))
			continue;

		if (!obey_all_transparency)
			if (p->getParent() != transagent)
				continue;

		return p;
	}
	
	return 0;
}

void World::setUNID(Agent *whofor, int unid) {
	assert(whofor->shared_from_this() == unidmap[unid].lock() || unidmap[unid].expired());
	whofor->unid = unid;
	unidmap[unid] = whofor->shared_from_this();
}

int World::newUNID(Agent *whofor) {
	do {
		int unid = rand();
		if (unid && unidmap[unid].expired()) {
			setUNID(whofor, unid);
			return unid;
		}
	} while (1);
}

void World::freeUNID(int unid) {
	unidmap.erase(unid);
}

shared_ptr<Agent> World::lookupUNID(int unid) {
	if (unid == 0) return shared_ptr<Agent>();
	return unidmap[unid].lock();
}

void World::drawWorld() {
	drawWorld(camera.get(), engine.backend->getMainRenderTarget());
}

void World::drawWorld(Camera *cam, RenderTarget *surface) {
	assert(surface);

	MetaRoom *m = cam->getMetaRoom();
	if (!m) {
		// Whoops - the room we're in vanished, or maybe we were never in one?
		// Try to get a new one ...
		m = map->getFallbackMetaroom();
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
	unsigned int heightinsprites = m->fullheight() / sprheight;
	unsigned int widthinsprites = m->fullwidth() / sprwidth;
	for (unsigned int i = 0; i < heightinsprites; i++) {
		for (int j = widthinsprites - 1; j >= 0; j--) { // reverse order, so wrapping always works
			// figure out which block number to use
			unsigned int whereweare = j * heightinsprites + i;
			
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
		if ((*i)->showOnRemoteCameras() || cam == camera.get()) {
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

	// render port connection lines. TODO: these should be rendered as some kind
	// of renderable, not directly like this.
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> a = *i;
		if (!a) continue;
		for (std::map<unsigned int, std::shared_ptr<OutputPort> >::iterator p = a->outports.begin();
		     p != a->outports.end(); p++) {
			for (PortConnectionList::iterator c = p->second->dests.begin(); c != p->second->dests.end(); c++) {
				if (!c->first) continue;
				InputPort *target = c->first->inports[c->second].get();
				surface->renderLine(a->x + p->second->x - adjustx, a->y + p->second->y - adjusty,
						c->first->x + target->x - adjustx, c->first->y + target->y - adjusty, 0x00ff00ff);
			}
		}
	}

	if (showrooms) {
		shared_ptr<Room> r = map->roomAt(hand()->x, hand()->y);
		for (std::vector<shared_ptr<Room> >::iterator i = cam->getMetaRoom()->rooms.begin();
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

	if (hand()->holdingWire) {
		if (!hand()->wireOriginAgent) {
			hand()->holdingWire = 0;
		} else {
			int x, y;
			if (hand()->holdingWire == 1) {
				// holding from outport
				OutputPort *out = hand()->wireOriginAgent->outports[hand()->wireOriginID].get();
				x = out->x; y = out->y;
			} else {
				// holding from inport
				InputPort *in = hand()->wireOriginAgent->inports[hand()->wireOriginID].get();
				x = in->x; y = in->y;
			}
			surface->renderLine(x + hand()->wireOriginAgent->x - adjustx,
					y + hand()->wireOriginAgent->y - adjusty, hand()->x - adjustx, hand()->y - adjusty, 0x00ff00ff);
		}
	}

	surface->renderDone();
}

void World::executeInitScript(fs::path p) {
	assert(fs::exists(p));
	assert(!fs::is_directory(p));

	std::string x = p.string();
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
	} catch (creaturesException &e) {
		std::cerr << "exec of \"" << p.filename() << "\" failed due to exception " << e.prettyPrint() << std::endl;
	} catch (std::exception &e) {
		std::cerr << "exec of \"" << p.filename() << "\" failed due to exception " << e.what() << std::endl;
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
		if ((!fs::is_directory(*d)) && (d->path().extension().string() == ".cos"))
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
		fs::path edenpath(data_directories[0] / "Eden.sfc");
		if (fs::exists(edenpath) && !fs::is_directory(edenpath)) {
			SFCFile sfc;
			std::ifstream f(edenpath.string().c_str(), std::ios::binary);
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
		fs::path b(*i / "Bootstrap/");
		if (fs::exists(b) && fs::is_directory(b)) {
			fs::directory_iterator fsend;
			// iterate through each bootstrap directory
			for (fs::directory_iterator d(b); d != fsend; ++d) {
				if (fs::exists(*d) && fs::is_directory(*d)) {
					std::string s = d->path().filename().string();
					// TODO: cvillage has switcher code in 'Startup', so i included it here too
					if (s == "000 Switcher" || s == "Startup") {
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

		fs::path c(*i / "Catalogue/");
		if (fs::exists(c) && fs::is_directory(c))
			catalogue.initFrom(c, "en");
	}
}

#include "PathResolver.h"
std::string World::findFile(std::string name) {
	// Go backwards, so we find files in more 'modern' directories first..
	for (int i = data_directories.size() - 1; i != -1; i--) {
		fs::path p = data_directories[i];
		std::string r = (p / fs::path(name)).string();
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
		std::string r = (p / fs::path(dir)).string();
		std::vector<std::string> results = findByWildcard(r, wild);
		possibles.insert(possibles.end(), results.begin(), results.end()); // merge results
	}

	return possibles;
}

std::string World::getUserDataDir() {
	if (data_directories.size() ==  0) {
		throw creaturesException("Can't get user data directory when there are no data directories");
	}
	return data_directories.back().string();
}

void World::selectCreature(std::shared_ptr<Agent> a) {
	if (a) {
		CreatureAgent *c = dynamic_cast<CreatureAgent *>(a.get());
		caos_assert(c);
	}

	if (selectedcreature != a) {
		for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
			if (!*i) continue;
			(*i)->queueScript(120, 0, caosVar(a), caosVar(selectedcreature)); // selected creature changed
		}

		selectedcreature = a;
	}
}

shared_ptr<genomeFile> World::loadGenome(std::string &genefile) {
	std::vector<std::string> possibles = findFiles("Genetics/", genefile + ".gen");
	if (possibles.empty()) return shared_ptr<genomeFile>();
	genefile = possibles[(int)((float)possibles.size() * (rand() / (RAND_MAX + 1.0)))];

	shared_ptr<genomeFile> p(new genomeFile());
	std::ifstream gfile(genefile.c_str(), std::ios::binary);
	caos_assert(gfile.is_open());
	gfile >> std::noskipws;
	gfile >> *(p.get());

	return p;
}

void World::newMoniker(shared_ptr<genomeFile> g, std::string genefile, AgentRef agent) {
	std::string d = history->newMoniker(g);
	world.history->getMoniker(d).addEvent(2, "", genefile);
	world.history->getMoniker(d).moveToAgent(agent);
}

std::string World::generateMoniker(std::string basename) {
	if (engine.version < 3) {
		/* old-style monikers are four characters in a format like 9GVC */
		unsigned int n = 1 + (unsigned int)(9.0 * (rand() / (RAND_MAX + 1.0)));
		std::string moniker = fmt::sprintf("%d", n);
		for (unsigned int i = 0; i < 3; i++) {
			unsigned int n = (unsigned int)(26.0 * (rand() / (RAND_MAX + 1.0)));
			moniker += fmt::sprintf("%c", (char)('A' + n));
		}
		return moniker;
	}

	// TODO: is there a better way to handle this? incoming basename is from catalogue files..
	if (basename.size() != 4) {
		std::cout << "World::generateMoniker got passed '" << basename << "' as a basename which isn't 4 characters, so ignoring it" << std::endl;
		basename = "xxxx";
	}

	std::string x = basename;
	for (unsigned int i = 0; i < 4; i++) {
		unsigned int n = (unsigned int) (0xfffff * (rand() / (RAND_MAX + 1.0)));
		x = x + "-" + fmt::sprintf("%05x", n);
	}
	
	return x;
}

std::shared_ptr<AudioSource> World::playAudio(std::string filename, AgentRef agent, bool controlled, bool loop, bool followviewport) {
	if (filename.size() == 0) return std::shared_ptr<AudioSource>();

	std::shared_ptr<AudioSource> sound = engine.audio->loadClip(filename);
	if (!sound) {
		// note that more specific error messages can be thrown by implementations of loadClip
		if (engine.version < 3) return std::shared_ptr<AudioSource>(); // creatures 1 and 2 ignore non-existent audio clips
		throw creaturesException("failed to load audio clip " + filename);
	}

	if (loop) {
		assert(controlled);
		sound->setLooping(true);
	}

	if (agent) {
		assert(!followviewport);

		agent->updateAudio(sound);
		if (controlled)
			agent->sound = sound;
		else
			uncontrolled_sounds.push_back(std::pair<std::shared_ptr<class AudioSource>, bool>(sound, false));
	} else {
		assert(!controlled);

		// TODO: handle non-agent sounds
		sound->setPos(camera->getXCentre(), camera->getYCentre(), 0);
		uncontrolled_sounds.push_back(std::pair<std::shared_ptr<class AudioSource>, bool>(sound, followviewport));
	}
	
	sound->play();

	return sound;
}

int World::findCategory(unsigned char family, unsigned char genus, unsigned short species) {
	if (engine.version < 3) {
		if (family == 2 && genus == 1 && species == 1) return genus; // 2 1 1 (hand) -> 1
		if (family == 2 && genus > 1 && genus < 26) return genus; // 2 2 0 to 2 25 0 -> 2 to 25
		if (family == 3 && genus < 10) return genus + 25; // 3 1 0 to 3 9 0 -> 26 to 35
		if (family == 4 && genus < 5) return genus + 35; // 4 1 0 to 4 4 0 -> 36 to 39

		return -1;
	}

	if (!catalogue.hasTag("Agent Classifiers")) return -1;

	const std::vector<std::string> &t = catalogue.getTag("Agent Classifiers");

	for (unsigned int i = 0; i < t.size(); i++) {
		std::string buffer = fmt::sprintf("%d %d %d", (int)family, (int)genus, (int)species);
		if (t[i] == buffer) return i;
		buffer = fmt::sprintf("%d %d 0", (int)family, (int)genus);
		if (t[i] == buffer) return i;
		buffer = fmt::sprintf("%d 0 0", (int)family);
		if (t[i] == buffer) return i;
		// leave it here: 0 0 0 would be silly to have in Agent Classifiers.
	}

	return -1;
}

/* vim: set noet: */
