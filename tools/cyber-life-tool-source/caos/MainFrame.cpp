////////////////////////////////////////////////////////////////////////////////

// MainFrame.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "Caos.h"
#include "../../Common/WindowState.h"
#include "MacroParse.h"
#include "EditQuickMacros.h"

#include <fstream>
#include <sstream>
#include <Shlwapi.h> // path manipulation

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Globals
extern HICON hCaosFrameIcon;
extern HICON hErrorFrameIcon;
extern HICON hOutputFrameIcon;
extern HICON hScriptoriumFrameIcon;
extern CGameInterface theGame;
extern char CurrentDirectory[512];

#include "windows.h"

WINDOWPLACEMENT mwp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_DROPFILES()
	ON_WM_QUERYENDSESSION()
	ON_COMMAND(IDM_NEW, OnNewDocument)
	ON_COMMAND(IDM_OPEN, OnOpenDocument)
	ON_COMMAND(IDM_CLOSE, OnCloseDocument)
	ON_COMMAND(IDM_CLOSEALL, OnCloseAllDocuments)
	ON_COMMAND(IDM_SAVE, OnSaveDocument)
	ON_COMMAND(IDM_SAVEAS, OnSaveDocumentAs)
	ON_COMMAND(IDM_SAVEALL, OnSaveAllDocuments)
	ON_COMMAND(IDM_CUT, OnCut)
	ON_COMMAND(IDM_COPY, OnCopy)
	ON_COMMAND(IDM_PASTE, OnPaste)
	ON_COMMAND(IDM_SELECTALL, OnSelectAll)	
	ON_COMMAND(IDM_FIND, OnFind)
	ON_COMMAND(IDM_FIND_NEXT, OnFindNext)
	ON_COMMAND(IDM_REPLACE, OnReplace)
	ON_COMMAND(IDM_GOTO, OnGoto)
	ON_COMMAND(IDM_INDENT, OnIndent)
	ON_COMMAND(IDM_OUTDENT, OnOutdent)
	ON_COMMAND(IDM_COMMENT, OnComment)
	ON_COMMAND(IDM_UNCOMMENT, OnUncomment)
	ON_COMMAND(IDM_VIEW_ERROR_WINDOW, OnViewErrorWindow)
	ON_COMMAND(IDM_VIEW_OUTPUT_WINDOW, OnViewOutputWindow)
	ON_COMMAND(IDM_VIEW_SCRIPTORIUM_WINDOW, OnViewScriptoriumWindow)
	ON_COMMAND(IDM_INJECT_INSTALL, OnInjectInstall)
	ON_COMMAND(IDM_INJECT_EVENT, OnInjectEvent)
	ON_COMMAND(IDM_INJECT_INSTALL_AND_EVENT, OnInjectInstallAndEvent)
	ON_COMMAND(IDM_INJECT_REMOVE, OnInjectRemove)
	ON_COMMAND(IDM_COLLAPSE, OnCollapse)
	ON_COMMAND(IDM_EXPAND, OnExpand)
	ON_COMMAND(IDM_REFRESH, OnRefresh)
	ON_COMMAND(IDM_WRITE, OnWrite)
	ON_COMMAND(IDM_DELETE, OnDelete)
	ON_COMMAND(IDM_FETCH, OnFetch)
	ON_UPDATE_COMMAND_UI(IDM_SAVE, OnUpdateSave)
	ON_UPDATE_COMMAND_UI(IDM_SAVEAS, OnUpdateSaveAs)
	ON_UPDATE_COMMAND_UI(IDM_SAVEALL, OnUpdateSaveAll)
	ON_UPDATE_COMMAND_UI(IDM_CLOSE, OnUpdateClose)
	ON_UPDATE_COMMAND_UI(IDM_CLOSEALL, OnUpdateCloseAll)
	ON_UPDATE_COMMAND_UI(IDM_CUT, OnUpdateCut)
	ON_UPDATE_COMMAND_UI(IDM_COPY, OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(IDM_PASTE, OnUpdatePaste)	
	ON_UPDATE_COMMAND_UI(IDM_SELECTALL, OnUpdateSelectAll)
	ON_UPDATE_COMMAND_UI(IDM_FIND, OnUpdateFind)
	ON_UPDATE_COMMAND_UI(IDM_REPLACE, OnUpdateReplace)
	ON_UPDATE_COMMAND_UI(IDM_FIND_NEXT, OnUpdateFindNext)
	ON_UPDATE_COMMAND_UI(IDM_GOTO, OnUpdateGoto)
	ON_UPDATE_COMMAND_UI(IDM_COMMENT, OnUpdateComment)
	ON_UPDATE_COMMAND_UI(IDM_UNCOMMENT, OnUpdateUncomment)
	ON_UPDATE_COMMAND_UI(IDM_INDENT, OnUpdateIndent)
	ON_UPDATE_COMMAND_UI(IDM_OUTDENT, OnUpdateOutdent)
	ON_UPDATE_COMMAND_UI(IDM_REFORMAT, OnUpdateReformat)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_ERROR_WINDOW, OnUpdateViewErrorWindow)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_OUTPUT_WINDOW, OnUpdateViewOutputWindow)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_SCRIPTORIUM_WINDOW, OnUpdateViewScriptoriumWindow)
	ON_UPDATE_COMMAND_UI(IDM_INJECT_EVENT, OnUpdateInjectEvent)
	ON_UPDATE_COMMAND_UI(IDM_INJECT_INSTALL, OnUpdateInjectInstall)
	ON_UPDATE_COMMAND_UI(IDM_INJECT_REMOVE, OnUpdateInjectRemove)
	ON_UPDATE_COMMAND_UI(IDM_INJECT_INSTALL_AND_EVENT, OnUpdateInjectInstallAndEvent)
	ON_UPDATE_COMMAND_UI(IDM_COLLAPSE, OnUpdateCollapse)
	ON_UPDATE_COMMAND_UI(IDM_EXPAND, OnUpdateExpand)
	ON_UPDATE_COMMAND_UI(IDM_REFRESH, OnUpdateRefresh)
	ON_UPDATE_COMMAND_UI(IDM_DELETE, OnUpdateDelete)
	ON_UPDATE_COMMAND_UI(IDM_FETCH, OnUpdateFetch)
	ON_UPDATE_COMMAND_UI(IDM_WRITE, OnUpdateWrite)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_HELP_ALPHA, OnHelpAlpha)
	ON_COMMAND(ID_HELP_CATEG, OnHelpCateg)
	ON_WM_TIMER()
	ON_COMMAND(IDM_REFORMAT, OnReformat)
	ON_COMMAND(ID_INJECT_EDITQUICKMACROS, OnInjectEditquickmacros)
	ON_COMMAND(ID_INJECT_QUICKMACRO1, OnInjectQuickmacro1)
	ON_COMMAND(ID_INJECT_QUICKMACRO2, OnInjectQuickmacro2)
	ON_COMMAND(ID_INJECT_QUICKMACRO3, OnInjectQuickmacro3)
	ON_COMMAND(ID_INJECT_QUICKMACRO4, OnInjectQuickmacro4)
	ON_COMMAND(ID_INJECT_QUICKMACRO5, OnInjectQuickmacro5)
	ON_COMMAND(ID_INJECT_QUICKMACRO6, OnInjectQuickmacro6)
	ON_COMMAND(ID_INJECT_QUICKMACRO7, OnInjectQuickmacro7)
	ON_COMMAND(ID_INJECT_QUICKMACRO8, OnInjectQuickmacro8)
	ON_COMMAND(ID_INJECT_QUICKMACRO9, OnInjectQuickmacro9)
	ON_COMMAND(ID_INJECT_QUICKMACRO0, OnInjectQuickmacro0)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDM_BLANKS,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_CaosFrameCount = 0;	
	m_NextX = 0;
	m_NextY = 0;
	m_NextID = 1;
	m_ErrorFrame = NULL;
	m_OutputFrame = NULL;
	m_ScriptoriumFrame = NULL;
	m_SearchText = "";
	m_ReplaceText = "";
	m_SearchFlags = 0;
	m_SearchDirectionDown = TRUE;

	myDefaultLoadDirectory = theCAOSApp.GetProfileString("","Default Load Directory");
}


CMainFrame::~CMainFrame()
{
}





void CMainFrame::RemoveCaosFrameFromFrameList(int id) 
{
	CString Blanks;
	int i, j;
	for (i=0; i<m_CaosFrameCount; i++) {
		if (m_CaosFrameList[i]->m_ID == id) {
			break;
		}
	}
	for (j=i+1; j<m_CaosFrameCount; j++) {
		m_CaosFrameList[j-1] = m_CaosFrameList[j];
		m_CaosFrameList[j-1]->m_ID = j-1;
	}
	m_CaosFrameCount--;
	if (m_CaosFrameCount == 0) {
		// If no more CAOS frames, then reset the position variables 
		// and the unique ID
		m_NextX = 0;
		m_NextY = 0;
		m_NextID = 1;		
		// Clear the line number pane
		Blanks.LoadString(IDM_BLANKS);
		m_wndStatusBar.SetPaneText(1, Blanks, TRUE);
	}
}


















/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG



//
// General message handlers
//


bool CMainFrame::DoClose()
{
	CCaosChildFrame *CaosFrame;
	int i, temp;
	CCaosChildFrame *CaosFrameListCopy[500];

	// Copy the frame list
	for (i=0; i<m_CaosFrameCount; i++)
		CaosFrameListCopy[i] = m_CaosFrameList[i];

	temp = m_CaosFrameCount;
	for (i=0; i<temp; i++) {
		CaosFrame = CaosFrameListCopy[i];
		// Activate the frame (just in case the user cancels)
		MDIActivate(CaosFrame);
		// Ask the frame to close itself
		CaosFrame->SendMessage(WM_CLOSE, 0, 0);	
		// The member variable "m_Cancelled" gets set if the user cancels
		// the close operation
		if (m_Cancelled)
			// Abort the Close All operation
			return TRUE;
	}
	return FALSE;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	WindowState::Load("Windows", "Main Frame", m_hWnd);

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Create the syntax error window by default
	if (AfxGetApp()->GetProfileInt("Windows", "Error Output Visible", NULL) != 0)
		CreateErrorWindow();
	if (AfxGetApp()->GetProfileInt("Windows", "Script Output Visible", NULL) != 0)
		CreateOutputWindow();

	m_defaultTitleString.LoadString(IDR_MAINFRAME);

	SetTimer(0,500,NULL);

	return 0;
}



BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Place the main application frame in the middle of the screen. 
	// Allow the window to fit on an 800x600 resolution
    int cxscreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyscreen = ::GetSystemMetrics(SM_CYSCREEN);
	cs.x = (cxscreen - 770) / 2;
	cs.y = (cyscreen - 570) / 2;
	cs.cx = 770;
	cs.cy = 570;
	return CMDIFrameWnd::PreCreateWindow(cs);
}



void CMainFrame::OnClose() 
{
	bool Cancelled = DoClose();
	if (!Cancelled)
	{
		// Close all other windows to force them to save state
		AfxGetApp()->WriteProfileInt("Windows", "Error Output Visible", m_ErrorFrame ? 1 : 0);
		AfxGetApp()->WriteProfileInt("Windows", "Script Output Visible", m_OutputFrame ? 1 : 0);
		if (m_ErrorFrame)
			OnViewErrorWindow();
		if (m_OutputFrame != NULL)
			OnViewOutputWindow();
		if (m_ScriptoriumFrame != NULL)
			OnViewScriptoriumWindow();

		WindowState::Save("Windows", "Main Frame", m_hWnd);
		// Close the main frame
		CMDIFrameWnd::OnClose();
	}
}


void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CScriptoriumChildFrame *ScriptoriumFrame;
	CCaosChildFrame *CaosFrame;
	CChildFrame *ChildFrame;
	CMDIChildWnd *ActiveChildFrame;

	// Call the default
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
	// Get the active child frame
	ActiveChildFrame = MDIGetActive(NULL);

	if (nState == WA_INACTIVE)
		return;

	if (!ActiveChildFrame)
		return;

	// Depending on the child frame that is being activated, set the
	// focus to its kid
	if (ActiveChildFrame->IsKindOf(RUNTIME_CLASS(CScriptoriumChildFrame))) {
		ScriptoriumFrame = (CScriptoriumChildFrame *)ActiveChildFrame;
		ScriptoriumFrame->m_ctrlTree.SetFocus();
	}
	else if (ActiveChildFrame->IsKindOf(RUNTIME_CLASS(CCaosChildFrame))) {
		CaosFrame = (CCaosChildFrame *)ActiveChildFrame;
		CaosFrame->m_ctrlEdit.SetFocus();
	}
	else if (ActiveChildFrame->IsKindOf(RUNTIME_CLASS(CChildFrame))) {
		ChildFrame = (CChildFrame *)ActiveChildFrame;
		ChildFrame->m_ctrlEdit.SetFocus();
	}
}



void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	int FileCount;
	int i;
	char FilenameBuffer[512];
	CString Filename;

	// How many files are being dropped?
	FileCount = ::DragQueryFile(hDropInfo, -1, NULL, 0);

	// Clear the error output window
	if (m_ErrorFrame) {
		m_ErrorFrame->m_ctrlEdit.SetWindowText("");
	}

	for (i=0; i<FileCount; i++) {
		// Get the filename of each dropped file
		::DragQueryFile(hDropInfo, i, FilenameBuffer, 
			sizeof(FilenameBuffer));
		Filename = FilenameBuffer;
		// Test that the filename has the proper extension
		if (Filename.GetLength() < 5)
			continue;
		Filename.MakeLower();
		if (Filename.Right(4) != ".cos")
			continue;
		// File OK, so open it and add the syntax errors on to the end
		// of the error window
		DoOpen(Filename, TRUE);
	}

	// Clean up the drag'n'drop operation
	::DragFinish(hDropInfo);
}



BOOL CMainFrame::OnQueryEndSession() 
{
	BOOL Cancelled;

	if (!CMDIFrameWnd::OnQueryEndSession())
		return FALSE;

	Cancelled = DoClose();
	if (Cancelled)
		return FALSE;
	else
		return TRUE;
}



//
// File menu implementation
//


CCaosChildFrame *CMainFrame::DoNew() 
{
	int CaosFrameCX, CaosFrameCY;
	int MainFrameCX, MainFrameCY;
	CRect MainFrameClient;
	CCaosChildFrame *CaosFrame;
	CString Caption;
	CRect EditRect;

	// Get the dimensions of the main MDI frame
	GetClientRect(&MainFrameClient);
	MainFrameCX = MainFrameClient.Width();
	MainFrameCY = MainFrameClient.Height();
	// Set up the dimensions of the new CAOS frame
	CaosFrameCX = 450;
	CaosFrameCY = 210;
	
	// Wrap-around the base location of the new frame if need be
	if (m_NextX + CaosFrameCX > MainFrameCX) {
		m_NextX = 0;
		m_NextY = 0;
	}
	if (m_NextY + CaosFrameCY > MainFrameCY) {
		m_NextX = 0;
		m_NextY = 0;
	}
	
	CRect CaosRect(m_NextX, m_NextY, m_NextX+CaosFrameCX, m_NextY+CaosFrameCY);

	CaosFrame = new CCaosChildFrame;
	CaosFrame->Create(NULL, "", WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CaosRect, this, NULL);
	CaosFrame->SetIcon(hCaosFrameIcon, FALSE);
	CaosFrame->GetClientRect(&EditRect);	
	CaosFrame->m_ctrlEdit.CreateVersion20(EditRect, CaosFrame, IDC_MACRO_EDIT);
	CaosFrame->m_Filename = "";
	CaosFrame->m_ID = m_CaosFrameCount;
	// Move the default location down diagonally
	m_NextX += 25;
	m_NextY += 25;
	// Store the CAOS frame in the frame list
	m_CaosFrameList[m_CaosFrameCount] = CaosFrame;
	m_CaosFrameCount++;
	// Set the focus to the edit control
	CaosFrame->m_ctrlEdit.SetFocus();
	return CaosFrame;
}



void CMainFrame::DoOpen(CString Filename, BOOL AddSyntaxErrorsToEnd) 
{
	int i;
	CString Message;
	int Result;
	CString Title;
	CCaosChildFrame *CaosFrame;
	CString Ext;
	CPtrArray TokenList;
	int  TokenCount;
	CPtrArray  CommandList;
	int   CommandCount;
	CUIntArray ErrorIndexList;
	int  ErrorCount;
	int TempErrorCount;
	CFileStatus fstatus;
	CFile f;
	CString Contents;
	CString TempContents;
	CString ErrorString;
	CString Line;
	CommandStructure *Command;
	BOOL OK;
	int Len;
	char *MacroCode;
	CWaitCursor WaitCursor;

	// Try to open the file for reading
	OK = f.Open(Filename, CFile::modeRead);
	if (!OK) {
		::Beep(450, 100);
		Message.Format("Unable to open file '%s' for reading", Filename);
		::AfxMessageBox(Message, MB_OK | MB_ICONSTOP);
		GetApp()->RemoveFromRecentFileList(Filename);
		return;
	}
	f.GetStatus(fstatus);
	if (fstatus.m_attribute & 0x01 == 0x01) {
		::Beep(450, 100);
		Message.Format("You will not be able to save changes to '%s' because it is read-only. Continue?", Filename);
		Result = ::AfxMessageBox(Message, MB_YESNO | MB_ICONEXCLAMATION);
		if (Result == IDNO) {
			f.Close();
			return;
		}
	}

	GetApp()->AddToRecentFileList(Filename);

	WaitCursor.Restore();

	// Get the title of the filename
	Title = f.GetFileName();
	Len = f.GetLength();
	MacroCode = new char[Len+1];
	f.Read(MacroCode, Len);
	MacroCode[Len] = '\0';
	f.Close();

	// Create a new CAOS child frame
	CaosFrame = DoNew();
	CaosFrame->SetWindowText(Title);
	CaosFrame->m_Filename = Filename;
	CaosFrame->m_Title = Title;

	// Load the contents of the file into the edit control
	CaosFrame->m_ctrlEdit.SetContentsFromMacroCode(MacroCode, TokenList, 
		TokenCount, CommandList, CommandCount, ErrorIndexList, 
		ErrorCount);
	delete []MacroCode;

	Contents.Format("Opening '%s'...\r\n", Filename);
	TempErrorCount = ErrorCount;
	if (ErrorCount > 0) {
		if (ErrorCount > 100) {
			Contents += "(Too many errors. The first 100 appear below)\r\n";
			TempErrorCount = 100;
		}
		for (i=0; i<TempErrorCount; i++) {
			Command = (CommandStructure *)CommandList[ErrorIndexList[i]];
			ErrorString = Command->ErrorString;
			Line.Format("Line %d: ", Command->LineNumber);
			Contents += Line + ErrorString + "\r\n";
		}
	}

	if (ErrorCount == 0)
		TempContents = "Done.\r\n";
	else
		TempContents.Format("Done. %d syntax error(s) found.\r\n", 
			ErrorCount);
	Contents += TempContents;

	if (m_ErrorFrame) {
		if (AddSyntaxErrorsToEnd) {
			m_ErrorFrame->m_ctrlEdit.GetWindowText(TempContents);
			TempContents += Contents;
			m_ErrorFrame->m_ctrlEdit.SetWindowText(TempContents);
		}
		else
			m_ErrorFrame->m_ctrlEdit.SetWindowText(Contents);
	}

	if (ErrorCount > 0) {
		::Beep(450, 100);
		Message.Format("%d syntax error(s) found in '%s'", ErrorCount, Filename);
		::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
		// Shift the focus to the edit control
		CaosFrame->m_ctrlEdit.SetFocus();
	}

	{
		// need some refresh thing to fix scroll bar here

	//	CaosFrame->m_ctrlEdit.SetSel(0, 0);
	//	CaosFrame->m_ctrlEdit.ReplaceSel(_T(" "));
	//	CaosFrame->m_ctrlEdit.Undo();
		// CaosFrame->m_ctrlEdit.Paste();
/*		CRect rect;
		CaosFrame->GetWindowRect(rect);
		CaosFrame->MoveWindow(rect);*/
	}
}



bool CMainFrame::DoSave(CCaosChildFrame *CaosFrame)
{
	CString Message;
	char *FilenameBuffer;
	CFileStatus fstatus;
	BOOL Exist;
	BOOL OK;
	CWaitCursor WaitCursor;
	
	// Get the filename
	FilenameBuffer = CaosFrame->m_Filename.GetBuffer(1);
	// Check read-only attributes
	Exist = CFile::GetStatus(FilenameBuffer, fstatus);
	if ((Exist) && (fstatus.m_attribute & 0x01 == 0x01)) {
		::Beep(450, 100);
		Message.Format("Unable to save to '%s' because it is read-only.",
			FilenameBuffer);
		::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
		// Shift the focus to the edit control
		CaosFrame->m_ctrlEdit.SetFocus();
		CaosFrame->m_Filename.ReleaseBuffer();
		return false;
	}
	OK = CaosFrame->m_ctrlEdit.Save(FilenameBuffer);
	CaosFrame->m_Filename.ReleaseBuffer();
	if (!OK) {
		::Beep(450, 100);
		Message.Format("Error while saving file '%s'", 
			CaosFrame->m_Filename);
		::AfxMessageBox(Message, MB_OK | MB_ICONSTOP);
		// Shift the focus to the edit control
		CaosFrame->m_ctrlEdit.SetFocus();
		return false;
	}
		
	// The document has been saved, so get rid of the "*" if present 
	CaosFrame->SetWindowText(CaosFrame->m_Title);
	if (CaosFrame->GetWindowPlacement(&mwp),mwp.showCmd == SW_SHOWNORMAL)
		SetWindowText(m_defaultTitleString + " - " + CaosFrame->m_Title);
	else
		SetWindowText(m_defaultTitleString);
	return true;
}


bool CMainFrame::DoSaveAs(CCaosChildFrame *CaosFrame)
{
	int Result;
	CString Filename;
	CString Title;
	CString Filter;
	DWORD Flags;
	CString DefaultFilename;
	CString Message;
	CString Ext;
	BOOL BadName;
	BOOL OK;
	BOOL Exist;
	char *FilenameBuffer;
	CFileStatus fstatus;


	// Get the default filename
	if (CaosFrame->m_Filename.IsEmpty()) {
		DefaultFilename.Format("%s.cos", CaosFrame->m_Title);
	}
	else {
		DefaultFilename = CaosFrame->m_Title;
	}

	Filter = "Creatures Object Source Files (*.cos)|*.cos|All Files (*.*)|*.*||";
	Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES |
		OFN_OVERWRITEPROMPT;
	CFileDialog FileDialog(FALSE, NULL, DefaultFilename, Flags, Filter, 
		this); 

	do {
		Result = FileDialog.DoModal();
		// Get rid of the dialog image
		UpdateWindow();
		if (Result == IDCANCEL) {
			// Shift the focus to the edit control
			CaosFrame->m_ctrlEdit.SetFocus();
			return false;
		}
		// Get the filename and title 
		Filename = FileDialog.GetPathName();
		Title = FileDialog.GetFileName();
		// Check the extension
		BadName = FALSE;
		Ext = FileDialog.GetFileExt();
		Ext.MakeLower();
		if (Ext == "") {
			Ext = "cos";
			Filename += ".cos";
			Title += ".cos";
		}
		if (Ext != "cos") {
			::Beep(450, 100);
			::AfxMessageBox("The chosen file does not have the proper extension. Please choose a different file."
				, MB_OK | MB_ICONEXCLAMATION);
			BadName = TRUE;
		}
		else {
			// Check read-only attributes
			Exist = CFile::GetStatus(Filename, fstatus);
			if ((Exist) && (fstatus.m_attribute & 0x01 == 0x01)) {
				::Beep(450, 100);
				Message.Format("Unable to save to file '%s' because it is read-only. Please use a different filename.", Filename);
				::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
				BadName = TRUE;
			}
		}
	} while (BadName);

	CWaitCursor WaitCursor;

	FilenameBuffer = Filename.GetBuffer(1);
	OK = CaosFrame->m_ctrlEdit.Save(FilenameBuffer);
	Filename.ReleaseBuffer();
	if (!OK) {
		::Beep(450, 100);
		Message.Format("Error while saving file '%s'", Filename);
		::AfxMessageBox(Message, MB_OK | MB_ICONSTOP);
		// Shift the focus to the edit control
		CaosFrame->m_ctrlEdit.SetFocus();
		return false;
	}
	GetApp()->AddToRecentFileList(Filename);
	CaosFrame->m_Filename = Filename;
	CaosFrame->m_Title = Title;
	// The document has been saved, so get rid of the "*" if present 
	CaosFrame->SetWindowText(Title);
	if (CaosFrame->GetWindowPlacement(&mwp),mwp.showCmd == SW_SHOWNORMAL)
		SetWindowText(m_defaultTitleString + " - " + CaosFrame->m_Title);
	else
		SetWindowText(m_defaultTitleString);
	// Shift the focus to the edit control
	CaosFrame->m_ctrlEdit.SetFocus();
	return true;
}


bool CMainFrame::DoGenericSave(CCaosChildFrame * CaosFrame) 
{
	if (CaosFrame->m_Filename.IsEmpty())
		return DoSaveAs(CaosFrame);
	else {
		return DoSave(CaosFrame);
	}
}


void CMainFrame::OnNewDocument() 
{
	CString Caption;
	CCaosChildFrame *CaosFrame;

	// Create a new CAOS child frame
	CaosFrame = DoNew();
	// Assign a unique title to the new frame
	Caption.Format("cos%d", m_NextID);
	CaosFrame->SetWindowText(Caption);
	CaosFrame->m_Title = Caption;
	// Advance the unique ID
	m_NextID++;
	CString mytitle;
	mytitle = m_defaultTitleString + " - "+Caption;
	SetWindowText(mytitle);
}



void CMainFrame::OnOpenDocument() 
{
	int Result;
	CString Filter;
	DWORD Flags;
	CString Title;
	CString Ext;

	Filter = "Creatures Object Source Files (*.cos)|*.cos|All Files (*.*)|*.*||";
	Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | 
		OFN_LONGNAMES | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	CFileDialog FileDialog(TRUE, NULL, (LPCTSTR)myDefaultLoadDirectory, Flags, Filter, this); 

	// Pop up the Open dialog
	Result = FileDialog.DoModal();
	// Get rid of the dialog image
	UpdateWindow();
	if (Result == IDCANCEL) {
		return;
	}

	if (!FileDialog.GetPathName().IsEmpty())
	{
		myDefaultLoadDirectory = FileDialog.GetPathName();
		theCAOSApp.WriteProfileString("","Default Load Directory",myDefaultLoadDirectory);
	}

	// Get the chosen filenames
	POSITION pos = FileDialog.GetStartPosition();
	while (pos != NULL)
	{
		CString Filename = FileDialog.GetNextPathName(pos);

		// Open this file
		DoOpen(Filename, FALSE);
	}
}


void CMainFrame::OnCloseDocument()
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);

	// Send a close message to the CAOS frame. This will
	// be caught by the frame and if the document is unsaved then 
	// the user will be given the chance to save it, not save it or 
	// abort the close
	CaosFrame->SendMessage(WM_CLOSE, 0, 0);
}


void CMainFrame::OnCloseAllDocuments() 
{
	DoClose();
}



void CMainFrame::OnSaveDocument() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	// Save the document contained in the frame
	DoGenericSave(CaosFrame);
}


void CMainFrame::OnSaveDocumentAs() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	// Save the document oontained in the frame under a new name
	DoSaveAs(CaosFrame);
}


void CMainFrame::OnSaveAllDocuments() 
{
	CCaosChildFrame *CaosFrame;
	int i;

	for (i=0; i<m_CaosFrameCount; i++) {
		// Get each open CAOS frame
		CaosFrame = m_CaosFrameList[i];
		// Save each frame's document
		DoGenericSave(CaosFrame);
	}
}




//
// Edit menu implementation
//



void CMainFrame::OnCut() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.Cut();
}


void CMainFrame::OnCopy() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.Copy();
}


void CMainFrame::OnPaste() 
{
	CCaosChildFrame *CaosFrame;
	CWaitCursor WaitCursor;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.Paste();	
}


void CMainFrame::OnSelectAll() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.SelectAll();
}


void CMainFrame::OnFind() 
{
	CCaosChildFrame * CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	CFindDialog FindDialog(this);	
	// Pop up the Find dialog box
	FindDialog.DoModal();
	// Get rid of the dialog image
	UpdateWindow();
	CaosFrame->m_ctrlEdit.SetFocus();
}


void CMainFrame::OnFindNext() 
{
	CCaosChildFrame *CaosFrame;
	char *SearchTextBuffer;
	BOOL Found;

	if (m_SearchText.IsEmpty()) {
		OnFind();
		return;
	}
	SearchTextBuffer = m_SearchText.GetBuffer(1);
	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	Found = CaosFrame->m_ctrlEdit.Find(SearchTextBuffer, m_SearchFlags,
		m_SearchDirectionDown);
	m_SearchText.ReleaseBuffer();
	CaosFrame->m_ctrlEdit.SetFocus();
}


void CMainFrame::OnReplace() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	CReplaceDialog ReplaceDialog(this);	
	// Pop up the Replace dialog box
	ReplaceDialog.DoModal();
	// Get rid of the dialog image
	UpdateWindow();
	CaosFrame->m_ctrlEdit.SetFocus();
}


void CMainFrame::OnGoto() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	CGotoDialog GotoDialog(this);	
	// Pop up the Goto dialog box
	GotoDialog.DoModal();
	// Get rid of the dialog image
	UpdateWindow();
	CaosFrame->m_ctrlEdit.SetFocus();
}


void CMainFrame::OnIndent() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.BlockIndent();
}


void CMainFrame::OnOutdent() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.BlockOutdent();	
}


void CMainFrame::OnComment() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.BlockComment();
}


void CMainFrame::OnUncomment() 
{
	CCaosChildFrame *CaosFrame;

	// Get the active CAOS child frame
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
	CaosFrame->m_ctrlEdit.BlockUncomment();
}


void CMainFrame::OnReformat() 
{
	CCaosChildFrame *CaosFrame;
	CString Contents;
	int TempErrorCount;
	CPtrArray TokenList;
	int TokenCount;
	CPtrArray CommandList;
	int CommandCount;
	CUIntArray ErrorIndexList;
	int ErrorCount;
	CString Temp;
	CString TempContents;
	CString ErrorString;
	CString Line;
	CString Message;
	int i;
	CommandStructure *Command;
	CString Title;
	CWaitCursor WaitCursor;

	// Parse and format the contents of the active CAOS document
	CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	CaosFrame->m_ctrlEdit.CheckContents(TokenList, TokenCount, 
		CommandList, CommandCount, ErrorIndexList, ErrorCount);

	Title = CaosFrame->m_Title;
	Contents.Format("Formatting '%s'...\r\n", Title);
	TempErrorCount = ErrorCount;
	if (ErrorCount > 0) {	
		if (ErrorCount > 100) {
			Contents += "(Too many errors. The first 100 appear below)\r\n";
			TempErrorCount = 100;
		}
		for (i=0; i<TempErrorCount; i++) {
			Command = (CommandStructure *)CommandList[ErrorIndexList[i]];
			ErrorString = Command->ErrorString;
			Line.Format("Line %d: ", Command->LineNumber);
			Contents += Line + ErrorString + "\r\n";
		}
	}

	if (ErrorCount == 0)
		TempContents = "Done.\r\n";
	else
		TempContents.Format("Done. %d syntax error(s) found.\r\n",
			ErrorCount);
	Contents += TempContents;

	if (m_ErrorFrame)
		m_ErrorFrame->m_ctrlEdit.SetWindowText(Contents);

	if (ErrorCount > 0) {
		::Beep(450, 100);
		Message.Format("%d syntax error(s) found in '%s'", ErrorCount, Title);
		::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
		CaosFrame->m_ctrlEdit.SetFocus();
	}
}




// 
// View menu implementation
//



void CMainFrame::CreateErrorWindow() 
{
	int cx = 490;
	int cy = 150;
	int w, h;
	int x, y;
	CRect rect;
	CHARFORMAT DefaultCharFormat;

	GetClientRect(&rect);
	w = rect.Width() - 60;
	h = rect.Height() - 60;
	x = 5;
	y = h - cy;
	if (y < 0)
		y = 5;

	rect.SetRect(x, y, x+cx, y+cy);

	m_ErrorFrame = new CChildFrame;
	m_ErrorFrame->Create(NULL, "Error Output", WS_CHILD | WS_VISIBLE | 
		WS_OVERLAPPEDWINDOW, rect, this, NULL);
	m_ErrorFrame->GetClientRect(&rect);	
	m_ErrorFrame->m_ctrlEdit.Create(WS_HSCROLL | WS_VSCROLL | 
		ES_NOHIDESEL | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | 
		ES_AUTOVSCROLL | WS_CHILD | WS_VISIBLE, rect, m_ErrorFrame, 
		IDC_ERROR_EDIT);
	m_ErrorFrame->m_ChildFrameType = 1;
	DefaultCharFormat.cbSize = sizeof(CHARFORMAT);
	DefaultCharFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE |
		CFM_STRIKEOUT | CFM_PROTECTED | CFM_SIZE |
		CFM_COLOR | CFM_FACE | CFM_OFFSET | CFM_CHARSET;
	DefaultCharFormat.dwEffects = 0;
	DefaultCharFormat.yHeight = 200;
	DefaultCharFormat.yOffset = 0;
	DefaultCharFormat.bCharSet = ANSI_CHARSET;
	DefaultCharFormat.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
	strcpy(DefaultCharFormat.szFaceName, "Courier");
	DefaultCharFormat.crTextColor = RGB(0,0,0);
	m_ErrorFrame->m_ctrlEdit.SetDefaultCharFormat(DefaultCharFormat);
	m_ErrorFrame->SetIcon(hErrorFrameIcon, FALSE);
	m_ErrorFrame->m_ctrlEdit.SetFocus();
	CCaosApp::DoBackgroundColour(m_ErrorFrame->m_ctrlEdit);
}


void CMainFrame::CreateOutputWindow() 
{
	CRect rect;
	int cx = 250;
	int cy = 150;
	int w, h;
	int x, y;
	CHARFORMAT DefaultCharFormat;

	GetClientRect(&rect);
	w = rect.Width();
	h = rect.Height() - 60;
	x = w - cx - 10;
	if (x<0)
		x = 15;
	y = h - cy;
	if (y<0)
		y = 15;
	rect.SetRect(x, y, x+cx, y+cy);

	m_OutputFrame = new CChildFrame;
	m_OutputFrame->Create(NULL, "Script Output", WS_CHILD | WS_VISIBLE | 
		WS_OVERLAPPEDWINDOW, rect, this, NULL);
	m_OutputFrame->GetClientRect(&rect);	
	m_OutputFrame->m_ctrlEdit.Create(WS_HSCROLL | WS_VSCROLL |
		ES_NOHIDESEL | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | 
		ES_AUTOVSCROLL | WS_CHILD | WS_VISIBLE, rect, m_OutputFrame, 
		IDC_OUTPUT_EDIT);
	m_OutputFrame->m_ChildFrameType = 2;
	DefaultCharFormat.cbSize = sizeof(CHARFORMAT);
	DefaultCharFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE |
		CFM_STRIKEOUT | CFM_PROTECTED | CFM_SIZE |
		CFM_COLOR | CFM_FACE | CFM_OFFSET | CFM_CHARSET;
	DefaultCharFormat.dwEffects = 0;
	DefaultCharFormat.yHeight = 200;
	DefaultCharFormat.yOffset = 0;
	DefaultCharFormat.bCharSet = ANSI_CHARSET;
	DefaultCharFormat.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
	strcpy(DefaultCharFormat.szFaceName, "Courier");
	DefaultCharFormat.crTextColor = RGB(0,0,0);
	m_OutputFrame->m_ctrlEdit.SetDefaultCharFormat(DefaultCharFormat);
	m_OutputFrame->SetIcon(hOutputFrameIcon, FALSE); 
	m_OutputFrame->m_ctrlEdit.SetFocus();
	CCaosApp::DoBackgroundColour(m_OutputFrame->m_ctrlEdit);
}



bool CMainFrame::CreateScriptoriumWindow() 
{
	CRect rect;
	int cx = 310;
	int cy = 157;
	CRect client;
	CRect treesize;
	CBitmap bm;
	BOOL OK;
	CWaitCursor WaitCursor;

	OK = theGame.Connect();
	if (!OK) {
		::Beep(450, 100);
		::AfxMessageBox("Unable to establish a connection with the game", 
			MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	WaitCursor.Restore();

	rect.SetRect(0, 0, cx, cy);
	m_ScriptoriumFrame = new CScriptoriumChildFrame;
	m_ScriptoriumFrame->Create(NULL, "Viewer - Scriptorium", WS_CHILD |
		WS_VISIBLE |
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | 
		WS_MINIMIZEBOX |WS_THICKFRAME, rect, this, NULL);
	m_ScriptoriumFrame->SetIcon(hScriptoriumFrameIcon, FALSE); 	
	m_ScriptoriumFrame->GetClientRect(&client);
	m_ScriptoriumFrame->m_ctrlTree.Create(WS_CHILD | WS_VISIBLE | 
		WS_VSCROLL | TVS_HASLINES | TVS_HASBUTTONS | 
		TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS, client, 
		m_ScriptoriumFrame, IDC_TREE);
	m_ScriptoriumFrame->m_ImageList.Create(16, 16, ILC_COLOR24, 5, 1);
	bm.LoadBitmap(IDB_IMAGELIST);
	m_ScriptoriumFrame->m_ImageList.Add(&bm, CLR_NONE);
	m_ScriptoriumFrame->m_ctrlTree.SetImageList(&(m_ScriptoriumFrame->m_ImageList),
		TVSIL_NORMAL);
	m_ScriptoriumFrame->m_ctrlTree.SetIndent(35);
	m_ScriptoriumFrame->LoadTree();
	m_ScriptoriumFrame->m_ctrlTree.SetFocus();
	theGame.Disconnect();
	return TRUE;
}



void CMainFrame::OnViewErrorWindow() 
{
	if (m_ErrorFrame) {
		m_ErrorFrame->SendMessage(WM_CLOSE, 0, 0);
		m_ErrorFrame = NULL;
	}
	else {
		CreateErrorWindow();
		MDIActivate(m_ErrorFrame);
	}
}


void CMainFrame::OnViewOutputWindow() 
{
	if (m_OutputFrame != NULL) {
		m_OutputFrame->SendMessage(WM_CLOSE, 0, 0);
		m_OutputFrame = NULL;
	}
	else {
		CreateOutputWindow();
		MDIActivate(m_OutputFrame);
	}
}


void CMainFrame::OnViewScriptoriumWindow() 
{
	bool OK;

	if (m_ScriptoriumFrame != NULL) {
		m_ScriptoriumFrame->SendMessage(WM_CLOSE, 0, 0);
		m_ScriptoriumFrame = NULL;
	}
	else {
		OK = CreateScriptoriumWindow();
		if (OK) 
			MDIActivate(m_ScriptoriumFrame);
	}
}





// 
// Inject menu implementation
//


void CMainFrame::DoInject(int InjectType)
{
	CCaosChildFrame *CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	if (!CaosFrame)
		return;

	AutoSave(CaosFrame);

	{
		CWaitCursor WaitCursor;

		// Parse and format the contents of the active CAOS document
		int ErrorCount;
		CPtrArray TokenList;
		int TokenCount;
		CPtrArray CommandList;
		int CommandCount;
		CUIntArray ErrorIndexList;
		CaosFrame->m_ctrlEdit.CheckContents(TokenList, TokenCount, 
			CommandList, CommandCount, ErrorIndexList, ErrorCount);
		CString Title = CaosFrame->m_Title;
		CString Contents;
		Contents.Format("Checking '%s'...\r\n", Title);
		// Display syntax errors in the error window (if it exists)
		int TempErrorCount = ErrorCount;
		if (ErrorCount > 0) {	
			if (ErrorCount > 100) {
				Contents += "(Too many errors. The first 100 appear below)\r\n";
				TempErrorCount = 100;
			}
			for (int i=0; i<TempErrorCount; i++) {
				CommandStructure *Command = (CommandStructure *)CommandList[ErrorIndexList[i]];
				CString ErrorString = Command->ErrorString;
				CString Line;
				Line.Format("Line %d: ", Command->LineNumber);
				Contents += Line + ErrorString + "\r\n";
			}
		}

		CString TempContents;
		if (ErrorCount == 0)
			TempContents = "Done.\r\n";
		else
			TempContents.Format("Done. %d syntax error(s) found.\r\n", 
				ErrorCount);
		Contents += TempContents;
		if (m_ErrorFrame)
			m_ErrorFrame->m_ctrlEdit.SetWindowText(Contents);

		if (ErrorCount > 0) {
			::Beep(450, 100);
			CString Message;
			Message.Format("%d syntax error(s) found in '%s'", ErrorCount, 
				Title);
			::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
			CaosFrame->m_ctrlEdit.SetFocus();
			return;
		}

		// Try to connect to the game
		BOOL OK = theGame.Connect();
		if (!OK) {
			::Beep(450, 100);
			::AfxMessageBox("Unable to establish a connection with the game", 
				MB_OK | MB_ICONSTOP);
			CaosFrame->m_ctrlEdit.SetFocus();
			return;
		}

		// Get script details from the last parse
		CPtrArray ScriptList;
		int ScriptCount;
		CaosFrame->m_ctrlEdit.GetScriptInformation(ScriptList, ScriptCount);

		// Do all event scripts BEFORE any inject script
		if ((InjectType & EVENT_SCRIPT) == EVENT_SCRIPT) {

			// Check for repeating script
			{
				for	(int i=2; i<ScriptCount; i++) {
					ScriptStructure *Script = (ScriptStructure *)ScriptList[i];
					for	(int j=i+1; j<ScriptCount; j++) {
						ScriptStructure *Script2 = (ScriptStructure *)ScriptList[j];
						if (Script->FGSEValues[0] == Script2->FGSEValues[0] &&
							Script->FGSEValues[1] == Script2->FGSEValues[1] &&
							Script->FGSEValues[2] == Script2->FGSEValues[2] &&
							Script->FGSEValues[3] == Script2->FGSEValues[3])
						{
							// Found two the same
							::Beep(450, 100);
							CString script_name;
							script_name.Format("Warning: Script %d %d %d %d", Script->FGSEValues[0],
									Script->FGSEValues[1], Script->FGSEValues[2], Script->FGSEValues[3]);
							CString Contents = script_name + " appears at least twice in your COS file.\r\n";
							OutputText(Contents);
							// Disconnect from the game
							theGame.Disconnect();
							CaosFrame->m_ctrlEdit.SetFocus();
							return;
						}
					}
				}
			}

			// inject scripts
			for	(int i=2; i<ScriptCount; i++) {
				ScriptStructure *Script = (ScriptStructure *)ScriptList[i];
				if (m_ScriptoriumFrame) {
					// Add the script to the Scriptorium viewer
					m_ScriptoriumFrame->AddFGSE(Script->FGSEValues);
				}
				CString InjectString;
				InjectString.Format("scrp %d %d %d %d\n", Script->FGSEValues[0],
					Script->FGSEValues[1], Script->FGSEValues[2], Script->FGSEValues[3]);
				InjectString += Script->InjectString;
				
				std::string reply;
				BOOL OK = theGame.Inject((LPCTSTR)InjectString, reply, false);
				CString InjectReply = reply.c_str();
				
				if (!OK) {
					::Beep(450, 100);
					CString script_name;
					script_name.Format("Script %d %d %d %d", Script->FGSEValues[0],
							Script->FGSEValues[1], Script->FGSEValues[2], Script->FGSEValues[3]);
					CString Contents = script_name + " caused an error or exception:\r\n" +
						InjectReply + "\r\n";
					OutputText(Contents);
					// Disconnect from the game
					theGame.Disconnect();
					CaosFrame->m_ctrlEdit.SetFocus();

					return;		
				}
			}
		}

		if ((InjectType & INSTALL_SCRIPT) == INSTALL_SCRIPT)
		{
			if (!InjectOneScript(ScriptList, 0, "Install"))
				return;
		}

		if ((InjectType & REMOVE_SCRIPT) == REMOVE_SCRIPT)
		{
			if (!InjectOneScript(ScriptList, 1, "Remove"))
				return;
		}
		
		// Diconnect from the game
		theGame.Disconnect();

		// Restore the wait cursor
	}
}

BOOL CMainFrame::InjectOneScript(CPtrArray& ScriptList, int script_no, std::string script_name)
{
	BOOL OK = TRUE;

	ScriptStructure *Script = (ScriptStructure *)ScriptList[script_no];
	if (Script->InjectString == NULL)
	{
	}
	else if (Script->InjectString[0] == '\0')
	{
	}
	else {
		// Inject the script and get the reply
		CString InjectString = CString("execute\n") + Script->InjectString;

		std::string reply;
		OK = theGame.Inject((LPCTSTR)InjectString, reply, false);
		CString InjectReply = reply.c_str();
		
		CString Contents;
		bool bNoPopup = false;
		if (!OK) {
			::Beep(450, 100);
			Contents.Format("%s script caused an error or exception:\r\n%s\r\n",
				script_name.c_str(), InjectReply);
			// Disconnect from the game
			theGame.Disconnect();
		}
		else
		{
			if (*InjectReply != '\0') {
				Contents.Format("%s script reply:\r\n%s\r\n", 
					script_name.c_str(), InjectReply);
			}			
			else
			{
				Contents = "No reply";
				bNoPopup = true;
			}
		}
	
		// Send the reply to the output window if it exists
		OutputText(Contents, bNoPopup);
	}

	return OK;
}


void CMainFrame::OnInjectEvent() 
{
	DoInject(EVENT_SCRIPT);
}

void CMainFrame::OnInjectInstall() 
{
	DoInject(INSTALL_SCRIPT);
}

void CMainFrame::OnInjectInstallAndEvent() 
{
	DoInject(EVENT_SCRIPT | INSTALL_SCRIPT);
}

void CMainFrame::OnInjectRemove() 
{
	DoInject(REMOVE_SCRIPT);
}




// 
// Scriptorium menu implementation
//

void CMainFrame::OnCollapse() 
{
	m_ScriptoriumFrame->Collapse();
}

void CMainFrame::OnExpand() 
{
	m_ScriptoriumFrame->Expand();
}

void CMainFrame::OnRefresh() 
{
	m_ScriptoriumFrame->Refresh();
}


void CMainFrame::OnWrite() 
{
	int Result;
	CString Filename;
	CString Title;
	CString Filter;
	DWORD Flags;
	CString DefaultFilename;
	CString Message;
	CString Ext;
	BOOL BadName;
	CFileStatus fstatus;
	BOOL Exist;

	DefaultFilename = "Scriptorium.txt";
	Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES |
		OFN_OVERWRITEPROMPT;
	CFileDialog FileDialog(FALSE, NULL, DefaultFilename, Flags, Filter, 
		this); 

	do {
		// Pop up the Save As dialog box
		Result = FileDialog.DoModal();
		// Get rid of the dialog image
		UpdateWindow();

		if (Result == IDCANCEL) {
			m_ScriptoriumFrame->m_ctrlTree.SetFocus();
			return;
		}

		Filename = FileDialog.GetPathName();
		Title = FileDialog.GetFileName();

		// Check the extension
		BadName = FALSE;
		Ext = FileDialog.GetFileExt();
		Ext.MakeLower();
		if (Ext == "") {
			Ext = "txt";
			Filename += ".txt";
			Title += ".txt";
		}

		if (Ext != "txt") {
			::Beep(450, 100);
			::AfxMessageBox("The chosen file does not have the proper extension. Please choose a different file."
				, MB_OK | MB_ICONEXCLAMATION);
			BadName = TRUE;
		}
		else {
			// Check read-only attributes
			Exist = CFile::GetStatus(Filename, fstatus);
			if ((Exist) && (fstatus.m_attribute & 0x01 == 0x01)) {
				::Beep(450, 100);
				Message.Format("Unable to write to file '%s' because it is read-only. Please use a different file.", Filename);
				::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
				BadName = TRUE;
			}
		}
	} while (BadName);
	m_ScriptoriumFrame->WriteScriptorium(Filename);
	m_ScriptoriumFrame->m_ctrlTree.SetFocus();
}


void CMainFrame::OnDelete() 
{
	m_ScriptoriumFrame->DeleteScripts();
}



void CMainFrame::DoFetchScript
	(CCaosChildFrame *CaosFrame, HTREEITEM hNode, int FGSEValues[4])
{
	char *Script;
	CPtrArray TokenList;
	int TokenCount;
	CPtrArray CommandList;
	int CommandCount;
	CUIntArray ErrorIndexList;
	int ErrorCount;
	CString ScriptID;
	int TempErrorCount;
	int i;
	CString Message;
	CStringArray ScriptList;
	CPtrArray  ScriptNodeList;
	int  ScriptCount;
	CString Contents;
	CString TempContents;
	CString ErrorString;
	CString Line;
	CommandStructure *Command;

	// Get the script
	m_ScriptoriumFrame->GetScripts(hNode, ScriptList, ScriptNodeList, 
		ScriptCount);
	Script = ScriptList[0].GetBuffer(1);
	// Load the script into the edit control
	CaosFrame->m_ctrlEdit.SetContentsFromMacroCode(Script, TokenList, 
		TokenCount, CommandList, CommandCount, ErrorIndexList, ErrorCount);
	ScriptList[0].ReleaseBuffer();

	ScriptID.Format("script [%d, %d, %d, %d]", FGSEValues[0], FGSEValues[1],
		FGSEValues[2], FGSEValues[3]);
	Contents.Format("Fetching %s...\r\n", ScriptID);
	TempErrorCount = ErrorCount;
	if (ErrorCount > 0) {
		if (ErrorCount > 100) {
			Contents += "(Too many errors. The first 100 appear below)\r\n";
			TempErrorCount = 100;
		}
		for (i=0; i<TempErrorCount; i++) {
			Command = (CommandStructure *)CommandList[ErrorIndexList[i]];
			ErrorString = Command->ErrorString;
			Line.Format("Line %d: ", Command->LineNumber);
			Contents += Line + ErrorString + "\r\n";
		}		
	}

	if (ErrorCount == 0)
		TempContents = "Done.\r\n";
	else
		TempContents.Format("Done. %d syntax error(s) found.\r\n", ErrorCount);
	
	Contents += TempContents;
	if (m_ErrorFrame)
		m_ErrorFrame->m_ctrlEdit.SetWindowText(Contents);

	if (ErrorCount > 0) {
		::Beep(450, 100);
		Message.Format("%d syntax error(s) found in %s", ErrorCount, ScriptID);
		::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
	}

	CaosFrame->m_ctrlEdit.SetFocus();
}





void CMainFrame::DoFetchScripts
	(CCaosChildFrame *CaosFrame, HTREEITEM hNode, int FGSEValues[4])
{
	CPtrArray TokenList;
	int TokenCount;
	CPtrArray CommandList;
	int CommandCount;
	CUIntArray ErrorIndexList;
	int ErrorCount;
	CString ScriptID;
	int i;
	CString Message;
	CStringArray ScriptList;
	CPtrArray  ScriptNodeList;
	int  ScriptCount;
	CString Contents;
	CString TempContents;
	CString ErrorString;
	CString Line;
	CommandStructure *Command;
	CMacroParse Parser;
	int ScriptNumber;
	CString RTF;
	char *Script;
	NodeStructure *Node;
	char *RTFBuffer;
	BOOL GotErrors;
	int TotalErrorCount;
	int FirstErrorLine;
	int LineBase;
	CString Temp;

	// Get the scripts and their FGSE values
	m_ScriptoriumFrame->GetScripts(hNode, ScriptList, ScriptNodeList, 
		ScriptCount);

	GotErrors = FALSE;
	TotalErrorCount = 0;
	LineBase = 0;
	RTF = Parser.GetRTFHeader();
	Contents.Format("Fetching all scripts for species [%d, %d, %d]...\r\n",
		FGSEValues[0], FGSEValues[1], FGSEValues[2]);

	for (ScriptNumber=0; ScriptNumber<ScriptCount; ScriptNumber++) {
		// Parse each script
		Script = ScriptList[ScriptNumber].GetBuffer(1);
		Parser.Parse(Script); 
		Parser.GetParseResults(TokenList, TokenCount, CommandList, 
			CommandCount, ErrorIndexList, ErrorCount);
		ScriptList[ScriptNumber].ReleaseBuffer();
		Node = (NodeStructure *)ScriptNodeList[ScriptNumber];

		if (ErrorCount > 0) {
			Temp.Format("Script [%d, %d, %d, %d]:\r\n", Node->FGSEValues[0], 
				Node->FGSEValues[1], Node->FGSEValues[2], Node->FGSEValues[3]);
			Contents += Temp;
			if (!GotErrors) {
				GotErrors = TRUE;
				Command = (CommandStructure *)CommandList[ErrorIndexList[0]];
				FirstErrorLine = Command->LineNumber + LineBase;
			}
			TotalErrorCount += ErrorCount;
			for (i=0; i<ErrorCount; i++) {
				Command = (CommandStructure *)CommandList[ErrorIndexList[i]];
				ErrorString = Command->ErrorString;
				Line.Format("Line %d: ", LineBase + Command->LineNumber);
				Contents += Line + ErrorString + "\r\n";
			}
		}
		Command = (CommandStructure *)CommandList[CommandCount-1];
		LineBase += Command->LineNumber;
		// Build up the RTF string representing all the scripts
		RTF += Parser.GetRTFBody();
	}

	// Load the RTF string into the edit control
	RTF += Parser.GetRTFTrailer();
	RTFBuffer = RTF.GetBuffer(1);
	CaosFrame->m_ctrlEdit.SetContentsFromRTF(RTFBuffer);
	RTF.ReleaseBuffer();

	if (TotalErrorCount == 0)
		Temp = "Done.\r\n";
	else
		Temp.Format("Done. %d syntax error(s) found in total.\r\n", 
			TotalErrorCount);

	Contents += Temp;
	if (m_ErrorFrame)
		m_ErrorFrame->m_ctrlEdit.SetWindowText(Contents);

	if (TotalErrorCount > 0)
		CaosFrame->m_ctrlEdit.SetLineNumber(FirstErrorLine);

	if (TotalErrorCount > 0) {
		::Beep(450, 100);
		Message.Format("%d syntax error(s) found in species [%d, %d, %d]", 
			TotalErrorCount, FGSEValues[0],FGSEValues[1],FGSEValues[2]);
		::AfxMessageBox(Message, MB_OK | MB_ICONEXCLAMATION);
	}
	CaosFrame->m_ctrlEdit.SetFocus();
}


void CMainFrame::DoGenericFetch(HTREEITEM hNode)
{	
	CCaosChildFrame *CaosFrame;
	CString Caption;
	DWORD dw;
	NodeStructure *Node;
	BOOL OK;
	CWaitCursor WaitCursor;

	// Extract the node information
	dw = m_ScriptoriumFrame->m_ctrlTree.GetItemData(hNode);
	Node = (NodeStructure *)dw;

	// Check the level
	if (Node->Level <= 2) {
		::Beep(450, 100);
		::AfxMessageBox("You cannot fetch scripts at this level",
			MB_OK | MB_ICONEXCLAMATION);
		m_ScriptoriumFrame->m_ctrlTree.SetFocus();
		return;
	}

	if (Node->Level == 3) 
		Caption.Format("Scripts for Species [%d, %d, %d]", 
			Node->FGSEValues[0], Node->FGSEValues[1], 
			Node->FGSEValues[2]);
	else
		Caption.Format("Script [%d, %d, %d, %d]", Node->FGSEValues[0], 
			Node->FGSEValues[1], Node->FGSEValues[2], 
			Node->FGSEValues[3]);


	// Connect to the game
	OK = theGame.Connect();
	if (!OK) {
		::Beep(450, 100);
		::AfxMessageBox("Unable to establish a connection with the game", 
			MB_OK | MB_ICONSTOP);
		m_ScriptoriumFrame->m_ctrlTree.SetFocus();
		return;
	}

	WaitCursor.Restore();

	// Create a new CAOS frame
	CaosFrame = DoNew();
	CaosFrame->m_Title = Caption;
	CaosFrame->SetWindowText(Caption);

	if (Node->Level == 3) {
		// Fetch all scripts for the species
		DoFetchScripts(CaosFrame, hNode, Node->FGSEValues);
	}
	else {
		// Fetch a single script
		DoFetchScript(CaosFrame, hNode, Node->FGSEValues);
	}
	theGame.Disconnect();
}



void CMainFrame::OnFetch() 
{
	HTREEITEM hNode;

	hNode = m_ScriptoriumFrame->m_ctrlTree.GetSelectedItem();
	DoGenericFetch(hNode);
}

//
// Enable/Disable menu and toolbar logic
//


bool CMainFrame::CheckCaosFrameActive() 
{
	bool Enable;
	CMDIChildWnd *w; 
	
	w = MDIGetActive();
	if ((m_CaosFrameCount > 0) && 
		(w->IsKindOf(RUNTIME_CLASS(CCaosChildFrame))))
		Enable = true;
	else
		Enable = false;
	return Enable;
}

void CMainFrame::CheckCaosFrameActive(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(CheckCaosFrameActive());
}

void CMainFrame::CheckScriptoriumFrameActive(CCmdUI *pCmdUI) 
{
	BOOL Enable;
	CMDIChildWnd *w; 
	
	w = MDIGetActive();
	if (m_ScriptoriumFrame && 
		(w->IsKindOf(RUNTIME_CLASS(CScriptoriumChildFrame)))) {
		Enable = TRUE;
	}
	else
		Enable = FALSE;
	pCmdUI->Enable(Enable);
}


void CMainFrame::CheckCaosFramesExist(CCmdUI *pCmdUI) 
{
	BOOL Enable;
	
	if (m_CaosFrameCount > 0)
		Enable = TRUE;
	else
		Enable = FALSE;
	pCmdUI->Enable(Enable);
}


void CMainFrame::CheckCaosFrameHasSelection(CCmdUI *pCmdUI) 
{
	BOOL Enable;
	CCaosChildFrame *CaosFrame;
	CMDIChildWnd *w; 
	
	w = MDIGetActive();
	if ((m_CaosFrameCount > 0) && 
		(w->IsKindOf(RUNTIME_CLASS(CCaosChildFrame)))){
		CaosFrame = (CCaosChildFrame *)w;
		if (CaosFrame->m_ctrlEdit.HasSelection())
			Enable = TRUE;
		else
			Enable = FALSE;
	}
	else
		Enable = FALSE;
	pCmdUI->Enable(Enable);
}


//
// File menu
//


void CMainFrame::OnUpdateSave(CCmdUI *pCmdUI)
{
	CheckCaosFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateSaveAs(CCmdUI *pCmdUI)
{
	CheckCaosFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateSaveAll(CCmdUI *pCmdUI)
{
	CheckCaosFramesExist(pCmdUI);
}


void CMainFrame::OnUpdateClose(CCmdUI *pCmdUI)
{
	CheckCaosFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateCloseAll(CCmdUI *pCmdUI) 
{
	CheckCaosFramesExist(pCmdUI);
}


//
// Edit menu
//


void CMainFrame::OnUpdateCut(CCmdUI *pCmdUI)
{
	CheckCaosFrameHasSelection(pCmdUI);
}


void CMainFrame::OnUpdateCopy(CCmdUI *pCmdUI)
{
	CheckCaosFrameHasSelection(pCmdUI);
}


void CMainFrame::OnUpdatePaste(CCmdUI *pCmdUI)
{
	CheckCaosFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateSelectAll(CCmdUI *pCmdUI) 
{
	CheckCaosFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateFind(CCmdUI *pCmdUI) 
{	
	CheckCaosFrameActive(pCmdUI);
}

void CMainFrame::OnUpdateReplace(CCmdUI *pCmdUI) 
{
	CheckCaosFrameActive(pCmdUI);
}

void CMainFrame::OnUpdateFindNext(CCmdUI *pCmdUI) 
{
	CheckCaosFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateGoto(CCmdUI *pCmdUI) 
{
	CheckCaosFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateComment(CCmdUI *pCmdUI) 
{
	CheckCaosFrameHasSelection(pCmdUI);
}


void CMainFrame::OnUpdateUncomment(CCmdUI *pCmdUI) 
{
	CheckCaosFrameHasSelection(pCmdUI);
}


void CMainFrame::OnUpdateIndent(CCmdUI *pCmdUI) 
{
	CheckCaosFrameHasSelection(pCmdUI);
}


void CMainFrame::OnUpdateOutdent(CCmdUI *pCmdUI) 
{
	CheckCaosFrameHasSelection(pCmdUI);		
}


void CMainFrame::OnUpdateReformat(CCmdUI *pCmdUI) 
{
	CheckCaosFrameActive(pCmdUI);			
}



// 
// View menu
//


void CMainFrame::OnUpdateViewOutputWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_OutputFrame != NULL);
}


void CMainFrame::OnUpdateViewErrorWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ErrorFrame != NULL);
}


void CMainFrame::OnUpdateViewScriptoriumWindow(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_ScriptoriumFrame != NULL);		
}



//
// Inject menu
//


void CMainFrame::OnUpdateInjectEvent(CCmdUI *pCmdUI) 
{
	CheckCaosFrameActive(pCmdUI);
}

void CMainFrame::OnUpdateInjectInstall(CCmdUI *pCmdUI) 
{
	CheckCaosFrameActive(pCmdUI);
}

void CMainFrame::OnUpdateInjectRemove(CCmdUI *pCmdUI) 
{	
	CheckCaosFrameActive(pCmdUI);
}

void CMainFrame::OnUpdateInjectInstallAndEvent(CCmdUI *pCmdUI) 
{	
	CheckCaosFrameActive(pCmdUI);
}



//
// Scriptorium menu
//


void CMainFrame::OnUpdateCollapse(CCmdUI *pCmdUI) 
{
	CheckScriptoriumFrameActive(pCmdUI);		
}


void CMainFrame::OnUpdateExpand(CCmdUI *pCmdUI) 
{
	CheckScriptoriumFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateRefresh(CCmdUI *pCmdUI) 
{
	CheckScriptoriumFrameActive(pCmdUI);
}


void CMainFrame::OnUpdateDelete(CCmdUI *pCmdUI) 
{
	CheckScriptoriumFrameActive(pCmdUI);	
}


void CMainFrame::OnUpdateFetch(CCmdUI *pCmdUI) 
{
	CheckScriptoriumFrameActive(pCmdUI);	
}


void CMainFrame::OnUpdateWrite(CCmdUI *pCmdUI) 
{
	CheckScriptoriumFrameActive(pCmdUI);	
}

CCaosApp* CMainFrame::GetApp() const
{
	return (static_cast<CCaosApp *>(AfxGetApp()));
}

void CMainFrame::OnEditUndo() 
{
	// Get the active CAOS child frame
	CCaosChildFrame *CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		

	bool bCanUndo = false;
	bool bUndoneNonFormat = false;
	do
	{
		UNDONAMEID undo_type = (UNDONAMEID)CaosFrame->m_ctrlEdit.SendMessage(EM_GETUNDONAME, 0, 0);
		CaosFrame->m_ctrlEdit.Undo();	
		bUndoneNonFormat = (undo_type != UID_UNKNOWN); // formatting counts as UID_UNKNOWN
		bCanUndo = (CaosFrame->m_ctrlEdit.SendMessage(EM_CANUNDO, 0, 0) != 0);
	} while (bCanUndo && !bUndoneNonFormat);
}

void CMainFrame::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	bool bEnable = CheckCaosFrameActive();
	if (bEnable)
	{
		CCaosChildFrame *CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
		if (CaosFrame->m_ctrlEdit.SendMessage(EM_CANUNDO, 0, 0) == 0)
			bEnable = false;
	}
		
	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnEditRedo() 
{
	// Get the active CAOS child frame
	CCaosChildFrame *CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		

	bool bCanRedo = false;
	UNDONAMEID undo_type;
	do
	{
		CaosFrame->m_ctrlEdit.SendMessage(EM_REDO, 0, 0);
		undo_type = (UNDONAMEID)CaosFrame->m_ctrlEdit.SendMessage(EM_GETREDONAME, 0, 0);
		bCanRedo = (CaosFrame->m_ctrlEdit.SendMessage(EM_CANREDO, 0, 0) != 0);
	} while (bCanRedo && (undo_type == UID_UNKNOWN));// formatting counts as UID_UNKNOWN
}

void CMainFrame::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	bool bEnable = CheckCaosFrameActive();
	if (bEnable)
	{
		CCaosChildFrame *CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);		
		if (CaosFrame->m_ctrlEdit.SendMessage(EM_CANREDO, 0, 0) == 0)
			bEnable = false;
	}
	
	pCmdUI->Enable(bEnable);
}

void CMainFrame::OutputText(CString& Contents, bool bNoPopup)
{
	CCaosChildFrame *CaosFrame = (CCaosChildFrame *)MDIGetActive(NULL);
	if (m_OutputFrame)
	{
		m_OutputFrame->m_ctrlEdit.SetWindowText(Contents);
		m_OutputFrame->BringWindowToTop();
		if (CaosFrame)
		{
			CaosFrame->BringWindowToTop();
			CaosFrame->m_ctrlEdit.SetFocus();
		}
	}
	else if (!bNoPopup)
	{
		::AfxMessageBox(Contents, MB_OK | MB_ICONEXCLAMATION);
		if (CaosFrame)
			CaosFrame->m_ctrlEdit.SetFocus();
	}
}


std::string GetModulePath(HMODULE hModule)
{
	std::string strModulePath;

	TCHAR pszModuleFilename[_MAX_PATH];
	if (GetModuleFileName(hModule, pszModuleFilename, _MAX_PATH))
	{
		PathRemoveFileSpec(pszModuleFilename);
		strModulePath = pszModuleFilename;
	}

	return strModulePath;
}

void CMainFrame::ShowHelp(bool bAlphabetic)
{
	std::string filename;
	if (bAlphabetic)
		filename = GetModulePath(GetModuleHandle(NULL)) + "\\CAOS Help Alphabetical.html";
	else
		filename = GetModulePath(GetModuleHandle(NULL)) + "\\CAOS Help Categorical.html";

	std::ostringstream out;
	out << "dbg: html " << (bAlphabetic ? 0 : 1);
	std::string reply;
	bool success = theGame.Connect();
	if (success)
	{
		success = theGame.Inject(out.str(), reply);
		theGame.Disconnect();
	}
	
	if (success)
	{
		std::ofstream out(filename.c_str());
		out << reply;
	}

	// Launch the default HTML viewer on the file
	DWORD Status = (DWORD)ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),
        NULL, filename.c_str(), NULL, CurrentDirectory, SW_SHOWNORMAL);

	if (Status <= 32) {
		::Beep(450, 100);
		CString Message;
		if (success)
			Message.Format("Documentation read from game, but failed to launch it.\n\n%s", filename);
		else
			Message.Format("No existing documentation on disk, and failed\nto access the game to get latest documentation.\n\n%s\n\n%s", reply.c_str(), filename.c_str());
		::AfxMessageBox(Message, MB_OK | MB_ICONWARNING);
	}
}

void CMainFrame::OnHelpAlpha() 
{
	ShowHelp(true);
}

void CMainFrame::OnHelpCateg() 
{
	ShowHelp(false);	
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	HKEY myKey;
	CString RegistryKey = "Software\\" + CString(theCAOSApp.m_pszRegistryKey) + "\\" + CString(theCAOSApp.m_pszProfileName) + "\\1.0\\Filename";
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,RegistryKey,0,KEY_ALL_ACCESS,&myKey))
	{
		int j = 0;
		return;
	}
	CString filename;
	long len;
	int ret = RegQueryValue(myKey,"Load This File",NULL,&len);
	if (ret != ERROR_SUCCESS)
	{
		RegCloseKey(myKey);
		return;
	}
	char* moose = new char[len];
	RegQueryValue(myKey,"Load This File",moose,&len);
	RegCloseKey(myKey);
	long delret = RegDeleteKey(HKEY_CURRENT_USER,RegistryKey);
	if (delret != ERROR_SUCCESS)
	{
		int i = 5;
	}
	
	filename = moose;
	filename.TrimRight();
	DoOpen(filename,false);
}

void CMainFrame::AutoSave(CCaosChildFrame * CaosFrame)
{
	CaosFrame->AutoSave();					   
}



void CMainFrame::OnInjectEditquickmacros() 
{
	// TODO: Add your command handler code here
	CEditQuickMacros eqm;
	eqm.DoModal();
}

void CMainFrame::OnInjectQuickmacro1() { InjectQuickMacro("Macro 1"); }
void CMainFrame::OnInjectQuickmacro2() { InjectQuickMacro("Macro 2"); }
void CMainFrame::OnInjectQuickmacro3() { InjectQuickMacro("Macro 3"); }
void CMainFrame::OnInjectQuickmacro4() { InjectQuickMacro("Macro 4"); }
void CMainFrame::OnInjectQuickmacro5() { InjectQuickMacro("Macro 5"); }
void CMainFrame::OnInjectQuickmacro6() { InjectQuickMacro("Macro 6"); }
void CMainFrame::OnInjectQuickmacro7() { InjectQuickMacro("Macro 7"); }
void CMainFrame::OnInjectQuickmacro8() { InjectQuickMacro("Macro 8"); }
void CMainFrame::OnInjectQuickmacro9() { InjectQuickMacro("Macro 9"); }
void CMainFrame::OnInjectQuickmacro0() { InjectQuickMacro("Macro 0"); }

void CMainFrame::InjectQuickMacro(std::string name) 
{
	CString cmacro = theCAOSApp.GetProfileString("Quick Macros",name.c_str());
	std::string macro,reply;
	macro = cmacro;
	bool success = theGame.Connect();
	if (success)
	{
		success = theGame.Inject(macro, reply);
		theGame.Disconnect();
	}
	cmacro = reply.c_str();
	OutputText(cmacro,false);
}

