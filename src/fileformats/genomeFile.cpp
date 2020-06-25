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
#include "endianlove.h"
#include <cstring>
#include <typeinfo>
#include <exception>
#include <iostream>

geneNote *genomeFile::findNote(uint8_t type, uint8_t subtype, uint8_t which) {
	for (std::vector<gene *>::iterator x = genes.begin(); x != genes.end(); x++) {
		gene *t = *x;
			if ((uint8_t)t->type() == type)
				if ((uint8_t)t->subtype() == subtype)
					if ((uint8_t)t->note.which == which)
						return &t->note;

	if (typeid(*t) == typeid(organGene))
		for (std::vector<gene *>::iterator y = ((organGene *)t)->genes.begin(); y != ((organGene *)t)->genes.end(); y++) {
			gene *s = *y;
				if ((uint8_t)s->type() == type)
				if ((uint8_t)s->subtype() == subtype)
					if ((uint8_t)s->note.which == which)
						return &s->note;
		}
	}

	return 0;
}

void genomeFile::readNotes(std::istream &s) {
	if (cversion == 3) {
		uint16_t gnover = read16le(s);
		uint16_t nosvnotes = read16le(s);
		std::cout << "we have " << nosvnotes << " notes" << std::endl;

		for (int i = 0; i < nosvnotes; i++) {
			uint16_t type = read16le(s);
			uint16_t subtype = read16le(s);
			uint16_t which = read16le(s);
			uint16_t rule = read16le(s);

			// TODO: we currently skip all the notes (note that there are 18 and then 1!)
			for (int i = 0; i < 19; i++) {
				uint16_t skip = read16le(s);
				uint8_t *dummy = new uint8_t[skip]; s.read((char *)dummy, skip); delete[] dummy;
			}
			}

		uint16_t ver = 0;

		while (ver != 0x02) {
			if (s.fail() || s.eof()) throw creaturesException("c3 gno loading broke ... second magic not present");
			ver = read16le(s);
		}
	}

	uint16_t noentries = read16le(s);

	for (int i = 0; i < noentries; i++) {
		uint16_t type = read16le(s);
		uint16_t subtype = read16le(s);
		uint32_t which = read32le(s);

		geneNote *n = findNote(type, subtype, which);

		uint16_t buflen = read16le(s);
		char *buffer = new char[buflen + 1];
		s.read(buffer, buflen); buffer[buflen] = 0;
		if (n != 0) n->description = buffer;
		buflen = read16le(s);
		delete[] buffer; buffer = new char[buflen + 1];
		s.read(buffer, buflen); buffer[buflen] = 0;
		if (n != 0) n->comments = buffer;
		delete[] buffer;		
	}
}

void genomeFile::writeNotes(std::ostream &s) const {
	// TODO
}

gene *genomeFile::nextGene(std::istream &s) {
	uint8_t majic[3];
	s.read((char *)majic, 3);
	if (strncmp((char *)majic, "gen", 3) != 0) throw creaturesException("bad majic for a gene");

	s >> majic[0];
	if (majic[0] == 'd') return 0;
	if (majic[0] != 'e')
		throw creaturesException("bad majic at stage2 for a gene");

	uint8_t type, subtype;
	s >> type >> subtype;

	gene *g = 0;
  
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
					} break;
				case 1: g = new organGene(cversion, true); break;
				case 2: g = new c2eBrainTractGene(cversion); break;
			} break;
		case 1:
			switch (subtype) {
				case 0: g = new bioReceptorGene(cversion); break;
				case 1: g = new bioEmitterGene(cversion); break;
				case 2: g = new bioReactionGene(cversion); break;
				case 3: g = new bioHalfLivesGene(cversion); break;
				case 4: g = new bioInitialConcentrationGene(cversion); break;
				case 5: g = new bioNeuroEmitterGene(cversion); break;
			} break;
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
			} break;
		case 3:
			switch (subtype) {
				case 0: g = new organGene(cversion, false); break;
			} break;
	}

	if (g == 0) throw creaturesException("genefactory failed");

	if (((typeid(*g) == typeid(bioReactionGene))
		|| (typeid(*g) == typeid(bioEmitterGene)))
		|| (typeid(*g) == typeid(bioReceptorGene))) {
		if (currorgan == 0) {
				if (cversion == 1) genes.push_back(g); // Creatures 1 doesn't have organs
				else throw creaturesException("reaction/emitter/receptor without an attached organ");
		} else currorgan->genes.push_back(g);
	} else {
		genes.push_back(g);
		if (typeid(*g) == typeid(organGene))
			if (!((organGene *)g)->isBrain())
				currorgan = (organGene *)g;
	}

	s >> *g;

	return g;
}

std::istream &operator >> (std::istream &s, genomeFile &f) {
	char majic[3]; s.read(majic, 3);
	if (strncmp((char *)majic, "gen", 3) == 0) {
		s >> majic[0];
		if (majic[0] == 'e') f.cversion = 1;
		else throw creaturesException("bad majic for genome");

		s.seekg(0, std::ios::beg);
	} else {
		if (strncmp((char *)majic, "dna", 3) != 0) throw creaturesException("bad majic for genome");

		s >> majic[0];
		f.cversion = majic[0] - 48; // 48 = ASCII '0'
		if ((f.cversion < 1) || (f.cversion > 3)) throw creaturesException("unsupported genome version in majic");
	}

	//std::cout << "creaturesGenomeFile: reading genome of version " << (unsigned int)f.cversion << ".\n";
	f.currorgan = 0;
	while (f.nextGene(s) != 0) { }
	f.currorgan = 0;
	//std::cout << "creaturesGenomeFile: read " << (unsigned int)f.genes.size() << " top-level genes.\n";

	return s;
}

std::ostream &operator << (std::ostream &s, const genomeFile &f) {
	s << "dna" << char(f.cversion + 48); // 48 = ASCII '0'

	// iterate through genes
	for (std::vector<gene *>::iterator x = ((genomeFile &)f).genes.begin(); x != ((genomeFile &)f).genes.end(); x++) {
		s << **x;
	}

	s << "gend";

	return s;
}

gene *genomeFile::getGene(uint8_t type, uint8_t subtype, unsigned int seq) {
	unsigned int c = 0;
	for (std::vector<gene *>::iterator i = genes.begin(); i != genes.end(); i++) {
		if ((*i)->type() == type)
			if ((*i)->subtype() == subtype) {
				c++;
				if (seq == c) return *i;
			}
	}

	return 0;
}

uint8_t geneFlags::operator () () const {
	return ((_mutable?1:0) + (dupable?2:0) + (delable?4:0) + (maleonly?8:0) +
		(femaleonly?16:0) + (notexpressed?32:0) + (reserved1?64:0) + (reserved2?128:0));
}

void geneFlags::operator () (uint8_t f) {
	_mutable = ((f & 1) != 0);
	dupable = ((f & 2) != 0);
	delable = ((f & 4) != 0);
	maleonly = ((f & 8) != 0);
	femaleonly = ((f & 16) != 0);
	notexpressed = ((f & 32) != 0);
	reserved1 = ((f & 64) != 0);
	reserved2 = ((f & 128) != 0);
}

std::ostream &operator << (std::ostream &s, const gene &g) {
	s << "gene" << g.type() << g.subtype();

	s << g.note.which << g.header.generation << uint8_t(g.header.switchontime) << g.header.flags();
	if (g.cversion > 1) s << g.header.mutweighting;
	if (g.cversion == 3) s << g.header.variant;

	g.write(s);

	return s;
}

std::istream &operator >> (std::istream &s, gene &g) {
	uint8_t b;
	s >> g.note.which >> g.header.generation >> b;
	g.header.switchontime = (lifestage)b;
	s >> b;
	g.header.flags(b);
	if (g.cversion > 1) s >> g.header.mutweighting;
	if (g.cversion == 3) s >> g.header.variant;

	g.read(s);

	return s;
}

void bioEmitterGene::write(std::ostream &s) const {
	s << organ << tissue << locus << chemical << threshold << rate << gain;
	uint8_t flags = (clear?1:0) + (digital?2:0) + (invert?4:0);
	s << flags;
}

void bioEmitterGene::read(std::istream &s) {
	s >> organ >> tissue >> locus >> chemical >> threshold >> rate >> gain;
	uint8_t flags;
	s >> flags;
	clear = ((flags & 1) != 0);
	digital = ((flags & 2) != 0);
	invert = ((flags & 4) != 0);
}

void bioHalfLivesGene::write(std::ostream &s) const {
	for (int i = 0; i < 256; i++) {
		s << halflives[i];
	}
}

void bioHalfLivesGene::read(std::istream &s) {
	for (int i = 0; i < 256; i++) {
		s >> halflives[i];
	}
}

void bioInitialConcentrationGene::write(std::ostream &s) const {
	s << chemical << quantity;
}

void bioInitialConcentrationGene::read(std::istream &s) {
	s >> chemical >> quantity;
}

void bioNeuroEmitterGene::write(std::ostream &s) const {
	for (int i = 0; i < 3; i++) {
		s << lobes[i] << neurons[i];
	}
	s << rate;
	for (int i = 0; i < 4; i++) {
		s << chemical[i] << quantity[i];
	}
}

void bioNeuroEmitterGene::read(std::istream &s) {
	for (int i = 0; i < 3; i++) {
		s >> lobes[i] >> neurons[i];
	}
	s >> rate;
	for (int i = 0; i < 4; i++) {
		s >> chemical[i] >> quantity[i];
	}
}

void bioReactionGene::write(std::ostream &s) const {
	for (int i = 0; i < 4; i++) {
		s << quantity[i];
		s << reactant[i];
	}
	
	s << rate;
}

void bioReactionGene::read(std::istream &s) {
	for (int i = 0; i < 4; i++) {
		s >> quantity[i];
		s >> reactant[i];
	}
	
	s >> rate;
}

void bioReceptorGene::write(std::ostream &s) const {
	s << organ << tissue << locus << chemical << threshold << nominal << gain;
	uint8_t flags = (inverted?1:0) + (digital?2:0);
	s << flags;
}

void bioReceptorGene::read(std::istream &s) {
	s >> organ >> tissue >> locus >> chemical >> threshold >> nominal >> gain;
	uint8_t flags;
	s >> flags;
	inverted = ((flags & 1) != 0);
	digital = ((flags & 2) != 0);
}

void c2eBrainLobeGene::write(std::ostream &s) const {
	for (int i = 0; i < 4; i++) s << id[i];

	write16be(s, updatetime);
	write16be(s, x);
	write16be(s, y);

	s << width << height;
	s << red << green << blue;
	s << WTA << tissue << initrulealways;

	for (int i = 0; i < 7; i++) s << spare[i];
	for (int i = 0; i < 48; i++) s << initialiserule[i];
	for (int i = 0; i < 48; i++) s << updaterule[i];
}

void c2eBrainLobeGene::read(std::istream &s) {
	for (int i = 0; i < 4; i++) s >> id[i];

	updatetime = read16be(s);
	x = read16be(s);
	y = read16be(s);

	s >> width >> height;
	s >> red >> green >> blue;
	s >> WTA >> tissue >> initrulealways;

	for (int i = 0; i < 7; i++) s >> spare[i];
	for (int i = 0; i < 48; i++) s >> initialiserule[i];
	for (int i = 0; i < 48; i++) s >> updaterule[i];
}

void c2eBrainTractGene::write(std::ostream &s) const {
	write16be(s, updatetime);
	for (int i = 0; i < 4; i++) s << srclobe[i];
	write16be(s, srclobe_lowerbound);
	write16be(s, srclobe_upperbound);
	write16be(s, src_noconnections);
	for (int i = 0; i < 4; i++) s << destlobe[i];
	write16be(s, destlobe_lowerbound);
	write16be(s, destlobe_upperbound);
	write16be(s, dest_noconnections);
	s << migrates << norandomconnections << srcvar << destvar << initrulealways;
	for (int i = 0; i < 5; i++) s << spare[i];
	for (int i = 0; i < 48; i++) s << initialiserule[i];
	for (int i = 0; i < 48; i++) s << updaterule[i];
}

void c2eBrainTractGene::read(std::istream &s) {
	updatetime = read16be(s);
	for (int i = 0; i < 4; i++) s >> srclobe[i];
	srclobe_lowerbound = read16be(s);
	srclobe_upperbound = read16be(s);
	src_noconnections = read16be(s);
	for (int i = 0; i < 4; i++) s >> destlobe[i];
	destlobe_lowerbound = read16be(s);
	destlobe_upperbound = read16be(s);
	dest_noconnections = read16be(s);
	s >> migrates >> norandomconnections >> srcvar >> destvar >> initrulealways;
	for (int i = 0; i < 5; i++) s >> spare[i];
	for (int i = 0; i < 48; i++) s >> initialiserule[i];
	for (int i = 0; i < 48; i++) s >> updaterule[i];
}

void creatureAppearanceGene::write(std::ostream &s) const {
	s << part << variant;
	if (cversion > 1) s << species;
}

void creatureAppearanceGene::read(std::istream &s) {
	s >> part >> variant;
	if (cversion > 1) s >> species;
}

void creatureFacialExpressionGene::write(std::ostream &s) const {
	write16le(s, expressionno);
	s << weight;

	for (int i = 0; i < 4; i++) {
		s << drives[i] << amounts[i];
	}
}

void creatureFacialExpressionGene::read(std::istream &s) {
	expressionno = read16le(s);
	s >> weight;

	for (int i = 0; i < 4; i++) {
		s >> drives[i] >> amounts[i];
	}
}

void creatureGaitGene::write(std::ostream &s) const {
	s << drive;

	for (int i = 0; i < gaitLength(); i++) {
		s << pose[i];
	}
}

void creatureGaitGene::read(std::istream &s) {
	s >> drive;

	for (int i = 0; i < gaitLength(); i++) {
		s >> pose[i];
	}
}

void creatureGenusGene::write(std::ostream &s) const {
	s << genus;

	const char *b;

	// TODO: we read past the end of the returned buffer here!
	b = mum.c_str();
	for (int i = 0; i < ((cversion == 3) ? 32 : 4); i++) s << b[i];
	b = dad.c_str();
	for (int i = 0; i < ((cversion == 3) ? 32 : 4); i++) s << b[i];
}

void creatureGenusGene::read(std::istream &s) {
	s >> genus;

	char buf[33];
	unsigned int len = ((cversion == 3) ? 32 : 4);

	s.read(buf, len);
	buf[len] = 0;
	mum = (char *)buf;

	s.read(buf, len);
	buf[len] = 0;
	dad = (char *)buf;
}

void creatureInstinctGene::write(std::ostream &s) const {
	for (int i = 0; i < 3; i++) {
		s << lobes[i] << neurons[i];
	}

	s << action;
	s << drive;
	s << level;
}

void creatureInstinctGene::read(std::istream &s) {
	for (int i = 0; i < 3; i++) {
		s >> lobes[i] >> neurons[i];
	}

	s >> action;
	s >> drive;
	s >> level;
}

void creaturePigmentGene::write(std::ostream &s) const {
	s << color << amount;
}

void creaturePigmentGene::read(std::istream &s) {
	s >> color >> amount;
}

void creaturePigmentBleedGene::write(std::ostream &s) const {
	s << rotation << swap;
}

void creaturePigmentBleedGene::read(std::istream &s) {
	s >> rotation >> swap;
}

void creaturePoseGene::write(std::ostream &s) const {
	s << poseno;

	for (int i = 0; i < poseLength(); i++) {
		s << pose[i];
	}
}

void creaturePoseGene::read(std::istream &s) {
	s >> poseno;

	for (int i = 0; i < poseLength(); i++) {
		s >> pose[i];
	}
}

void creatureStimulusGene::write(std::ostream &s) const {
	s << stim << significance << sensoryneuron << intensity;
	uint8_t flags = (modulate?1:0) + (addoffset?2:0) + (whenasleep?4:0);
	if (silent[0]) flags += 16;
	if (silent[1]) flags += 32;
	if (silent[2]) flags += 64;
	if (silent[3]) flags += 128;
	s << flags;

	for (int i = 0; i < 4; i++) {
		s << drives[i] << amounts[i];
	}
}

void creatureStimulusGene::read(std::istream &s) {
	s >> stim >> significance >> sensoryneuron >> intensity;
	uint8_t flags;
	s >> flags;
	modulate = ((flags & 1) != 0);
	addoffset = ((flags & 2) != 0);
	whenasleep = ((flags & 4) != 0);

	for (int i = 0; i < 4; i++) {
		s >> drives[i] >> amounts[i];
	}

	silent[0] = ((flags & 16) != 0);
	silent[1] = ((flags & 32) != 0);
	silent[2] = ((flags & 64) != 0);
	silent[3] = ((flags & 128) != 0);
}

void oldBrainLobeGene::write(std::ostream &s) const {
	s << x << y << width << height << perceptflag << nominalthreshold << leakagerate << reststate << inputgain;
	s.write((char *)staterule, (cversion == 1) ? 8 : 12);
	s << flags;

	s << dendrite1 << dendrite2;
}

void oldBrainLobeGene::read(std::istream &s) {
	s >> x >> y >> width >> height >> perceptflag >> nominalthreshold >> leakagerate >> reststate >> inputgain;
	s.read((char *)staterule, (cversion == 1) ? 8 : 12);
	s >> flags;

	s >> dendrite1 >> dendrite2;
}

void organGene::write(std::ostream &s) const {
	s << clockrate << damagerate << lifeforce << biotickstart << atpdamagecoefficient;

	// iterate through children
	for (std::vector<gene *>::iterator x = ((organGene *)this)->genes.begin(); x != ((organGene *)this)->genes.end(); x++)
		s << **x;
}

void organGene::read(std::istream &s) {
	s >> clockrate >> damagerate >> lifeforce >> biotickstart >> atpdamagecoefficient;
}

std::ostream &operator << (std::ostream &s, const oldDendriteInfo &i) {
	s << i.srclobe << i.min << i.max << i.spread << i.fanout << i.minLTW << i.maxLTW;
	s << i.minstr << i.maxstr << i.migrateflag << i.relaxsuscept << i.relaxSTW << i.LTWgainrate;

	s << i.strgain;
	s.write((char *)i.strgainrule, (i.cversion == 1) ? 8 : 12);
	s << i.strloss;
	s.write((char *)i.strlossrule, (i.cversion == 1) ? 8 : 12);
	s.write((char *)i.susceptrule, (i.cversion == 1) ? 8 : 12);
	s.write((char *)i.relaxrule, (i.cversion == 1) ? 8 : 12);

	if (i.cversion == 2) {
		s.write((char *)i.backproprule, 12);
		s.write((char *)i.forproprule, 12);
	}

	return s;
}

std::istream &operator >> (std::istream &s, oldDendriteInfo &i) {
	s >> i.srclobe >> i.min >> i.max >> i.spread >> i.fanout >> i.minLTW >> i.maxLTW;
	s >> i.minstr >> i.maxstr >> i.migrateflag >> i.relaxsuscept >> i.relaxSTW >> i.LTWgainrate;

	s >> i.strgain;
	s.read((char *)i.strgainrule, (i.cversion == 1) ? 8 : 12);
	s >> i.strloss;
	s.read((char *)i.strlossrule, (i.cversion == 1) ? 8 : 12);
	s.read((char *)i.susceptrule, (i.cversion == 1) ? 8 : 12);
	s.read((char *)i.relaxrule, (i.cversion == 1) ? 8 : 12);

	if (i.cversion == 2) {
		s.read((char *)i.backproprule, 12);
		s.read((char *)i.forproprule, 12);
	}

	return s;
}

/* vim: set noet: */
