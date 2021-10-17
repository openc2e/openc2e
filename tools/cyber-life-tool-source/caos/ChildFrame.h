#if !defined(AFX_CHILDFRAME_H__C3F47F83_3F5D_11D2_A805_0060B05E3B36__INCLUDED_)
#define AFX_CHILDFRAME_H__C3F47F83_3F5D_11D2_A805_0060B05E3B36__INCLUDED_
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ChildFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChildFrame frame


// This class is used as an MDI child frame container for an edit box. It 
// is used for the syntax error frame and for the script output frame

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)

public:
	// Constructor
	CChildFrame();    

	// Public Members 
	CRichEditCtrl m_ctrlEdit;
	int m_ChildFrameType;     // 1 for syntax error frame
							  // 2 for script output frame
									
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();

	// Generated message map functions
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, 
		CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRAME_H__C3F47F83_3F5D_11D2_A805_0060B05E3B36__INCLUDED_)

