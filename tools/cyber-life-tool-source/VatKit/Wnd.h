// ----------------------------------------------------------------------------
// Filename:	Wnd.h
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
//				paramaters for CreateWindow. Wnd::InitInstance is called AFTER 
//				construction to create the window and bind it to the object,
// -----------------------------------------------------------------------------

#pragma warning( disable : 4786 )

#ifndef Wnd_H
#define Wnd_H
#include <windows.h>
#include <windowsx.h>
class Wnd;

class Wnd
{
	typedef struct  
	{
		Wnd *previous;
		Wnd *next;
	} WndList;


	typedef struct 
	{
		DWORD dwExStyle;	// extended style   
		char *lpClassName;	// pointer to registered class name
		char *lpWindowName; // pointer to window name
		DWORD dwStyle;      // window style
		int x;              // horizontal position of window
		int y;              // vertical position of window
		int nWidth;         // window width  
		int nHeight;        // window height
		HWND hWndParent;    // handle to parent or owner window
		HMENU hMenu;        // handle to menu or child-window identifier
		HINSTANCE hInstance;// handle to application instance
		LPVOID lpParam;        
	} WndCreateStyle;	// paramaters used in CreateWindowEx
	
public:

	class FailConstructorException{};


	// -------------------------------------------------------------------------
	// Method:		Create
	// Arguments:	none                       
	// Returns:		Handle to window created
	// Description:	Creates a window object using myWs as paramaters to 
	//				CreateWindowEx. Binds window to this object                             
	// -------------------------------------------------------------------------
	virtual HWND Create();


	// -------------------------------------------------------------------------
	// Method:		MsgHandler
	// Arguments:	Standard WindProc paramters                   
	// Returns:		Results of object specific windproc
	// Description:	Distibutes messages to object specific WndProc functions   
	//				Note: WM_CREATE messages are not distrubuted to a object 
	//				specific WndProc as the window and object have not been 
	//				bound yet.                        
	// -------------------------------------------------------------------------
	static LRESULT CALLBACK MsgHandler(HWND hWnd, UINT msg, UINT wParam, 
			LONG lParam);

	// -------------------------------------------------------------------------
	// Method:		HWnd
	// Arguments:	Wnd object pointer                      
	// Returns:		Window Handle
	// Description:	Returns the window handle of the window bound to the Wnd 
	//				object. Searches linked list and doesnt just return myHWnd 
	//				incase the  Wnd pointer is duff                             
	// -------------------------------------------------------------------------
	inline static HWND HWnd(Wnd *SeekWnd);

	// -------------------------------------------------------------------------
	// Method:		IsOwnedByMe
	// Arguments:	hWnd window handle                     
	// Returns:		true if hWnd is owned by this Wnd
	// Description:	Returns the window handle of the window bound to the Wnd 
	//				object. Searches linked list and doesnt just return myHWnd 
	//				incase the  Wnd pointer is duff                             
	// -------------------------------------------------------------------------
	bool IsOwnedByMe( HWND hWnd );

protected:
	
	HWND myHWnd;		// handle to window bound to this object
	Wnd *myOwnerWnd;	// pointer to object that owns this object
	WNDCLASSEX myWCEX;	// structure for inheriting Registration styles
	WndCreateStyle myWCS;			// structure for inheriting CreateWindow styles

	// -------------------------------------------------------------------------
	// Method:		Wnd
	// Arguments:	none                      
	// Returns:		none
	// Description:	Constructor. Initialises members and adds this window to the 
	//				linked list of Wnd objects                             
	// -------------------------------------------------------------------------
	Wnd();	

	// -------------------------------------------------------------------------
	// Method:		~Wnd
	// Arguments:	none                      
	// Returns:		none
	// Description:	Destructor. Virtual allows derived class destructors to be 
	//				from from Wnd before base (used in On_Destroy).                          
	// -------------------------------------------------------------------------
	virtual ~Wnd();	

	// -------------------------------------------------------------------------
	// Method:		RegisterWndClass
	// Arguments:	none                      
	// Returns:		Success of failure
	// Description:	Calls RegisterClass with myWCEX. Calls exception if fails                          
	// -------------------------------------------------------------------------
	ATOM RegisterWndClass();

	// -------------------------------------------------------------------------
	// Method:		GetWnd
	// Arguments:	hWnd - handle to a window                      
	// Returns:		Wnd object wound to the window hWnd
	// Description:	Searches linked list of existing Wnd objects returns Wnd
	//				pointer of object bound to  the window
	// -------------------------------------------------------------------------
	inline static void *Wnd::GetWnd(HWND hWnd);

	// -------------------------------------------------------------------------
	// Method:		WndProc
	// Arguments:	Standard windproc paramaters                  
	// Returns:		result of DefWindowProc
	// Description:	Base object specific WndProc. Calls DefWindowProc
	//				Virtual so derived objects have thier own WndProc message 
	//				handlers
	// -------------------------------------------------------------------------
	virtual LRESULT WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);


private:
	static Wnd *ourFirstWnd;	// pointer to first object in linked list
	WndList myWndListMember;	// linked list details of this wnd object
	bool	myIsDeleting,		// flag used on deleteing object
			myIsDestroying;		// flag used on destroying window bound to
								// this object



	// -------------------------------------------------------------------------
	// Method:		AddWnd
	// Arguments:	none                       
	// Returns:		none
	// Description:	Adds this object to the linked list of wnd objects                              
	// -------------------------------------------------------------------------
	void AddWnd();
	
	// -------------------------------------------------------------------------
	// Method:		RemoveWnd
	// Arguments:	none                       
	// Returns:		none
	// Description:	Removes this object from the linked list of wnd objects                              
	// -------------------------------------------------------------------------
	void RemoveWnd();
	
	// -------------------------------------------------------------------------
	// Method:		On_Destroy
	// Arguments:	Standard WindProc arguments                 
	// Returns:		Value passed back by objects own wnd proc
	// Description:	Used when DestroyWindow is called to assure the bound Wnd 
	//				object is deleted.                              
	// -------------------------------------------------------------------------
	LRESULT On_Destroy(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
};


// -----------------------------------------------------------------------------
// Method:		HWnd
// Arguments:	Wnd object pointer                      
// Returns:		Window Handle
// Description:	Returns the window handle of the window bound to the Wnd object
//				Searches linked list and doesnt just return myHWnd incase the 
//				Wnd pointer is duff                             
// -----------------------------------------------------------------------------
HWND Wnd::HWnd(Wnd *SeekWnd)
{
	Wnd *wnd = ourFirstWnd;

	if(ourFirstWnd == NULL)
		return NULL;

	while(wnd != SeekWnd && wnd->myWndListMember.next != NULL)
	{
		wnd = wnd->myWndListMember.next;
	}
	
	if(wnd == SeekWnd)
	{
		if(IsWindow(wnd->myHWnd))
			return wnd->myHWnd;	
		else
			return NULL;			
	}
	else
		return NULL;
}


// -------------------------------------------------------------------------
// Method:		GetWnd
// Arguments:	hWnd - handle to a window                      
// Returns:		Wnd object wound to the window hWnd
// Description:	Searches linked list of existing Wnd objects returns Wnd
//				pointer of object bound to  the window
// -------------------------------------------------------------------------
void *Wnd::GetWnd(HWND hWnd)
{
	Wnd *wnd = ourFirstWnd;

	if(hWnd == NULL)
		return NULL;

	if(ourFirstWnd == NULL)
		return NULL;

	if(!IsWindow(hWnd))
		return NULL;

	while(wnd->myHWnd != hWnd && wnd->myWndListMember.next != NULL)
	{
		wnd = wnd->myWndListMember.next;
	}
	
	if(wnd->myHWnd == hWnd)
		return wnd;
	else
		return NULL;
};



// ----------------------------------------------------------------------------
// Function:	GetChildWindowRect
// Arguments:	hChildWnd - handle to the window whose RECT is to be found 
//				lpRect - long pointer to a RECT structure which will be filled 
//				with the dimensions.                                
// Returns:		false if window des not exist. true if it does
// Description:	Function to make life easier. Returns the dimensions of a 
//				window relative to its parent                                 
// ----------------------------------------------------------------------------
bool GetChildWindowRect(HWND hChildWnd, LPRECT lpRect);

#define GetHwnd(wnd_ptr) Wnd::HWnd(wnd_ptr)

#endif





