#include "Resource.h"
#include <windows.h>
#include <windowsx.h>

#include "Viewport.h"

#include "Vat.h"		//general project definitions

bool Viewport::ourIsRegistered = false;

Viewport::Viewport(HINSTANCE hInstance, HWND hWndOwner) : Wnd()
{
	// initialise window instance (fails class creation if cant)
		// if static ourClassName is empty the class cant have been ourIsRegistered yet
	
	LoadString(hInstance, IDR_VIEWPORT, ourClassName, MAXRESOURCESTRING);
	
	if(!ourIsRegistered)
	{

		// Register Class
		myWCEX.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		//myWCEX.lpfnWndProc =	// dont fill this in will be set by RegisterWndClass 
		myWCEX.cbClsExtra = 0;
		myWCEX.cbWndExtra = 0;
		myWCEX.hInstance = hInstance;
		myWCEX.hIcon = NULL;
		myWCEX.hCursor = LoadCursor(NULL, IDC_ARROW);
		myWCEX.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		myWCEX.lpszMenuName = NULL;	
		myWCEX.lpszClassName = ourClassName;

		RegisterWndClass();

		ourIsRegistered = true;
	}		
		// Create Window
	RECT parentClientRect;

	GetClientRect(hWndOwner, &parentClientRect);
	myWCS.lpClassName = ourClassName;
	myWCS.lpWindowName = NULL;
	myWCS.dwStyle = WS_CHILD | WS_VSCROLL | WS_HSCROLL;
	myWCS.x = parentClientRect.left;
	myWCS.y = parentClientRect.top;
	myWCS.nWidth = parentClientRect.right - parentClientRect.left;
	myWCS.nHeight = parentClientRect.bottom - parentClientRect.top;
	myWCS.hWndParent = hWndOwner;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;
	

	// clear vars

	myErase = RDW_ERASE;

	myCanvas.XOrg=0;
	myCanvas.YOrg=0;
	myCanvas.XExt=1;
	myCanvas.YExt=1;
	myViewportXExt=1;
	myViewportYExt=1;
	myBorder = 0;

	myScaleX = 1;
	myScaleY = 1;
	myZoom = 1;

	myWindowXOrg = 0;
	myWindowYOrg = 0;



};


HWND Viewport::Create()
{
	if(!Wnd::Create())
		return NULL;

	InitViewport(true);

	return myHWnd;
}


void Viewport::InitViewport(bool reset)
{
	// initialise client area

	// use floats for repositioning properly - do not use int scaling functions
	float oldXOrg = ((float)myWindowXOrg)/myScaleX;	
	float oldYOrg = ((float)myWindowYOrg)/myScaleY;

	SetCanvasSpec();
	CalcViewportExt();
	CalcScale();

	if(reset)
	{
		myZoom = 1;
		myWindowXOrg = (int)ScaleX((float)myCanvas.XOrg);
		myWindowYOrg = (int)ScaleY((float)myCanvas.YOrg);
	}
	else
	{
		if(oldXOrg > myCanvas.XOrg && myZoom != 1)
			myWindowXOrg = Round(oldXOrg*myScaleX); // keep position : scale may have changed
		else
			myWindowXOrg = (int)ScaleX((float)myCanvas.XOrg);
		
		if(oldYOrg > myCanvas.YOrg && myZoom != 1)		
			myWindowYOrg = Round(oldYOrg*myScaleY);	// keep position : scale may have changed
		else
			myWindowYOrg = (int)ScaleY((float)myCanvas.YOrg);
		
	}

	// initialze scroll bars
	SetScrollBoxSize();
	SetHScrollPos();
	SetVScrollPos();
}	

void Viewport::CalcViewportExt()
{
	RECT clientRect;
	GetClientRect(myHWnd, &clientRect);
	myViewportYExt = (clientRect.bottom - clientRect.top);
	myViewportXExt = clientRect.right - clientRect.left;
}


void Viewport::SetCanvasSpec()
{
	// defaults
	myCanvas.XOrg = 0;
	myCanvas.YOrg = 0;
	myCanvas.XExt = 1;
	myCanvas.YExt = 1;
	myBorder = 0;
}


void Viewport::CalcScale()
{
	myScaleX = (float)(myViewportXExt-(myBorder*2))/myCanvas.XExt;
	myScaleY = (float)(myViewportYExt-(myBorder*2))/myCanvas.YExt;
}



// Scroll routines ///////////////////////////////////

void Viewport::SetScrollBoxSize()
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;

	// set vertical
	si.nMin = (int)ScaleY((float)myCanvas.YOrg);
	si.nMax = (int)ScaleY((float)myCanvas.YOrg+myCanvas.YExt);
	si.nPage = (unsigned int)Round((ScaleY((float)myCanvas.YExt)+1) / myZoom);
	SetScrollInfo(myHWnd, SB_VERT, &si, true);
	
	// set horizontal
	si.nMin = (int)ScaleX((float)myCanvas.XOrg);
	si.nMax = (int)ScaleX((float)myCanvas.XOrg+myCanvas.XExt);
	si.nPage = (unsigned int)Round(((float)ScaleX((float)myCanvas.XExt)+1) / myZoom);
	SetScrollInfo(myHWnd, SB_HORZ, &si, true);
}


void Viewport::SetVScrollPos()
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);

	// set vertical
	si.fMask = SIF_POS;
	si.nPos = myWindowYOrg;
	SetScrollInfo(myHWnd, SB_VERT, &si, true);	
}


void Viewport::SetHScrollPos()
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);

	// set horizontal
	si.fMask = SIF_POS;
	si.nPos = myWindowXOrg;
	SetScrollInfo(myHWnd, SB_HORZ, &si, true);
}


// Message Handleing ///////////////////

LRESULT Viewport::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	case WM_SIZE:
		HANDLE_WM_SIZE(hwnd, wParam, lParam, On_Size);
		break;

	case WM_VSCROLL:
		HANDLE_WM_VSCROLL(hwnd, wParam, lParam, On_VScroll);
		break;

	case WM_HSCROLL:
		HANDLE_WM_HSCROLL(hwnd, wParam, lParam, On_HScroll);
		break;

	default:
		// call default superclass handler
		return(Wnd::WndProc(hwnd, msg, wParam, lParam));	
 	}
	return(NULL);
}


void Viewport::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	switch(id)
	{
	case ID_BUTTONZOOMIN:
		if(NotifyCode == 0)
		// click
		On_ZoomIn();
		break;
	case ID_BUTTONZOOMOUT:
		if(NotifyCode == 0)
			// click
			On_ZoomOut();
			break;
	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, NotifyCode, DefWindowProc);

	}
}



void Viewport::On_Size(HWND  hwnd, UINT sizeState, int cx, int cy)
{
	myViewportXExt = cx;
	myViewportYExt = cy;
	CalcScale();
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}


void Viewport::On_VScroll(HWND hwnd, HWND hCtl, UINT code, int pos)
{

	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	GetScrollInfo(hwnd, SB_VERT, &si);

	switch(code)
	{
	case SB_LINEUP:
		if(myWindowYOrg > si.nMin)
			myWindowYOrg--;
		break;
	case SB_LINEDOWN:
		if(myWindowYOrg < si.nMax - ((int)si.nPage-1))
			myWindowYOrg++;
		break;
	case SB_PAGEUP:
		myWindowYOrg -= si.nPage;
		if(myWindowYOrg < si.nMin)
			myWindowYOrg = si.nMin;
		break;
	case SB_PAGEDOWN:
		myWindowYOrg += si.nPage;
		if(myWindowYOrg > si.nMax - ((int)si.nPage-1))
			myWindowYOrg = si.nMax - ((int)si.nPage-1);
		break;
	case SB_THUMBPOSITION:
		myWindowYOrg = pos;
		if(myWindowYOrg < si.nMin)
			myWindowYOrg = si.nMin;
		else if(myWindowYOrg > si.nMax - ((int)si.nPage-1))
			myWindowYOrg = si.nMax - ((int)si.nPage-1);
		break;
	}

	SetVScrollPos();
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |myErase);
}


void Viewport::On_HScroll(HWND hwnd, HWND hCtl, UINT code, int pos)
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;
	GetScrollInfo(hwnd, SB_HORZ, &si);

	switch(code)
	{
	case SB_LINELEFT:
		if(myWindowXOrg > si.nMin)
			myWindowXOrg--;
		break;
	case SB_LINERIGHT:
		if(myWindowXOrg < si.nMax - ((int)si.nPage-1))
			myWindowXOrg++;
		break;
	case SB_PAGELEFT:
		myWindowXOrg -= si.nPage;
		if(myWindowXOrg < si.nMin)
			myWindowXOrg = si.nMin;
		break;
	case SB_PAGERIGHT:
		myWindowXOrg += si.nPage;
		if(myWindowXOrg > si.nMax - ((int)si.nPage-1))
			myWindowXOrg = si.nMax - ((int)si.nPage-1);
		break;
	case SB_THUMBPOSITION:
		myWindowXOrg = pos;
		if(myWindowXOrg < si.nMin)
			myWindowXOrg = si.nMin;
		else if(myWindowXOrg > si.nMax - ((int)si.nPage-1))
			myWindowXOrg = si.nMax - ((int)si.nPage-1);
		break;
	}
	
	SetHScrollPos();
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |myErase);
}


void Viewport::On_ZoomIn()
{
	SCROLLINFO si;
	int yPage, xPage;
	
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;

	// get vertical page
	GetScrollInfo(myHWnd, SB_VERT, &si);
	yPage = si.nPage;
	
	// get horizontal
	GetScrollInfo(myHWnd, SB_HORZ, &si);
	xPage = si.nPage;

	// check if already myZoomed in to 1 logical coordinate
	if(yPage > myScaleY && xPage > myScaleX)
	{
		myZoom *= 2;
		SetScrollBoxSize();
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |myErase);
	}
}


void Viewport::On_ZoomOut()
{
	if(myZoom != 1)
	{
		myZoom /= 2;
		SetScrollBoxSize();
		
		SCROLLINFO si;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;

		GetScrollInfo(myHWnd, SB_VERT, &si);
		if(myWindowYOrg > si.nMax - ((int)si.nPage-1))
			myWindowYOrg = si.nMax - ((int)si.nPage-1);
		
		GetScrollInfo(myHWnd, SB_HORZ, &si);
		if(myWindowXOrg > si.nMax - ((int)si.nPage-1))
			myWindowXOrg = si.nMax - ((int)si.nPage-1);
		
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |myErase);
	};
}



