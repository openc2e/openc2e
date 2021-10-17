#if !defined(AFX_BACKGROUNDFILEDLG_H__7BABBEE2_CA3B_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_BACKGROUNDFILEDLG_H__7BABBEE2_CA3B_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackgroundFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBackgroundFileDlg dialog

class CBackgroundFileDlg : public CDialog
{
// Construction
public:
	CBackgroundFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBackgroundFileDlg)
	enum { IDD = IDD_BACKGROUND_FILE };
	CEdit	m_MinEdit;
	CEdit	m_MaxEdit;
	CString	m_Source;
	int		m_Max;
	int		m_Min;
	BOOL	m_Preview;
	BOOL	m_Sprite;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackgroundFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBackgroundFileDlg)
	afx_msg void OnMakePreviewFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKGROUNDFILEDLG_H__7BABBEE2_CA3B_11D2_9D30_0090271EEABE__INCLUDED_)

