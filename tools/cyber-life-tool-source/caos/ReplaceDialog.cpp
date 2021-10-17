// ReplaceDialog.cpp : implementation file
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
// CReplaceDialog dialog


CReplaceDialog::CReplaceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CReplaceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReplaceDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CReplaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplaceDialog)
	DDX_Control(pDX, IDC_REPLACE, m_ctrlReplace);
	DDX_Control(pDX, IDC_REPLACE_FIND, m_ctrlReplaceFind);
	DDX_Control(pDX, IDC_REPLACE_WHOLE_WORD, m_ctrlWholeWord);
	DDX_Control(pDX, IDC_REPLACE_WITH, m_ctrlWith);
	DDX_Control(pDX, IDC_REPLACE_WHAT, m_ctrlWhat);
	DDX_Control(pDX, IDC_REPLACE_CASE, m_ctrlCase);
	DDX_Control(pDX, IDC_REPLACE_SELECTION, m_ctrlSelection);
	DDX_Control(pDX, IDC_REPLACE_WHOLE_FILE, m_ctrlWholeFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReplaceDialog, CDialog)
	//{{AFX_MSG_MAP(CReplaceDialog)
	ON_BN_CLICKED(IDC_REPLACE, OnReplace)
	ON_BN_CLICKED(IDC_REPLACE_ALL, OnReplaceAll)
	ON_BN_CLICKED(IDC_REPLACE_FIND, OnReplaceFind)
	ON_BN_CLICKED(IDC_REPLACE_SELECTION, OnReplaceSelectionToggled)
	ON_BN_CLICKED(IDC_REPLACE_WHOLE_FILE, OnReplaceWholeFileToggled)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog message handlers


BOOL CReplaceDialog::OnInitDialog() 
{
	CCaosChildFrame *CaosChildFrame;

	CDialog::OnInitDialog();
	
	// Set the dialog's whole-word and case indicators
	if ((theFrame->m_SearchFlags & FR_MATCHCASE) == FR_MATCHCASE) 
		m_ctrlCase.SetCheck(TRUE);
	if ((theFrame->m_SearchFlags & FR_WHOLEWORD) == FR_WHOLEWORD) 
		m_ctrlWholeWord.SetCheck(TRUE);

	// Set and highlight the dialog's 'find what' text
	m_ctrlWhat.SetWindowText(theFrame->m_SearchText);
	m_ctrlWhat.SetSel(0, -1);
	m_ctrlWhat.SetFocus();
	// Set the dialog's 'what with' text
	m_ctrlWith.SetWindowText(theFrame->m_ReplaceText);

	// Get the active CAOS child frame
	CaosChildFrame = (CCaosChildFrame *)theFrame->MDIGetActive();

	if (CaosChildFrame->m_ctrlEdit.HasSelection()) {
		// Got a selection, so switch on the Selection indicator
		m_ctrlSelection.SetCheck(TRUE);
		// In selection mode, we can only do replace all
		m_ctrlReplace.EnableWindow(FALSE);
		m_ctrlReplaceFind.EnableWindow(FALSE);
	}
	else {
		// No selection, so switch on the Whole File indicator
		// and disable the Selection indicator
		m_ctrlWholeFile.SetCheck(TRUE);
		m_ctrlSelection.EnableWindow(FALSE);
	}
	return FALSE;  // return TRUE unless you set the focus to a control
}



void CReplaceDialog::OnReplaceFind() 
{
	CCaosChildFrame *CaosChildFrame;
	DWORD SearchFlags;
	char SearchText[200];
	CString s;
	BOOL Found;

	// Get the text to search for
	m_ctrlWhat.GetWindowText(SearchText, 200);
	if (strlen(SearchText) == 0) {
		::Beep(450, 100);
		::AfxMessageBox("Please enter some text to search for", MB_ICONEXCLAMATION);
		// Set and highlight the dialog's 'find what' text
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

	// Get the active CAOS child frame
	CaosChildFrame = (CCaosChildFrame *)theFrame->MDIGetActive();
	// Search for the text (in the down direction)
	Found = CaosChildFrame->m_ctrlEdit.Find(SearchText, SearchFlags, TRUE);
	if (Found) {
		// Store the search criteria
		theFrame->m_SearchText = SearchText;
		theFrame->m_SearchFlags = SearchFlags;
	}
	// Highlight the dialog's 'find what' text
	m_ctrlWhat.SetSel(0, -1);
	m_ctrlWhat.SetFocus();	
}



void CReplaceDialog::OnReplace() 
{	
	CCaosChildFrame *CaosChildFrame;
	DWORD SearchFlags;
	char SearchText[200];
	char ReplaceText[200];
	BOOL Replaced;

	// Get the text to search for
	m_ctrlWhat.GetWindowText(SearchText, 200);
	if (strlen(SearchText) == 0) {
		::Beep(450, 100);
		::AfxMessageBox("Please enter some text to search for", MB_ICONEXCLAMATION);
		m_ctrlWhat.SetFocus();
		return;
	}

	// Get the replacement text
	m_ctrlWith.GetWindowText(ReplaceText, 200);

	// Get the case/whole-word flags
	SearchFlags = FR_DOWN;
	if (m_ctrlCase.GetCheck())
		SearchFlags |= FR_MATCHCASE;
	if (m_ctrlWholeWord.GetCheck())
		SearchFlags |= FR_WHOLEWORD;

	// Get the active CAOS child frame
	CaosChildFrame = (CCaosChildFrame *)theFrame->MDIGetActive();
	// Try to replace the supplied text
	Replaced = CaosChildFrame->m_ctrlEdit.Replace(SearchText, ReplaceText, 
		SearchFlags);
	if (Replaced) {
		// Set the search and replace criteria
		theFrame->m_SearchText = SearchText;
		theFrame->m_ReplaceText = ReplaceText;
		theFrame->m_SearchFlags = SearchFlags;
	}
	// Highlight the dialog's 'find what' text
	m_ctrlWhat.SetSel(0, -1);
	m_ctrlWhat.SetFocus();	
}



void CReplaceDialog::OnReplaceAll() 
{
	CCaosChildFrame *CaosChildFrame;
	DWORD SearchFlags;
	char SearchText[200];
	char ReplaceText[200];
	int ReplaceCount;
	CString Contents;

	// Get the text to search for
	m_ctrlWhat.GetWindowText(SearchText, 200);
	if (strlen(SearchText) == 0) {
		::Beep(450, 100);
		::AfxMessageBox("Please enter some text to search for", MB_ICONEXCLAMATION);
		m_ctrlWhat.SetFocus();
		return;
	}

	// Get the replacement text
	m_ctrlWith.GetWindowText(ReplaceText, 200);
	

	// Get the case/whole-word flags
	SearchFlags = FR_DOWN;
	if (m_ctrlCase.GetCheck())
		SearchFlags |= FR_MATCHCASE;
	if (m_ctrlWholeWord.GetCheck())
		SearchFlags |= FR_WHOLEWORD;

	// Get the active CAOS child frame
	CaosChildFrame = (CCaosChildFrame *)theFrame->MDIGetActive();
	if (m_ctrlWholeFile.GetCheck()) {
		ReplaceCount = CaosChildFrame->m_ctrlEdit.ReplaceAll(SearchText, 
			ReplaceText, SearchFlags, TRUE);
	}
	else {
		ReplaceCount = CaosChildFrame->m_ctrlEdit.ReplaceAll(SearchText, 
			ReplaceText, SearchFlags, FALSE);
	}

	if (ReplaceCount > 0) {
		// Set the search and replace criteria
		theFrame->m_SearchText = SearchText;
		theFrame->m_ReplaceText = ReplaceText;
		theFrame->m_SearchFlags = SearchFlags;
	}
	// Highlight the dialog's 'find what' text
	m_ctrlWhat.SetSel(0, -1);
	m_ctrlWhat.SetFocus();	
}



void CReplaceDialog::OnReplaceSelectionToggled() 
{
	if (m_ctrlSelection.GetCheck()) {
		m_ctrlWholeFile.SetCheck(FALSE);
		m_ctrlReplace.EnableWindow(FALSE);
		m_ctrlReplaceFind.EnableWindow(FALSE);
	}
	else {
		m_ctrlWholeFile.SetCheck(TRUE);	
		m_ctrlReplace.EnableWindow(TRUE);
		m_ctrlReplaceFind.EnableWindow(TRUE);
	}
}


void CReplaceDialog::OnReplaceWholeFileToggled() 
{
	if (!m_ctrlSelection.IsWindowEnabled()) {
		// If the Selection indicator is disabled then the Whole File 
		// indicator must be forced to be on always
		m_ctrlWholeFile.SetCheck(TRUE);
		return;
	}

	if (m_ctrlWholeFile.GetCheck()) {
		m_ctrlSelection.SetCheck(FALSE);
		m_ctrlReplace.EnableWindow(TRUE);
		m_ctrlReplaceFind.EnableWindow(TRUE);
	}
	else {
		m_ctrlSelection.SetCheck(TRUE);	
		m_ctrlReplace.EnableWindow(FALSE);
		m_ctrlReplaceFind.EnableWindow(FALSE);
	}
}

