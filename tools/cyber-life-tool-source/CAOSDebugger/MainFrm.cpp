// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CAOSDebugger.h"

#include "MainFrm.h"

#include "../../common/GameInterface.h"
#include "../../common/WindowState.h"

#include <algorithm>
#include <mmsystem.h>
#include <sstream>
#include <fstream>
#include <Shlwapi.h> // path manipulation

const int CMainFrame::SAMPLE_RATE = 1000;

CMainFrame* theMainWnd = 0;

static bool myTickerPaused;
static bool myTickerPresent;
static HANDLE myTickerThread;

// dummy so we can include engine code
std::string ErrorMessageHandler::Format(std::string baseTag, int offsetID, std::string source, ...)
{
	return baseTag;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, base)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_PAWS, OnPaws)
	ON_COMMAND(ID_DRIVE_GAME, OnDriveGame)
	ON_COMMAND(ID_STEP, OnStep)
	ON_COMMAND(ID_FLUSH, OnFlush)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_WATCH_CLOSEALL, OnWatchCloseall)
	ON_COMMAND(ID_REFRESH_FROM_GAME, OnRefreshFromGame)
	ON_COMMAND(ID_WATCH_REFRESHONTOCK, OnWatchRefreshontock)
	ON_UPDATE_COMMAND_UI(ID_WATCH_REFRESHONTOCK, OnUpdateWatchRefreshontock)
	ON_UPDATE_COMMAND_UI(ID_PAWS, OnUpdatePaws)
	ON_UPDATE_COMMAND_UI(ID_DRIVE_GAME, OnUpdateDriveGame)
	ON_UPDATE_COMMAND_UI(ID_STEP, OnUpdateStep)
	ON_COMMAND(ID_CAMERA_TO_AGENT, OnCameraToAgent)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_TO_AGENT, OnUpdateCameraToAgent)
	ON_COMMAND(ID_KILL_AGENT, OnKillAgent)
	ON_UPDATE_COMMAND_UI(ID_KILL_AGENT, OnUpdateKillAgent)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_AGENT_TRACK, OnAgentTrack)
	ON_UPDATE_COMMAND_UI(ID_AGENT_TRACK, OnUpdateAgentTrack)
	ON_WM_TIMER()
	ON_COMMAND(ID_FAST_DRIVE, OnFastDrive)
	ON_UPDATE_COMMAND_UI(ID_FAST_DRIVE, OnUpdateFastDrive)
	ON_MESSAGE(WM_REFRESH_ALL_TIMER, OnRefreshAllTimer)
	ON_COMMAND(ID_PROFILE_CLEARPROFILE, OnProfileClearprofile)
	ON_COMMAND(ID_PROFILE_GENERATEPROFILE, OnProfileGenerateprofile)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_MONITOR, OnViewMonitor)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MONITOR, OnUpdateViewMonitor)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: ourTimerID(0), m_bDriving(false), m_bDrivenPaused(false),
	m_bMonitorVisible(false), m_bPausedLastQuery(false),
	m_bFastDrive(false), m_TackLastQuery(-1)
{
}

CMainFrame::~CMainFrame()
{
	if (GetMultimediaTimer())
		SetMultimediaTimer(false);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (base::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	WindowState::Load("Windows", "Main", m_hWnd, false);

	CRect rect(0, 50, 300, 300);

	if (!m_wndAgentTree.Create(AFX_WS_DEFAULT_VIEW, rect,
		this, AFX_IDW_PANE_FIRST))
	{
		ASSERT(false);
	}
	m_wndAgentTree.RefreshFromGame();
	m_wndAgentTree.ShowWindow(SW_SHOW);
	 
	if (!m_Monitor.Create(IDD_MONITOR))
	{
		ASSERT(false);
		return FALSE;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
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

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	theMainWnd = this;

	SetTimer(0,250,NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !base::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnPaws() 
{
	if (!m_bDriving)
	{
		std::string send = "doif paws = 1 dbg: play else dbg: paws endi";
		std::string reply;
		if (!theGameInterface.Inject(send, reply))
			AfxMessageBox(reply.c_str());	
	}
	else
		m_bDrivenPaused = !m_bDrivenPaused;
}

void CMainFrame::OnStep() 
{
	if (!m_bDriving || m_bDrivenPaused)
	{
		DoStep();
	}
}

void CMainFrame::DoStep()
{
	std::string command;
	
	if (m_TackLastQuery > -1)
	{
		std::ostringstream send;
		send << "dbg: tack agnt " << m_TackLastQuery;
		command = send.str();
	}
	else
		command = "dbg: tock";

	std::string reply;
	if (!theGameInterface.Inject(command, reply))
		return;

	RefreshAllFromGame();
}

void CMainFrame::OnFlush() 
{
	std::string reply;
	if (!theGameInterface.Inject("dbg: flsh", reply))
		AfxMessageBox(reply.c_str());
}

void CMainFrame::OnClose() 
{
	WindowState::Save("Windows", "Main", m_hWnd);
	CloseAllWatchWindows();
	m_Monitor.SaveState();

	base::OnClose();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	base::OnSize(nType, cx, cy);
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CMainFrame::AddWatchWindow(CWatchDialog* wnd)
{
	m_Watches.push_back(wnd);
}

void CMainFrame::RemoveWatchWindow(CWatchDialog* wnd)
{
	std::list<CWatchDialog*>::iterator it = std::find(m_Watches.begin(), m_Watches.end(), wnd);
	if (it != m_Watches.end())
		m_Watches.erase(it);
	else 
		ASSERT(false);
}

void CMainFrame::RefreshAllFromGame()
{
	QueryPaused();
	QueryTacking();

	std::list<CWatchDialog*>::iterator it = m_Watches.begin();
	for (; it != m_Watches.end(); ++it )
	{
		(*it)->RefreshFromGame();
	}

	m_wndAgentTree.RefreshFromGame();
	
	if (m_bMonitorVisible)
		m_Monitor.RefreshFromGame();
}

void CMainFrame::CloseAllWatchWindows()
{
	// copy the set of watches so we don't cut
	// down the tree as we're climbing it.
	std::list<CWatchDialog*> copyWatches = m_Watches;
	std::list<CWatchDialog*>::iterator it = copyWatches.begin();
	for (; it != copyWatches.end(); ++it )
	{
		(*it)->OnClose();
	}
}

void CMainFrame::OnWatchCloseall() 
{
	CloseAllWatchWindows();
}

void CMainFrame::OnRefreshFromGame() 
{
	RefreshAllFromGame();
}

void CMainFrame::OnWatchRefreshontock() 
{
	SetMultimediaTimer(!GetMultimediaTimer());
}

DWORD WINAPI Ticker( LPVOID ob )
{
	// Whilst we are running, let's gogogo
	myTickerPaused = false;
	myTickerPresent = true;

	while (myTickerPresent)
	{
		Sleep(CMainFrame::SAMPLE_RATE);
		if (!myTickerPaused)
		{
			if (theMainWnd)
				theMainWnd->PostMessage(WM_REFRESH_ALL_TIMER, 0, 0);
		}
	}

	myTickerPaused = false;
	myTickerPresent = false;
	return 0;
}

bool CMainFrame::SetMultimediaTimer(bool bActive)
{
	if (!myTickerPresent)
	{
		DWORD dummy;
		myTickerThread = CreateThread( NULL,
			0,
			Ticker,
			(LPVOID)0,
			0,
			&dummy );
		myTickerPresent = myTickerThread != 0;
	}
	if (!myTickerPresent)
	{
		::MessageBox(NULL,
			"Failed to start ticker thread",
			"SetMultimediaTimer",
			MB_SYSTEMMODAL);
		return false;
	}
	myTickerPaused = !bActive;
	return true;
}

bool CMainFrame::GetMultimediaTimer()
{
	return (!myTickerPaused) && myTickerPresent;
}

void CMainFrame::OnUpdateWatchRefreshontock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetMultimediaTimer());
}

afx_msg LONG CMainFrame::OnRefreshAllTimer(UINT uiParm, LONG lParm)
{	
	if (!m_bDriving || (m_bDriving && m_bDrivenPaused))
		RefreshAllFromGame();

	return 0;
}

bool CMainFrame::QueryPaused()
{
	std::string reply;
	if (!theGameInterface.Inject("outv paws", reply))
		m_bPausedLastQuery = false;
	else
		m_bPausedLastQuery = (reply == "1");

	return m_bPausedLastQuery;
}

bool CMainFrame::QueryTracking()
{
	std::string reply;
	if (theGameInterface.Inject("doif trck eq null outv 0 else outv 1 endi", reply))
		if (reply == "1")
			return true;

	return false;
}

int CMainFrame::QueryTacking()
{
	std::string reply;
	if (theGameInterface.Inject("doif tack eq null outv -1 else targ tack outv unid endi", reply))
		m_TackLastQuery = atoi(reply.c_str());
	else
		m_TackLastQuery = -1;

	if (m_TackLastQuery > -1)
		m_bDriving = true;

	return m_TackLastQuery;
}

int CMainFrame::GetTackAgent()
{
	return m_TackLastQuery;
}

void CMainFrame::OnUpdatePaws(CCmdUI* pCmdUI) 
{
	bool check = (m_bDrivenPaused && m_bDriving) || (!m_bDriving && m_bPausedLastQuery);
	pCmdUI->SetCheck(check);
}

void CMainFrame::OnUpdateStep(CCmdUI* pCmdUI) 
{
	bool enable = (m_bPausedLastQuery && !m_bDriving) || (m_bDrivenPaused && m_bDriving);
	pCmdUI->Enable(enable);
}

void CMainFrame::OnUpdateDriveGame(CCmdUI* pCmdUI) 
{
	int check = m_bDriving ? 1 : 0;
	if (m_TackLastQuery > -1)
		check = 2;
	pCmdUI->SetCheck(check);
}

void CMainFrame::OnDriveGame() 
{
	m_bDriving = !m_bDriving;
	if (m_bDriving)
	{
		m_bDrivenPaused = QueryPaused();
		std::string reply;
		if (!theGameInterface.Inject("dbg: paws", reply))
			AfxMessageBox(reply.c_str());	
	}
	else
	{
		if (m_TackLastQuery > -1)
		{
			std::string reply;
			if (!theGameInterface.Inject("dbg: tack null", reply))
				AfxMessageBox(reply.c_str());	
		}
		if (!m_bDrivenPaused)
		{
			std::string reply;
			if (!theGameInterface.Inject("dbg: play", reply))
				AfxMessageBox(reply.c_str());	
		}
	}
}

bool CMainFrame::IdleDrivenTick()
{
	if (m_bFastDrive)
		return DrivenTick();

	return false;
}

bool CMainFrame::DrivenTick()
{
	if (!m_bDriving || m_bDrivenPaused)
		return false;

	DoStep();

	return true;
}

void CMainFrame::OnCameraToAgent() 
{
	int unid = m_wndAgentTree.GetSelectedUNID();
	if (unid >= 0)
	{
		std::ostringstream send;
		send << "targ agnt " << unid << " cmrt 0";
		std::string reply;
		if (!theGameInterface.Inject(send.str(), reply))
			AfxMessageBox(reply.c_str());
	}
}

void CMainFrame::OnUpdateCameraToAgent(CCmdUI* pCmdUI) 
{
	int unid = m_wndAgentTree.GetSelectedUNID();
	pCmdUI->Enable(unid >= 0);	
}

void CMainFrame::OnKillAgent() 
{
	FGSU fgsu = m_wndAgentTree.GetSelectedFGSU();
	if (fgsu.f < 0)
		return;

	std::ostringstream send;
	if (fgsu.unid != -1)
		send << "kill agnt " << fgsu.unid;
	else
	{
		if (fgsu.g < 0)
			fgsu.g = 0;
		if (fgsu.s < 0)
			fgsu.s = 0;
		send << "enum " << fgsu.f << " " << fgsu.g << " " << fgsu.s << " kill targ next";
	}

	std::string reply;
	if (!theGameInterface.Inject(send.str(), reply))
		AfxMessageBox(reply.c_str());		

	RefreshAllFromGame();
}

void CMainFrame::OnUpdateKillAgent(CCmdUI* pCmdUI) 
{
	FGSU fgsu = m_wndAgentTree.GetSelectedFGSU();
	pCmdUI->Enable(fgsu.f > 0);
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd) 
{
	base::OnSetFocus(pOldWnd);
	
	m_wndAgentTree.SetFocus();	
}

void CMainFrame::OnAgentTrack() 
{
	int unid = m_wndAgentTree.GetSelectedUNID();
	if (unid >= 0 || QueryTracking())
	{
		std::ostringstream send;
		send << "doif trck eq null trck agnt " << unid << " 0 0 2 0"
			<< " else trck null 0 0 0 0 endi";
		std::string reply;
		if (!theGameInterface.Inject(send.str(), reply))
			AfxMessageBox(reply.c_str());		
	}	
}

void CMainFrame::OnUpdateAgentTrack(CCmdUI* pCmdUI) 
{
	bool bTicked = QueryTracking();
	pCmdUI->SetCheck(bTicked);

	int unid = m_wndAgentTree.GetSelectedUNID();
	pCmdUI->Enable(bTicked || unid >= 0);		
}

void CMainFrame::OnViewMonitor() 
{
	if (m_bMonitorVisible)
		m_Monitor.ShowWindow(SW_HIDE);
	else
		m_Monitor.ShowWindow(SW_SHOW);

	m_bMonitorVisible = !m_bMonitorVisible;
}

void CMainFrame::OnUpdateViewMonitor(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bMonitorVisible);
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	if (!m_bFastDrive)
		DrivenTick();
}

void CMainFrame::OnFastDrive() 
{
	m_bFastDrive = !m_bFastDrive;
}

void CMainFrame::OnUpdateFastDrive(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bFastDrive);	
}

void CMainFrame::OnProfileClearprofile() 
{
	std::string reply;
	if (!theGameInterface.Inject("dbg: cpro", reply))
		AfxMessageBox(reply.c_str());			
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


void CMainFrame::OnProfileGenerateprofile() 
{
	std::string reply;
	if (!theGameInterface.Inject("dbg: prof", reply))
		AfxMessageBox(reply.c_str());			
	else
	{
		std::string filename;
		filename = GetModulePath(GetModuleHandle(NULL)) + "\\Agent Profile Data.csv";

		{
			std::ofstream out(filename.c_str());
			out << reply;
		}

		// Launch the default CSV (comma separated value) viewer on the file
		// - this will usually be a spreadsheet
		DWORD Status = (DWORD)ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),
			NULL, filename.c_str(), NULL, GetModulePath(GetModuleHandle(NULL)).c_str(), SW_SHOWNORMAL);

		if (Status <= 32) {
			::Beep(450, 100);
			CString Message;
			Message.Format("Unable to view the profile CSV file [error code %d]", Status);
			::AfxMessageBox(Message, MB_OK | MB_ICONWARNING);
		}	
	}

}

