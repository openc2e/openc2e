#if !defined(AFX_HEIGHTCHECKDLG_H__10174544_7653_11D3_9D31_0090271EEABE__INCLUDED_)
#define AFX_HEIGHTCHECKDLG_H__10174544_7653_11D3_9D31_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeightCheckDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeightCheckDlg dialog

class CHeightCheckDlg : public CDialog
{
// Construction
public:
	CHeightCheckDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHeightCheckDlg)
	enum { IDD = IDD_HEIGHT_CHECK };
	int		m_Height;
	int		m_Permiability;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeightCheckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHeightCheckDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEIGHTCHECKDLG_H__10174544_7653_11D3_9D31_0090271EEABE__INCLUDED_)

