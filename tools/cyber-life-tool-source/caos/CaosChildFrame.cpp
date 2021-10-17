// CaosChildFrame.cpp : implementation file
//

#include "windows.h"
#include "stdafx.h"
#include "Caos.h"
#include "../../Common/WindowState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// The main MDI application window
extern CMainFrame *theFrame;

WINDOWPLACEMENT wp;

/////////////////////////////////////////////////////////////////////////////
// CCaosChildFrame

IMPLEMENT_DYNCREATE(CCaosChildFrame, CMDIChildWnd)


// Constructor
CCaosChildFrame::CCaosChildFrame()
{
	m_Filename = "";
	m_Title = "";
}


// Destructor
CCaosChildFrame::~CCaosChildFrame()
{
}


BEGIN_MESSAGE_MAP(CCaosChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CCaosChildFrame)
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_CONTENTS_EDITED, OnContentsEdited)
	ON_MESSAGE(WM_LINE_ERROR, OnLineError)
	ON_MESSAGE(WM_SELECTION_CHANGED, OnSelectionChanged)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCaosChildFrame message handlers


void CCaosChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	int StartLineNumber, EndLineNumber;
	CString s;

	// Call the default
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	if (::IsWindow(m_ctrlEdit.m_hWnd))
	{
		if (bActivate) {
			// Get the line numbers of the current selection
			m_ctrlEdit.GetLineNumbers(StartLineNumber, EndLineNumber);
			if (StartLineNumber == EndLineNumber)
				s.Format("Line %d", StartLineNumber);
			else
				s.Format("Lines %d - %d", StartLineNumber, EndLineNumber);
			// Set the line number pane to reflect the current selection
			theFrame->m_wndStatusBar.SetPaneText(1, s, TRUE);
			// Set the titlebar to reflect the status change...
			CString mytitle;
			GetWindowText(mytitle);
			if (GetWindowPlacement(&wp),wp.showCmd == SW_SHOWNORMAL)
				theFrame->SetWindowText(theFrame->m_defaultTitleString + " - " + mytitle);
			else
				theFrame->SetWindowText(theFrame->m_defaultTitleString);
			// Set the focus to the edit box
			m_ctrlEdit.SetFocus();
		}
	}
}



void CCaosChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CRect client;

	// Call the default
	CMDIChildWnd::OnSize(nType, cx, cy);

	CString mytitle;
	GetWindowText(mytitle);
	if (GetWindowPlacement(&wp),wp.showCmd == SW_SHOWNORMAL)
		theFrame->SetWindowText(theFrame->m_defaultTitleString + " - " + mytitle);
	else
		theFrame->SetWindowText(theFrame->m_defaultTitleString);
	// Get the new client area of the CAOS child frame
	GetClientRect(&client);
	if (::IsWindow(m_ctrlEdit.m_hWnd))
		// Force the edit control to completely fill the CAOS child frame
		m_ctrlEdit.SetWindowPos(NULL, 0, 0, client.Width(), 
			client.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);	
}



void CCaosChildFrame::OnClose() 
{
	int Result;
	CString Message;

	if (m_ctrlEdit.IsModified()) {
		// The COS document contains edited macro code. Give opportunity 
		// to save it, not save it or abort the close operation
		Message.Format("'%s' contains unsaved data. Do you want to save now?",
			m_Title);
		::Beep(450, 100);
		Result = ::AfxMessageBox(Message, MB_YESNOCANCEL);
		if (Result == IDYES) {
			// Try to save the document
			if (!theFrame->DoGenericSave(this)) {
				// Inform the MDI frame that the Close operation was aborted
				theFrame->m_Cancelled = TRUE;
				// Set the focus to the edit box
				m_ctrlEdit.SetFocus();
				return;
			}
		}
		else if (Result == IDCANCEL) {
			// Inform the MDI frame that the Close operation was aborted
			theFrame->m_Cancelled = TRUE;
			// Set the focus to the edit box
			m_ctrlEdit.SetFocus();
			return;
		}
	}
	// Remove this CAOS child frame from the MDI frame's list
	theFrame->RemoveCaosFrameFromFrameList(m_ID);
	theFrame->m_Cancelled = FALSE;

	WindowState::Save("Windows", "CAOS Editor Default", m_hWnd);

	// Kill the CAOS child frame

	theFrame->SetWindowText(theFrame->m_defaultTitleString);

	CMDIChildWnd::OnClose();
}




LONG CCaosChildFrame::OnContentsEdited(UINT dummy1, LONG dummy2)
{
	// The contents of the control have been edited, so add 
	// a star to the title and set this as the CAOS frame 
	// title bar
	SetWindowText(m_Title + " *");
	
		if (GetWindowPlacement(&wp),wp.showCmd == SW_SHOWNORMAL)
			theFrame->SetWindowText(theFrame->m_defaultTitleString + " - " + m_Title + " *");
		else
			theFrame->SetWindowText(theFrame->m_defaultTitleString);
	return 0;
}



LONG CCaosChildFrame::OnLineError(UINT LineNumber, LONG dummy)
{
	char *LineError;
	CString Contents;
	CString Temp;
	int TotalLength;
	
	// A syntax error has occurred in the edit control, so display 
	// information about it (though only if the error frame exists)
	if (theFrame->m_ErrorFrame) {
		// Get the error text
		LineError = m_ctrlEdit.GetLineErrorString();
		// Add the title, line number and error string to the end of the
		// error frame's error text
		theFrame->m_ErrorFrame->m_ctrlEdit.GetWindowText(Contents);
		Temp.Format("%s%s: Line %d: %s\r\n", Contents, m_Title, 
			LineNumber, LineError);
		theFrame->m_ErrorFrame->m_ctrlEdit.SetWindowText(Temp);
		// Move the cursor to the end in order to display the most recent
		// errors
		TotalLength = theFrame->m_ErrorFrame->m_ctrlEdit.GetTextLength();
		theFrame->m_ErrorFrame->m_ctrlEdit.SetSel(TotalLength, TotalLength);
	}
	return 0;
}


LONG CCaosChildFrame::OnSelectionChanged
	(UINT StartLineNumber, LONG EndLineNumber)
{
	CString s;

	if (StartLineNumber == (UINT)EndLineNumber)
		s.Format("Line %d", StartLineNumber);
	else
		s.Format("Lines %d - %d", StartLineNumber, EndLineNumber);
	// Set the line number pane to reflect the current selection
	theFrame->m_wndStatusBar.SetPaneText(1, s, TRUE);
	return 0;
}

void CCaosChildFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_CAOS_POPUP));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL); 
//	SetActiveWindow();
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

int CCaosChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	WindowState::Load("Windows", "CAOS Editor Default", m_hWnd);

	SetTimer(47, 1000 * 60 * 5, NULL);

	return 0;
}

void CCaosChildFrame::AutoSave()
{
	char temp[MAX_PATH];
	if (GetTempPath(MAX_PATH, temp) != 0)
	{
		CString tempfile = m_Filename.GetBuffer(1);
		tempfile.Replace('\\', '_');
		tempfile.Replace('/', '_');
		tempfile.Replace(':', '_');
		if (tempfile == "")
			tempfile = "unnamed.cos";
		tempfile = tempfile.Right(_MAX_FNAME - 1 - 4);
		tempfile = tempfile.Right(_MAX_PATH - 6 - strlen(temp) - 4);
		tempfile = temp + tempfile;

		CopyFile(tempfile, tempfile + ".bak", false);

		m_ctrlEdit.Save((LPCTSTR)tempfile, false);
	}
}

void CCaosChildFrame::OnTimer(UINT nIDEvent) 
{
	AutoSave();
	
	CMDIChildWnd::OnTimer(nIDEvent);
}

