// C2E Room Editor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "C2ERoomEditor.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "C2ERoomEditorDoc.h"
#include "C2ERoomEditorView.h"
#include "utils.h"
#include "OptionsDlg.h"
#include "Tipdlg.h"

#include "CtlLicense.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LICENSE_CHECK

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorApp

BEGIN_MESSAGE_MAP(CC2ERoomEditorApp, CWinApp)
	ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
	//{{AFX_MSG_MAP(CC2ERoomEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_FILE_LOADFROMGAME, OnFileLoadFromGame)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_CDN_WEBSITE, OnCDNWebsite)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorApp construction

CC2ERoomEditorApp::CC2ERoomEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CC2ERoomEditorApp object

CC2ERoomEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorApp initialization

BOOL CC2ERoomEditorApp::InitInstance()
{
	char *RegistryKey = "Software\\CyberLife Technology\\Map Editor";
	LONG ProductNumber = 351;

#ifdef LICENSE_CHECK
	if (!CTL_ValidKeyInRegistryApp(ProductNumber, RegistryKey)) {
		// Bomb out with error message
		::Beep(450, 100);
		::AfxMessageBox("Unable to start: invalid license",
			MB_OK | MB_ICONSTOP);
		return FALSE;
	}
#endif

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("CyberLife Technology"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	CString defaultDir = GetExeDirectory() + "Previews\\";
	m_PreviewDir = GetProfileString( "Directories", "Previews", defaultDir );
	::CreateDirectory( defaultDir, NULL );

	m_ValidationOption = GetProfileInt( "Settings", "Validation", Validation::AutoUndo );
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_C2EROOTYPE,
		RUNTIME_CLASS(CC2ERoomEditorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CC2ERoomEditorView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// CG: This line inserted by 'Tip of the Day' component.
	ShowTipAtStartup();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CC2ERoomEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorApp message handlers


int CC2ERoomEditorApp::ExitInstance() 
{
	WriteProfileString( "Directories", "Previews", m_PreviewDir );
	WriteProfileInt( "Settings", "Validation", m_ValidationOption );
	return CWinApp::ExitInstance();
}

void CC2ERoomEditorApp::OnToolsOptions() 
{
	COptionsDlg dlg;
	dlg.m_Validation = m_ValidationOption;

	if( dlg.DoModal() == IDOK )
		m_ValidationOption = dlg.m_Validation;
}

void CC2ERoomEditorApp::ShowTipAtStartup(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_bShowSplash)
	{
		CTipDlg dlg;
		if (dlg.m_bStartup)
			dlg.DoModal();
	}

}

void CC2ERoomEditorApp::ShowTipOfTheDay(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CTipDlg dlg;
	dlg.DoModal();

}

void CC2ERoomEditorApp::OnFileLoadFromGame() 
{
	OnFileNew();
	CC2ERoomEditorDoc* doc = (CC2ERoomEditorDoc*)(((CMainFrame*)(m_pMainWnd))->MDIGetActive()->GetActiveDocument());
	doc->DoLoadFromGame();
}

void CC2ERoomEditorApp::OnHelpContents()
{
	WORD Status = (DWORD)ShellExecute(m_pMainWnd->m_hWnd,
		NULL, "Help/C3DSMapEditor.html", NULL, "", SW_SHOWNORMAL);
	if (Status <= 32)
	{
		::AfxMessageBox("Failed to launch help - try http://cdn.creatures.net instead", MB_OK);
	}
}

void CC2ERoomEditorApp::OnCDNWebsite()
{
	WORD Status = (DWORD)ShellExecute(m_pMainWnd->m_hWnd,
		NULL, "http://cdn.creatures.net", NULL, "", SW_SHOWNORMAL);
	if (Status <= 32)
	{
		::AfxMessageBox("Failed to launch http://cdn.creatures.net", MB_OK);
	}
}
