#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <cstring>

#include "VarGraphDlg.h"
#include "BrainDlgContainer.h"

////////////////////////////////////////////////////////////////////////////////////

VarGraphDlg::VarGraphDlg(HINSTANCE hInstance, HWND hWndOwner, LobeNames &lobeNames, BrainDlgContainer &container) : 
GraphDlg(hInstance, hWndOwner),
myLobeNames(lobeNames),
myContainer(container)
{
	myWCS.dwExStyle = WS_EX_TOPMOST;
	myWCS.dwStyle = WS_THICKFRAME | WS_CLIPCHILDREN  | WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_OVERLAPPED | WS_SYSMENU;

	std::string varnames[SVRule::noOfVariables];
	varnames[0] = "Var 0";
	varnames[1] = "Var 1";
	varnames[2] = "Var 2";
	varnames[3] = "Var 3";
	varnames[4] = "Var 4";
	varnames[5] = "Var 5";
	varnames[6] = "Var 6";
	varnames[7] = "Var 7";

	std::string labels[3];
	labels[0] = "-1.0";
	labels[1] = "0.0";
	labels[2] = "1.0";
	ChangeSpec(100, 250, SVRule::noOfVariables, varnames, labels);
}


VarGraphDlg::~VarGraphDlg()
{	
	myContainer.RemoveDlg(this);
}


HWND VarGraphDlg::Create()
{

	if(!GraphDlg::Create())
		return NULL;

	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);

	return myHWnd;
}


// derived classes

void NeuronVarGraphDlg::Update()
{ 
	float state;
	for(int v = 0; v != SVRule::noOfVariables; v++)
	{
		state = GetVar(myNeuron, v);
		myTestTrace[v].AddSample(0.5f+(state/2)); // sine
	}
	myITime++;
	PostSample();
}

NeuronVarGraphDlg::NeuronVarGraphDlg(HINSTANCE hInstance, HWND hWndOwner, Neuron &neuron, Lobe &lobe, LobeNames &lobeNames, BrainDlgContainer &container) :
VarGraphDlg(hInstance, hWndOwner, lobeNames, container),
myLobe(lobe),
myNeuron(neuron)
{
	return;
}

HWND NeuronVarGraphDlg::Create()
{
	if(!VarGraphDlg::Create())
		return NULL;

	Update();
	char caption[100];
	int n = NeuronIdInList(myNeuron);
	sprintf(caption,"(N%d, %s) Neuron: %s in Lobe: %s", n, myLobeNames.GetLobeFullName(LobeIdInList(myLobe)), 
		myLobeNames.GetNeuronName(LobeIdInList(myLobe), n), myLobeNames.GetLobeFullName(LobeIdInList(myLobe)));
	SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)caption);

	return myHWnd;
}

// DendriteVarGraphDlg ///////////////////////////

void DendriteVarGraphDlg::Update()
{ 

	int v;
	float weight;
	for(v = 0; v != SVRule::noOfVariables; v++)
	{
		weight = GetVar(myDendrite, v);
		myTestTrace[v].AddSample(0.5f+(weight/2)); // sine
	}

	myITime++;
	PostSample();

	if(mySrc != myDendrite.srcNeuron)
	{
		// either setting up or migrating
		char tractName[50];
		int srcNeuron, dstNeuron, dendriteNumber;
		char caption[100];
		
		dendriteNumber = GetDendriteDesc(myDendrite, myTract, &srcNeuron, &dstNeuron);
		sprintf(tractName, "%s->%s", myLobeNames.GetLobeFullName(LobeIdInList(GetSrcLobe(myTract))), myLobeNames.GetLobeFullName(LobeIdInList(GetDstLobe(myTract))));

		sprintf(caption,"(D:(%d)%d->%d, %s) Dendrite: (%d) from %s to %s in Tract: %s", dendriteNumber, srcNeuron, dstNeuron, tractName, dendriteNumber, myLobeNames.GetNeuronName(LobeIdInList(GetSrcLobe(myTract)), srcNeuron), myLobeNames.GetNeuronName(LobeIdInList(GetDstLobe(myTract)), dstNeuron), tractName);
		SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)caption);

		mySrc = myDendrite.srcNeuron;
	}

}


DendriteVarGraphDlg::DendriteVarGraphDlg(HINSTANCE hInstance, HWND hWndOwner, Dendrite &dendrite, Tract &tract, LobeNames &lobeNames, BrainDlgContainer &container) :
VarGraphDlg(hInstance, hWndOwner, lobeNames, container),
myDendrite(dendrite),
myTract(tract)
{
	mySrc = NULL;
}

HWND DendriteVarGraphDlg::Create()
{
	if(!VarGraphDlg::Create())
		return NULL;

	Update();

	return myHWnd;
}

