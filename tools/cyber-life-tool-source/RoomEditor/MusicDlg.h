#if !defined(AFX_MUSICDLG_H__49DF63A4_02FB_11D3_9D31_0090271EEABE__INCLUDED_)
#define AFX_MUSICDLG_H__49DF63A4_02FB_11D3_9D31_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MusicDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMusicDlg dialog

class CMusicDlg : public CDialog
{
// Construction
public:
	CMusicDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMusicDlg)
	enum { IDD = IDD_MUSIC };
	CString	m_Music;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMusicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMusicDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MUSICDLG_H__49DF63A4_02FB_11D3_9D31_0090271EEABE__INCLUDED_)

