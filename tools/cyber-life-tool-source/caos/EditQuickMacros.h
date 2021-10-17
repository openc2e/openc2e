#if !defined(AFX_EDITQUICKMACROS_H__2756B881_4F02_11D3_BFF0_005004B1F42F__INCLUDED_)
#define AFX_EDITQUICKMACROS_H__2756B881_4F02_11D3_BFF0_005004B1F42F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditQuickMacros.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditQuickMacros dialog

class CEditQuickMacros : public CDialog
{
// Construction
public:
	CEditQuickMacros(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditQuickMacros)
	enum { IDD = IDD_EDITQUICKMACROS };
	CString	m_Macro1;
	CString	m_Macro2;
	CString	m_Macro3;
	CString	m_Macro4;
	CString	m_Macro5;
	CString	m_Macro6;
	CString	m_Macro7;
	CString	m_Macro8;
	CString	m_Macro9;
	CString	m_Macro0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditQuickMacros)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditQuickMacros)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	virtual void OnCancel();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITQUICKMACROS_H__2756B881_4F02_11D3_BFF0_005004B1F42F__INCLUDED_)

