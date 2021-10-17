// GotoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Caos.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The main MDI application window
extern CMainFrame *theFrame;


/////////////////////////////////////////////////////////////////////////////
// CGotoDialog dialog


CGotoDialog::CGotoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGotoDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGotoDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGotoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGotoDialog)
	DDX_Control(pDX, IDC_GOTO_LINE, m_ctrlLine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGotoDialog, CDialog)
	//{{AFX_MSG_MAP(CGotoDialog)
	ON_BN_CLICKED(IDC_GOTO, OnGoto)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGotoDialog message handlers

BOOL CGotoDialog::OnInitDialog() 
{
	CCaosChildFrame *CaosFrame;
	int StartLineNumber, EndLineNumber;
	CString s;

	CDialog::OnInitDialog();
	
	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)theFrame->MDIGetActive();
	// Get the line number of the start of the selection
	CaosFrame->m_ctrlEdit.GetLineNumbers(StartLineNumber, EndLineNumber);
	s.Format("%d", StartLineNumber);
	// Set and highlight the line number edit box
	m_ctrlLine.SetWindowText(s);
	m_ctrlLine.SetSel(0, -1);
	m_ctrlLine.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CGotoDialog::OnGoto() 
{
	CCaosChildFrame *CaosFrame;
	int LineNumber;
	CString Message;
	char *ContentsBuffer;
	char *EndPtr;
	BOOL OK;
	CString Contents;
	int LineCount;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)theFrame->MDIGetActive();
	// Get the number of lines in the edit control
	LineCount = CaosFrame->m_ctrlEdit.GetLineCount();

	// Get the trimmed contents of the line number edit control
	m_ctrlLine.GetWindowText(Contents);
	Contents.TrimLeft();
	Contents.TrimRight();
	if (Contents.GetLength() == 0)
		// Empty
		OK = FALSE;
	else {
		ContentsBuffer = Contents.GetBuffer(1);
		LineNumber = strtol(ContentsBuffer, &EndPtr, 10);
		if (*EndPtr != '\0')
			OK = FALSE;
		else
			OK = TRUE;
		Contents.ReleaseBuffer();
	}

	if (!OK) {
		::Beep(450, 100);
		Message.Format("Please enter a line number between 1 and %d", LineCount);
		::AfxMessageBox(Message, MB_ICONEXCLAMATION);
		// Highlight the line number edit box
		m_ctrlLine.SetSel(0, -1);
		m_ctrlLine.SetFocus();
		return;	
	}

	if ((LineNumber < 1) || (LineNumber > LineCount))
		OK = FALSE;
	else
		OK = TRUE;

	if (!OK) {
		::Beep(450, 100);
		Message.Format("Please enter a line number between 1 and %d", LineCount);
		::AfxMessageBox(Message, MB_ICONEXCLAMATION);
		// Highlight the line number edit box
		m_ctrlLine.SetSel(0, -1);
		m_ctrlLine.SetFocus();
		return;
	}

	// Move to the given line number
	CaosFrame->m_ctrlEdit.SetLineNumber(LineNumber);
	// Pop down the dialog
	EndDialog(IDOK);
}

