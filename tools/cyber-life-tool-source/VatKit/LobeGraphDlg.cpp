#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <cstring>

#include "LobeGraphDlg.h"
#include "BrainDlgContainer.h"

////////////////////////////////////////////////////////////////////////////////////

LobeGraphDlg::LobeGraphDlg(HINSTANCE hInstance, HWND hWndOwner, Lobe &lobe, int var, LobeNames &lobeNames, BrainDlgContainer &container) : 
GraphDlg(hInstance, hWndOwner),
myVariable(abs(var % SVRule::noOfVariables)),
myLobe(lobe),
myLobeNames(lobeNames),
myContainer(container)
{
	myWCS.dwExStyle = WS_EX_TOPMOST;
	myWCS.dwStyle = WS_THICKFRAME | WS_CLIPCHILDREN  | WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_OVERLAPPED | WS_SYSMENU;



	std::string *varnames;
	varnames = new std::string[GetLobeSize(myLobe)];
	for(int i = 0; i != GetLobeSize(myLobe); i++)
	{
		varnames[i] = myLobeNames.GetNeuronName(LobeIdInList(myLobe), i);
	}
	std::string labels[3];
	labels[0] = "-1.0";
	labels[1] = "0.0";
	labels[2] = "1.0";

	ChangeSpec(100, 250, GetLobeSize(myLobe), varnames, labels);

	delete []varnames;
}


LobeGraphDlg::~LobeGraphDlg()
{	
	myContainer.RemoveDlg(this);
}


HWND LobeGraphDlg::Create()
{

	if(!GraphDlg::Create())
		return NULL;
	
	char caption[100];
	sprintf(caption,"(L:%s, V:%d, Lobe:%s Variable: %d", myLobeNames.GetLobeFullName(LobeIdInList(myLobe)), myVariable, myLobeNames.GetLobeFullName(LobeIdInList(myLobe)), myVariable);
	SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)caption);

	Update();

	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);
	
	return myHWnd;
}


LRESULT LobeGraphDlg::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_RBUTTONDOWN:
		HANDLE_WM_RBUTTONDOWN(hwnd, wParam, lParam, On_RButtonDown);
		break;
	
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	default:
		// call default superclass handler
		return(GraphDlg::WndProc(hwnd, msg, wParam, lParam));
	}
	return (LRESULT)1;
}


void LobeGraphDlg::On_RButtonDown(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags)
{
	static bool popupActive = false;

	if(popupActive)
		return;
	
	popupActive = true;



	HMENU menus = LoadMenu(GetWindowInstance(myHWnd), MAKEINTRESOURCE(IDR_POPUPS));
	HMENU hMenuPopup = GetSubMenu(menus, 5);

	switch(myVariable) 
	{
	case 0:
		CheckMenuItem(hMenuPopup, ID_VAR0, MF_CHECKED);
		break;
	case 1:
		CheckMenuItem(hMenuPopup, ID_VAR1, MF_CHECKED);
		break;
	case 2:
		CheckMenuItem(hMenuPopup, ID_VAR2, MF_CHECKED);
		break;
	case 3:
		CheckMenuItem(hMenuPopup, ID_VAR3, MF_CHECKED);
		break;
	case 4:
		CheckMenuItem(hMenuPopup, ID_VAR4, MF_CHECKED);
		break;
	case 5:
		CheckMenuItem(hMenuPopup, ID_VAR5, MF_CHECKED);
		break;
	case 6:
		CheckMenuItem(hMenuPopup, ID_VAR6, MF_CHECKED);
		break;
	case 7:
		CheckMenuItem(hMenuPopup, ID_VAR7, MF_CHECKED);
		break;
	}

	RECT rect;
	GetClientRect(myHWnd, &rect);

	UINT align;
	if(x > (rect.right-rect.left)/2)
		align = TPM_RIGHTALIGN;
	else
		align = TPM_LEFTALIGN;
	if(y > (rect.bottom-rect.top)/2)
		align |= TPM_BOTTOMALIGN;
	else
		align |= TPM_TOPALIGN;

	POINT p;
	p.x = x;
	p.y = y;
	ClientToScreen(myHWnd, &p);

	int menuID = TrackPopupMenu(hMenuPopup, align, p.x, p.y, 0, myHWnd, NULL); 

	popupActive = false;
	DestroyMenu(menus);

}


void LobeGraphDlg::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	int oldVariable = myVariable;
	switch(id)
	{	
	case ID_VAR0:
		myVariable = 0;
		break;
	case ID_VAR1:
		myVariable = 1;
		break;
	case ID_VAR2:
		myVariable = 2;
		break;
	case ID_VAR3:
		myVariable = 3;
		break;
	case ID_VAR4:
		myVariable = 4;
		break;
	case ID_VAR5:
		myVariable = 5;
		break;
	case ID_VAR6:
		myVariable = 6;
		break;
	case ID_VAR7:
		myVariable = 7;
		break;
	}
	if(oldVariable != myVariable)
		Reset();
}

void LobeGraphDlg::Update()
{ 
	float state;
	for(int n = 0; n != GetLobeSize(myLobe); n++)
	{
		state = GetVar(GetNeuron(myLobe, n), myVariable);
		myTestTrace[n].AddSample(0.5f+(state/2)); // sine
	}
	myITime++;
	PostSample();
}

