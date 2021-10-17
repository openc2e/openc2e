#ifndef MONITOR_DIALOG_H
#define MONITOR_DIALOG_H

#pragma once

#include "../../common/GameInterface.h"
#include "../../common/WindowState.h"

/////////////////////////////////////////////////////////////////////////////
// CMonitorDialog dialog

class CMonitorDialog : public CDialog
{
// Construction
public:
	CMonitorDialog(CWnd* pParent = NULL);   // standard constructor
	void RefreshFromGame();
	void SaveState();

// Dialog Data
	//{{AFX_DATA(CMonitorDialog)
	enum { IDD = IDD_MONITOR };
	CRichEditCtrl	m_ctrlEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonitorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AddText(CString text);
	void DoSizing();

	// Generated message map functions
	//{{AFX_MSG(CMonitorDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // MONITOR_DIALOG_H

