#if !defined(AFX_REPLACEDIALOG_H__FAB48692_5202_11D2_A807_0060B05E3B36__INCLUDED_)
#define AFX_REPLACEDIALOG_H__FAB48692_5202_11D2_A807_0060B05E3B36__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ReplaceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog dialog

class CReplaceDialog : public CDialog
{
// Construction
public:
	CReplaceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReplaceDialog)
	enum { IDD = IDD_REPLACE };
	CButton	m_ctrlReplace;
	CButton	m_ctrlReplaceFind;
	CButton	m_ctrlWholeWord;
	CEdit	m_ctrlWith;
	CEdit	m_ctrlWhat;
	CButton	m_ctrlCase;
	CButton	m_ctrlSelection;
	CButton	m_ctrlWholeFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplaceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReplaceDialog)
	afx_msg void OnReplace();
	afx_msg void OnReplaceAll();
	afx_msg void OnReplaceFind();
	virtual BOOL OnInitDialog();
	afx_msg void OnReplaceSelectionToggled();
	afx_msg void OnReplaceWholeFileToggled();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPLACEDIALOG_H__FAB48692_5202_11D2_A807_0060B05E3B36__INCLUDED_)

