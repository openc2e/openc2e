#if !defined(AFX_PROPERTYTYPEDLG_H__E0028AA5_C008_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_PROPERTYTYPEDLG_H__E0028AA5_C008_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyTypeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyTypeDlg dialog

class CPropertyTypeDlg : public CDialog
{
// Construction
public:
	CPropertyTypeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertyTypeDlg)
	enum { IDD = IDD_PROPERTY_TYPE };
	CEdit	m_EnumValuesCtl;
	CString	m_EnumValues;
	BOOL	m_Enumerated;
	int		m_Max;
	int		m_Min;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertyTypeDlg)
	afx_msg void OnEnumerated();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYTYPEDLG_H__E0028AA5_C008_11D2_9D30_0090271EEABE__INCLUDED_)

