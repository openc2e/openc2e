#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "LobeInputDlg.h"
#include "BrainDlgContainer.h"


// INPUTS Dlg ////////////////////////////////////

bool LobeInputDlg::ourIsRegistered = false;

LobeInputDlg::LobeInputDlg(HINSTANCE hInstance, HWND hWndOwner, Lobe &lobe, LobeNames &lobeNames, BrainDlgContainer &container) : Wnd(),
myContainer(container), 
myLobe(lobe), 
myLobeNames(lobeNames),
myCurrentSel(0),
myFont(NULL),
myNeuronNames(NULL),
myInputValues(NULL)
{
	LoadString(hInstance, IDR_LOBEINPUTDLG, ourClassName, MAXRESOURCESTRING);

	if(!ourIsRegistered)
	{

		// Register Class
		myWCEX.style = CS_HREDRAW | CS_VREDRAW;
		//myWCEX.lpfnWndProc =	// dont fill this in will be set by RegisterWndClass 
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
	myWCS.lpWindowName = "Input Dlg";
	myWCS.dwStyle = WS_CLIPCHILDREN | WS_DLGFRAME ;
	myWCS.x = CW_USEDEFAULT;
	myWCS.y = CW_USEDEFAULT;
	myWCS.nWidth = 200;
	myWCS.nHeight = 350;
	myWCS.hWndParent = hWndOwner;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;

	myOwnerWnd = (Wnd *)GetWnd(hWndOwner); // not child but has owner

}


LobeInputDlg::~LobeInputDlg()
{	
	if(myNeuronNames)
	{
	
		for(int l = 0; l != GetLobeSize(myLobe); l++)
			delete []myNeuronNames[l];
	
		delete []myNeuronNames;
	}
	
	if(myInputValues)
		delete []myInputValues;
	

	myContainer.RemoveDlg(this);

	if(myFont)
		DeleteObject(myFont);

}


HWND LobeInputDlg::Create()
{	
	if(!Wnd::Create())
		return NULL;

	int const border = 3;

	// create font;
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfHeight = 12;
	logfont.lfWeight = 14;
	strcpy(logfont.lfFaceName, "MS Sans Serif");
	myFont = CreateFontIndirect(&logfont);
	SetWindowFont(myHWnd, myFont, false);
	
	// calculate character size
	SIZE textSize;
	HDC hdc;
	
	if(!(hdc = GetDC(myHWnd))) 
		return NULL;
	
	char testChar[2] = "W";
	GetTextExtentPoint(hdc, testChar, 1, &textSize);
	ReleaseDC(myHWnd, hdc);


	SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)myLobeNames.GetLobeFullName(LobeIdInList(myLobe)));

	// create listbox
	RECT clientRect;
	GetClientRect(myHWnd, &clientRect);
	
	myHWndInputList = CreateWindow("listbox", NULL, 
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL |  LBS_USETABSTOPS,
		border, border+40, CW_USEDEFAULT, (clientRect.bottom-border)-(border+border+40+20),
		myHWnd, (HMENU)IDC_LISTBOX, GetWindowInstance(myHWnd),	NULL);
	
	SetWindowFont(myHWndInputList, myFont, false);


	// calc largest neuron name

	int maxLen = {0};
	int maxChars=0;
	SIZE neuronNameSize;
	HDC listDC;
	
	if(!(listDC = GetDC(myHWndInputList))) 
		return false;
	
	int i;
	for(i = 0; i != GetLobeSize(myLobe); i++)
	{
	
		const char *neuronName = myLobeNames.GetNeuronName(LobeIdInList(myLobe), i);
		GetTextExtentPoint(listDC, neuronName, strlen(neuronName), &neuronNameSize);
		if(strlen(neuronName) > maxChars) maxChars = strlen(neuronName);
	
		if(neuronNameSize.cx > maxLen)
			maxLen = neuronNameSize.cx;
	}

	char test[9] = "-0.00000";
	SIZE valueSize;
	GetTextExtentPoint( listDC, test, 8, &valueSize);
	int lineLength = maxLen + valueSize.cx;

	ReleaseDC(myHWnd, listDC);

	int tabLen[1] = { maxLen / 1.8};
	SendMessage(myHWndInputList, LB_SETTABSTOPS, 1, (LPARAM) (LPINT) tabLen);

	// add listbox entries
	myInputValues = new float[GetLobeSize(myLobe)];
	myNeuronNames = new char *[GetLobeSize(myLobe)];
	char *textString = new char[maxChars+15];
	for(i = 0; i != GetLobeSize(myLobe); i++)
	{
		const char *neuronName = myLobeNames.GetNeuronName(LobeIdInList(myLobe), i);
		myNeuronNames[i] = new char[strlen(neuronName)+2];
		strcpy(myNeuronNames[i], neuronName);

		myInputValues[i] = 0;

		sprintf(textString, "%s%s    %1.4f", myNeuronNames[i], "	", myInputValues[i]);

		SendMessage(myHWndInputList, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)textString);
	}
	delete []textString;

	SendMessage(myHWndInputList, LB_SETCURSEL, 0, myCurrentSel);

	// move window to take account of line length
	MoveWindow(myHWndInputList, border, border+40+textSize.cy, lineLength, (clientRect.bottom-border)-(border+border+40+20+textSize.cy), true);



	// create track bar label
	myHWndTrackbarLabel = CreateWindow("static",
					NULL,
					WS_CHILD | WS_VISIBLE | SS_CENTER,
					border, border, lineLength, textSize.cy,
					myHWnd,
					NULL,
					GetWindowInstance(myHWnd),
					NULL);
	SetWindowFont(myHWndTrackbarLabel, myFont, false);


	// Create a trackbar //////////////////////////////////

	InitCommonControls(); // loads common control's DLL

	myHWndInputTrack = CreateWindow(TRACKBAR_CLASS,
					NULL,
					WS_CHILD | WS_VISIBLE | TBS_TOP | TBS_AUTOTICKS,
					border, border+textSize.cy, lineLength, 40,
					myHWnd,
					(HMENU) IDC_TRACKBAR,
					GetWindowInstance(myHWnd),
					NULL);
	
	SendMessage(myHWndInputTrack, TBM_SETRANGE, (WPARAM) true,(LPARAM) MAKELONG(0, 1000)); 
	SendMessage(myHWndInputTrack, TBM_SETPAGESIZE, 0, (LPARAM) 1);
	SendMessage(myHWndInputTrack, TBM_SETTICFREQ, (WPARAM) 25, (LPARAM)0);
	SetTrackbar(0);

	// close button
	myHWndCloseButton = CreateWindow("button",
					NULL,
					WS_CHILD | WS_VISIBLE,
					border+((lineLength/2)-(((textSize.cx*5)+4)/2)), (clientRect.bottom-border)-(textSize.cy+4), ((textSize.cx*5)+4), (textSize.cy+4),
					myHWnd, 
					(HMENU) ID_BUTTONCLOSE,
					GetWindowInstance(myHWnd),
					NULL);
	SetWindowFont(myHWndCloseButton, myFont, false);
	char text[]="Close";
	SendMessage(myHWndCloseButton, WM_SETTEXT, 0, (LPARAM)text);
	
	//resize window to take account of list size
	RECT windowRect;
	GetWindowRect(myHWnd, &windowRect);
	MoveWindow(myHWnd, windowRect.left, windowRect.top, border+lineLength+border+(GetSystemMetrics(SM_CXDLGFRAME)*2), windowRect.bottom-windowRect.top, true);

	return myHWnd;
}




// Message Handleing ///////////////////

LRESULT LobeInputDlg::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	case WM_VSCROLL:
	case WM_HSCROLL:
		
		On_ScrollTrackbar();
		break;

	default:
		// call default superclass handler
		return(Wnd::WndProc(hwnd, msg, wParam, lParam));	

	}
	return(NULL);
}


void LobeInputDlg::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	switch(id)
	{
	case ID_BUTTONCLOSE:
		ShowWindow(myHWnd, SW_HIDE);
		UpdateWindow(myHWnd);
		SetActiveWindow(GetHwnd(myOwnerWnd));
		break;

	case IDC_LISTBOX:
		myCurrentSel = SendMessage(myHWndInputList, LB_GETCURSEL, 0, 0);
		SetTrackbar(myInputValues[myCurrentSel]);
		break;

	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, NotifyCode, DefWindowProc);
		
	}
}


void LobeInputDlg::Activate()
{
	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);
	SetActiveWindow(myHWnd);
};


void LobeInputDlg::On_ScrollTrackbar()
{
	int i =  SendMessage(myHWndInputTrack, TBM_GETPOS, 0, 0);
	
	if(myCurrentSel < 0) return;

	char *textString = new char[strlen(myNeuronNames[myCurrentSel])+15];
	
	myInputValues[myCurrentSel] = (((float)i-500)/500);

	sprintf(textString, "%s%s    %1.4f", myNeuronNames[myCurrentSel], "	", myInputValues[myCurrentSel]);
	SendMessage(myHWndInputList, LB_DELETESTRING, myCurrentSel, 0);
	SendMessage(myHWndInputList, LB_INSERTSTRING, myCurrentSel, (LPARAM)(LPCTSTR)textString);
	SendMessage(myHWndInputList, LB_SETCURSEL, myCurrentSel, 0);
}


void LobeInputDlg::SetTrackbar(float val)
{
	SendMessage(myHWndTrackbarLabel, WM_SETTEXT, 0, (LPARAM)myNeuronNames[myCurrentSel]); 
	SendMessage(myHWndInputTrack, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)500+(val*500));
}


void LobeInputDlg::DumpToLobe()
{ 
	SetLobeInputs(myLobe, myInputValues);
}


