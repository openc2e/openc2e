// CAOSDebugger.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CAOSDebugger.h"

#include "MainFrm.h"

#include "../../common/GameInterface.h"
#include "../../common/WhichEngine.h"
#include "../caos/MacroParse.h"
#include "../../engine/app.h"

#include "WatchDialog.h"
#include <sstream>

/////////////////////////////////////////////////////////////////////////////
// CCAOSDebuggerApp

BEGIN_MESSAGE_MAP(CCAOSDebuggerApp, CWinApp)
	//{{AFX_MSG_MAP(CCAOSDebuggerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_GLOBAL_WATCH, OnGlobalWatch)
	ON_COMMAND(ID_REFRESH_SYNTAX, OnRefreshSyntax)
	ON_COMMAND(IDM_HOT_WATCH, OnHotWatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

App theApp;
App::App(){}
App::~App(){}
InputManager::InputManager(){}

/////////////////////////////////////////////////////////////////////////////
// CCAOSDebuggerApp construction

CCAOSDebuggerApp::CCAOSDebuggerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCAOSDebuggerApp object

CCAOSDebuggerApp theCDBApp;

/////////////////////////////////////////////////////////////////////////////
// CCAOSDebuggerApp initialization

BOOL CCAOSDebuggerApp::InitInstance()
{
	AfxEnableControlContainer();

	AfxInitRichEdit();

	theGameInterface.SetDynConnect(true);

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
	SetRegistryKey(_T(COMPANY));

	// Get syntax for CAOS commands
	if (!DoRefreshSyntax())
		return FALSE;

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// Needed to use Rich Edit controls
	AfxInitRichEdit();

	std::string title = "CAOS Debugger - " + theWhichEngine.GameName();
	pFrame->SetWindowText(title.c_str());

	// try to load shared MDI menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need. 

	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_CAOSDETYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_CAOSDETYPE));

	// The main window has been initialized, so show and update it.
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	dynamic_cast<CMainFrame*>(pFrame)->SetMultimediaTimer(true);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCAOSDebuggerApp message handlers
 
int CCAOSDebuggerApp::ExitInstance() 
{
	//TODO: handle additional resources you may have added
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
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
	//}w}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCAOSDebuggerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCAOSDebuggerApp message handlers


void CAboutDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	
}

void CCAOSDebuggerApp::OnGlobalWatch() 
{
	CreateWatch(FGSU(), "", "Global");
}

void CCAOSDebuggerApp::OnHotWatch() 
{
	CreateWatch(FGSU(), "doif hots eq null stop endi targ hots", "Hotspot");
}

// title is also the file read for default watches, and watches in menu
void CCAOSDebuggerApp::CreateWatch(const FGSU& fgsu, const std::string& non_fgsu_prefix, const std::string& non_fgsu_title)
{
	CWatchDialog* pWatch = new CWatchDialog;
	if (!pWatch->Create(IDD_WATCH))
	{
		ASSERT(false);
		delete pWatch;
		return;
	}

	pWatch->SetClassifier(fgsu, non_fgsu_prefix, non_fgsu_title);
	pWatch->ShowWindow(SW_SHOW);	
}

std::string CCAOSDebuggerApp::GetScript(Classifier c, int& poffset)
{
	// Get script from game
	std::ostringstream str;
	str << "outs sorc " << (int)c.Family() << " " << (int)c.Genus()
		<< " " << (int)c.Species() << " " << (int)c.Event();
	std::string reply;
	std::string send = str.str();
	if (!theGameInterface.Inject(send, reply))
		return reply;

	std::ostringstream scrp;
	scrp << "scrp " << (int)c.Family() << " " << (int)c.Genus()
		<< " " << (int)c.Species() << " " << (int)c.Event() << " ";
	std::string final_script = scrp.str();
	poffset = final_script.size();
	final_script += reply;
	final_script += " endm";

	return final_script;
}

BOOL CCAOSDebuggerApp::OnIdle(LONG lCount) 
{
	if (lCount == 0)
		CWinApp::OnIdle(lCount);
	
	// If driving the game, and we've nothing else to do, 
	// then do a tick
	if (!dynamic_cast<CMainFrame*>(AfxGetMainWnd())->IdleDrivenTick())
		return 0;
	else
		return 1;
}


void CCAOSDebuggerApp::OnRefreshSyntax()
{
	DoRefreshSyntax();
}

bool CCAOSDebuggerApp::DoRefreshSyntax()
{
	std::string syntaxfile;
	syntaxfile = theWhichEngine.MainDir() + "caos.syntax";
	bool bResult = CMacroParse::LoadSyntax(syntaxfile);	
	if (!bResult)
	{
		const std::string message(syntaxfile + "\n\nThis file does not exist or is not compatible\nwith this version of CAOS Debugger. You need the right version\nof the engine for the right version of CAOS Debugger.");
		MessageBox(NULL, message.c_str(), "CAOS Debugger", MB_APPLMODAL);
	}
	return bResult;
}

