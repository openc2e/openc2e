// MonitorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "caosdebugger.h"
#include "MonitorDialog.h"
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CMonitorDialog dialog


CMonitorDialog::CMonitorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMonitorDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonitorDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMonitorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorDialog)
	DDX_Control(pDX, IDC_MONITOR_RICH_EDIT, m_ctrlEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonitorDialog, CDialog)
	//{{AFX_MSG_MAP(CMonitorDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorDialog message handlers

void CMonitorDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	DoSizing();
}

void CMonitorDialog::DoSizing()
{
	if (::IsWindow(m_ctrlEdit.m_hWnd))
	{
		CRect rect;
		GetClientRect(rect);
		m_ctrlEdit.MoveWindow(rect);
	}
}

void CMonitorDialog::AddText(CString text)
{
	int len = m_ctrlEdit.GetTextLength() + 1;
	m_ctrlEdit.SetSel(len, len);
	m_ctrlEdit.ReplaceSel(text);
}

int CMonitorDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	WindowState::Load("Windows", "Monitor", m_hWnd, false);
		
	return 0;
}

void CMonitorDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	DoSizing();	
	CDialog::OnShowWindow(bShow, nStatus);
}


void CMonitorDialog::RefreshFromGame()
{
	std::string reply;
	if (!theGameInterface.Inject("dbg: poll", reply))
		return;
	if (reply.empty())
		return;

	CString mfc_reply = reply.c_str();
	mfc_reply.Replace("\n", "\r\n");
	AddText(mfc_reply);
}

void CMonitorDialog::OnClose() 
{
	dynamic_cast<CMainFrame*>(AfxGetMainWnd())->OnViewMonitor();
}

void CMonitorDialog::SaveState() 
{
	WindowState::Save("Windows", "Monitor", m_hWnd);
}

