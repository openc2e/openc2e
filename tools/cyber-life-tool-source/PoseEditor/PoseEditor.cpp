// PoseEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PoseEditor.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "PoseEditorDoc.h"
#include "PoseEditorView.h"
#include "CtlLicense.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorApp

BEGIN_MESSAGE_MAP(CPoseEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CPoseEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NAMES_IMPORTPOSENAMES, OnFileNamesImportposenames)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorApp construction
char UserGuideFile[512];
char CurrentDirectory[512];

CPoseEditorApp::CPoseEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPoseEditorApp object

CPoseEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorApp initialization

BOOL CPoseEditorApp::InitInstance()
{
/*
	char *RegistryKey = "Software\\CyberLife Technology\\Pose and Gait Editor\\1.0";
	LONG ProductNumber = 300;

	if (!CTL_ValidKeyInRegistryApp(ProductNumber, RegistryKey)) {
		// Bomb out with error message
		::Beep(450, 100);
		::AfxMessageBox("Unable to start: invalid license",
			MB_OK | MB_ICONSTOP);
		return FALSE;
	}
*/

	
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

	char buff[256];
	CString nameFile;
	GetModuleFileName( AfxGetInstanceHandle( ), buff, 255 );
	CString exeName( buff );
	int lastSlash = exeName.ReverseFind( '\\' );
	if( lastSlash != -1 )
		nameFile = exeName.Left( lastSlash ) + "\\posegait.txt";
	m_PoseNameFile = GetProfileString( "Settings", "NameFile", nameFile );
	LoadPoseDescriptions( std::string( m_PoseNameFile ) );
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_GENOMETYPE,
		RUNTIME_CLASS(CPoseEditorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPoseEditorView));
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

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Get the current working directory and make sure that it ends
	// in a backslash
	GetCurrentDirectory(sizeof(CurrentDirectory), CurrentDirectory);
	if (CurrentDirectory[strlen(CurrentDirectory)-1] != '\\')
		strcat(CurrentDirectory, "\\Help\\");
	// Create the full filename of the user guide file
	strcpy(UserGuideFile, CurrentDirectory);
	strcat(UserGuideFile, "PoseEditor.htm");

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

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
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CPoseEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorApp message handlers


int CPoseEditorApp::ExitInstance() 
{
	WriteProfileString( "Settings", "NameFile", m_PoseNameFile );
	SavePoseDescriptions( std::string( m_PoseNameFile ) );
	return CWinApp::ExitInstance();
}

void CPoseEditorApp::OnFileNamesImportposenames() 
{
	CFileDialog dlg( TRUE, "posename.txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Pose Names (posename.txt)|posename.txt" );
	if( dlg.DoModal() == IDOK )
	{
		ImportPoseDescriptions( std::string( dlg.GetPathName() ) );
	}
}

