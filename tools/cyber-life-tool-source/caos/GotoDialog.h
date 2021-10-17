#if !defined(AFX_GOTODIALOG_H__1E447271_4814_11D2_A806_0060B05E3B36__INCLUDED_)
#define AFX_GOTODIALOG_H__1E447271_4814_11D2_A806_0060B05E3B36__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GotoDialog.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CGotoDialog dialog

class CGotoDialog : public CDialog
{
// Construction
public:
	CGotoDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGotoDialog)
	enum { IDD = IDD_GOTO };
	CEdit	m_ctrlLine;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGotoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGotoDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGoto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTODIALOG_H__1E447271_4814_11D2_A806_0060B05E3B36__INCLUDED_)

