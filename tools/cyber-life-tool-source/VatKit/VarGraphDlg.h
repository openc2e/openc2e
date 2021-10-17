#ifndef VarGraphDlg_H
#define VarGraphDlg_H

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <vector>

#include "Wnd.h"
#include "../../engine/Creature/Brain/BrainAccess.h"
#include "LobeNames.h"
#include "BrainDlg.h"
#include "GraphDlg.h"

class BrainDlgContainer;

class VarGraphDlg : public BrainAccess, public GraphDlg, public BrainDlg
{	
public:

	VarGraphDlg(HINSTANCE hInstance, HWND hWndOwner, LobeNames &lobeNames, BrainDlgContainer &container);
	~VarGraphDlg();
	HWND Create();
	virtual void Activate() {SetActiveWindow(myHWnd);};
	bool Updatable() {return true;};

protected:
	LobeNames &myLobeNames;

private:

	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];
	BrainDlgContainer &myContainer;
};



class NeuronVarGraphDlg : public VarGraphDlg
{	
public:

	NeuronVarGraphDlg(HINSTANCE hInstance, HWND hWndOwner, Neuron &neuron, Lobe &lobe, LobeNames &lobeNames, BrainDlgContainer &container);
	HWND Create();
	const void *LinkedTo(){return (void *)&myNeuron;};
	const void *OrganLinkedTo(){return (void *)&myLobe;};
	void Update();

private:
	Neuron &myNeuron;
	Lobe &myLobe;
};



class DendriteVarGraphDlg : public VarGraphDlg
{	
public:

	DendriteVarGraphDlg(HINSTANCE hInstance, HWND hWndOwner, Dendrite &dendrite, Tract &tract, LobeNames &lobeNames, BrainDlgContainer &container);
	HWND Create();
	const void *LinkedTo(){return (void *)&myDendrite;};
	const void *OrganLinkedTo(){return (void *)&myTract;};
	void Update();

private:

	Dendrite &myDendrite;
	Tract &myTract;
	Neuron *mySrc;
};

#endif

