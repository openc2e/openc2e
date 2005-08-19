#include <string>
#include <vector>
#include <list>
#include <istream>
#include "exceptions.h"

using namespace std;

void mngrestart(std::istream *is);

class MNGNode {
};

class MNGFile {
	private:
		FILE * f;
		long filesize;
		char * map;
		string name;
		int numsamples, scriptoffset, scriptlength, scriptend;
		char * script;
		vector< pair< string, pair< char *, int > > > samples;
		list<MNGNode *> nodes;
	
	public:
		 MNGFile(string);
		 void enumerateSamples();
		 ~MNGFile();
		 void add(MNGNode *n) { nodes.push_front(n); }
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

class MNGEffectDecNode : public MNGNamedNode { // effectdec
public:
	MNGEffectDecNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children; // stagelist
};

class MNGStageNode : public MNGNode { // stage
public:
	std::list<MNGNode *> *children; // stagesettinglist
};

class MNGTrackDecNode : public MNGNamedNode { // trackdec
public:
	MNGTrackDecNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children; // track
};

class MNGEffectNode : public MNGNamedNode { // effect
public:
	MNGEffectNode(std::string n) : MNGNamedNode(n) { }
};

class MNGExpression : public MNGNode { // (expression)
};

class MNGBinaryExpression : public MNGExpression {
protected:
	MNGExpression *one, *two;

public:
	MNGBinaryExpression(MNGExpression *o, MNGExpression *t) { one = o; two = t; }
};

class MNGConstantNode : public MNGExpression { // MNG_number
protected:
	float value;

public:
	MNGConstantNode(float n) { value = n; }
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
};

class MNGLayerVolumeNode : public MNGExpressionContainer { // layervolume
public:
	MNGLayerVolumeNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGEffectVolumeNode : public MNGExpressionContainer { // effectvolume
public:
	MNGEffectVolumeNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGDelayNode : public MNGExpressionContainer { // delay
public:
	MNGDelayNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGRandomNode : public MNGBinaryExpression { // random
public:
	MNGRandomNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
};

class MNGTempoDelayNode : public MNGExpressionContainer { // tempodelay
public:
	MNGTempoDelayNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGFadeInNode : public MNGExpressionContainer { // fadein
public:
	MNGFadeInNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGFadeOutNode : public MNGExpressionContainer { // fadeout
public:
	MNGFadeOutNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGBeatLengthNode : public MNGExpressionContainer { // beatlength
public:
	MNGBeatLengthNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGLoopLayerNode : public MNGNamedNode { // looplayerdec
public:
	MNGLoopLayerNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children;
};

class MNGAleotoricLayerNode : public MNGNamedNode { // aleotoriclayerdec
public:
	MNGAleotoricLayerNode(std::string n) : MNGNamedNode(n) { }
	std::list<MNGNode *> *children;
};

class MNGAddNode : public MNGBinaryExpression { // add
public:
	MNGAddNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
};

class MNGSubtractNode : public MNGBinaryExpression { // subtract
public:
	MNGSubtractNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
};

class MNGMultiplyNode : public MNGBinaryExpression { // multiply
public:
	MNGMultiplyNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
};

class MNGDivideNode : public MNGBinaryExpression { // divide
public:
	MNGDivideNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
};

class MNGSineWaveNode : public MNGBinaryExpression { // sinewave
public:
	MNGSineWaveNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
};

class MNGCosineWaveNode : public MNGBinaryExpression { // cosinewave
public:
	MNGCosineWaveNode(MNGExpression *o, MNGExpression *t) : MNGBinaryExpression(o, t) { } 
};

class MNGBeatSynchNode : public MNGExpressionContainer { // beatsynch
public:
	MNGBeatSynchNode(MNGExpression *n) : MNGExpressionContainer(n) { } 
};

class MNGUpdateRateNode : public MNGExpressionContainer { // updaterate
public:
	MNGUpdateRateNode(MNGExpression *n) : MNGExpressionContainer(n) { } 
};

class MNGWaveNode : public MNGNamedNode { // wave
public:
	MNGWaveNode(std::string n) : MNGNamedNode(n) { }
};

class MNGIntervalNode : public MNGExpressionContainer { // interval
public:
	MNGIntervalNode(MNGExpression *n) : MNGExpressionContainer(n) { }
};

class MNGVariableDecNode : public MNGNamedNode {
protected:
	// TODO: we should evaluate this right away, and delete the unwanted node
	MNGExpression *initialexpression;
	
public:
	MNGVariableDecNode(std::string n, MNGExpression *e) : MNGNamedNode(n) { initialexpression = e; }
};

enum variabletypes { NAMED, INTERVAL, VOLUME, PAN };

class MNGVariableNode : public MNGExpression { // variable
protected:
	std::string name;
	variabletypes variabletype;

public:
	MNGVariableNode(std::string n) { variabletype = NAMED; name = n; }
	MNGVariableNode(variabletypes t) { variabletype = t; }
};

class MNGAssignmentNode : public MNGNode { // assignment
protected:
	MNGVariableNode *variable;
	MNGExpression *expression;
	
public:
	MNGAssignmentNode(MNGVariableNode *v, MNGExpression *e) { variable = v; expression = e; }
};

class MNGConditionNode : public MNGNode { // assignment
protected:
	MNGVariableNode *variable;
	float one, two;

public:
	MNGConditionNode(MNGVariableNode *v, float o, float t) { variable = v; one = o; two = t; }
};

class MNGUpdateNode : public MNGNode { // update
public:
	std::list<MNGNode *> *children; // assignmentlist
};

class MNGVoiceNode : public MNGNode { // voiceblock
public:
	std::list<MNGNode *> *children; // voicecommands
};

