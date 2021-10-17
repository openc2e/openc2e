#if !defined(AFX_DIRDLG_H__96647D04_CC00_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_DIRDLG_H__96647D04_CC00_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirDlg dialog

class CDirDlg : public CDialog
{
// Construction
public:
	CDirDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDirDlg)
	enum { IDD = IDD_DIRECTORIES };
	CString	m_Bitmaps;
	CString	m_BodyData;
	CString	m_Suffix;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDirDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDLG_H__96647D04_CC00_11D2_9D30_0090271EEABE__INCLUDED_)

