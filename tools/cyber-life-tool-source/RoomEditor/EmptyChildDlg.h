#if !defined(AFX_EMPTYCHILDDLG_H__9359ECC6_C026_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_EMPTYCHILDDLG_H__9359ECC6_C026_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EmptyChildDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEmptyChildDlg dialog

class CEmptyChildDlg : public CDialog
{
// Construction
public:
	CEmptyChildDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEmptyChildDlg)
	enum { IDD = IDD_EMPTY_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmptyChildDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEmptyChildDlg)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMPTYCHILDDLG_H__9359ECC6_C026_11D2_9D30_0090271EEABE__INCLUDED_)

