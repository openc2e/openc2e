#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "VarDlg.h"
#include "BrainDlgContainer.h"
#include "../../common/GameInterface.h"
#include "BrainViewport.h"

extern GameInterface theGameInterface;



bool VarDlg::ourIsRegistered = false;

VarDlg::VarDlg(HINSTANCE hInstance, HWND hWndOwner, LobeNames &lobeNames, BrainDlgContainer &container) : Wnd(),
myFont(NULL),
myOwnerHWnd(hWndOwner),
myContainer(container),
myLobeNames(lobeNames)
{
	LoadString(hInstance, IDR_VARDLG, ourClassName, MAXRESOURCESTRING);

	if(!ourIsRegistered)
	{
		// Register Class
		myWCEX.style = CS_HREDRAW | CS_VREDRAW;
		myWCEX.cbClsExtra = 0;
		myWCEX.cbWndExtra = 0;
		myWCEX.hInstance = hInstance;
		myWCEX.hIcon = NULL;
		myWCEX.hCursor = LoadCursor(NULL, IDC_ARROW);
		myWCEX.hbrBackground = GetSysColorBrush(COLOR_MENU);
		myWCEX.lpszMenuName = NULL;	
		myWCEX.lpszClassName = ourClassName;

		RegisterWndClass();

		ourIsRegistered = true;
	}

	// Create Window
	myWCS.dwExStyle = WS_EX_TOPMOST;
	myWCS.lpClassName = ourClassName;	
	myWCS.lpWindowName = "Var Dlg";
	myWCS.dwStyle = WS_CLIPCHILDREN  | WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_OVERLAPPED | WS_SYSMENU;
	myWCS.x = CW_USEDEFAULT;
	myWCS.y = CW_USEDEFAULT;
	myWCS.nWidth = CW_USEDEFAULT;
	myWCS.nHeight = CW_USEDEFAULT;
	myWCS.hWndParent = NULL;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;

}


HWND VarDlg::Create()
{
	// create font;
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfHeight = 12;
	logfont.lfWeight = 14;
	strcpy(logfont.lfFaceName, "MS Sans Serif");
	myFont = CreateFontIndirect(&logfont);
	SetWindowFont(myHWnd, myFont, false);
	
	// calculate test character size
	SIZE textSize;
	HDC hdc;
	
	if(!(hdc = GetDC(myHWnd))) 
		return NULL;

	char testChar[2] = "0";

	GetTextExtentPoint(hdc, testChar, 1, &textSize);
	ReleaseDC(myHWnd, hdc);
	
	// calc positioning vars
	myVarDlgSpacer = 5;
	myVarDescHeight = textSize.cy;
	myVarTop = myVarDlgSpacer+(myVarDescHeight*2)+myVarDlgSpacer;
	myVarWidth = (textSize.cx*4)+2;
	myVarTrackbarHeight = 130;
	myVarLabelHeight = textSize.cy;
	
	myWCS.nWidth = (GetSystemMetrics(SM_CXDLGFRAME)*2)+myVarDlgSpacer+((myVarDlgSpacer+myVarWidth)*SVRule::noOfVariables);
	myWCS.nHeight = GetSystemMetrics(SM_CYCAPTION)+(GetSystemMetrics(SM_CYDLGFRAME)*2)+myVarTop+
		myVarTrackbarHeight+myVarLabelHeight+myVarDlgSpacer;
		
	if(!Wnd::Create())
		return NULL;

	InitCommonControls(); // loads common control's DLL

	for(int i = 0; i != SVRule::noOfVariables; i++)
		CreateVar(i);

	RECT windowRect;
	GetWindowRect(myHWnd, &windowRect);

	myHWndDescription1 = CreateWindow("static",
					NULL,
					WS_CHILD | WS_VISIBLE | SS_LEFT,
					myVarDlgSpacer, 5, 400, myVarDescHeight,
					myHWnd,
					NULL,
					GetWindowInstance(myHWnd),
					NULL);
	
	SetWindowFont(myHWndDescription1, myFont, false);

	myHWndDescription2 = CreateWindow("static",
					NULL,
					WS_CHILD | WS_VISIBLE | SS_LEFT,
					myVarDlgSpacer, 5+myVarDescHeight, 300, myVarDescHeight,
					myHWnd,
					NULL,
					GetWindowInstance(myHWnd),
					NULL);
	
	SetWindowFont(myHWndDescription2, myFont, false);

	// GRAPH button
	myHWndGraphButton = CreateWindow("button",
					NULL,
					WS_CHILD | WS_VISIBLE,
					myVarDlgSpacer+((myVarDlgSpacer+myVarWidth)*(SVRule::noOfVariables))-((textSize.cx*5)+4)-myVarDlgSpacer, 5+myVarDescHeight, (textSize.cx*5)+4, textSize.cy+4,
					myHWnd, 
					(HMENU) ID_BUTTONGRAPH,
					GetWindowInstance(myHWnd),
					NULL);
	SetWindowFont(myHWndGraphButton, myFont, false);

	char text[]="graph";
	SendMessage(myHWndGraphButton, WM_SETTEXT, 0, (LPARAM)text);

	return myHWnd;
}


VarDlg::~VarDlg()
{	
	myContainer.RemoveDlg(this);

	if(myFont)
		DeleteObject(myFont);
}


void VarDlg::CreateVar(int i)
{
	// create trackbar
	myHWndVarTrack[i] = CreateWindow(TRACKBAR_CLASS,
					NULL,
					WS_CHILD | WS_VISIBLE | TBS_RIGHT | TBS_VERT | TBS_AUTOTICKS,
					myVarDlgSpacer+((myVarDlgSpacer+myVarWidth)*i), myVarTop, myVarWidth, myVarTrackbarHeight,
					myHWnd,
					(HMENU) i,
					GetWindowInstance(myHWnd),
					NULL);
	
	SendMessage(myHWndVarTrack[i], TBM_SETRANGE, (WPARAM) TRUE,(LPARAM) MAKELONG(0, 1000)); 
	SendMessage(myHWndVarTrack[i], TBM_SETPAGESIZE, 0, (LPARAM) 1);
	SendMessage(myHWndVarTrack[i], TBM_SETTICFREQ, (WPARAM) 25, (LPARAM)0);

	// create static of trackbar value 
	myHWndVarStatic[i] = CreateWindow("static",
					NULL,
					WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SUNKEN,
					myVarDlgSpacer+(myVarDlgSpacer+myVarWidth)*i, myVarTop+myVarTrackbarHeight, myVarWidth, myVarLabelHeight,
					myHWnd,
					NULL,
					GetWindowInstance(myHWnd),
					NULL);
	SetWindowFont(myHWndVarStatic[i], myFont, false);

	SetTrackbar(i, 0);
}


void VarDlg::SetTrackbar(int trackbar, float value)
{
	char buffer[5];
	SendMessage(myHWndVarTrack[trackbar], TBM_SETPOS, (WPARAM) TRUE, (int)500-(value*500));
	sprintf(buffer, "%1.3f", value);
	SendMessage(myHWndVarStatic[trackbar], WM_SETTEXT, 0, (LPARAM)buffer);
};

// Message Handleing ///////////////////

LRESULT VarDlg::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	case WM_VSCROLL:
	case WM_HSCROLL:
		On_Scroll((HWND)lParam);
		break;

	default:
		// call default superclass handler
		return(Wnd::WndProc(hwnd, msg, wParam, lParam));	
 
	}
	return(NULL);
}


void VarDlg::On_Scroll(HWND hwnd)
{
	float f; 
	int v=0;
	char buffer[5];
	
	while(myHWndVarTrack[v] != hwnd && v < SVRule::noOfVariables) v++;
		
	f = ((float)500 - SendMessage(myHWndVarTrack[v], TBM_GETPOS, 0, 0))/500;
	SetVar(v, f);
	sprintf(buffer, "%1.3f", f);
	SendMessage(myHWndVarStatic[v], WM_SETTEXT, 0, (LPARAM)buffer);

}





// NeuronVarDlg ///////////////////////////

void NeuronVarDlg::SetVar(int var, float value)
{ 
	BrainAccess::SetVar(myNeuron, var , value);

	if(theGameInterface.Connected())
	{
		char macro[50];
		sprintf(macro, "execute\nTARG AGNT %d BRN: SETN %d %d %d %f", theBrainViewport->GetOnlineId(), *(myLobe.GetPointerToIdInList()), myNeuron.idInList, var, value);
		theGameInterface.Inject(macro, NULL);
	}

	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
}


void NeuronVarDlg::Update()
{ 
	int v;
	float state;
	for(v = 0; v != SVRule::noOfVariables; v++)
	{
		state = GetVar(myNeuron, v);
		SetTrackbar(v, state);
	}
}


NeuronVarDlg::NeuronVarDlg(HINSTANCE hInstance, HWND hWndOwner, Neuron &neuron, Lobe &lobe, LobeNames &lobeNames, BrainDlgContainer &container) : 
VarDlg(hInstance, hWndOwner, lobeNames, container),
myLobe(lobe),
myNeuron(neuron)
{

}


HWND NeuronVarDlg::Create()
{
	if(!VarDlg::Create())
		return NULL;

	char caption[100];
	int n = NeuronIdInList(myNeuron);
	sprintf(caption,"(N%d, %s) Neuron: %s in Lobe: %s", n, 
		myLobeNames.GetLobeFullName(LobeIdInList(myLobe)), 
		myLobeNames.GetNeuronName(LobeIdInList(myLobe), n), 
		myLobeNames.GetLobeFullName(LobeIdInList(myLobe)));

	
	SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)caption);

	char description[100];
	sprintf(description,"Neuron: %s ", myLobeNames.GetNeuronName(LobeIdInList(myLobe), n));
	SendMessage(myHWndDescription1, WM_SETTEXT, 0, (LPARAM)description);

	sprintf(description,"Lobe: %s ", myLobeNames.GetLobeFullName(LobeIdInList(myLobe)));
	SendMessage(myHWndDescription2, WM_SETTEXT, 0, (LPARAM)description);

	Update();

	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);

	return myHWnd;
}

void NeuronVarDlg::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	switch(id)
	{
	case ID_BUTTONGRAPH:
		myContainer.CreateNeuronVarGraphDlg(myNeuron, myLobe, GetParent(myHWnd));
		break;

	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, NotifyCode, DefWindowProc);
		
	}
}



// DendriteVarDlg ///////////////////////////

void DendriteVarDlg::SetVar(int var, float value)
{ 
	BrainAccess::SetVar(myDendrite, var , value);

	if(theGameInterface.Connected())
	{
		char macro[50];
		sprintf(macro, "execute\nTARG AGNT %d BRN: SETD %d %d %d %f", theBrainViewport->GetOnlineId(), *(myTract.GetPointerToIdInList()), myDendrite.idInList, var, value);
		theGameInterface.Inject(macro, NULL);
	}

	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
}

void DendriteVarDlg::Update()
{ 
	int v;
	float weight;
	for(v = 0; v != SVRule::noOfVariables; v++)
	{
		weight = GetVar(myDendrite, v);
		SetTrackbar(v, weight);
	}

	if(mySrc != myDendrite.srcNeuron || myDst != myDendrite.dstNeuron)
	{
		// either setting up or migrating
		char description[110];
		char tractName[100];
		int srcNeuron, dstNeuron, dendriteNumber;
		char caption[200];
		
		dendriteNumber = GetDendriteDesc(myDendrite, myTract, &srcNeuron, &dstNeuron);
		sprintf(tractName, "%s->%s", myLobeNames.GetLobeFullName(LobeIdInList(GetSrcLobe(myTract))), myLobeNames.GetLobeFullName(LobeIdInList(GetDstLobe(myTract))));

		sprintf(caption,"(D:(%d)%d->%d, %s) Dendrite: (%d) from %s to %s in Tract: %s", dendriteNumber, srcNeuron, dstNeuron, tractName, dendriteNumber, myLobeNames.GetNeuronName(LobeIdInList(GetSrcLobe(myTract)), srcNeuron), myLobeNames.GetNeuronName(LobeIdInList(GetDstLobe(myTract)), dstNeuron), tractName);
		SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)caption);

		if(mySrc && myDst)
		{
			// had a source so must have migrated
			int oldSrcNeuron = LobeIdInList(GetSrcLobe(myTract));
			int oldDstNeuron = LobeIdInList(GetDstLobe(myTract));
			sprintf(description,"Dendrite: (%d), %s->%s (was %d->%d)", dendriteNumber, myLobeNames.GetNeuronName(LobeIdInList(GetSrcLobe(myTract)), srcNeuron), myLobeNames.GetNeuronName(LobeIdInList(GetDstLobe(myTract)), dstNeuron), oldSrcNeuron, oldDstNeuron);
		}
		else
		{
			// no previous source - must be initialising dialog
			sprintf(description,"Dendrite: (%d), %s->%s", dendriteNumber, 
				myLobeNames.GetNeuronName(LobeIdInList(GetSrcLobe(myTract)), srcNeuron), 
				myLobeNames.GetNeuronName(LobeIdInList(GetDstLobe(myTract)), dstNeuron));
		}
		SendMessage(myHWndDescription1, WM_SETTEXT, 0, (LPARAM)description);

		sprintf(description,"Tract: %s ", tractName);
		SendMessage(myHWndDescription2, WM_SETTEXT, 0, (LPARAM)description);

		mySrc = myDendrite.srcNeuron;
		myDst = myDendrite.dstNeuron;

	}

}


DendriteVarDlg::DendriteVarDlg(HINSTANCE hInstance, HWND hWndOwner, Dendrite &dendrite, Tract &tract, LobeNames &lobeNames, BrainDlgContainer &container) :
VarDlg(hInstance, hWndOwner, lobeNames, container),
myTract(tract),
myDendrite(dendrite)
{

	mySrc = NULL;
	myDst = NULL;
}


HWND DendriteVarDlg::Create()
{
	if(!VarDlg::Create())
		return NULL;
	
	Update();

	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);

	return myHWnd;
}

void DendriteVarDlg::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	switch(id)
	{
	case ID_BUTTONGRAPH:
		myContainer.CreateDendriteVarGraphDlg(myDendrite, myTract, GetParent(myHWnd));
		break;

	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, NotifyCode, DefWindowProc);
		
	}
}

