/*
 *  genomeFile.h
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

#pragma once

#include "creaturesException.h"
#include "creatures/lifestage.h"

#include <vector>
#include <string>

using std::ostream;
using std::istream;
using std::vector;

class gene;
struct geneNote;
class organGene;
	
//! A class for handling Creatures genome (.gen) files.
class genomeFile {
  uint8_t cversion;
  organGene *currorgan;

  gene *nextGene(istream &); // returns NULL upon 'gend'
  geneNote *findNote(uint8_t type, uint8_t subtype, uint8_t which);

  friend ostream &operator << (ostream &, const genomeFile &);
  friend istream &operator >> (istream &, genomeFile &);

public:
  vector<gene *> genes;

  void readNotes(istream &);
  void writeNotes(ostream &) const;

  uint8_t getVersion() { return cversion; }

  gene *getGene(uint8_t type, uint8_t subtype, unsigned int seq);
};

struct geneNote {
	std::string description;
	std::string comments;
	uint8_t which;
};

//! header flags of a gene
struct geneFlags {
  bool _mutable; // 'mutable' is a reserved keyword
  bool dupable;
  bool delable;
  bool maleonly;
  bool femaleonly;
  bool notexpressed;
  bool reserved1;
  bool reserved2;
  
  uint8_t operator () () const;
  void operator () (uint8_t);

  geneFlags() { _mutable = true; dupable = true; delable = true; maleonly = false; femaleonly = false; notexpressed = false; reserved1 = false; reserved2 = false; }
};

//! header of a gene
struct geneHeader {
  geneFlags flags;

  uint8_t generation;
  lifestage switchontime;
  uint8_t mutweighting; // c2/c2e only
  uint8_t variant; // c2e only

  // todo: are these values 'correct' for default values? - Fuzzie
  geneHeader() { generation = 0; switchontime = baby; mutweighting = 255; variant = 0; }
};

/*
  nextGene does the 'dirty work' of working out which class to create, and then calls
  the << operator as appropriate

  however the >> operator must do this work itself ('gene' and the type/subtype)
*/

//! The base class for all Creatures genes.
class gene {
  friend ostream &operator << (ostream &, const gene &);
  friend istream &operator >> (istream &, gene &);

protected:
  uint8_t cversion;

  virtual uint8_t type() const = 0;
  virtual uint8_t subtype() const = 0;

  virtual void write(ostream &) const = 0;
  virtual void read(istream &) = 0;

  friend class genomeFile;

public:
  geneNote note;
  geneHeader header;

  gene(uint8_t v) : cversion(v) { }
  virtual ~gene() { }

  uint8_t version() { return cversion; }

  virtual const char *name() = 0;
  virtual const char *typeName() = 0;
};
 
//! gene: organ, either brain or normal
class organGene : public gene {
  bool brainorgan;

protected:
  uint8_t type() const { if (brainorgan) return 0; else return 3; }
  uint8_t subtype() const { if (brainorgan) return 1; else return 0; }

  void write(ostream &) const;
  void read(istream &);

public:
  vector<gene *> genes;

  bool isBrain() { return brainorgan; }

  uint8_t clockrate;
  uint8_t damagerate;
  uint8_t lifeforce;
  uint8_t biotickstart;
  uint8_t atpdamagecoefficient;

  organGene(uint8_t v, bool b) : gene(v), brainorgan(b) { }
  const char *name() { return "Organ"; }
  const char *typeName() { if (brainorgan) return "Brain"; return "Biochemistry"; }
};

//! gene: brain-type base class (not including brain organ)
class brainGene : public gene {
protected:
  uint8_t type() const { return 0; }

  brainGene(uint8_t v) : gene(v) { }
  const char *typeName() { return "Brain"; }
};

//! gene: c2e brain lobe
class c2eBrainLobeGene : public brainGene {
protected:
  uint8_t subtype() const { return 0; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t id[4]; // todo: string
  uint16_t updatetime; // 0 for never
  uint16_t x;
  uint16_t y;
  uint8_t width;
  uint8_t height;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t WTA; // unused in final game?
  uint8_t tissue;
  uint8_t initrulealways;
  uint8_t spare[7];
  uint8_t initialiserule[48];
  uint8_t updaterule[48];

  c2eBrainLobeGene(uint8_t v) : brainGene(v) { }
  const char *name() { return "Lobe"; }
};

struct oldDendriteInfo {
  uint8_t cversion;

  // rules have size 8 for C1, size 12 for C2

  uint8_t srclobe;
  uint8_t min;
  uint8_t max;
  uint8_t spread;
  uint8_t fanout;
  uint8_t minLTW;
  uint8_t maxLTW;
  uint8_t minstr;
  uint8_t maxstr;
  uint8_t migrateflag;
  uint8_t relaxsuscept;
  uint8_t relaxSTW;
  uint8_t LTWgainrate;
  uint8_t strgain;
  uint8_t strloss;

  uint8_t strgainrule[12];
  uint8_t strlossrule[12];
  uint8_t susceptrule[12];
  uint8_t relaxrule[12];

  // back/forward propogation is C2 only
  uint8_t backproprule[12];
  uint8_t forproprule[12];

  friend ostream &operator << (ostream &, const oldDendriteInfo &);
  friend istream &operator >> (istream &, oldDendriteInfo &);

  oldDendriteInfo(uint8_t v) { cversion = v; }
};

//! gene: old-style (c1/c2) brain lobe
class oldBrainLobeGene : public brainGene {
protected:
  uint8_t subtype() const { return 0; }

  void write(ostream &) const;
  void read(istream &);

public:  
  // rules have size 8 for C1, size 12 for C2

  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
  uint8_t perceptflag;
  uint8_t nominalthreshold;
  uint8_t leakagerate;
  uint8_t reststate;
  uint8_t inputgain;
  uint8_t flags;

  uint8_t staterule[12];

  oldDendriteInfo dendrite1;
  oldDendriteInfo dendrite2;

  oldBrainLobeGene(uint8_t v) : brainGene(v), dendrite1(v), dendrite2(v) { }
  virtual const char *name() { return "Lobe"; }
};

//! gene: c2e brain tract
class c2eBrainTractGene : public brainGene {
protected:
  uint8_t subtype() const { return 2; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint16_t updatetime;
  uint8_t srclobe[4];
  uint16_t srclobe_lowerbound;
  uint16_t srclobe_upperbound;
  uint16_t src_noconnections;
  uint8_t destlobe[4];
  uint16_t destlobe_lowerbound;
  uint16_t destlobe_upperbound;
  uint16_t dest_noconnections;
  uint8_t migrates; // flag
  uint8_t norandomconnections; // flag
  uint8_t srcvar;
  uint8_t destvar;
  uint8_t initrulealways; // flag
  uint8_t spare[5];
  uint8_t initialiserule[48];
  uint8_t updaterule[48];

  c2eBrainTractGene(uint8_t v) : brainGene(v) { }
  const char *name() { return "Tract"; }
};

class bioGene : public gene {
protected:
  uint8_t type() const { return 1; }

  bioGene(uint8_t v) : gene(v) { }
  const char *typeName() { return "Biochemistry"; }
};

//! gene: receptor
class bioReceptorGene : public bioGene {
protected:
  uint8_t subtype() const { return 0; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t organ;
  uint8_t tissue;
  uint8_t locus;
  uint8_t chemical;
  uint8_t threshold;
  uint8_t nominal;
  uint8_t gain;
  bool inverted; // 1 in flags
  bool digital; // 2 in flags

  bioReceptorGene(uint8_t v) : bioGene(v) { }
  const char *name() { return "Receptor"; }
};

//! gene: emitter
class bioEmitterGene : public bioGene {
protected:
  uint8_t subtype() const { return 1; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t organ;
  uint8_t tissue;
  uint8_t locus;
  uint8_t chemical;
  uint8_t threshold;
  uint8_t rate;
  uint8_t gain;
  bool clear; // 1 in flags
  bool digital; // 2 in flags
  bool invert; // 4 in flags

  bioEmitterGene(uint8_t v) : bioGene(v) { }
  const char *name() { return "Emitter"; }
};

//! gene: reaction
class bioReactionGene : public bioGene {
protected:
  uint8_t subtype() const { return 2; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t reactant[4];
  uint8_t quantity[4];
  uint8_t rate;

  bioReactionGene(uint8_t v) : bioGene(v) { }
  const char *name() { return "Reaction"; }
};

//! gene: half-lives
class bioHalfLivesGene : public bioGene {
protected:
  uint8_t subtype() const { return 3; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t halflives[256];

  bioHalfLivesGene(uint8_t v) : bioGene(v) { }
  const char *name() { return "Half-Life"; }
};

//! gene: initial concentration
class bioInitialConcentrationGene : public bioGene {
protected:
  uint8_t subtype() const { return 4; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t chemical;
  uint8_t quantity;

  bioInitialConcentrationGene(uint8_t v) : bioGene(v) { }
  const char *name() { return "Initial Concentration"; }
};

//! gene: neuroemitter
class bioNeuroEmitterGene : public bioGene {
protected:
  uint8_t subtype() const { return 5; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t lobes[3];
  uint8_t neurons[3];
  uint8_t rate;
  uint8_t chemical[4];
  uint8_t quantity[4];

  bioNeuroEmitterGene(uint8_t v) : bioGene(v) { }
  const char *name() { return "Neuro Emitter"; }
};

//! gene: creature-type base class
class creatureGene : public gene {
protected:
  uint8_t type() const { return 2; }

  creatureGene(uint8_t v) : gene(v) { }
  const char *typeName() { return "Creature"; }
};

//! gene: stimulus
class creatureStimulusGene : public creatureGene {
protected:
  uint8_t subtype() const { return 0; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t stim;
  uint8_t significance;
  uint8_t sensoryneuron;
  uint8_t intensity;
  bool modulate; // 1 in flags
  bool addoffset; // 2 in flags
  bool whenasleep; // 4 in flags
  bool silent[4]; // 16, 32, 64, 128 in flags
  uint8_t drives[4];
  uint8_t amounts[4];

  creatureStimulusGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Stimulus"; }
};

//! gene: genus (must be first gene in file, must only be one instance)
class creatureGenusGene : public creatureGene {
protected:
  uint8_t subtype() const { return 1; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t genus;
  std::string mum; // length is: 4 for C1/C2, 32 for c2e
  std::string dad;

  creatureGenusGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Genus"; }
};

//! gene: appearance
class creatureAppearanceGene : public creatureGene {
protected:
  uint8_t subtype() const { return 2; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t part;
  uint8_t variant;
  uint8_t species; // genusofdonor; not present in c1

  creatureAppearanceGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Appearance"; }
};

//! gene: pose
class creaturePoseGene : public creatureGene {
protected:
  uint8_t subtype() const { return 3; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t poseno;
  uint8_t pose[16];

  int poseLength() const { return (cversion == 3) ? 16 : 15; }
  std::string getPoseString() const { return std::string((char *)pose, poseLength()); }
  
  creaturePoseGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Pose"; }
};

//! gene: gait
class creatureGaitGene : public creatureGene {
protected:
  uint8_t subtype() const { return 4; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t drive;
  uint8_t pose[8];

  int gaitLength() const { return 8; }

  creatureGaitGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Gait"; }
};

//! gene: instinct
class creatureInstinctGene : public creatureGene {
protected:
  uint8_t subtype() const { return 5; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t lobes[3];
  uint8_t neurons[3];
  uint8_t action;
  uint8_t drive;
  uint8_t level;

  creatureInstinctGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Instinct"; }
};

//! gene: pigment
class creaturePigmentGene : public creatureGene {
protected:
  uint8_t subtype() const { return 6; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t color;
  uint8_t amount;

  creaturePigmentGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Pigment"; }
};

//! gene: pigment bleed
class creaturePigmentBleedGene : public creatureGene {
protected:
  uint8_t subtype() const { return 7; }

  void write(ostream &) const;
  void read(istream &);

public:
  uint8_t rotation;
  uint8_t swap;

  creaturePigmentBleedGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Pigment Bleed"; }
};

//! gene: facial expression
class creatureFacialExpressionGene : public creatureGene {
protected:
  uint8_t subtype() const { return 8; }
  
  void write(ostream &) const;
  void read(istream &);

public:
  uint16_t expressionno;
  uint8_t weight;
  uint8_t drives[4];
  uint8_t amounts[4];

  creatureFacialExpressionGene(uint8_t v) : creatureGene(v) { }
  const char *name() { return "Facial Expression"; }
};

/* vim: set noet: */
