#ifndef VarDlg_H
#define VarDlg_H

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>

#include "Wnd.h"
#include "../../engine/Creature/Brain/BrainAccess.h"
#include "BrainDlg.h"
#include "LobeNames.h"
class BrainDlgContainer;

class VarDlg : public Wnd, public BrainAccess, public BrainDlg
{	
public:


	VarDlg(HINSTANCE hInstance, HWND hWndOwner, LobeNames &lobeNames, BrainDlgContainer &container);
	~VarDlg();
	HWND Create();
	virtual void Activate() {SetActiveWindow(myHWnd);};
	bool Updatable() {return true;};


protected:

	HWND myOwnerHWnd;
	HWND myHWndVarTrack[SVRule::noOfVariables];
	HWND myHWndVarStatic[SVRule::noOfVariables];
	HWND myHWndDescription1;
	HWND myHWndDescription2;
	HWND myHWndGraphButton;
	
	BrainDlgContainer &myContainer;
	LobeNames &myLobeNames;

	void SetTrackbar(int trackbar, float value);
	virtual void On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)=0;

private:

	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];


	HFONT myFont;

	int myVarDlgSpacer;
	int myVarDescHeight;
	int myVarTop;
	int myVarWidth;
	int myVarTrackbarHeight;
	int myVarLabelHeight;

	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	
	void CreateVar(int i);
	void On_Scroll(HWND hwnd);
	virtual void SetVar(int var , float value) = 0;		
};



class NeuronVarDlg : public VarDlg
{	
public:

	NeuronVarDlg(HINSTANCE hInstance, HWND hWndOwner, Neuron &neuron, Lobe &lobe, LobeNames &lobeNames, BrainDlgContainer &container);
	HWND Create();
	const void *LinkedTo(){return (void *)&myNeuron;};
	const void *OrganLinkedTo(){return (void *)&myLobe;};
	void Update();

private:
	Neuron &myNeuron;
	Lobe &myLobe;
	void SetVar(int var, float value);
	void On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode);


};



class DendriteVarDlg : public VarDlg
{	
public:

	DendriteVarDlg(HINSTANCE hInstance, HWND hWndOwner, Dendrite &dendrite, Tract &tract, LobeNames &lobeNames, BrainDlgContainer &container);
	HWND Create();
	const void *LinkedTo(){return (void *)&myDendrite;};
	const void *OrganLinkedTo(){return (void *)&myTract;};
	void Update();

private:

	Dendrite &myDendrite;
	Neuron *mySrc;
	Neuron *myDst;
	Tract &myTract;
	void SetVar(int var, float value);
	void On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode);

};

#endif

