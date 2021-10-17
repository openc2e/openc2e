// ----------------------------------------------------------------------------
// Filename:	MainFrame.h
// Class:		MainFrame
// Purpose:		Main Application Window
// Description:	Main Vat application window. Has file open option of which the 
//				returned genome filename is used to creates a viewport window.
//				Controls brain play thread, and closes viewportwindows cleanly
// Usage:		May be called from winmain and only requires the application
//				instance. InitInstance must be called after the constructor
//				and after the window has been created
// -----------------------------------------------------------------------------

#ifndef MainFrame_H
#define MainFrame_H

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include <cstring>
#include <vector>
#include "Wnd.h"
#include "BrainViewport.h"

class BrainViewport;

class MainFrame : public Wnd
{	
public:

	// -------------------------------------------------------------------------
	// Method:		MainFrame
	// Arguments:	hInstance - application instance                       
	//				hWndOwner - handle to owner window
	// Returns:		none
	// Description:	Constructor - uses hInstance and hWndOwner to register the 
	//				and create the Window
	// -------------------------------------------------------------------------
	MainFrame(const HINSTANCE hInstance, const HWND hWndOwner);

	// -------------------------------------------------------------------------
	// Method:		~MainFrame
	// Arguments:	none
	// Returns:		none
	// Description:	Destructor
	// -------------------------------------------------------------------------
	~MainFrame();

	// -------------------------------------------------------------------------
	// Method:		Create
	// Arguments:	none
	// Returns:		handle to window created
	// Description:	Creates the window
	// -------------------------------------------------------------------------
	virtual HWND Create();


	

protected:
	
	// -------------------------------------------------------------------------
	// Method:		WndProc
	// Arguments:	standard windproc arguments
	// Returns:		result of handled message
	// Description:	Object specific message handler
	// -------------------------------------------------------------------------
	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);


private:

	char ourClassName[MAXRESOURCESTRING];
	static bool ourIsRegistered;



	HMENU myHOfflineInactiveMenu;
	HMENU myHOnlineInactiveMenu;
	HMENU myCreatureMenu;

	int myCreatureMenuPos;
	int myLastCreatureId;
	std::string myCreatureLoadedName;

	HWND myHWndToolbar;
	HWND myHWndStatus;
	HWND myHWndBrainViewport;
	BrainViewport *myBrainViewportWnd;

	UINT myTimerID;
	bool myPlayState;
	bool myLoopState;
	bool myInstinctState;

	std::vector<int> myUniqueCreatureIDs;
	std::vector<std::string> myCreatureNames;


	// -------------------------------------------------------------------------
	// Method:		CreateToolbar
	// Arguments:	none
	// Returns:		none
	// Description:	Creates the toolbar window
	// -------------------------------------------------------------------------
	void CreateToolbar();

	// -------------------------------------------------------------------------
	// Method:		CreateViewportWindow
	// Arguments:	null terminated character string filename
	// Returns:		success or failure
	// Description:	Creates a the brainviewport window with:
	//				the genome file or online id which ever is specified 
	// -------------------------------------------------------------------------
	bool CreateViewportWindow(char filename[], int id = 0);

	// -------------------------------------------------------------------------
	// Method:		On_Command
	// Arguments:	standard WM_COMMAND handling paramaters
	// Returns:		none
	// Description:	handles command messages. Passes most on to the open viewport
	// -------------------------------------------------------------------------
	void On_Command(HWND hwnd, int wID, HWND hwndCtl, UINT wNotifyCode);

	// -------------------------------------------------------------------------
	// Method:		On_Size
	// Arguments:	standard on size paramaters
	// Returns:		none
	// Description:	Handles resizing of window - passes new sizes to children
	// -------------------------------------------------------------------------
	void On_Size(HWND  hwnd, UINT sizeState, int cx, int cy);

	// -------------------------------------------------------------------------
	// Method:		OpenFIle
	// Arguments:	none
	// Returns:		none
	// Description:	Provides a  open file dialog and passes the filename to 
	//				CreateViewport
	// -------------------------------------------------------------------------
	void OpenFile();

	// -------------------------------------------------------------------------
	// Method:		GoOnLine
	// Arguments:	none
	// Returns:		sucess
	// Description:	connects to game, gets a brain description, passes the 
	//				description to CreateViewport
	// -------------------------------------------------------------------------
	bool GoOnLine();
	
	// -------------------------------------------------------------------------
	// Method:		RefreshCreatureList
	// Arguments:	none
	// Returns:		false if coulnt find any
	// Description:	Updates creature names and ids and menu
	// -------------------------------------------------------------------------
	bool RefreshCreatureList();
	
	// -------------------------------------------------------------------------
	// Method:		LinkToCreature
	// Arguments:	pos in menu
	// Returns:		success
	// Description:	creates a viewport for the creature
	// -------------------------------------------------------------------------
	bool LinkToCreature(int id);

	// -------------------------------------------------------------------------
	// Method:		CloseBrainViewport
	// Arguments:	none
	// Returns:		none
	// Description:	Closes the open viewport cleanly
	// -------------------------------------------------------------------------
	void CloseBrainViewport();

	// -------------------------------------------------------------------------
	// Method:		StopPlay
	// Arguments:	none
	// Returns:		none
	// Description:	if Playing, captures the thread mutex to ensure update cycle 
	//				is finished suspends thread flags myPlayState. 
	// -------------------------------------------------------------------------
	void MainFrame::StopPlay();

	// -------------------------------------------------------------------------
	// Method:		SetStatus
	// Arguments:	none
	// Description:	Places the text string in the status bar
	// -------------------------------------------------------------------------
	void SetStatus();
};




#endif

