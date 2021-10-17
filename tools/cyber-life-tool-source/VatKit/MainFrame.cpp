// ----------------------------------------------------------------------------
// Filename:	MainFrame.cpp
// Class:		MainFrame
// Purpose:		Main Application Window
// Description:	Main Vat application window. Has file open option of which the 
//				returned genome filename is used to creates a viewport window.
//				Controls brain play thread, and closes viewportwindows cleanly
//	Usage:		May be called from winmain and only requires the application
//				instance. InitInstance must be called after the constructor
//				and after the window has been created
// -----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>	// must add comctl32.lib to settings
#include <algorithm>
#include "../../common/GameInterface.h"
#include "../../common/WhichEngine.h"
#include "MainFrame.h"
#include "BrainViewport.h"
#include "Vat.h"		//general project definitions

extern GameInterface theGameInterface;
extern WhichEngine theWhichEngine;

#define THREADMUTEXNAME "Vat.Mutex.PlayBrain"

bool MainFrame::ourIsRegistered = false;

// -------------------------------------------------------------------------
// Method:		MainFrame
// Arguments:	hInstance - application instance                       
//				hWndOwner - handle to owner window
// Returns:		none
// Description:	Constructor - uses hInstance and hWndOwner to register the 
//				and create the Window
// -------------------------------------------------------------------------
MainFrame::MainFrame(const HINSTANCE hInstance, const HWND hWndOwner) : Wnd(),
	myPlayState(false), myLoopState(false), myInstinctState(false),
	myHWndToolbar(NULL), myHWndStatus(NULL),
	myBrainViewportWnd(NULL), myHWndBrainViewport(NULL)
{
	LoadString(hInstance, IDR_MAINFRAME, ourClassName, MAXRESOURCESTRING);

	if(!ourIsRegistered)
	{
		// Register Class
		myWCEX.style = CS_HREDRAW | CS_VREDRAW;
		myWCEX.cbClsExtra = 0;
		myWCEX.cbWndExtra = 0;
		myWCEX.hInstance = hInstance;
		myWCEX.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
		myWCEX.hCursor = LoadCursor (NULL, IDC_ARROW);
		myWCEX.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
		myWCEX.lpszMenuName = NULL;	
		myWCEX.lpszClassName = ourClassName;

		RegisterWndClass();
		
		ourIsRegistered = true;
	}
	
	// Create Window
	myWCS.lpClassName = ourClassName;
	myWCS.lpWindowName = "CyberLife Vat Kit for Creatures 3 v1.8";
	myWCS.dwStyle = WS_CLIPCHILDREN | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
	myWCS.x = CW_USEDEFAULT;
	myWCS.y = CW_USEDEFAULT;
	myWCS.nWidth = CW_USEDEFAULT;
	myWCS.nHeight = CW_USEDEFAULT;
	myWCS.hWndParent = hWndOwner;
	myWCS.hMenu = NULL;
	myWCS.hInstance = hInstance;
	myWCS.lpParam = NULL;	
};


// -------------------------------------------------------------------------
// Method:		~MainFrame
// Arguments:	none
// Returns:		none
// Description:	Destructor
// -------------------------------------------------------------------------
MainFrame::~MainFrame()
{
	// children will be automatically destroyed
	if(theGameInterface.Connected())
		theGameInterface.Inject("execute\nDBG: PLAY", NULL);

	// menus may have already been destroyed so check before destroying
	if(IsMenu(myCreatureMenu)) DestroyMenu(myCreatureMenu);
	if(IsMenu(myHOfflineInactiveMenu)) DestroyMenu(myHOfflineInactiveMenu);
	if(IsMenu(myHOnlineInactiveMenu)) DestroyMenu(myHOnlineInactiveMenu);


};


// -------------------------------------------------------------------------
// Method:		Create
// Arguments:	none
// Returns:		handle to window created
// Description:	Creates the window
// -------------------------------------------------------------------------
HWND MainFrame::Create()
{
	if(!Wnd::Create())
		return NULL;

	myHOnlineInactiveMenu = LoadMenu(GetWindowInstance(myHWnd), MAKEINTRESOURCE(IDR_ONLINEINACTIVEMENU));
	myHOfflineInactiveMenu = LoadMenu(GetWindowInstance(myHWnd), MAKEINTRESOURCE(IDR_OFFLINEINACTIVEMENU));
	myCreatureMenu = CreateMenu();
	HMENU subMenu = GetSubMenu(myHOnlineInactiveMenu, 0);
	InsertMenu(subMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)myCreatureMenu, "Select a Creature");
	SetMenu(myHWnd, myHOfflineInactiveMenu);

	// create statusbar, toolbar created when you have a viewport
	myHWndStatus = CreateStatusWindow(WS_CHILD|WS_VISIBLE, "", myHWnd, 0);	
	SetStatus();
	CreateToolbar();

	return myHWnd;
}


// -------------------------------------------------------------------------
// Method:		CreateToolbar
// Arguments:	none
// Returns:		none
// Description:	Creates the toolbar window
// -------------------------------------------------------------------------
void MainFrame::CreateToolbar()
{
	InitCommonControls();


	TBBUTTON tbButtons[] = {{0, ID_BUTTONZOOMIN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
							{1, ID_BUTTONZOOMOUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
							{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, -1},
							{2, ID_BUTTONPLAY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},	
							{3, ID_BUTTONSTOP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
							{4, ID_BUTTONLOOP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
							{5, ID_BUTTONINSTINCT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}};

	myHWndToolbar = CreateToolbarEx (myHWnd, 
									WS_BORDER, 
									IDR_VIEWPORTTOOLBAR, 6, GetWindowInstance(myHWnd), IDR_VIEWPORTTOOLBAR, 
									(LPCTBBUTTON)&tbButtons, 7, 0, 0, 16, 15, sizeof (TBBUTTON));  

	UpdateWindow(myHWndToolbar);
}


// -------------------------------------------------------------------------
// Method:		CreateViewportWindow
// Arguments:	null terminated character string filename
// Returns:		success or failure
// Description:	Creates a the brainviewport window with:
//				the genome file or online id which ever is specified
// -------------------------------------------------------------------------
bool MainFrame::CreateViewportWindow(char filename[], int id)
{
	RECT clientRect;
	RECT toolbarRect;
	RECT statusRect;

	GetClientRect(myHWnd, &clientRect);
	GetChildWindowRect(myHWndToolbar, &toolbarRect);
	GetChildWindowRect(myHWndStatus, &statusRect);
	
	try
	{
		if(filename)
			myBrainViewportWnd = new BrainViewport(GetWindowInstance(myHWnd), myHWnd, filename);
		else
			myBrainViewportWnd = new BrainViewport(GetWindowInstance(myHWnd), myHWnd, NULL, id);
		theBrainViewport = myBrainViewportWnd;
	}
	catch(BrainViewport::InitFailedException)
	{
	}


	try
	{
		if((myHWndBrainViewport = myBrainViewportWnd->Create(clientRect.left, toolbarRect.bottom, clientRect.right-clientRect.left, statusRect.top-(toolbarRect.bottom))))
		{
			SetMenu(myHWnd, myBrainViewportWnd->GetHMenu(filename==NULL ? 2 : 0));
			ShowWindow(myHWndBrainViewport, SW_SHOW);
			ShowWindow(myHWndToolbar, SW_SHOW);
			return true;
		}
		else
		{
			return false;
		}
	}
	catch(BrainViewport::InitFailedException)
	{
		delete myBrainViewportWnd;
		myBrainViewportWnd = NULL;
		myHWndBrainViewport = NULL;
		return false;
	}


}



// -------------------------------------------------------------------------
// Method:		WndProc
// Arguments:	standard windproc arguments
// Returns:		result of handled message
// Description:	Object specific message handler
// -------------------------------------------------------------------------
LRESULT MainFrame::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{

	case WM_TIMER:
		myBrainViewportWnd->Update();	
		if(!myLoopState) StopPlay();
		break;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	case WM_SIZE:
		HANDLE_WM_SIZE(hwnd, wParam, lParam, On_Size);
		break;

	case WM_CLOSE:
		if(myHWndBrainViewport)
			CloseBrainViewport();	// assure proper close down with mutexing
		return(Wnd::WndProc(hwnd, msg, wParam, lParam));
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		// call default superclass handler
		return(Wnd::WndProc(hwnd, msg, wParam, lParam));	
 
	}
	return(NULL);
}


// -------------------------------------------------------------------------
// Method:		On_Command
// Arguments:	standard WM_COMMAND handling paramaters
// Returns:		none
// Description:	handles command messages. Passes most on to the open viewport
// -------------------------------------------------------------------------
void MainFrame::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	switch(id)
	{
	case ID_FILE_OPENGENOME:
		// open another genome
		if(myHWndBrainViewport)
		{
			if(MessageBox(myHWnd, "This operation will close the existing brain.\nDo you wish to proceed", "Open File", MB_OKCANCEL | MB_ICONWARNING | MB_APPLMODAL) == IDOK)
			{		
				CloseBrainViewport();
				OpenFile();					
			}
		}
		else
			OpenFile();
		break;

	case ID_FILE_CONNECTTOCREATURES:
		// go online
		if(myHWndBrainViewport)
		{
			if(MessageBox(myHWnd, "This operation will close the existing brain.\nDo you wish to proceed", "Open File", MB_OKCANCEL | MB_ICONWARNING | MB_APPLMODAL) == IDOK)
			{		
				CloseBrainViewport();
				if(GoOnLine()) SetMenu(myHWnd, myHOnlineInactiveMenu);		
			}
		}
		else
			if(GoOnLine()) SetMenu(myHWnd, myHOnlineInactiveMenu);

		break;

	case ID_FILE_RECONNECTTOCREATURES:
		CloseBrainViewport();
		if(GoOnLine())
		{
			int pos = std::distance(myUniqueCreatureIDs.begin(), std::find(myUniqueCreatureIDs.begin(), myUniqueCreatureIDs.end(), myLastCreatureId));
			if(pos < myUniqueCreatureIDs.size() && LinkToCreature(pos))
			{
				myCreatureMenuPos = pos;
				CheckMenuItem(myCreatureMenu, myCreatureMenuPos, MF_CHECKED | MF_BYPOSITION);
				SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONPLAY, (LPARAM) MAKELONG(true, 0));			
				SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONSTOP, (LPARAM) MAKELONG(true, 0));			
				SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONLOOP, (LPARAM) MAKELONG(true, 0));			
				SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONINSTINCT, (LPARAM) MAKELONG(false, 0));			
			}
			else
			{
				 SetMenu(myHWnd, myHOnlineInactiveMenu);
			}
		}

		break;

	case ID_FILE_REFRESHONLINECREATURELIST:
		RefreshCreatureList();
		SetStatus();
		break;

	case ID_FILE_DISCONNECTFROMCREATURES:
		// go off line
		StopPlay();
		if(myHWndBrainViewport)
		{
			myLastCreatureId = myUniqueCreatureIDs[myCreatureMenuPos];
			SetMenu(myHWnd, myBrainViewportWnd->GetHMenu(1));	// reconnect menu
		}
		else
			SetMenu(myHWnd, myHOfflineInactiveMenu);

		if(myBrainViewportWnd) myBrainViewportWnd->Update(true);	// update hidden lobes and tracts	
		theGameInterface.Inject("execute\nDBG: PLAY", NULL);
		theGameInterface.Disconnect();
		
		SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONPLAY, (LPARAM) MAKELONG(false, 0));			
		SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONSTOP, (LPARAM) MAKELONG(false, 0));			
		SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONLOOP, (LPARAM) MAKELONG(false, 0));			
		SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONINSTINCT, (LPARAM) MAKELONG(false, 0));			

		break;

	case ID_FILE_EXIT:
		// assure proper viewport close down with mutexing;	
		if(myHWndBrainViewport)
			CloseBrainViewport();	
		delete this;
		break;

	case ID_VIEW_SETTHRESHHOLDVIEW:
		myBrainViewportWnd->SetThreshHold();
		break;

	default:

		// check if link to creature command
		if(abs(id-65535) < GetMenuItemCount(myCreatureMenu) )
		{
	
			int pos = abs(id-65535);

			if(myHWndBrainViewport)
			{
				if(MessageBox(myHWnd, "This operation will close the existing brain.\nDo you wish to proceed", "Open File", MB_OKCANCEL | MB_ICONWARNING | MB_APPLMODAL) == IDOK)
				{		
					CheckMenuItem(myCreatureMenu, myCreatureMenuPos, MF_UNCHECKED | MF_BYPOSITION);

					CloseBrainViewport();
					if(LinkToCreature(pos))
					{		
						myCreatureMenuPos = pos;
						CheckMenuItem(myCreatureMenu, myCreatureMenuPos, MF_CHECKED | MF_BYPOSITION);
					}
					else
						myCreatureMenuPos = -1;

				}
			}
			else
			{
				if(LinkToCreature(pos))
				{		
					myCreatureMenuPos = pos;
					CheckMenuItem(myCreatureMenu, myCreatureMenuPos, MF_CHECKED | MF_BYPOSITION);
				}
			}
			break;
		}


		if(myHWndBrainViewport)
		{
			// rest of commands requre a brainviewport

			switch(id)
			{
			case ID_BUTTONZOOMIN:
				if(NotifyCode == 0 && myHWndBrainViewport != NULL)
					// click
					FORWARD_WM_COMMAND(myHWndBrainViewport, id, hwndCtl, NotifyCode, SendMessage);
				break;
			
			case ID_BUTTONZOOMOUT:
				if(NotifyCode == 0 && myHWndBrainViewport != NULL)
					// click
					FORWARD_WM_COMMAND(myHWndBrainViewport, id, hwndCtl, NotifyCode, SendMessage);
				break;
			
			case ID_BUTTONPLAY:
				if(!myPlayState)
				{
					myPlayState = true;
					myTimerID = SetTimer(myHWnd, 1, 250, NULL);					
				}
				SendMessage(myHWndToolbar, TB_PRESSBUTTON,(WPARAM) ID_BUTTONPLAY, (LPARAM) MAKELONG(true, 0));		
				break;

			case ID_BUTTONSTOP:
				StopPlay();
				break;

			case ID_BUTTONLOOP:
				myLoopState = myLoopState ? false : true;
				SendMessage(myHWndToolbar, TB_PRESSBUTTON,(WPARAM) ID_BUTTONLOOP, (LPARAM) MAKELONG(myLoopState, 0)); 	
				if(!myLoopState && myPlayState) StopPlay();
				break;

			case ID_BUTTONINSTINCT:
				myInstinctState = myInstinctState ? false : true;
				SendMessage(myHWndToolbar, TB_PRESSBUTTON,(WPARAM) ID_BUTTONINSTINCT, (LPARAM) MAKELONG(myInstinctState, 0)); 	
				myBrainViewportWnd->SwitchInstinctProcessingOnOff(myInstinctState);
				break;

			case ID_FILE_CLOSEBRAIN:
				CloseBrainViewport();
				break;


			default:
				// forward all menu messages and other commands to the viewport
				FORWARD_WM_COMMAND(myHWndBrainViewport, id, hwndCtl, NotifyCode, SendMessage);
				break;

			}
		}
		else
		{
			FORWARD_WM_COMMAND(hwnd, id, hwndCtl, NotifyCode, DefWindowProc);
			break;
		}
	}
}


// -------------------------------------------------------------------------
// Method:		On_Size
// Arguments:	standard on size paramaters
// Returns:		none
// Description:	Handles resizing of window - passes new sizes to children
// -------------------------------------------------------------------------
void MainFrame::On_Size(HWND  hwnd, UINT sizeState, int cx, int cy)
{
	
	if(myHWndToolbar)		
		SendMessage(myHWndToolbar, WM_SIZE, 0, 0);
	if(myHWndStatus)		
		SendMessage(myHWndStatus, WM_SIZE, 0, 0);

	
	if(myHWndBrainViewport)
	{
		// tell viewport to resize 
		RECT clientRect;
		RECT toolbarRect;
		RECT statusRect;

		GetClientRect(myHWnd, &clientRect);
		GetChildWindowRect(myHWndToolbar, &toolbarRect);
		GetChildWindowRect(myHWndStatus, &statusRect);
		MoveWindow(myHWndBrainViewport, clientRect.left, toolbarRect.bottom, clientRect.right-clientRect.left, statusRect.top-toolbarRect.bottom, true);
	}
}


// -------------------------------------------------------------------------
// Method:		GoOnLine
// Arguments:	none
// Returns:		sucess
// Description:	connects to game, gets a brain description, passes the 
//				description to CreateViewport
// -------------------------------------------------------------------------
bool MainFrame::GoOnLine()
{
	int result;
	
	char statustext[] = "Connecting to creatures";
	SendMessage(myHWndStatus, WM_SETTEXT, 0, (LPARAM)statustext);

	if(theGameInterface.Connect()) {
		myCreatureMenuPos = -1;

		do {

			if(!theGameInterface.Inject("execute\nDBG: PAWS", NULL)) {
				if((result = MessageBox(NULL, "Could not suspend game.", "Connect to Creatures", MB_RETRYCANCEL | MB_ICONINFORMATION | MB_APPLMODAL)) == IDCANCEL)
					theGameInterface.Disconnect();
			}
			else
			{
				if(RefreshCreatureList())
				{
					SetStatus();
					return true;
				} 
				else 
				{
					theGameInterface.Disconnect();
					return false;
				}
			}
		} while (result == IDRETRY);


	} else {
		result = MessageBox(NULL, "Failed to connect to game.", "Connect to Creatures", MB_RETRYCANCEL | MB_ICONINFORMATION | MB_APPLMODAL);
	}

	
	SetStatus();

	return false;
} 



// -------------------------------------------------------------------------
// Method:		RefreshCreatureList
// Arguments:	none
// Returns:		false if coulnt find any
// Description:	Updates creature names and ids and menu
// -------------------------------------------------------------------------
bool MainFrame::RefreshCreatureList()
{
	int result;

	// clear old menu
	while(GetMenuItemCount(myCreatureMenu) != 0)
		DeleteMenu(myCreatureMenu, GetMenuItemCount(myCreatureMenu)-1, MF_BYPOSITION);
	
	// delete lists
	myUniqueCreatureIDs.erase(myUniqueCreatureIDs.begin(), myUniqueCreatureIDs.end());
	myCreatureNames.erase(myCreatureNames.begin(), myCreatureNames.end());

	do
	{
		char *reply;
		if(theGameInterface.Inject("execute\nENUM 0 0 0 DOIF TYPE TARG = 7 OUTS HIST NAME GTOS 0 OUTS \"\\n\" OUTV UNID OUTS \"\\n\" ENDI NEXT", &reply))
		{


			char *creatures = reply;
			while(strlen(creatures) != 0)
			{
				// get name
				*strchr(creatures, 10) = 0;
				std::string name = creatures;
				creatures += strlen(creatures)+1;
/*
				//get moniker
				*strchr(creatures, 10) = 0;
				std::string moniker = creatures;
				creatures += strlen(creatures)+1;
*/

				if(name == "")
					myCreatureNames.push_back("dunno name"/*moniker*/);
				else
					myCreatureNames.push_back(name);
	
				// get id		
				int id = atoi(creatures);
				myUniqueCreatureIDs.push_back(id);
				creatures = strchr(creatures, 10)+1;
			}

			if(myCreatureNames.size() != 0)
			{
				for(int c = 0; c != myUniqueCreatureIDs.size(); c++)
					AppendMenu(myCreatureMenu, MF_ENABLED | MF_STRING, (c+1)*-1, myCreatureNames[c].begin());
		
				CheckMenuItem(myCreatureMenu, myCreatureMenuPos, MF_CHECKED | MF_BYPOSITION);

				return true;
			}
		}


		if((result = MessageBox(NULL, "Could not find any creatures or perhaps some other problem.", "Connect to Creatures", MB_RETRYCANCEL | MB_ICONINFORMATION | MB_APPLMODAL)) == IDCANCEL)
			break;

	} while (result == IDRETRY);

	return false;
}



// -------------------------------------------------------------------------
// Method:		LinkToCreature
// Arguments:	creature pos in menu
// Returns:		success
// Description:	creates a viewport for the creature
// -------------------------------------------------------------------------
bool MainFrame::LinkToCreature(int pos)
{
	char statustext[200];
	sprintf(statustext, "Downloading brain: %s", myCreatureNames[pos].begin());
	SendMessage(myHWndStatus, WM_SETTEXT, 0, (LPARAM)statustext);

	int id = myUniqueCreatureIDs[pos];

	if(CreateViewportWindow(NULL, id))
	{
		myCreatureLoadedName =  myCreatureNames[pos].begin();
		HMENU subMenu = GetSubMenu(myBrainViewportWnd->GetHMenu(2), 0);	// connected menu
		InsertMenu(subMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)myCreatureMenu, "Select a Creature");
		SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONINSTINCT, (LPARAM) MAKELONG(false, 0));			

		SetStatus();
		return true;
	}

	SetStatus();

	return false;	
}



// -------------------------------------------------------------------------
// Method:		OpenFIle
// Arguments:	none
// Returns:		none
// Description:	Provides a  open file dialog and passes the filename to 
//				CreateViewport
// -------------------------------------------------------------------------
void MainFrame::OpenFile()
{
	std::string path = theWhichEngine.GetStringKey("Genetics Directory");
	OPENFILENAME ofn;      
	char szFile[260]= " ";    

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = myHWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Genetics\0*.Gen\0";
	ofn.nFilterIndex = 2;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = path.begin(); 
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(GetOpenFileName(&ofn))
	{
		char statustext[] = "Loading genome";
		SendMessage(myHWndStatus, WM_SETTEXT, 0, (LPARAM)statustext);
		if(CreateViewportWindow(ofn.lpstrFile))
			myCreatureLoadedName = ofn.lpstrFile;
		SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONINSTINCT, (LPARAM) MAKELONG(true, 0));			

		SetStatus();

	}
} 


// -------------------------------------------------------------------------
// Method:		CloseBrainViewport
// Arguments:	none
// Returns:		none
// Description:	Closes the open viewport cleanly
// -------------------------------------------------------------------------
void MainFrame::CloseBrainViewport()
{

	StopPlay();

	// remove creature list submenu from viewport menu befre viewport is deleted
	// and takes it with it
	HMENU subMenu = GetSubMenu(myBrainViewportWnd->GetHMenu(2), 0);	// connected menu
	RemoveMenu(subMenu, 0, MF_BYPOSITION);
	
	if(theGameInterface.Connected())
		SetMenu(myHWnd, myHOnlineInactiveMenu);
	else
		SetMenu(myHWnd, myHOfflineInactiveMenu);

	DestroyWindow(myHWndBrainViewport);

	myBrainViewportWnd = NULL;
	myHWndBrainViewport = NULL;
	

	ShowWindow(myHWndToolbar, SW_HIDE);
	SetStatus();

	SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONPLAY, (LPARAM) MAKELONG(true, 0));			
	SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONSTOP, (LPARAM) MAKELONG(true, 0));			
	SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONLOOP, (LPARAM) MAKELONG(true, 0));			
	SendMessage(myHWndToolbar, TB_ENABLEBUTTON,(WPARAM) ID_BUTTONINSTINCT, (LPARAM) MAKELONG(true, 0));			

}


// -------------------------------------------------------------------------
// Method:		StopPlay
// Arguments:	none
// Returns:		none
// Description:	if Playing, captures the thread mutex to ensure update cycle 
//				is finished suspends thread flags myPlayState. 
// -------------------------------------------------------------------------
void MainFrame::StopPlay()
{
	if(myPlayState)
	{
		KillTimer(myHWnd, myTimerID);
		myPlayState = false;
		SendMessage(myHWndToolbar, TB_PRESSBUTTON,(WPARAM) ID_BUTTONPLAY, (LPARAM) MAKELONG(false, 0)); 
	}
}



// -------------------------------------------------------------------------
// Method:		SetStatus
// Arguments:	none
// Description:	Places the text string in the status bar
// -------------------------------------------------------------------------
void MainFrame::SetStatus()
{
	char status[255];
	sprintf(status, "%s, %s.", (theGameInterface.Connected() ? 
		"Connected to Creatures" : "In a Vat"),
		(myHWndBrainViewport ? myCreatureLoadedName.begin() : "no brain loaded"));
		
	SendMessage(myHWndStatus, WM_SETTEXT, 0, (LPARAM)status);
}

