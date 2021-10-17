#include "windowsx.h"
#include "Tips.h"



bool Tips::ourIsRegistered = false;



Tips::Tips(HINSTANCE hInstance, HWND hWndOwner) : Wnd(),
 myTimerID(0), myShowDelay(100), myHideDelay(500), myVisible(false), myFont(NULL)
{
	 DefineWindow(hInstance, hWndOwner);
}



Tips::Tips(HINSTANCE hInstance, HWND hWndOwner, UINT showDelay, UINT hideDelay) : Wnd(),
	myTimerID(0), myShowDelay(showDelay), myHideDelay(hideDelay), myVisible(false), myFont(NULL)
{
	DefineWindow(hInstance, hWndOwner);
}



void Tips::DefineWindow(HINSTANCE hInstance, HWND hWndOwner)
{
	LoadString(hInstance, IDR_TIPS, ourClassName, MAXRESOURCESTRING);

	if(!ourIsRegistered)
	{
		// Register Class
		myWCEX.style = CS_HREDRAW | CS_VREDRAW;
		myWCEX.cbClsExtra = 0;
		myWCEX.cbWndExtra = 0;
		myWCEX.hInstance = hInstance;
		myWCEX.hIcon = NULL;
		myWCEX.hCursor = LoadCursor(NULL, IDC_ARROW);
		myWCEX.hbrBackground = GetSysColorBrush(COLOR_INFOBK);
		myWCEX.lpszMenuName = NULL;	
		myWCEX.lpszClassName = ourClassName;

		RegisterWndClass();

		ourIsRegistered = true;
	}

	// Create Window
	myWCS.dwExStyle = WS_EX_TOPMOST;
	myWCS.lpClassName = ourClassName;	
	myWCS.lpWindowName = NULL;
	myWCS.dwStyle = WS_POPUP | WS_BORDER | WS_DISABLED ;
	myWCS.x = 0;
	myWCS.y = 0;
	myWCS.nWidth = 10;
	myWCS.nHeight = 10;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;
	myWCS.hWndParent = myOwnerHWnd = hWndOwner;

	// create font
	LOGFONT logfont;

	memset(&logfont, 0, sizeof(logfont));
    
	logfont.lfHeight = 10;
	logfont.lfWeight = 9;
	strcpy(logfont.lfFaceName, "MS Sans Serif");

	if( !(myFont = CreateFontIndirect(&logfont)) )
	{
		throw Wnd::FailConstructorException();
	}
}



HWND Tips::Create()
{
	if(!Wnd::Create())
		return NULL;

	return myHWnd;
}



Tips::~Tips()
{
	if(myFont) DeleteObject(myFont);
}



void Tips::PopUp( LPCTSTR label, POINT pos, bool adjustToCursor, bool sticky, bool frameColour, COLORREF colFrame)
{
	if( myOwnerHWnd )
		ClientToScreen(myOwnerHWnd, &pos);

	if(myPos.x != pos.x || myPos.y != pos.y || myLabel != label)
	{
		GoAway();

		myLabel = label;
		myPos = pos;
		myAdjustToCursor = adjustToCursor;
		mySticky = sticky;
		myFrameColour = frameColour;
		myColFrame = colFrame;
	

		// don't pop up immediately
		if (myShowDelay == 0)
		{
			// bypass message queue if delay is zero as
			// timer messages are dodgy as things get slow
			myTimerID = 424;
			FORWARD_WM_TIMER(myHWnd, myTimerID, SendMessage);
		}
		else
			myTimerID = SetTimer(myHWnd, 854, myShowDelay, NULL );

	}
}


void Tips::GoAway()
{
	if( myTimerID )
		KillTimer( myHWnd, myTimerID );

	ShowWindow( myHWnd, SW_HIDE );

	myVisible = false;
	myLabel = "";
}



LRESULT Tips::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_TIMER:
		HANDLE_WM_TIMER(hwnd, wParam, lParam, On_Timer);
		break;

	case WM_PAINT:
		On_Paint();
		break;

	case WM_MOUSEMOVE:
		if( !mySticky )
			GoAway();
		break;

	default:
		// call default superclass handler
		return(Wnd::WndProc(hwnd, msg, wParam, lParam));
	}

	return (LRESULT)1;
}




void Tips::On_Paint() 
{
	PAINTSTRUCT ps ;
	HDC dc = BeginPaint (myHWnd, &ps) ;

	HFONT oldFont = (HFONT) SelectObject( dc, myFont );

	
	if(myFrameColour)
	{
		HPEN framePen = CreatePen(PS_SOLID, 1, myColFrame);
		HPEN oldPen = (HPEN) SelectObject(dc, framePen);
		RECT clientRect;
		GetClientRect(myHWnd, &clientRect);
		EmptyRectangle(dc, 0, 0, clientRect.right-clientRect.left, clientRect.bottom-clientRect.top);
		SelectObject(dc, oldPen);
		DeletePen(framePen);
	}

	SetBkMode(dc, TRANSPARENT );
	SetTextColor(dc, GetSysColor(COLOR_INFOTEXT));



	SIZE textSize;
	GetTextExtentPoint( dc, myLabel.begin(), myLabel.length(), &textSize);

	RECT ownerRect;
	GetClientRect(GetDesktopWindow(), &ownerRect);
	int ownerWidth = ownerRect.right - ownerRect.left;

	int tipWidth = (int)textSize.cx + (border*2) + (GetSystemMetrics(SM_CXBORDER)*2);

	if(ownerWidth > tipWidth)
	{
		// fits on 1 line
		RECT textRect = {border, border, (int)textSize.cx+border, (int)textSize.cy+border};
		DrawText(dc, myLabel.begin(), myLabel.length(), &textRect, 0 );
	}
	else
	{
		// adjust if to wide

		// calc no lines needed
		int maxWidth = ownerWidth - ((border*2) + (GetSystemMetrics(SM_CXBORDER)*2));
	
		RECT clientRect;
		GetClientRect(myHWnd, &clientRect);
		int noLines = (ownerRect.right - ownerRect.left) / textSize.cy;
		textSize.cx = maxWidth;

		// calc average no chars on line 
		char testChar[2] = "0";
		SIZE charSize;
		GetTextExtentPoint(dc, testChar, 1, &charSize);
		int aveCharsOnLine = textSize.cx/charSize.cx;

		char *start = myLabel.begin();
		for(int l = 0; l != noLines; l++)
		{
			char *end = GetNextLine(start, myLabel.end(), aveCharsOnLine);

			RECT textRect = {0+border, (int)(l*textSize.cy)+border, (int)textSize.cx+border, (int)(l*textSize.cy)+border+textSize.cy};
			DrawText(dc, start, (int)(end - start), &textRect, 0 );
			start = end;
		}
	} 
	
	SelectObject(dc, oldFont );
	EndPaint(myHWnd, &ps);
}



void Tips::On_Timer(HWND hwnd, UINT nIDEvent) 
{

	if( myTimerID == nIDEvent )	// dont pick up old messages
	{
		KillTimer( myHWnd, myTimerID );
		myTimerID = NULL;

			
		HDC dc = GetDC (myHWnd) ;

		if(dc)
		{
			static std::string lastLabel;

			if (mySticky && myVisible)
			{
				// disappear after time
				GoAway();
			}
			else if (lastLabel != myLabel || !myVisible)
			{
				
				RECT ownerRect;
				GetClientRect(GetDesktopWindow(), &ownerRect);
				int ownerHeight = (ownerRect.bottom - ownerRect.top);
				int ownerWidth = ownerRect.right - ownerRect.left;

				HFONT oldFont = (HFONT) SelectObject( dc, myFont );
				SIZE textSize;
				GetTextExtentPoint( dc, myLabel.begin(), myLabel.length(), &textSize);
				SelectObject(dc, oldFont );

				int tipWidth = (int)textSize.cx + (border*2) + (GetSystemMetrics(SM_CXBORDER)*2);
				int tipHeight = (int)textSize.cy + (border*2) + (GetSystemMetrics(SM_CYBORDER)*2);
				
				POINT windowPos;
				windowPos.x = myPos.x + (myAdjustToCursor ? SM_CXCURSOR : 0);
				windowPos.y = myPos.y + (myAdjustToCursor ? SM_CYCURSOR : 0);
				
	
				// adjust x if goes off edge of screen
				if(tipWidth > ownerWidth - windowPos.x)
					windowPos.x = ownerWidth - tipWidth;
					
				// adjust if to wide
				if(windowPos.x < 0)
				{	
					HFONT oldFont = (HFONT) SelectObject( dc, myFont );
					char testChar[2] = "0";
					SIZE charSize;
					GetTextExtentPoint(dc, testChar, 1, &charSize);
					SelectObject(dc, oldFont );

					int maxTextWidth = ownerWidth - ((border*2) + (GetSystemMetrics(SM_CXBORDER)*2));
					int aveCharsOnLine = maxTextWidth/charSize.cx;

					int noLines = 0;
					char *start = myLabel.begin();
					do 
					{
						start = GetNextLine(start, myLabel.end(), aveCharsOnLine);
						noLines++;
					} while(start != myLabel.end());

					tipHeight = (textSize.cy * noLines) +
								(border*2) + (GetSystemMetrics(SM_CYBORDER)*2);
			
					tipWidth = ownerWidth;
					windowPos.x = 0;	// position by left of window
				}
		

				// fail if to big for parent window
				if(tipHeight > ownerHeight)
					return;
				
				// adjust y if goes off edge of screen
				if(tipHeight > ownerHeight - windowPos.y)
					windowPos.y = myPos.y - tipHeight;		// try to place above cursor
				
				if(windowPos.y < 0)
					windowPos.y = ownerHeight-tipHeight;	// try to place near cursor
				

				MoveWindow(myHWnd, windowPos.x, windowPos.y, tipWidth, tipHeight, myVisible);
				
				ShowWindow( myHWnd, SW_SHOW );
				RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				myVisible = true;
				
			
				if (mySticky)
					myTimerID = SetTimer( myHWnd, 854, myHideDelay, NULL );

				lastLabel = myLabel;
		
			}


		}
		ReleaseDC(myHWnd, dc);
		
	}
}



// -------------------------------------------------------------------------
// Method:		EmptyRectangle
// Arguments:	device contect, coords of rectangle
// Returns:		none
// Description:	Prints an unfilled rectangle, used for printing lobes
// -------------------------------------------------------------------------
void Tips::EmptyRectangle(HDC hdc, int x1, int y1, int x2, int y2)
{
	// print an unfilled rectangle
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x1, y2);
	LineTo(hdc, x2, y2);
	LineTo(hdc, x2, y1);
	LineTo(hdc, x1, y1);
}

