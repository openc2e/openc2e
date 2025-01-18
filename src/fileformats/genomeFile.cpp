/*
 *  genomeFile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 13 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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
#include "genomeFile.h"

#include "common/endianlove.h"

#include <cstring>
#include <exception>
#include <iostream>
#include <typeinfo>

geneNote* genomeFile::findNote(uint8_t type, uint8_t subtype, uint8_t which) {
	for (auto& x : genes) {
		gene* t = x.get();
		if ((uint8_t)t->type() == type)
			if ((uint8_t)t->subtype() == subtype)
				if ((uint8_t)t->note.which == which)
					return &t->note;
	}

	return 0;
}

void genomeFile::readNotes(std::istream& s) {
	if (cversion == 3) {
		(void)read16le(s); // gnover
		uint16_t nosvnotes = read16le(s);
		fmt::print("we have {} notes\n", nosvnotes);

		for (int i = 0; i < nosvnotes; i++) {
			(void)read16le(s); // type
			(void)read16le(s); // subtype
			(void)read16le(s); // which
			(void)read16le(s); // rule

			// TODO: we currently skip all the notes (note that there are 18 and then 1!)
			for (int i = 0; i < 19; i++) {
				uint16_t skip = read16le(s);
				uint8_t* dummy = new uint8_t[skip];
				s.read((char*)dummy, skip);
				delete[] dummy;
			}
		}

		uint16_t ver = 0;

		while (ver != 0x02) {
			if (s.fail() || s.eof())
				throw Exception("c3 gno loading broke ... second magic not present");
			ver = read16le(s);
		}
	}

	uint16_t noentries = read16le(s);

	for (int i = 0; i < noentries; i++) {
		uint16_t type = read16le(s);
		uint16_t subtype = read16le(s);
		uint32_t which = read32le(s);

		geneNote* n = findNote(type, subtype, which);

		uint16_t buflen = read16le(s);
		char* buffer = new char[buflen + 1];
		s.read(buffer, buflen);
		buffer[buflen] = 0;
		if (n != 0)
			n->description = buffer;
		buflen = read16le(s);
		delete[] buffer;
		buffer = new char[buflen + 1];
		s.read(buffer, buflen);
		buffer[buflen] = 0;
		if (n != 0)
			n->comments = buffer;
		delete[] buffer;
	}
}

void genomeFile::writeNotes(std::ostream&) const {
	// TODO
}

gene* genomeFile::nextGene(std::istream& s) {
	uint8_t majic[3];
	s.read((char*)majic, 3);
	if (strncmp((char*)majic, "gen", 3) != 0)
		throw Exception("bad majic for a gene");

	majic[0] = read8(s);
	if (majic[0] == 'd')
		return 0;
	if (majic[0] != 'e')
		throw Exception("bad majic at stage2 for a gene");

	uint8_t type = read8(s);
	uint8_t subtype = read8(s);

	gene* g = 0;

	// the switch statement of doom... is there a better way to do this?
	switch (type) {
		case 0:
			switch (subtype) {
				case 0:
					switch (cversion) {
						case 1: g = new oldBrainLobeGene(cversion); break;
						case 2: g = new oldBrainLobeGene(cversion); break;
						case 3: g = new c2eBrainLobeGene(cversion); break;
						default: g = 0; break;
					}
					break;
				case 1: g = new organGene(cversion, true); break;
				case 2: g = new c2eBrainTractGene(cversion); break;
			}
			break;
		case 1:
			switch (subtype) {
				case 0: g = new bioReceptorGene(cversion); break;
				case 1: g = new bioEmitterGene(cversion); break;
				case 2: g = new bioReactionGene(cversion); break;
				case 3: g = new bioHalfLivesGene(cversion); break;
				case 4: g = new bioInitialConcentrationGene(cversion); break;
				case 5: g = new bioNeuroEmitterGene(cversion); break;
			}
			break;
		case 2:
			switch (subtype) {
				case 0: g = new creatureStimulusGene(cversion); break;
				case 1: g = new creatureGenusGene(cversion); break;
				case 2: g = new creatureAppearanceGene(cversion); break;
				case 3: g = new creaturePoseGene(cversion); break;
				case 4: g = new creatureGaitGene(cversion); break;
				case 5: g = new creatureInstinctGene(cversion); break;
				case 6: g = new creaturePigmentGene(cversion); break;
				case 7: g = new creaturePigmentBleedGene(cversion); break;
				case 8: g = new creatureFacialExpressionGene(cversion); break;
			}
			break;
		case 3:
			switch (subtype) {
				case 0: g = new organGene(cversion, false); break;
			}
			break;
	}

	if (g == 0)
		throw Exception("genefactory failed");

	genes.push_back(std::unique_ptr<gene>(g));
	s >> *g;

	return g;
}

std::istream& operator>>(std::istream& s, genomeFile& f) {
	char majic[3];
	s.read(majic, 3);
	if (strncmp((char*)majic, "gen", 3) == 0) {
		majic[0] = read8(s);
		if (majic[0] == 'e')
			f.cversion = 1;
		else
			throw Exception("bad majic for genome");

		s.seekg(0, std::ios::beg);
	} else {
		if (strncmp((char*)majic, "dna", 3) != 0)
			throw Exception("bad majic for genome");

		majic[0] = read8(s);
		f.cversion = majic[0] - 48; // 48 = ASCII '0'
		if ((f.cversion < 1) || (f.cversion > 3))
			throw Exception("unsupported genome version in majic");
	}

	//fmt::print("creaturesGenomeFile: reading genome of version {}.\n", (unsigned int)f.cversion);
	f.currorgan = 0;
	while (f.nextGene(s) != 0) {
	}
	f.currorgan = 0;
	//fmt::print("creaturesGenomeFile: read {} top-level genes.\n", (unsigned int)f.genes.size());

	return s;
}

std::ostream& operator<<(std::ostream& s, const genomeFile& f) {
	char majic[4] = {'d', 'n', 'a', static_cast<char>(f.cversion + 48)}; // 48 = ASCII '0';
	s.write(majic, 4);

	// iterate through genes
	for (auto& gene : ((genomeFile&)f).genes) {
		s << *gene;
	}

	s.write("gend", 4);

	return s;
}

gene* genomeFile::getGene(uint8_t type, uint8_t subtype, unsigned int seq) {
	unsigned int c = 0;
	for (auto& gene : genes) {
		if (gene->type() == type)
			if (gene->subtype() == subtype) {
				c++;
				if (seq == c)
					return gene.get();
			}
	}

	return nullptr;
}

uint8_t geneFlags::operator()() const {
	return ((_mutable ? 1 : 0) + (dupable ? 2 : 0) + (delable ? 4 : 0) + (maleonly ? 8 : 0) +
			(femaleonly ? 16 : 0) + (notexpressed ? 32 : 0) + (reserved1 ? 64 : 0) + (reserved2 ? 128 : 0));
}

void geneFlags::operator()(uint8_t f) {
	_mutable = ((f & 1) != 0);
	dupable = ((f & 2) != 0);
	delable = ((f & 4) != 0);
	maleonly = ((f & 8) != 0);
	femaleonly = ((f & 16) != 0);
	notexpressed = ((f & 32) != 0);
	reserved1 = ((f & 64) != 0);
	reserved2 = ((f & 128) != 0);
}

std::ostream& operator<<(std::ostream& s, const gene& g) {
	s.write("gene", 4);
	write8(s, g.type());
	write8(s, g.subtype());

	write8(s, g.note.which);
	write8(s, g.header.generation);
	write8(s, g.header.switchontime);
	write8(s, g.header.flags());
	if (g.cversion > 1)
		write8(s, g.header.mutweighting);
	if (g.cversion == 3)
		write8(s, g.header.variant);

	g.write(s);

	return s;
}

std::istream& operator>>(std::istream& s, gene& g) {
	g.note.which = read8(s);
	g.header.generation = read8(s);
	g.header.switchontime = (lifestage)read8(s);
	g.header.flags(read8(s));

	if (g.cversion > 1)
		g.header.mutweighting = read8(s);
	if (g.cversion == 3)
		g.header.variant = read8(s);

	g.read(s);

	return s;
}

void bioEmitterGene::write(std::ostream& s) const {
	write8(s, organ);
	write8(s, tissue);
	write8(s, locus);
	write8(s, chemical);
	write8(s, threshold);
	write8(s, rate);
	write8(s, gain);
	uint8_t flags = (clear ? 1 : 0) + (digital ? 2 : 0) + (invert ? 4 : 0);
	write8(s, flags);
}

void bioEmitterGene::read(std::istream& s) {
	organ = read8(s);
	tissue = read8(s);
	locus = read8(s);
	chemical = read8(s);
	threshold = read8(s);
	rate = read8(s);
	gain = read8(s);
	uint8_t flags = read8(s);
	clear = ((flags & 1) != 0);
	digital = ((flags & 2) != 0);
	invert = ((flags & 4) != 0);
}

void bioHalfLivesGene::write(std::ostream& s) const {
	for (unsigned char halflive : halflives) {
		write8(s, halflive);
	}
}

void bioHalfLivesGene::read(std::istream& s) {
	for (unsigned char& halflive : halflives) {
		halflive = read8(s);
	}
}

void bioInitialConcentrationGene::write(std::ostream& s) const {
	write8(s, chemical);
	write8(s, quantity);
}

void bioInitialConcentrationGene::read(std::istream& s) {
	chemical = read8(s);
	quantity = read8(s);
}

void bioNeuroEmitterGene::write(std::ostream& s) const {
	for (int i = 0; i < 3; i++) {
		write8(s, lobes[i]);
		write8(s, neurons[i]);
	}
	write8(s, rate);
	for (int i = 0; i < 4; i++) {
		write8(s, chemical[i]);
		write8(s, quantity[i]);
	}
}

void bioNeuroEmitterGene::read(std::istream& s) {
	for (int i = 0; i < 3; i++) {
		lobes[i] = read8(s);
		neurons[i] = read8(s);
	}
	rate = read8(s);
	for (int i = 0; i < 4; i++) {
		chemical[i] = read8(s);
		quantity[i] = read8(s);
	}
}

void bioReactionGene::write(std::ostream& s) const {
	for (int i = 0; i < 4; i++) {
		write8(s, quantity[i]);
		write8(s, reactant[i]);
	}

	write8(s, rate);
}

void bioReactionGene::read(std::istream& s) {
	for (int i = 0; i < 4; i++) {
		quantity[i] = read8(s);
		reactant[i] = read8(s);
	}

	rate = read8(s);
}

void bioReceptorGene::write(std::ostream& s) const {
	write8(s, organ);
	write8(s, tissue);
	write8(s, locus);
	write8(s, chemical);
	write8(s, threshold);
	write8(s, nominal);
	write8(s, gain);
	uint8_t flags = (inverted ? 1 : 0) + (digital ? 2 : 0);
	write8(s, flags);
}

void bioReceptorGene::read(std::istream& s) {
	organ = read8(s);
	tissue = read8(s);
	locus = read8(s);
	chemical = read8(s);
	threshold = read8(s);
	nominal = read8(s);
	gain = read8(s);

	uint8_t flags = read8(s);
	inverted = ((flags & 1) != 0);
	digital = ((flags & 2) != 0);
}

void c2eBrainLobeGene::write(std::ostream& s) const {
	for (unsigned char i : id)
		write8(s, i);

	write16be(s, updatetime);
	write16be(s, x);
	write16be(s, y);

	write8(s, width);
	write8(s, height);
	write8(s, red);
	write8(s, green);
	write8(s, blue);
	write8(s, WTA);
	write8(s, tissue);
	write8(s, initrulealways);

	for (unsigned char i : spare)
		write8(s, i);
	for (unsigned char i : initialiserule)
		write8(s, i);
	for (unsigned char i : updaterule)
		write8(s, i);
}

void c2eBrainLobeGene::read(std::istream& s) {
	for (unsigned char& i : id)
		i = read8(s);

	updatetime = read16be(s);
	x = read16be(s);
	y = read16be(s);

	width = read8(s);
	height = read8(s);
	red = read8(s);
	green = read8(s);
	blue = read8(s);
	WTA = read8(s);
	tissue = read8(s);
	initrulealways = read8(s);

	for (unsigned char& i : spare)
		i = read8(s);
	for (unsigned char& i : initialiserule)
		i = read8(s);
	for (unsigned char& i : updaterule)
		i = read8(s);
}

void c2eBrainTractGene::write(std::ostream& s) const {
	write16be(s, updatetime);
	for (unsigned char i : srclobe)
		write8(s, i);
	write16be(s, srclobe_lowerbound);
	write16be(s, srclobe_upperbound);
	write16be(s, src_noconnections);
	for (unsigned char i : destlobe)
		write8(s, i);
	write16be(s, destlobe_lowerbound);
	write16be(s, destlobe_upperbound);
	write16be(s, dest_noconnections);
	write8(s, migrates);
	write8(s, norandomconnections);
	write8(s, srcvar);
	write8(s, destvar);
	write8(s, initrulealways);
	for (unsigned char i : spare)
		write8(s, i);
	for (unsigned char i : initialiserule)
		write8(s, i);
	for (unsigned char i : updaterule)
		write8(s, i);
}

void c2eBrainTractGene::read(std::istream& s) {
	updatetime = read16be(s);
	for (unsigned char& i : srclobe)
		i = read8(s);
	srclobe_lowerbound = read16be(s);
	srclobe_upperbound = read16be(s);
	src_noconnections = read16be(s);
	for (unsigned char& i : destlobe)
		i = read8(s);
	destlobe_lowerbound = read16be(s);
	destlobe_upperbound = read16be(s);
	dest_noconnections = read16be(s);
	migrates = read8(s);
	norandomconnections = read8(s);
	srcvar = read8(s);
	destvar = read8(s);
	initrulealways = read8(s);
	for (unsigned char& i : spare)
		i = read8(s);
	for (unsigned char& i : initialiserule)
		i = read8(s);
	for (unsigned char& i : updaterule)
		i = read8(s);
}

void creatureAppearanceGene::write(std::ostream& s) const {
	write8(s, part);
	write8(s, variant);
	if (cversion > 1)
		write8(s, species);
}

void creatureAppearanceGene::read(std::istream& s) {
	part = read8(s);
	variant = read8(s);
	if (cversion > 1)
		species = read8(s);
}

void creatureFacialExpressionGene::write(std::ostream& s) const {
	write16le(s, expressionno);
	write8(s, weight);

	for (int i = 0; i < 4; i++) {
		write8(s, drives[i]);
		write8(s, amounts[i]);
	}
}

void creatureFacialExpressionGene::read(std::istream& s) {
	expressionno = read16le(s);
	weight = read8(s);

	for (int i = 0; i < 4; i++) {
		drives[i] = read8(s);
		amounts[i] = read8(s);
	}
}

void creatureGaitGene::write(std::ostream& s) const {
	write8(s, drive);

	for (int i = 0; i < gaitLength(); i++) {
		write8(s, pose[i]);
	}
}

void creatureGaitGene::read(std::istream& s) {
	drive = read8(s);

	for (int i = 0; i < gaitLength(); i++) {
		pose[i] = read8(s);
	}
}

void creatureGenusGene::write(std::ostream& s) const {
	write8(s, genus);

	// TODO: we read past the end of the returned buffer here!
	s.write(mum.c_str(), (cversion == 3) ? 32 : 4);
	s.write(dad.c_str(), (cversion == 3) ? 32 : 4);
}

void creatureGenusGene::read(std::istream& s) {
	genus = read8(s);

	char buf[33];
	unsigned int len = ((cversion == 3) ? 32 : 4);

	s.read(buf, len);
	buf[len] = 0;
	mum = (char*)buf;

	s.read(buf, len);
	buf[len] = 0;
	dad = (char*)buf;
}

void creatureInstinctGene::write(std::ostream& s) const {
	for (int i = 0; i < 3; i++) {
		write8(s, lobes[i]);
		write8(s, neurons[i]);
	}

	write8(s, action);
	write8(s, drive);
	write8(s, level);
}

void creatureInstinctGene::read(std::istream& s) {
	for (int i = 0; i < 3; i++) {
		lobes[i] = read8(s);
		neurons[i] = read8(s);
	}

	action = read8(s);
	drive = read8(s);
	level = read8(s);
}

void creaturePigmentGene::write(std::ostream& s) const {
	write8(s, color);
	write8(s, amount);
}

void creaturePigmentGene::read(std::istream& s) {
	color = read8(s);
	amount = read8(s);
}

void creaturePigmentBleedGene::write(std::ostream& s) const {
	write8(s, rotation);
	write8(s, swap);
}

void creaturePigmentBleedGene::read(std::istream& s) {
	rotation = read8(s);
	swap = read8(s);
}

void creaturePoseGene::write(std::ostream& s) const {
	write8(s, poseno);

	for (int i = 0; i < poseLength(); i++) {
		write8(s, pose[i]);
	}
}

void creaturePoseGene::read(std::istream& s) {
	poseno = read8(s);

	for (int i = 0; i < poseLength(); i++) {
		pose[i] = read8(s);
	}
}

void creatureStimulusGene::write(std::ostream& s) const {
	write8(s, stim);
	write8(s, significance);
	write8(s, sensoryneuron);
	write8(s, intensity);
	uint8_t flags = (modulate ? 1 : 0) + (addoffset ? 2 : 0) + (whenasleep ? 4 : 0);
	if (silent[0])
		flags += 16;
	if (silent[1])
		flags += 32;
	if (silent[2])
		flags += 64;
	if (silent[3])
		flags += 128;
	write8(s, flags);

	for (int i = 0; i < 4; i++) {
		write8(s, drives[i]);
		write8(s, amounts[i]);
	}
}

void creatureStimulusGene::read(std::istream& s) {
	stim = read8(s);
	significance = read8(s);
	sensoryneuron = read8(s);
	intensity = read8(s);
	uint8_t flags = read8(s);
	modulate = ((flags & 1) != 0);
	addoffset = ((flags & 2) != 0);
	whenasleep = ((flags & 4) != 0);

	for (int i = 0; i < 4; i++) {
		drives[i] = read8(s);
		amounts[i] = read8(s);
	}

	silent[0] = ((flags & 16) != 0);
	silent[1] = ((flags & 32) != 0);
	silent[2] = ((flags & 64) != 0);
	silent[3] = ((flags & 128) != 0);
}

void oldBrainLobeGene::write(std::ostream& s) const {
	write8(s, x);
	write8(s, y);
	write8(s, width);
	write8(s, height);
	write8(s, perceptflag);
	write8(s, nominalthreshold);
	write8(s, leakagerate);
	write8(s, reststate);
	write8(s, inputgain);
	s.write((char*)staterule, (cversion == 1) ? 8 : 12);
	write8(s, flags);

	s << dendrite1;
	s << dendrite2;
}

void oldBrainLobeGene::read(std::istream& s) {
	x = read8(s);
	y = read8(s);
	width = read8(s);
	height = read8(s);
	perceptflag = read8(s);
	nominalthreshold = read8(s);
	leakagerate = read8(s);
	reststate = read8(s);
	inputgain = read8(s);
	s.read((char*)staterule, (cversion == 1) ? 8 : 12);
	flags = read8(s);

	s >> dendrite1;
	s >> dendrite2;
}

void organGene::write(std::ostream& s) const {
	write8(s, clockrate);
	write8(s, damagerate);
	write8(s, lifeforce);
	write8(s, biotickstart);
	write8(s, atpdamagecoefficient);
}

void organGene::read(std::istream& s) {
	clockrate = read8(s);
	damagerate = read8(s);
	lifeforce = read8(s);
	biotickstart = read8(s);
	atpdamagecoefficient = read8(s);
}

std::ostream& operator<<(std::ostream& s, const oldDendriteInfo& i) {
	write8(s, i.srclobe);
	write8(s, i.min);
	write8(s, i.max);
	write8(s, i.spread);
	write8(s, i.fanout);
	write8(s, i.minLTW);
	write8(s, i.maxLTW);
	write8(s, i.minstr);
	write8(s, i.maxstr);
	write8(s, i.migrateflag);
	write8(s, i.relaxsuscept);
	write8(s, i.relaxSTW);
	write8(s, i.LTWgainrate);

	write8(s, i.strgain);
	s.write((char*)i.strgainrule, (i.cversion == 1) ? 8 : 12);
	write8(s, i.strloss);
	s.write((char*)i.strlossrule, (i.cversion == 1) ? 8 : 12);
	s.write((char*)i.susceptrule, (i.cversion == 1) ? 8 : 12);
	s.write((char*)i.relaxrule, (i.cversion == 1) ? 8 : 12);

	if (i.cversion == 2) {
		s.write((char*)i.backproprule, 12);
		s.write((char*)i.forproprule, 12);
	}

	return s;
}

std::istream& operator>>(std::istream& s, oldDendriteInfo& i) {
	i.srclobe = read8(s);
	i.min = read8(s);
	i.max = read8(s);
	i.spread = read8(s);
	i.fanout = read8(s);
	i.minLTW = read8(s);
	i.maxLTW = read8(s);
	i.minstr = read8(s);
	i.maxstr = read8(s);
	i.migrateflag = read8(s);
	i.relaxsuscept = read8(s);
	i.relaxSTW = read8(s);
	i.LTWgainrate = read8(s);

	i.strgain = read8(s);
	s.read((char*)i.strgainrule, (i.cversion == 1) ? 8 : 12);
	i.strloss = read8(s);
	s.read((char*)i.strlossrule, (i.cversion == 1) ? 8 : 12);
	s.read((char*)i.susceptrule, (i.cversion == 1) ? 8 : 12);
	s.read((char*)i.relaxrule, (i.cversion == 1) ? 8 : 12);

	if (i.cversion == 2) {
		s.read((char*)i.backproprule, 12);
		s.read((char*)i.forproprule, 12);
	}

	return s;
}

/* vim: set noet: */
