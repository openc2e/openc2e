/*
 *  mngfile.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue 16 Nov 2004.
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
#include <string>
#include <vector>
#include <list>
#include <map>
#include <istream>
#include <cmath>
#include "openc2e.h"
#include <fmt/printf.h>

void mngrestart(std::istream *is);

struct processState {
	class MNGLayer *layer;
	class MNGFile *mngfile;
	class MNGVoiceNode *voice;
	class MNGStageNode *stage;
	class MNGTrackDecNode *track;
	
	processState(class MNGFile *m) { mngfile = m; layer = 0; voice = 0; stage = 0; track = 0; }
};

class MNGNode {
public:
	virtual std::string dump() { return "[unknown node]"; }
	virtual void postProcess(processState *s) { } // walk the tree, setting up parent pointers correctly
	virtual ~MNGNode() { }
};

class MNGFileException : public creaturesException {
	public:
		int lineno;
		MNGFileException(const char * m) throw() : creaturesException(m) { lineno = 0; }
		MNGFileException(const std::string &m) throw() : creaturesException(m) { lineno = 0; }
		MNGFileException(const char * m, int l) throw() : creaturesException(m) { lineno = l; }
		MNGFileException(const std::string &m, int l) throw() : creaturesException(m) { lineno = l; }
};

class MNGNamedNode : public MNGNode {
protected:
	std::string name;

public:
	MNGNamedNode(std::string n) { name = n; }
	std::string getName() { return name; }
	virtual std::string dump() { return std::string("[unknown named node: ") + getName() + "]"; }
};

inline std::string dumpChildren(std::list<MNGNode *> *c) {
	std::string t = "\n";
	for (std::list<MNGNode *>::iterator i = c->begin(); i != c->end(); i++) t = t + (*i)->dump() + "\n";
	return t;
}

class MNGStageNode : public MNGNode { // stage
public:
	std::list<MNGNode *> *children; // stagesettinglist
	virtual std::string dump() { return std::string("Stage { ") + dumpChildren(children) + "}\n"; }
	virtual void postProcess(processState *s) { s->stage = this; for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) (*i)->postProcess(s); s->stage = 0; }
	virtual ~MNGStageNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

inline std::string dumpEffectChildren(std::list<MNGStageNode *> *c) {
	std::string t = "\n";
	for (std::list<MNGStageNode *>::iterator i = c->begin(); i != c->end(); i++) t = t + (*i)->dump() + "\n";
	return t;
}

class MNGEffectDecNode : public MNGNamedNode { // effectdec
public:
	MNGEffectDecNode(std::string n) : MNGNamedNode(n) { }
	std::list<class MNGStageNode *> *children; // stagelist
	virtual std::string dump() { return std::string("Effect(" + name + ") { ") + dumpEffectChildren(children) + "}\n"; }
	virtual void postProcess(processState *s) { for (std::list<MNGStageNode *>::iterator i = children->begin(); i != children->end(); i++) (*i)->postProcess(s); }
	virtual ~MNGEffectDecNode() { for (std::list<MNGStageNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGTrackDecNode : public MNGNamedNode { // trackdec
public:
	MNGTrackDecNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children; // track
	virtual std::string dump() { return std::string("Track(" + name + ") { ") + dumpChildren(children) + "}\n"; }
	virtual void postProcess(processState *s) { s->track = this; for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) (*i)->postProcess(s); s->track = 0; }
	virtual ~MNGTrackDecNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGEffectNode : public MNGNamedNode { // effect
public:
	MNGEffectNode(std::string n) : MNGNamedNode(n) { }
	virtual std::string dump() { return std::string("Effect(") + name + ")"; }
};

class MNGExpression : public MNGNode { // (expression)
public:
};

class MNGBinaryExpression : public MNGExpression {
protected:
	MNGExpression *one, *two;

public:
	MNGBinaryExpression(MNGExpression *o, MNGExpression *t) { one = o; two = t; }
	virtual void postProcess(processState *s) { one->postProcess(s); two->postProcess(s); }
	virtual ~MNGBinaryExpression() { delete one; delete two; }
	MNGExpression *first() { return one; }
	MNGExpression *second() { return two; }
};

class MNGConstantNode : public MNGExpression { // MNG_number
protected:
	float value;

public:
	MNGConstantNode(float n) { value = n; }
	std::string dump() { return fmt::sprintf("%f", value); }
	float getValue() { return value; }
};

class MNGExpressionContainer : public MNGNode {
protected:
	MNGExpression *subnode;

public:
	MNGExpressionContainer(MNGExpression *n) { subnode = n; }
	virtual void postProcess(processState *s) { subnode->postProcess(s); }
	virtual ~MNGExpressionContainer() { delete subnode; }
	MNGExpression *getExpression() { return subnode; }
};

class MNGPanNode : public MNGExpressionContainer { // pan
public:
	MNGPanNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("Pan(") + subnode->dump() + ")"; }
};

class MNGLayerVolumeNode : public MNGExpressionContainer { // layervolume
public:
	MNGLayerVolumeNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("Volume(") + subnode->dump() + ")"; }
};

class MNGEffectVolumeNode : public MNGExpressionContainer { // effectvolume
public:
	MNGEffectVolumeNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("Volume(") + subnode->dump() + ")"; }
};

class MNGDelayNode : public MNGExpressionContainer { // delay
public:
	MNGDelayNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("Delay(") + subnode->dump() + ")"; }
};

class MNGRandomNode : public MNGBinaryExpression { // random
public:
	MNGRandomNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
	std::string dump() { return std::string("Random(") + one->dump() + ", " + two->dump() + ")"; }
};

class MNGTempoDelayNode : public MNGExpressionContainer { // tempodelay
public:
	MNGTempoDelayNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("TempoDelay(") + subnode->dump() + ")"; }
};

class MNGFadeInNode : public MNGExpressionContainer { // fadein
public:
	MNGFadeInNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("FadeIn(") + subnode->dump() + ")"; }
};

class MNGFadeOutNode : public MNGExpressionContainer { // fadeout
public:
	MNGFadeOutNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("FadeOut(") + subnode->dump() + ")"; }
};

class MNGBeatLengthNode : public MNGExpressionContainer { // beatlength
public:
	MNGBeatLengthNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("BeatLength(") + subnode->dump() + ")"; }
};

class MNGLayer : public MNGNamedNode {
public:
	MNGLayer(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children;
	std::map<std::string, class MNGVariableDecNode *> variables;
	virtual void postProcess(processState *s) {
		s->layer = this;
		for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++)
			(*i)->postProcess(s);
		s->layer = 0;
	}
	virtual ~MNGLayer() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGLoopLayerNode : public MNGLayer { // looplayerdec
public:
	MNGLoopLayerNode(std::string n) : MNGLayer(n) { }
	virtual std::string dump() { return std::string("LoopLayer(" + name + ") { ") + dumpChildren(children) + "}\n"; }
};

class MNGAleotoricLayerNode : public MNGLayer { // aleotoriclayerdec
public:
	MNGAleotoricLayerNode(std::string n) : MNGLayer(n) { }
	virtual std::string dump() { return std::string("AleotoricLayer(" + name + ") { ") + dumpChildren(children) + "}\n"; }
};

class MNGAddNode : public MNGBinaryExpression { // add
public:
	MNGAddNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
	virtual std::string dump() { return std::string("Add(") + one->dump() + ", " + two->dump() + ")"; }
};

class MNGSubtractNode : public MNGBinaryExpression { // subtract
public:
	MNGSubtractNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
	virtual std::string dump() { return std::string("Subtract(") + one->dump() + ", " + two->dump() + ")"; }
};

class MNGMultiplyNode : public MNGBinaryExpression { // multiply
public:
	MNGMultiplyNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
	virtual std::string dump() { return std::string("Multiply(") + one->dump() + ", " + two->dump() + ")"; }
};

class MNGDivideNode : public MNGBinaryExpression { // divide
public:
	MNGDivideNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
	virtual std::string dump() { return std::string("Divide(") + one->dump() + ", " + two->dump() + ")"; }
};

class MNGSineWaveNode : public MNGBinaryExpression { // sinewave
public:
	MNGSineWaveNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
	virtual std::string dump() { return std::string("SineWave(") + one->dump() + ", " + two->dump() + ")"; }
};

class MNGCosineWaveNode : public MNGBinaryExpression { // cosinewave
public:
	MNGCosineWaveNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
	virtual std::string dump() { return std::string("CosineWave(") + one->dump() + ", " + two->dump() + ")"; }
};

class MNGBeatSynchNode : public MNGExpressionContainer { // beatsynch
public:
	MNGBeatSynchNode(MNGExpression *n) : MNGExpressionContainer(n) { } 
	std::string dump() { return std::string("BeatSynch(") + subnode->dump() + ")"; }
};

class MNGUpdateRateNode : public MNGExpressionContainer { // updaterate
public:
	MNGUpdateRateNode(MNGExpression *n) : MNGExpressionContainer(n) { } 
	std::string dump() { return std::string("UpdateRate(") + subnode->dump() + ")"; }
};

class MNGIntervalNode : public MNGExpressionContainer { // interval
public:
	MNGIntervalNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("Interval(") + subnode->dump() + ")"; }
};

class MNGVariableDecNode : public MNGNamedNode {
protected:
	MNGExpression *value;
	
public:
	// TODO: we should ensure the expression passed here is a constant.. ?
	MNGVariableDecNode(std::string n, MNGExpression *e) : MNGNamedNode(n) { value = e; }
	virtual ~MNGVariableDecNode() { delete value; }
	std::string dump() { return std::string("Variable(") + name + ", " + value->dump() + ")"; }
	virtual void postProcess(processState *s) { s->layer->variables[name] = this; }
	MNGExpression *getExpression() { return value; }
};

enum variabletypes { NAMED, INTERVAL, VOLUME, PAN };

class MNGVariableNode : public MNGExpression { // variable
protected:
	std::string name;
	variabletypes variabletype;
	MNGVariableDecNode *real;
	
	union {
	class MNGLayer *layer;
	class MNGVoiceNode *voice;
	class MNGStageNode *stage;
	class MNGTrackDecNode *track;
	};

public:
	// TODO: i'm assuming layer clears them all, ie, it really is a union. is that right? - fuzzie
	MNGVariableNode(std::string n) { layer = 0; variabletype = NAMED; name = n; }
	MNGVariableNode(variabletypes t) { layer = 0; variabletype = t; }
	virtual void postProcess(processState *s) {
		switch (variabletype) {
			case NAMED:
				// TODO: make sure variables[name] exists, if not, look up globally
				real = s->layer->variables[name];
				break;

			case INTERVAL:
				if (s->voice) voice = s->voice;
				else if (s->layer) layer = s->layer;
				else throw creaturesException("wah, interval in variable node confused me"); // TODO
				break;
				
			case PAN:
				if (s->stage) stage = s->stage;
				else if (s->layer) layer = s->layer;
				else throw creaturesException("wah, pan in variable node confused me"); // TODO
				break;

			case VOLUME:
				if (s->stage) stage = s->stage;
				else if (s->layer) layer = s->layer;
				else if (s->track) track = s->track;
				else throw creaturesException("wah, volume in variable node confused me"); // TODO
				break;

			default:
				throw creaturesException("wah, unknown variabletype in variable node"); // TODO
		}
	}
	std::string dump() {
		switch (variabletype) {
			case NAMED: return name;
			case INTERVAL: return "Interval";
			case VOLUME: return "Volume";
			case PAN: return "Pan";
		}
		
		return "MNGVariableNodeIsConfused"; // TODO: exception? :P
	}
	variabletypes getType() { return variabletype; }
	std::string getName() { return name; }
};

class MNGAssignmentNode : public MNGNode { // assignment
protected:
	MNGVariableNode *variable;
	MNGExpression *expression;
	
public:
	MNGAssignmentNode(MNGVariableNode *v, MNGExpression *e) { variable = v; expression = e; }
	std::string dump() { return variable->dump() + " = " + expression->dump(); }
	virtual ~MNGAssignmentNode() { delete variable; delete expression; }
	virtual void postProcess(processState *s) { variable->postProcess(s); expression->postProcess(s); }
	MNGVariableNode *getVariable() { return variable; }
	MNGExpression *getExpression() { return expression; }
};

inline std::string dumpAssignmentChildren(std::list<MNGAssignmentNode *> *c) {
	std::string t = "\n";
	for (std::list<MNGAssignmentNode *>::iterator i = c->begin(); i != c->end(); i++) t = t + (*i)->dump() + "\n";
	return t;
}

class MNGConditionNode : public MNGNode { // condition
protected:
	MNGVariableNode *variable;
	float one, two;

public:
	MNGConditionNode(MNGVariableNode *v, float o, float t) { variable = v; one = o; two = t; }
	std::string dump() { return "Condition(" + variable->dump() + ", " + MNGConstantNode(one).dump() + ", " + MNGConstantNode(two).dump() + ")"; } // hacky..
	virtual void postProcess(processState *s) { variable->postProcess(s); }
	virtual ~MNGConditionNode() { delete variable; }
	float minimum() { return one; }
	float maximum() { return two; }
	MNGVariableNode *getVariable() { return variable; }
};

class MNGUpdateNode : public MNGNode { // update
public:
	std::list<MNGAssignmentNode *> *children; // assignmentlist
	virtual std::string dump() { return std::string("Update { ") + dumpAssignmentChildren(children) + "}\n"; }
	virtual void postProcess(processState *s) { for (std::list<MNGAssignmentNode *>::iterator i = children->begin(); i != children->end(); i++) (*i)->postProcess(s); }
	virtual ~MNGUpdateNode() { for (std::list<MNGAssignmentNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGVoiceNode : public MNGNode { // voiceblock
public:
	std::list<MNGNode *> *children; // voicecommands
	virtual std::string dump() { return std::string("Voice { ") + dumpChildren(children) + "}\n"; }
	virtual void postProcess(processState *s) { s->voice = this; for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) (*i)->postProcess(s); s->voice = 0; }
	virtual ~MNGVoiceNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGFile {
	private:
		class mmapifstream *stream;
		std::string name;
		int numsamples, scriptoffset, scriptlength, scriptend;
		unsigned int sampleno;

		static int yylineno;
		static const char *mngfile_parse_p;
		static void yyinit(const char *buf);
		static int mnglex();
		friend int mnglex();
		friend void mngerror(const char*);

	public:
		std::map<std::string, class MNGVariableDecNode *> variables; // TODO: should be private?
		MNGFile(std::string);
		void enumerateSamples();
		~MNGFile();
		void add(class MNGEffectDecNode *n);
		void add(class MNGTrackDecNode *n);
		void add(class MNGVariableDecNode *n);
		unsigned int getSampleForName(std::string name);
		char * script;
		std::map<std::string, unsigned int> samplemappings;
		std::vector< std::pair< char *, int > > samples;
		std::map<std::string, class MNGEffectDecNode *> effects;
		std::map<std::string, class MNGTrackDecNode *> tracks;

		std::string dump();
};

extern MNGFile *g_mngfile;

class MNGWaveNode : public MNGNamedNode { // wave
protected:
	unsigned int sampleno;

public:
	MNGWaveNode(std::string n) : MNGNamedNode(n) { sampleno = g_mngfile->getSampleForName(n); }
	std::string dump() { return std::string("Wave(") + name + ")"; }
	unsigned int getSampleNumber() { return sampleno; }
};

