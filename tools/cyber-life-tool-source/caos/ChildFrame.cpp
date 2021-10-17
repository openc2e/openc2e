////////////////////////////////////////////////////////////////////////////////
// ChildFrame.cpp : implementation file
//


#include "stdafx.h"
#include "Caos.h"
#include "../../common/WindowState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// The main MDI application window
extern CMainFrame *theFrame;

#include "windows.h"
WINDOWPLACEMENT cwp;

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

// Constructor
CChildFrame::CChildFrame()
{
}

// Destructor
CChildFrame::~CChildFrame()
{
}


BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_MDIACTIVATE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers


void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CRect client;

	// Call the default
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// Get the client area of the child frame
	GetClientRect(&client);
	if (::IsWindow(m_ctrlEdit.m_hWnd))
		// Force the contained edit box to completely fill the client
		// area of the child frame
		m_ctrlEdit.SetWindowPos(NULL, 0, 0, client.Width(), client.Height(), 
			SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);	
	CString mytitle;GetWindowText(mytitle);
	if (!theFrame)
		return;
	if (GetWindowPlacement(&cwp),cwp.showCmd == SW_SHOWNORMAL)
		theFrame->SetWindowText(theFrame->m_defaultTitleString + " - " + mytitle);
	else
		theFrame->SetWindowText(theFrame->m_defaultTitleString);

}


void CChildFrame::OnClose() 
{
	if (m_ChildFrameType == 1)
		// Notify the main MDI frame that the error frame is dead
		theFrame->m_ErrorFrame = NULL;
	else
		// Notify the main MDI frame that the script output frame is dead
		theFrame->m_OutputFrame = NULL;

	CString title;
	GetWindowText(title);
	WindowState::Save("Windows", title, m_hWnd);

	// Kill the child frame
	CMDIChildWnd::OnClose();
}



void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CString Blanks;

	// Call the default
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if ((bActivate) && ::IsWindow(m_ctrlEdit.m_hWnd)) {
		// The child frame has been activated, so clear the line number pane
		// and set the focus to the contained edit box
		Blanks.LoadString(IDM_BLANKS);
		theFrame->m_wndStatusBar.SetPaneText(1, Blanks, TRUE);
		m_ctrlEdit.SetFocus();
	}
	CString mytitle;GetWindowText(mytitle);
	if (!theFrame)
		return;
	if (GetWindowPlacement(&cwp),cwp.showCmd == SW_SHOWNORMAL)
		theFrame->SetWindowText(theFrame->m_defaultTitleString + " - " + mytitle);
	else
		theFrame->SetWindowText(theFrame->m_defaultTitleString);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int ret = CMDIChildWnd::OnCreate(lpCreateStruct);
	
	if (ret != -1)
	{
		CString title;
		GetWindowText(title);
		WindowState::Load("Windows", title, m_hWnd);
	}

	return ret;
}

