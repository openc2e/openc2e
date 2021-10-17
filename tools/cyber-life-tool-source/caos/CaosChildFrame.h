#if !defined(AFX_CAOSCHILDFRAME_H__C3F47F81_3F5D_11D2_A805_0060B05E3B36__INCLUDED_)
#define AFX_CAOSCHILDFRAME_H__C3F47F81_3F5D_11D2_A805_0060B05E3B36__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CaosChildFrame.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CCaosChildFrame frame

class CCaosChildFrame : public CMDIChildWnd
{	
public:
	DECLARE_DYNCREATE(CCaosChildFrame)

	CCaosChildFrame();
	virtual ~CCaosChildFrame();

	// Public Members
	CMacroEditCtrl m_ctrlEdit;
	CString m_Filename;
	CString m_Title;
	int m_ID;

	void AutoSave();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaosChildFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CCaosChildFrame)
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg LONG OnContentsEdited(UINT, LONG);
	afx_msg LONG OnLineError(UINT LineNumber, LONG);
	afx_msg LONG OnSelectionChanged(UINT StartLineNumber, LONG EndLineNumber);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAOSCHILDFRAME_H__C3F47F81_3F5D_11D2_A805_0060B05E3B36__INCLUDED_)

