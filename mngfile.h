#include <string>
#include <vector>
#include <list>
#include <istream>
#include "exceptions.h"

using namespace std;

void mngrestart(std::istream *is);

class MNGNode {
public:
	virtual std::string dump() { return "[unknown node]"; }
	virtual ~MNGNode() { }
};

class MNGFile {
	private:
		FILE * f;
		long filesize;
		char * map;
		string name;
		int numsamples, scriptoffset, scriptlength, scriptend;
		char * script;
		vector< pair< char *, int > > samples;
		list<MNGNode *> nodes;
		unsigned int sampleno;
	
	public:
		 MNGFile(string);
		 void enumerateSamples();
		 ~MNGFile();
		 void add(MNGNode *n) { nodes.push_back(n); }
		 pair<char *, int> *getNextSample() { sampleno++; return &samples[sampleno - 1]; }

		 std::string dump() {
		 	std::string t = "\n";

			for (std::list<MNGNode *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
				t = t + (*i)->dump() + "\n";
			}

			return t;
		}
};

extern MNGFile *g_mngfile;

class MNGFileException : public creaturesException {
	public:
		int error;
		MNGFileException(const char * m, int e) throw() : creaturesException(m) { error = e; }
};

class MNGNamedNode : public MNGNode {
protected:
	std::string name;

public:
	MNGNamedNode(std::string n) { name = n; }
};

inline std::string dumpChildren(std::list<MNGNode *> *c) {
	std::string t = "\n";

	for (std::list<MNGNode *>::iterator i = c->begin(); i != c->end(); i++) {
		t = t + (*i)->dump() + "\n";
	}

	return t;
}

class MNGEffectDecNode : public MNGNamedNode { // effectdec
public:
	MNGEffectDecNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children; // stagelist
	virtual std::string dump() { return std::string("Effect(" + name + ") { ") + dumpChildren(children) + "}\n"; }
	virtual ~MNGEffectDecNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGStageNode : public MNGNode { // stage
public:
	std::list<MNGNode *> *children; // stagesettinglist
	virtual std::string dump() { return std::string("Stage { ") + dumpChildren(children) + "}\n"; }
	virtual ~MNGStageNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGTrackDecNode : public MNGNamedNode { // trackdec
public:
	MNGTrackDecNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children; // track
	virtual std::string dump() { return std::string("Track(" + name + ") { ") + dumpChildren(children) + "}\n"; }
	virtual ~MNGTrackDecNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGEffectNode : public MNGNamedNode { // effect
public:
	MNGEffectNode(std::string n) : MNGNamedNode(n) { }
	virtual std::string dump() { return std::string("Effect(") + name + ")"; }
};

class MNGExpression : public MNGNode { // (expression)
};

class MNGBinaryExpression : public MNGExpression {
protected:
	MNGExpression *one, *two;

public:
	MNGBinaryExpression(MNGExpression *o, MNGExpression *t) { one = o; two = t; }
	virtual ~MNGBinaryExpression() { delete one; delete two; }
};

class MNGConstantNode : public MNGExpression { // MNG_number
protected:
	float value;

public:
	MNGConstantNode(float n) { value = n; }
	std::string dump() { char buf[20]; snprintf(buf, 20, "%f", value); return buf; }
};

class MNGExpressionContainer : public MNGNode {
protected:
	MNGExpression *subnode;

public:
	MNGExpressionContainer(MNGExpression *n) { subnode = n; }
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

class MNGLoopLayerNode : public MNGNamedNode { // looplayerdec
public:
	MNGLoopLayerNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children;
	virtual std::string dump() { return std::string("LoopLayer(" + name + ") { ") + dumpChildren(children) + "}\n"; }
	virtual ~MNGLoopLayerNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGAleotoricLayerNode : public MNGNamedNode { // aleotoriclayerdec
public:
	MNGAleotoricLayerNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children;
	virtual std::string dump() { return std::string("AleotoricLayer(" + name + ") { ") + dumpChildren(children) + "}\n"; }
	virtual ~MNGAleotoricLayerNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
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

class MNGWaveNode : public MNGNamedNode { // wave
protected:
	pair<char *, int> *sample;

public:
	MNGWaveNode(std::string n) : MNGNamedNode(n) { sample = g_mngfile->getNextSample(); }
	std::string dump() { return std::string("Wave(") + name + ")"; }
};

class MNGIntervalNode : public MNGExpressionContainer { // interval
public:
	MNGIntervalNode(MNGExpression *n) : MNGExpressionContainer(n) { }
	std::string dump() { return std::string("Interval(") + subnode->dump() + ")"; }
};

class MNGVariableDecNode : public MNGNamedNode {
protected:
	// TODO: we should evaluate this right away, and delete the unwanted node
	MNGExpression *initialexpression;
	
public:
	MNGVariableDecNode(std::string n, MNGExpression *e) : MNGNamedNode(n) { initialexpression = e; }
	std::string dump() { return std::string("Variable(") + name + ", " + initialexpression->dump() + ")"; }
};

enum variabletypes { NAMED, INTERVAL, VOLUME, PAN };

class MNGVariableNode : public MNGExpression { // variable
protected:
	std::string name;
	variabletypes variabletype;

public:
	MNGVariableNode(std::string n) { variabletype = NAMED; name = n; }
	MNGVariableNode(variabletypes t) { variabletype = t; }
	std::string dump() {
		switch (variabletype) {
			case NAMED: return name;
			case INTERVAL: return "Interval";
			case VOLUME: return "Volume";
			case PAN: return "Pan";
		}
		
		return "MNGVariableNodeIsConfused"; // TODO: exception? :P
	}
};

class MNGAssignmentNode : public MNGNode { // assignment
protected:
	MNGVariableNode *variable;
	MNGExpression *expression;
	
public:
	MNGAssignmentNode(MNGVariableNode *v, MNGExpression *e) { variable = v; expression = e; }
	std::string dump() { return variable->dump() + " = " + expression->dump(); }
	virtual ~MNGAssignmentNode() { delete variable; delete expression; }
};

class MNGConditionNode : public MNGNode { // assignment
protected:
	MNGVariableNode *variable;
	float one, two;

public:
	MNGConditionNode(MNGVariableNode *v, float o, float t) { variable = v; one = o; two = t; }
	std::string dump() { return "Condition(" + variable->dump() + ", " + MNGConstantNode(one).dump() + ", " + MNGConstantNode(two).dump() + ")"; } // hacky..
	virtual ~MNGConditionNode() { delete variable; }
};

class MNGUpdateNode : public MNGNode { // update
public:
	std::list<MNGNode *> *children; // assignmentlist
	virtual std::string dump() { return std::string("Update { ") + dumpChildren(children) + "}\n"; }
	virtual ~MNGUpdateNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

class MNGVoiceNode : public MNGNode { // voiceblock
public:
	std::list<MNGNode *> *children; // voicecommands
	virtual std::string dump() { return std::string("Update { ") + dumpChildren(children) + "}\n"; }
	virtual ~MNGVoiceNode() { for (std::list<MNGNode *>::iterator i = children->begin(); i != children->end(); i++) delete *i; delete children; }
};

