#if !defined(AFX_FINDDIALOG_H__FAB48691_5202_11D2_A807_0060B05E3B36__INCLUDED_)
#define AFX_FINDDIALOG_H__FAB48691_5202_11D2_A807_0060B05E3B36__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FindDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDialog dialog

class CFindDialog : public CDialog
{
// Construction
public:
	CFindDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindDialog)
	enum { IDD = IDD_FIND };
	CEdit	m_ctrlWhat;
	CButton	m_ctrlWholeWord;
	CButton	m_ctrlCase;
	CButton m_ctrlUp;
	CButton m_ctrlDown;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindDialog)
	afx_msg void OnFind();
	virtual BOOL OnInitDialog();
	afx_msg void OnFindUpToggled();
	afx_msg void OnFindDownToggled();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDIALOG_H__FAB48691_5202_11D2_A807_0060B05E3B36__INCLUDED_)

