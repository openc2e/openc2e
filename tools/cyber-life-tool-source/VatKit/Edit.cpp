#include "Edit.h"

HWND Edit::Create(int x, int y, int nWidth, int nHeight)
{
	myWCS.dwStyle = WS_CHILD | WS_VSCROLL | ES_READONLY | ES_MULTILINE;
	myWCS.dwExStyle = WS_EX_CLIENTEDGE;

	myWCS.x = x;
	myWCS.y = y;
	myWCS.nWidth = nWidth;
	myWCS.nHeight = nHeight;

	myWCS.lpClassName = "edit";

	
	if(!Wnd::Create())
		return NULL;

	// create font;
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfHeight = 12;
	logfont.lfWeight = 14;
	strcpy(logfont.lfFaceName, "MS Sans Serif");
	myFont = CreateFontIndirect(&logfont);

	SetWindowFont(myHWnd, myFont, false);
	SetWindowText( myHWnd, myText.c_str() );

	return myHWnd;
}

Edit::~Edit()
{
	if(myFont)
		DeleteObject(myFont);
}

