#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>

#include "GraphDlg.h"

bool GraphDlg::ourIsRegistered = false;

GraphDlg::GraphDlg(HINSTANCE hInstance, HWND hWndOwner) : Wnd(),
myTips(NULL)
{
	LoadString(hInstance, IDR_GRAPHDLG, ourClassName, MAXRESOURCESTRING);

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
	myWCS.lpClassName = ourClassName;	
	myWCS.lpWindowName = "Graph Dlg";
	myWCS.dwStyle = WS_CLIPCHILDREN  | WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU;
	myWCS.x = CW_USEDEFAULT;
	myWCS.y = CW_USEDEFAULT;
	myWCS.nWidth = 350;
	myWCS.nHeight = 250;
	myWCS.hWndParent = NULL;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;

	myGraph.OnCreate();
}


HWND GraphDlg::Create()
{
	if(!Wnd::Create())
		return NULL;

	try
	{
		if((myTips = new Tips(GetWindowInstance(myHWnd), myHWnd, 0, 500)))
		{
			if(!myTips->Create())
			{
				delete myTips;
				myTips = NULL;
			}
		}
	}
	catch(Wnd::FailConstructorException){};

	return myHWnd;
}


LRESULT GraphDlg::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		On_Paint();
		break;

	case WM_MOUSEMOVE:
		HANDLE_WM_MOUSEMOVE(hwnd, wParam, lParam, TipsTrace);
		break;

	default:
		// call default superclass handler
		return(Wnd::WndProc(hwnd, msg, wParam, lParam));
	}

	return (LRESULT)1;
}


void GraphDlg::On_Paint() 
{
	RECT rect;
	PAINTSTRUCT ps ;
	HDC dc = BeginPaint (myHWnd, &ps) ;
	GetClientRect(myHWnd, &rect );
	myGraph.DoPaint( dc, rect );
	EndPaint(myHWnd, &ps);
}


void GraphDlg::ChangeSpec(int historySize, int sampleRate, int traceSize, std::string names[], std::string labels[])
{
	myITime = 0;
	static char name[]= "temp name";
	
	myHistorySize = historySize;

	myGraph.SetHSpan( myHistorySize );
	myGraph.SetHUnits( sampleRate );
	myGraph.SetTraceSize(traceSize);

	myTestTrace.resize(traceSize);
	int n = myTestTrace.size();
	for (int i = 0; i < n; ++i)
	{	
		myTestTrace[i].SetName(names[i]);
		myTestTrace[i].Reset(myHistorySize);
		myGraph.SetTrace(i, &myTestTrace[i]);
	}

	// the following must be done after settrace
	myGraph.SetHistorySize( myHistorySize );

	m_bDrawKey = true;

	myGraph.m_vLabel[0] = labels[0];
	myGraph.m_vLabel[1] = labels[1];
	myGraph.m_vLabel[2] = labels[2];
}


void GraphDlg::Reset()
{

	int n = myTestTrace.size();
	for (int i = 0; i < n; ++i)
	{	
		myTestTrace[i].Reset(myHistorySize);
	}
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
}


void GraphDlg::PostSample()
{
	if( myITime > myGraph.GetHSpan() )
		myGraph.SetTime( myITime );
	else
		myGraph.SetTime( myGraph.GetHSpan() );
	RECT rect;
	GetClientRect(myHWnd, &rect );
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
	SendMessage(myHWnd, WM_PAINT, 0, 0);

	// update tips
	POINT pos; 
	GetCursorPos(&pos);
	ScreenToClient(myHWnd, &pos);
	TipsTrace(myHWnd, pos.x, pos.y, 0);

}



void GraphDlg::TipsTrace(HWND hwnd, int x, int y, UINT keyflags)
{
	const int within = 16; // number of pixels to be near trace by

	POINT tipPos = {x, y};
	POINT snap;
	float value;
	std::vector<int>varMatches = myGraph.HitTestTraces( tipPos, snap, within, value);
	int noVarMatches = varMatches.size();
	if( noVarMatches != 0 )
	{
		std::string tip;
		char sValue[5];
		sprintf(sValue, " = %.3f", (value*2)-1);
	
		for(int i = 0; i != noVarMatches; i++)
		{
			std::string name;
			myTestTrace[varMatches[i]].GetName(name);
			tip = tip + name + (noVarMatches > (i+1) ? ", " : sValue);
		}

		if(myTips) myTips->PopUp(tip.begin(), tipPos, true, false);

	}
	else
		if(myTips) myTips->GoAway();
}



