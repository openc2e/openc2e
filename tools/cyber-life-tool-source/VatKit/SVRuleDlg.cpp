#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "Edit.h"

#include "SVRuleDlg.h"
#include "BrainDlgContainer.h"
#include "../../common/GameInterface.h"
#include "BrainViewport.h"

extern GameInterface theGameInterface;

#define VALUE 11 
#define NEGATIVEVALUE 12
#define FLOATTOINT 248
#define STOPCODE 1
#define RETURNCODE 0
#define FIRSTIF 4
#define LASTIF 15

bool SVRuleDlg::ourIsRegistered = false;

SVRuleDlg::SVRuleDlg(HINSTANCE hInstance, HWND hWndOwner, const SVCaptions::Caption * const captions, LobeNames &lobeNames, BrainDlgContainer &container, SVRule &svUpdateRule, SVRule &svInitRule, bool useInit) : Wnd(),
	myNoOfLines(0),
	myLobeNames(lobeNames),
	myContainer(container),
	myCaptions(captions),
	myInitSVRule(svInitRule),
	myUpdateSVRule(svUpdateRule),
	myUseInit(useInit),
	myFont(NULL),
	myGeneralNotesWnd(NULL)
{
	LoadString(hInstance, IDR_SVRULEDLG, ourClassName, MAXRESOURCESTRING);

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
	myWCS.lpWindowName = "SV Rule Dlg";
	myWCS.dwStyle = WS_CLIPCHILDREN | WS_MAXIMIZEBOX | 
					WS_DLGFRAME | WS_OVERLAPPED | WS_SYSMENU;
	myWCS.x = CW_USEDEFAULT;
	myWCS.y = CW_USEDEFAULT;
	myWCS.nWidth = CW_USEDEFAULT;
	myWCS.nHeight = CW_USEDEFAULT;
	myWCS.hWndParent = NULL;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;
}


SVRuleDlg::~SVRuleDlg()
{	
	myContainer.RemoveDlg(this);

	if(myGeneralNotesWnd)
		delete myGeneralNotesWnd;
	if(myFont)
		DeleteObject(myFont);
}


HWND SVRuleDlg::Create()
{

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
	
	// calculate character size
	SIZE textSize;
	HDC hdc;
	
	if(!(hdc = GetDC(myHWnd))) 
		return false;
	
	char testChar[2] = "0";
	GetTextExtentPoint(hdc, testChar, 1, &textSize);
	ReleaseDC(myHWnd, hdc);

	mySVLineHeight = textSize.cy;
	mySVDlgSpacer = 2;
	mySVLineSpacer = 3;
	mySVDescHeight = textSize.cy;
	myGeneralNoteHeight = 70;
	mySVTrackbarWidth = 100;
	mySVCommentWidth = 120;
	mySVValueWidth = 38;
	mySVWidth = (mySVDlgSpacer*3)+mySVCommentWidth+mySVTrackbarWidth+mySVValueWidth;
	mySVTop = mySVDescHeight+mySVDlgSpacer+myGeneralNoteHeight+mySVDlgSpacer;

	const char cr = 13;
	const char lf = 10;
	std::string notes = (myUseInit ? myCaptions->notes[0]+ cr + lf + cr + lf : "");
	notes += myCaptions->notes[1];
	if((myGeneralNotesWnd = new Edit(GetWindowInstance(myHWnd), myHWnd, notes.c_str())))
		if((myHWndGeneralNotes = myGeneralNotesWnd->Create(mySVDlgSpacer, mySVDescHeight+mySVDlgSpacer, mySVWidth, myGeneralNoteHeight)))
			ShowWindow(myHWndGeneralNotes, SW_SHOW);
		else
			return NULL;

	InitCommonControls(); // loads common control's DLL


	struct SVRuleEntry sVRuleEntry[totalNoRules];

	for(int i = 0; i != totalNoRules; i++)
	{

		sVRuleEntry[i] = GetSVRuleEntry(GetInitOrUpdateRule(i), GetInitOrUpdateLine(i));

		if(!myUseInit && i < SVRule::length)
			continue;	// not displaying init rule

		if(sVRuleEntry[i].opCode == STOPCODE || sVRuleEntry[i].opCode == RETURNCODE) 
			continue;

		if(sVRuleEntry[i].operandVariable == VALUE || sVRuleEntry[i].operandVariable == NEGATIVEVALUE)
			CreateLine(i, sVRuleEntry[i], myCaptions->comment[GetSVCaptionNo(i)][GetInitOrUpdateLine(i)]);

	}

	RECT windowRect;
	GetWindowRect(myHWnd, &windowRect);

	myHWndDescription = CreateWindow("static",
					NULL,
					WS_CHILD | WS_VISIBLE | SS_LEFT,
					mySVDlgSpacer, 0, mySVWidth, mySVDescHeight,
					myHWnd,
					NULL,
					GetWindowInstance(myHWnd),
					NULL);
	SetWindowFont(myHWndDescription, myFont, false);

	
	MoveWindow(myHWnd, windowRect.left, windowRect.top, 
			(GetSystemMetrics(SM_CXDLGFRAME)*2)+mySVWidth+(mySVDlgSpacer*2), 
			GetSystemMetrics(SM_CYCAPTION)+(GetSystemMetrics(SM_CYDLGFRAME)*2)
			+mySVTop+((mySVLineHeight+mySVLineSpacer)*myNoOfLines), false);


	return myHWnd;
}


void SVRuleDlg::CreateLine(const int i, const struct SVRuleEntry& sVRuleEntry, const std::string comment)
{
	// create trackbar
	myHWndSVTrack[myNoOfLines] = CreateWindow(TRACKBAR_CLASS,
			NULL,
			WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_BOTH | TBS_NOTICKS,
			mySVDlgSpacer, mySVTop+((mySVLineHeight+mySVLineSpacer)*myNoOfLines), 
			mySVTrackbarWidth, mySVLineHeight,
			myHWnd,
			(HMENU) myNoOfLines,
			GetWindowInstance(myHWnd),
			NULL);
	
	SendMessage(myHWndSVTrack[myNoOfLines], TBM_SETRANGE, (WPARAM) true,(LPARAM) MAKELONG(0, 1000)); 
	SendMessage(myHWndSVTrack[myNoOfLines], TBM_SETPAGESIZE, 0, (LPARAM) 1);
	SendMessage(myHWndSVTrack[myNoOfLines], TBM_SETPOS, (WPARAM) TRUE, (sVRuleEntry.floatValue*1000)+0.5);

	// create track bar value
	myHWndSVValue[myNoOfLines] = CreateWindow("static",
			NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SUNKEN,
			(mySVDlgSpacer*2)+mySVTrackbarWidth, 
			mySVTop+((mySVLineHeight+mySVLineSpacer)*myNoOfLines),
			mySVValueWidth, mySVLineHeight,
			myHWnd,
			NULL,
			GetWindowInstance(myHWnd),
			NULL);
	SetWindowFont(myHWndSVValue[myNoOfLines], myFont, false);

	if(sVRuleEntry.operandVariable == VALUE)
		mySVSign[myNoOfLines] = posative;
	else
		mySVSign[myNoOfLines] = negative;


	char buffer[5];
	sprintf(buffer, "%s%1.3f", (mySVSign[myNoOfLines] == posative ? "+" : "-"), sVRuleEntry.floatValue);
	SendMessage(myHWndSVValue[myNoOfLines], WM_SETTEXT, 0, (LPARAM)buffer);

	// create sv note
	myHWndSVComment[myNoOfLines] = CreateWindow("static",
			NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER | SS_SUNKEN,
			(mySVDlgSpacer*3)+mySVTrackbarWidth+mySVValueWidth, 
			mySVTop+((mySVLineHeight+mySVLineSpacer)*myNoOfLines),
			mySVCommentWidth, mySVLineHeight,
			myHWnd,
			NULL,
			GetWindowInstance(myHWnd),
			NULL);
	SetWindowFont(myHWndSVComment[myNoOfLines], myFont, false);

	SendMessage(myHWndSVComment[myNoOfLines], WM_SETTEXT, 0, (LPARAM)comment.c_str());


	mySVRuleEntryNo[myNoOfLines] = i; 

	myNoOfLines++;
}


// Message Handleing ///////////////////

LRESULT SVRuleDlg::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
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


void SVRuleDlg::On_Scroll(HWND hWnd)
{
	float f; 
	int l=0;
	char buffer[5];
	while(myHWndSVTrack[l] != hWnd && l < myNoOfLines) l++;
	
	f = ((float)SendMessage(myHWndSVTrack[l], TBM_GETPOS, 0, 0))/1000;	
								
	SetSVRuleEntry(GetInitOrUpdateRule(mySVRuleEntryNo[l]), GetInitOrUpdateLine(mySVRuleEntryNo[l]), f);

	SetOnlineValue(mySVRuleEntryNo[l], f);
	
	sprintf(buffer, "%s%1.3f", (mySVSign[l] == posative ? "+" : "-"), f);
	SendMessage(myHWndSVValue[l], WM_SETTEXT, 0, (LPARAM)buffer);
}


HWND LobeDlg::Create()
{
	
	if(!SVRuleDlg::Create())
		return NULL;

	char caption[100];
	sprintf(caption,"(L:%s) Lobe: %s", myLobeNames.GetLobeFullName(LobeIdInList(myLobe)), myLobeNames.GetLobeFullName(LobeIdInList(myLobe)));
	SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)caption);

	char description[100];
	sprintf(description,"Lobe: %s ", myLobeNames.GetLobeFullName(LobeIdInList(myLobe)));
	SendMessage(myHWndDescription, WM_SETTEXT, 0, (LPARAM)description);

	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);

	return myHWnd;
}


void LobeDlg::SetOnlineValue(int line, float value)
{	
	if(theGameInterface.Connected())
	{
		char macro[50];
		sprintf(macro, "execute\nTARG AGNT %d BRN: SETL %d %d %f", theBrainViewport->GetOnlineId(), LobeIdInList(myLobe), line, value);
		theGameInterface.Inject(macro, NULL);
	}
}


HWND TractDlg::Create()
{
	
	if(!SVRuleDlg::Create())
		return NULL;

	char caption[100];
	sprintf(caption,"(T:%s) Tract: %s", GetTractName(myTract), GetTractName(myTract));
	SendMessage(myHWnd, WM_SETTEXT, 0, (LPARAM)caption);

	char description[100];
	sprintf(description, "Tract: %s->%s", myLobeNames.GetLobeFullName(LobeIdInList(GetSrcLobe(myTract))), myLobeNames.GetLobeFullName(LobeIdInList(GetDstLobe(myTract))));
	SendMessage(myHWndDescription, WM_SETTEXT, 0, (LPARAM)description);

	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);

	return myHWnd;
}

void TractDlg::SetOnlineValue(int line, float value)
{	
	if(theGameInterface.Connected())
	{
		char macro[50];
		sprintf(macro, "execute\nTARG AGNT %d BRN: SETT %d %d %f", theBrainViewport->GetOnlineId(), TractIdInList(myTract), line, value);
		theGameInterface.Inject(macro, NULL);
	}
}

