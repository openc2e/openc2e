////////////////////////////////////////////////////////////////////////////////
// FindDialog.cpp : implementation file
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
// CFindDialog dialog


CFindDialog::CFindDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFindDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDialog)
	DDX_Control(pDX, IDC_FIND_WHAT, m_ctrlWhat);
	DDX_Control(pDX, IDC_FIND_WHOLE_WORD, m_ctrlWholeWord);
	DDX_Control(pDX, IDC_FIND_CASE, m_ctrlCase);
	DDX_Control(pDX, IDC_FIND_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_FIND_DOWN, m_ctrlDown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindDialog, CDialog)
	//{{AFX_MSG_MAP(CFindDialog)
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_FIND_UP, OnFindUpToggled)
	ON_BN_CLICKED(IDC_FIND_DOWN, OnFindDownToggled)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDialog message handlers


BOOL CFindDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the dialog's whole-word and case indicators
	if ((theFrame->m_SearchFlags & FR_MATCHCASE) == FR_MATCHCASE) 
		m_ctrlCase.SetCheck(TRUE);
	if ((theFrame->m_SearchFlags & FR_WHOLEWORD) == FR_WHOLEWORD) 
		m_ctrlWholeWord.SetCheck(TRUE);

	// Set the dialog's up/down direction indicators
	if (theFrame->m_SearchDirectionDown) {
		m_ctrlDown.SetCheck(TRUE);
	}
	else {
		m_ctrlUp.SetCheck(TRUE);
	}

	// Set and highlight the dialog's 'find what' text
	m_ctrlWhat.SetWindowText(theFrame->m_SearchText);
	m_ctrlWhat.SetSel(0, -1);
	m_ctrlWhat.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CFindDialog::OnFind() 
{
	CCaosChildFrame *CaosFrame;
	DWORD SearchFlags;
	BOOL DirectionDown;
	char SearchText[200];
	BOOL Found;

	// Get the text to search for
	m_ctrlWhat.GetWindowText(SearchText, 200);
	if (strlen(SearchText) == 0) {
		::Beep(450, 100);
		::AfxMessageBox("Please enter some text to search for", 
			MB_ICONEXCLAMATION);
		// Hghlight the dialog's 'find what' text
		m_ctrlWhat.SetSel(0, -1);
		m_ctrlWhat.SetFocus();
		return;
	}

	// Get the case/whole-word flags
	SearchFlags = FR_DOWN;
	if (m_ctrlCase.GetCheck())
		SearchFlags |= FR_MATCHCASE;
	if (m_ctrlWholeWord.GetCheck())
		SearchFlags |= FR_WHOLEWORD;

	// Get the direction of the search
	DirectionDown = m_ctrlDown.GetCheck();

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)theFrame->MDIGetActive();
	Found = CaosFrame->m_ctrlEdit.Find(SearchText, SearchFlags, DirectionDown);

	if (Found) {
		// Pop down the Find dialog
		EndDialog(IDOK);
		// Store the search criteria
		theFrame->m_SearchText = SearchText;
		theFrame->m_SearchFlags = SearchFlags;
		theFrame->m_SearchDirectionDown = DirectionDown;
	}
	else {
		// Set and highlight the dialog's 'find what' text
		m_ctrlWhat.SetFocus();
		m_ctrlWhat.SetSel(0, -1);
		return;
	}
}


void CFindDialog::OnFindUpToggled() 
{
	if (m_ctrlUp.GetCheck()) {
		m_ctrlDown.SetCheck(FALSE);
	}
	else {
		m_ctrlDown.SetCheck(TRUE);
	}
}


void CFindDialog::OnFindDownToggled() 
{
	if (m_ctrlDown.GetCheck()) {
		m_ctrlUp.SetCheck(FALSE);
	}
	else {
		m_ctrlUp.SetCheck(TRUE);
	}
}

