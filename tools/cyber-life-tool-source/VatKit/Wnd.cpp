// ----------------------------------------------------------------------------
// Filename:	Wnd.cpp
// Class:		Wnd
// Purpose:		Base class for all classes that have a window.
// Description:	Controls message handleing  as registered windprocs can only 
//				be static functions. 
//				Handles destruction to ensure DestroyWindow is called when an
//				object is deleted, and that deleted is called when
//				DestroyWindow is used on a window that is bound to an object 
//				derived from Wnd.
//	Usage:		Used as a base class. Sub class constructors define myWCEX and
//				myWCS (which are a member of the base class and can be 
//				inhertted). myWCEX is used by the sub class constructor to 
//				register the window. myWCS is used by Wnd:InitInstance as
//				paramaters for CreateWindow. Wnd::Create is called AFTER 
//				construction to create the window and bind it to the object,
// -----------------------------------------------------------------------------


#include <windows.h>
#include <string.h>
#include "Wnd.h"


Wnd *Wnd::ourFirstWnd = NULL;




// ----------------------------------------------------------------------------
// Method:		InitInstance
// Arguments:	none                   
// Returns:		Handle to window created
// Description:	Creates a window object using myWs as paramaters to 
//				CreateWindowEx. Binds window to this object                             
// ----------------------------------------------------------------------------
HWND Wnd::Create()
{
	if(!(myHWnd = CreateWindowEx(myWCS.dwExStyle, myWCS.lpClassName, 
		myWCS.lpWindowName, myWCS.dwStyle, myWCS.x, myWCS.y, myWCS.nWidth, 
		myWCS.nHeight, myWCS.hWndParent, myWCS.hMenu, myWCS.hInstance, myWCS.lpParam)))
		return NULL;

	if(!myOwnerWnd)	// hasnt been set elsewhere
		myOwnerWnd = (Wnd *)GetWnd(myWCS.hWndParent);

	return myHWnd;
};



// ----------------------------------------------------------------------------
// Method:		MsgHandler
// Arguments:	Standard WindProc paramters                   
// Returns:		Results of object specific windproc
// Description:	Distibutes messages to object specific WndProc functions   
//				Note: WM_CREATE messages are not distrubuted to a object 
//				specific WndProc as the window and object have not been bound
//				yet.                        
// ----------------------------------------------------------------------------
LRESULT CALLBACK Wnd::MsgHandler(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	Wnd *wnd;
	
	wnd = (Wnd *)GetWnd(hWnd);

	if(wnd)
	{	
		if(msg == WM_DESTROY)
			// special handling of DestroyWindow
			return wnd->Wnd::On_Destroy(hWnd, msg, wParam, lParam);
		else 
			// call window classes own window message handler
			return wnd->WndProc(hWnd, msg, wParam, lParam);
	}
	else
	{
		// handle messages for windows without Wnd objects 
		// this happens in the create stage cos the Wnd doesnt get
		// a handle for the window until the create is finished
		// so all specific create processes must occur in the constructor
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}


// ----------------------------------------------------------------------------
// Method:		Wnd
// Arguments:	none                      
// Returns:		none
// Description:	Constructor. Initialises members and adds this window to the 
//				linked list of Wnd objects                             
// ----------------------------------------------------------------------------
Wnd::Wnd()
	:myHWnd(NULL),
	myOwnerWnd(NULL),
	myIsDeleting(false),
	myIsDestroying(false)
{
	memset(&myWCEX,0,sizeof(myWCEX));
	myWCEX.cbSize = sizeof(myWCEX);
	memset(&myWCS,0,sizeof(myWCS));
	AddWnd();
};


// ----------------------------------------------------------------------------
// Method:		~Wnd
// Arguments:	none                      
// Returns:		none
// Description:	Destructor. Virtual allows derived class destructors to be 
//				from from Wnd before base (used in On_Destroy).                          
// ----------------------------------------------------------------------------
Wnd::~Wnd()
{	
	myIsDeleting = true;	// stops recursive loops with On_Destroy

	if(IsWindow(myHWnd) && !myIsDestroying)
		DestroyWindow(myHWnd);
		
	RemoveWnd();
};


// ----------------------------------------------------------------------------
// Method:		RegisterWndClass
// Arguments:	none                      
// Returns:		Success of failure
// Description:	Calls RegisterClass with myWCEX. Calls exception if fails                          
// ----------------------------------------------------------------------------
ATOM Wnd::RegisterWndClass()
{
	ATOM rc;
	myWCEX.lpfnWndProc = MsgHandler;
	if(!(rc = RegisterClassEx(&myWCEX)))
		throw Wnd::FailConstructorException();

	return rc;
}


// ----------------------------------------------------------------------------
// Method:		AddWnd
// Arguments:	none                       
// Returns:		none
// Description:	Adds this object to the linked list of wnd objects                              
// ----------------------------------------------------------------------------
void Wnd::AddWnd()
{
	Wnd *wnd = ourFirstWnd;

	if(ourFirstWnd == NULL)
	{
		ourFirstWnd = this;
		myWndListMember.previous = NULL;
	}
	else
	{
		while (wnd->myWndListMember.next != NULL) 
		{
			wnd = wnd->myWndListMember.next;
		}
		wnd->myWndListMember.next = this;
		myWndListMember.previous = wnd;
	}

	myWndListMember.next = NULL;

	return ;
}


// -------------------------------------------------------------------------
// Method:		WndProc
// Arguments:	Standard windproc paramaters                  
// Returns:		result of DefWindowProc
// Description:	Base object specific WndProc. Calls DefWindowProc
//				Virtual so derived objects have thier own WndProc message 
//				handlers
// -------------------------------------------------------------------------
LRESULT Wnd::WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	return(DefWindowProc(hWnd, msg, wParam, lParam));
};


// ----------------------------------------------------------------------------
// Method:		RemoveWnd
// Arguments:	none                       
// Returns:		none
// Description:	Removes this object from the linked list of wnd objects                              
// ----------------------------------------------------------------------------
void Wnd::RemoveWnd()
{
	// delete from linked list
	if(ourFirstWnd == this)
	{
		if(myWndListMember.next == NULL)
		{
			ourFirstWnd = NULL;
			
			// if this is the only existing window make sure a quit has been 
			// sent
			
			if(IsWindow(myHWnd))
			{
				MSG peekMsg;
				if(!PeekMessage(&peekMsg, myHWnd, WM_QUIT, WM_QUIT, PM_NOREMOVE))
					PostQuitMessage(0);
			}
			else
				PostQuitMessage(0);
		}
		else
		{
			ourFirstWnd = myWndListMember.next;
			ourFirstWnd->myWndListMember.previous = NULL;
		} 

	}
	else
	{
		myWndListMember.previous->myWndListMember.next = myWndListMember.next;
		if(myWndListMember.next != NULL)
			myWndListMember.next->myWndListMember.previous = myWndListMember.previous;
	}

	return;
}
		

// ----------------------------------------------------------------------------
// Method:		On_Destroy
// Arguments:	Standard WindProc arguments                 
// Returns:		Value passed back by objects own wnd proc
// Description:	Used when DestroyWindow is called to assure the bound Wnd object 
//				is deleted.                              
// ----------------------------------------------------------------------------
LRESULT Wnd::On_Destroy(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	long returnVal;
	myIsDestroying = true;
	
	// forward message to derived window class handler
	returnVal = WndProc(hWnd, msg, wParam, lParam);

	// delete this Wnd object if not already doing so
	if(!myIsDeleting)
		delete this;

	return returnVal;
}

// Additional support functions ///////////////////////////////////////////

// ----------------------------------------------------------------------------
// Function:	GetChildWindowRect
// Arguments:	hChildWnd - handle to the window whose RECT is to be found 
//				lpRect - long pointer to a RECT structure which will be filled 
//				with the dimensions.                                
// Returns:		false if window des not exist. true if it does
// Description:	Function to make life easier. Returns the dimensions of a 
//				window relative to its parent                                 
// ----------------------------------------------------------------------------
bool GetChildWindowRect(HWND hChildWnd, LPRECT lpRect)
{
	if(IsWindow(hChildWnd))
	{
		HWND hParentWnd;
		GetWindowRect(hChildWnd, lpRect);
		if((hParentWnd = GetParent(hChildWnd)))
		{
			ScreenToClient(hParentWnd, (LPPOINT)&lpRect->left);
			ScreenToClient(hParentWnd, (LPPOINT)&lpRect->right);
			return true;
		}
		else
			return false;
	}
	else
	{
		memset(lpRect, 0, sizeof(RECT));
		return false;
	}
}
bool Wnd::IsOwnedByMe( HWND hWnd )
{
	HWND window= hWnd;
	while( window )
	{
		if( window == myHWnd )
			return true;
		window = GetWindow( window, GW_OWNER );
	}
	return false;
}

