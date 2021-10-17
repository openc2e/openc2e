#if !defined(AFX_FAVOURITENAME_H__9121BEC5_D619_11D2_AC73_00105A56C7E5__INCLUDED_)
#define AFX_FAVOURITENAME_H__9121BEC5_D619_11D2_AC73_00105A56C7E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FavouriteName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFavouriteName dialog

class CFavouriteName : public CDialog
{
// Construction
public:
	CFavouriteName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFavouriteName)
	enum { IDD = IDD_NAME_FAVOURITE };
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFavouriteName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFavouriteName)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAVOURITENAME_H__9121BEC5_D619_11D2_AC73_00105A56C7E5__INCLUDED_)

