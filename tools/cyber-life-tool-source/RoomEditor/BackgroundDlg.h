#if !defined(AFX_BACKGROUNDDLG_H__7BABBEE1_CA3B_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_BACKGROUNDDLG_H__7BABBEE1_CA3B_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackgroundDlg.h : header file
//
#include <vector>
class CC2ERoomEditorDoc;
/////////////////////////////////////////////////////////////////////////////
// CBackgroundDlg dialog

class CBackgroundDlg : public CDialog
{
// Construction
public:
	CBackgroundDlg(CC2ERoomEditorDoc *pDoc, int metaroom, CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CBackgroundDlg)
	enum { IDD = IDD_METAROOM_BACKGROUND };
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackgroundDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBackgroundDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnProperties();
	afx_msg void OnDblclkList();
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	afx_msg void OnSet();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CC2ERoomEditorDoc *m_Doc;
	int m_Metaroom;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKGROUNDDLG_H__7BABBEE1_CA3B_11D2_9D30_0090271EEABE__INCLUDED_)

