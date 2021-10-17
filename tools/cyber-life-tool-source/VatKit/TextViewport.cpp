#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include "TextViewport.h"

bool TextViewport::ourIsRegistered = false;

TextViewport::TextViewport(HINSTANCE hInstance, HWND hWndOwner, const char *text) :
Viewport(hInstance, hWndOwner),
myText(text),
myFont(NULL)
{
	LoadString(hInstance, IDR_TEXTVIEWPORT, ourClassName, MAXRESOURCESTRING);

	if(!ourIsRegistered)
	{
		// Register Class
		
		myWCEX.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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
	myWCS.lpClassName = ourClassName;
	myWCS.dwExStyle = WS_EX_CLIENTEDGE;
	myWCS.lpWindowName = NULL;
	myWCS.dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL ;
	myWCS.hWndParent = hWndOwner;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;	
	

	// clear client area vars
	myNoLines = 0;
	myLineLength = 0;
	unsigned int lineLength = 0;
	const char *pos = myText;
	while(strlen(pos) > (lineLength = (strchr(pos, 10)-pos)+1))
	{
		if(lineLength > myLineLength)
			myLineLength = lineLength;

		pos += lineLength;
		myNoLines++;
	}

};


TextViewport::~TextViewport()
{
	if(myFont)
		DeleteObject(myFont);
}


HWND TextViewport::Create(int x, int y, int nWidth, int nHeight)
{	
	myWCS.x = x;
	myWCS.y = y;
	myWCS.nWidth = nWidth;
	myWCS.nHeight = nHeight;


	if(!Viewport::Create())
		return NULL;

	// create font;
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfHeight = 12;
	logfont.lfWeight = 14;
	strcpy(logfont.lfFaceName, "MS Sans Serif");
	myFont = CreateFontIndirect(&logfont);
	SetWindowFont(myHWnd, myFont, false);

	// calculate character size
	HDC hdc;
	
	if(!(hdc = GetDC(myHWnd))) 
		return NULL;
	
	char testChar[2] = "0";
	GetTextExtentPoint(hdc, testChar, 1, &myTextSize);
	ReleaseDC(myHWnd, hdc);

	SetCanvasSpec();
	CalcViewportExt();

	
	// initialze scroll bars
	SetScrollBoxSize();
	SetHScrollPos();
	SetVScrollPos();

	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

	return myHWnd;
}
	

void TextViewport::SetCanvasSpec()
{
	// defaults
	myCanvas.XOrg=0;
	myCanvas.YOrg=0;
	myCanvas.XExt= myLineLength * myTextSize.cx;
	myCanvas.YExt= (myNoLines+2) * myTextSize.cy;
}


void TextViewport::SetScrollBoxSize()
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE;

	// set vertical
	si.nMin = myCanvas.YOrg;
	si.nMax = myCanvas.YOrg+myCanvas.YExt;
	si.nPage = (unsigned int)myViewportYExt+1;
	SetScrollInfo(myHWnd, SB_VERT, &si, true);
	
	// set horizontal
	si.nMin = myCanvas.XOrg;
	si.nMax = myCanvas.XOrg+myCanvas.XExt;
	si.nPage = (unsigned int)myViewportXExt+1;
	SetScrollInfo(myHWnd, SB_HORZ, &si, true);
}


LRESULT TextViewport::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_ERASEBKGND:
		Viewport::WndProc(hwnd, msg, wParam, lParam);
		UpdateWindow(myHWnd);	// force repaint now;
		return(LRESULT)1;

	case WM_PAINT:
		On_Paint();
		break;

	default:
		// call default superclass handler
		return(Viewport::WndProc(hwnd, msg, wParam, lParam));	

	}
	return(NULL);
}


void TextViewport::On_Paint()
{
	HDC  hdc ;
    PAINTSTRUCT ps ;
	hdc = BeginPaint (myHWnd, &ps) ;
	HFONT oldFont = (HFONT)SelectObject(hdc, myFont );

	SetWindowOrgEx(hdc, myWindowXOrg, myWindowYOrg, NULL);
	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetWindowExtEx(hdc, 1, 1, NULL);	
	SetViewportExtEx(hdc, 1, 1, NULL);

	const char *pos = myText;
	unsigned int l;
	for(l = 0; l != myNoLines; l++)
	{
		TextOut(hdc,0, (l*myTextSize.cy)+1, pos, (strchr(pos, 13)-pos));
		pos += (strchr(pos, 10)-pos)+1;
	}
	TextOut(hdc,0, (l*myTextSize.cy)+1, pos, strlen(pos));

	SelectObject(hdc, oldFont);
	EndPaint (myHWnd, &ps) ;
}

