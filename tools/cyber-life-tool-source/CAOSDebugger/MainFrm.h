
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MAIN_FRM_H
#define MAIN_FRM_H

#include "AgentTree.h"
#include <list>
#include "WatchDialog.h"
#include "MonitorDialog.h"

#pragma once

#define WM_REFRESH_ALL_TIMER WM_USER + 250

class CMainFrame : public CFrameWnd
{
	typedef CFrameWnd base;

	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	bool DrivenTick();
	bool IdleDrivenTick();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();

	void AddWatchWindow(CWatchDialog* wnd);
	void RemoveWatchWindow(CWatchDialog* wnd);
	void RefreshAllFromGame();
	void CloseAllWatchWindows();

	static const int SAMPLE_RATE;
	bool GetMultimediaTimer();
	bool SetMultimediaTimer(bool bActive);

	int GetTackAgent();

protected:
	static void CALLBACK TickProc(unsigned int uiD,
							unsigned int uMsg,
							unsigned long dwuser,
							unsigned long dw1,
							unsigned long dw2);

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CAgentTree	m_wndAgentTree;
	CMonitorDialog m_Monitor;

	std::list<CWatchDialog*> m_Watches;
	UINT ourTimerID;

	bool m_bDriving;
	bool m_bDrivenPaused;
	bool m_bMonitorVisible;
	bool m_bPausedLastQuery;
	bool m_bFastDrive;
	int m_TackLastQuery;

// Generated message map functions
protected:
	bool QueryPaused();
	bool QueryTracking();
	int QueryTacking();
	void DoStep();
	
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaws();
	afx_msg void OnDriveGame();
	afx_msg void OnStep();
	afx_msg void OnFlush();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWatchCloseall();
	afx_msg void OnRefreshFromGame();
	afx_msg void OnWatchRefreshontock();
	afx_msg void OnUpdateWatchRefreshontock(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaws(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDriveGame(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStep(CCmdUI* pCmdUI);
	afx_msg void OnCameraToAgent();
	afx_msg void OnUpdateCameraToAgent(CCmdUI* pCmdUI);
	afx_msg void OnKillAgent();
	afx_msg void OnUpdateKillAgent(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnAgentTrack();
	afx_msg void OnUpdateAgentTrack(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFastDrive();
	afx_msg void OnUpdateFastDrive(CCmdUI* pCmdUI);
	afx_msg void OnProfileClearprofile();
	afx_msg void OnProfileGenerateprofile();
	//}}AFX_MSG
	afx_msg LONG OnRefreshAllTimer(UINT uiParm, LONG lParm);
	afx_msg void OnUpdateViewMonitor(CCmdUI* pCmdUI);
public:
	afx_msg void OnViewMonitor();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // MAIN_FRM_H

