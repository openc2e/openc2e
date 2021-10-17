////////////////////////////////////////////////////////////////////////////////
// Caos.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Caos.h"
#include <afxadv.h>
#include "../../common/whichengine.h"
#include "MacroParse.h"
#include <windows.h>
#include "../../engine/CAOS/CAOSDescription.h"

#include "../../engine/Display/ErrorMessageHandler.h"

// #define VALIDATE_LICENSE

// Needs to be present for linking purposes.
std::string ErrorMessageHandler::Format(std::string baseTag, int offsetID, std::string source, ...)
{
	return baseTag;
}

BEGIN_MESSAGE_MAP(CCaosApp, CWinApp)
	//{{AFX_MSG_MAP(CCaosApp)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	ON_COMMAND(ID_REFRESH_SYNTAX, OnRefreshSyntax)
	ON_COMMAND(IDM_BINDGAME, OnBindGame)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_LAUNCH_ENGINE, OnLaunchEngine)
END_MESSAGE_MAP()


// Constructor
CCaosApp::CCaosApp()
{
}


//
// Globals
//

CCaosApp theCAOSApp;
CMainFrame *theFrame;
HICON hCaosFrameIcon;
HICON hErrorFrameIcon;
HICON hOutputFrameIcon;
HICON hScriptoriumFrameIcon;
CGameInterface theGame;
char CurrentDirectory[512];

/////////////////////////////////////////////////////////////////////////////
// CCaosApp initialization

BOOL CCaosApp::InitInstance()
{
	AfxEnableControlContainer();
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	if (!LoadLibrary("Riched20.dll"))
	{
		AfxMessageBox(_T("Requires Riched20.dll"));
		exit(0);
	}

	SetRegistryKey(_T(COMPANY));
	CString RegistryKey2 = "Software\\" + CString(m_pszRegistryKey) + "\\CECAOS\\1.0";
	CString RegistryKey = RegistryKey2+"\\Filename";

	CCommandLineInfo CommandLineInfo;
	ParseCommandLine(CommandLineInfo);
	
	// Here we should check to see if the CAOS tool is currently running.
	// If it is, we should pass the filename on to it.

	HANDLE mutex = CreateMutex(NULL, true, "CyberLifeTechnology_CAOS_Tool_Running");
	if (mutex)
	{
		// We managed to create the mutex, so the tool is _not_ running already...
		CloseHandle(mutex);
		// And create an event to prevent the mutex being created again
		CreateEvent(NULL,false,true,"CyberLifeTechnology_CAOS_Tool_Running");
	}
	else
	{
		// The mutex creation failed, so CAOS is already running.
		if (CommandLineInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
		{
			// Hmm, we need to drop a filename into the registry...
			// Do it the __HARD__ way :)
			HKEY myKey;
			int result = RegOpenKey(HKEY_CURRENT_USER,RegistryKey,&myKey);
			if (result != ERROR_SUCCESS)
				result = RegCreateKey(HKEY_CURRENT_USER,RegistryKey,&myKey);
			if (result != ERROR_SUCCESS)
			{
				MessageBox(NULL,"CAOS Tool cannot establish connection with registry.","CAOS",NULL);
				return false;
			}
			RegSetValue(myKey,"Load This File",REG_SZ,CommandLineInfo.m_strFileName,CommandLineInfo.m_strDriverName.GetLength());
			RegCloseKey(myKey);
		}
		else
			MessageBox(NULL,"CAOS is already running, and no filename was specified at the command line.","CAOS",NULL);

		return false;
	}

#ifdef VALIDATE_LICENSE
	// Validate the license information in the registry
	LONG ProductNumber = 350;

	if (!CTL_ValidKeyInRegistryApp(ProductNumber, const_cast<char*>((LPCTSTR)RegistryKey2))) 
	{
		// Bomb out with no error
		return FALSE;
	}
#endif

	LoadStdProfileSettings(9);  // Load standard INI file options (including MRU)

	// Get the current working directory and make sure that it ends
	// in a backslash
	GetCurrentDirectory(sizeof(CurrentDirectory), CurrentDirectory);
	if (CurrentDirectory[strlen(CurrentDirectory)-1] != '\\')
		strcat(CurrentDirectory, "\\");

	// Load the icon resources
	hCaosFrameIcon = LoadIcon(IDI_CAOS);
	hErrorFrameIcon = LoadIcon(IDI_SYNTAXERROR);
	hOutputFrameIcon = LoadIcon(IDI_OUTPUT);
	hScriptoriumFrameIcon = LoadIcon(IDI_SCRIPTORIUM);

	// Needed to use Rich Edit controls
	AfxInitRichEdit();

	// Not dynamic connecting - we do that ourselves, rather than
	// let game interface do it.
	theGame.SetDynConnect(false);

	// Create the main MDI frame window
	CMainFrame* pMainFrame = new CMainFrame;
	theFrame = pMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW, NULL, NULL))
		return FALSE;
	m_pMainWnd = pMainFrame;

	RefreshTitle();

	// The main window has been initialized, so show and update it
	// and allow it to process dragged and dropped COS files
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pMainFrame->DragAcceptFiles();

	// Load in syntax file that the game saved
	if (!DoRefreshSyntax())
		return FALSE;

	if (CommandLineInfo.m_nShellCommand == CCommandLineInfo::FileOpen) 
	{
		// Open the file specified on the command line
		theFrame->DoOpen(CommandLineInfo.m_strFileName, FALSE);
	}

	// Enter the message loop
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CCaosApp commands

void CCaosApp::OnAbout() 
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CCaosApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	CWinApp::AddToRecentFileList(lpszPathName);

	if (m_pRecentFileList)
	{
		m_pRecentFileList->WriteList();
	}
}

BOOL CCaosApp::OnOpenRecentFile(UINT nID)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);

	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

	TRACE2("MRU: open file (%d) '%s'.\n", (nIndex) + 1,
			(LPCTSTR)(*m_pRecentFileList)[nIndex]);

	static_cast<CMainFrame *>(m_pMainWnd)->DoOpen((*m_pRecentFileList)[nIndex], FALSE);

	return TRUE;
}

// From AfxImpl.h
BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

void CCaosApp::RemoveFromRecentFileList(LPCTSTR lpszPathName)
{
	ASSERT_VALID(this);
	ASSERT(lpszPathName != NULL);
	ASSERT(AfxIsValidString(lpszPathName));

	if (m_pRecentFileList)
	{
		// update the MRU list, if an existing MRU string matches file name
		for (int iMRU = 0; iMRU < m_pRecentFileList->GetSize(); ++iMRU)
		{
			if (AfxComparePath((*m_pRecentFileList)[iMRU], lpszPathName))
			{
				m_pRecentFileList->Remove(iMRU);
				break;
			}
		}
		m_pRecentFileList->WriteList();
	}
}

void CCaosApp::OnRefreshSyntax()
{
	DoRefreshSyntax();
}

bool CCaosApp::DoRefreshSyntax()
{
	std::string syntaxfile;
	syntaxfile = theWhichEngine.MainDir() + "caos.syntax";
	bool bResult = CMacroParse::LoadSyntax(syntaxfile);

	if (!bResult)
	{
		const std::string message(syntaxfile + "\n\nThis file does not exist or is not compatible\nwith this version of CAOS. You need the right version\nof the engine for the right version of CAOS.");
		MessageBox(NULL, message.c_str(), "CAOS Tool", MB_APPLMODAL);
		return false;
	}
	else
	{
		if (theFrame->CheckCaosFrameActive())
			theFrame->OnReformat();
		return true;
	}
}

CCaosApp::RefreshTitle()
{
	// Add game name to title
	CString title;
	title.LoadString(IDR_MAINFRAME);
	title += " - ";
	title += theWhichEngine.GameName().c_str();
	
	m_pMainWnd->SetWindowText(title);
	((CMainFrame*)m_pMainWnd)->m_defaultTitleString = title;
	if (((CMainFrame*)m_pMainWnd)->MDIGetActive(false))
	{
		CString str;
		((CMainFrame*)m_pMainWnd)->MDIGetActive(false)->GetWindowText(str);
		((CMainFrame*)m_pMainWnd)->SetWindowText(title + " - "+ str);
	}
}


void CCaosApp::OnBindGame()
{
	theWhichEngine.UseDefaultGameName();
	RefreshTitle();
	OnRefreshSyntax();
}

void CCaosApp::OnLaunchEngine()
{
	std::string name = theWhichEngine.MainDir();
	name += "engine.exe ";
	name += theWhichEngine.GameName();

	// Launch the default HTML viewer on the file
	UINT Status = WinExec(name.c_str(), SW_SHOW);

	if (Status <= 32) {
		::Beep(450, 100);
		CString Message;
		Message.Format("Failed to launch the Creatures Engine\n\n%s", name.c_str());
		::AfxMessageBox(Message, MB_OK | MB_ICONWARNING);
	}
}

void CCaosApp::DoBackgroundColour(CRichEditCtrl& edit)
{
	// Cope with dark background colours...

	// Get default colour
	if (::IsWindow(edit.m_hWnd))
	{
		COLORREF oldColour = edit.SetBackgroundColor(TRUE, 0);
		BYTE r = GetRValue(oldColour);
		BYTE g = GetGValue(oldColour);
		BYTE b = GetBValue(oldColour);
		// If background is dark, change it to light grey
		if (r < 100 && g < 100 && b < 100)
		{
			COLORREF newColour = RGB(200, 200, 200);
			edit.SetBackgroundColor(FALSE, newColour);
		}
	}
}

