#if !defined(AFX_WORLDSIZEDLG_H__7034D3E1_D07F_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_WORLDSIZEDLG_H__7034D3E1_D07F_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorldSizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorldSizeDlg dialog

class CWorldSizeDlg : public CDialog
{
// Construction
public:
	CWorldSizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldSizeDlg)
	enum { IDD = IDD_WORLD_SIZE };
	int		m_Height;
	int		m_Width;
	int		m_MetaroomBase;
	int		m_RoomBase;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldSizeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDSIZEDLG_H__7034D3E1_D07F_11D2_9D30_0090271EEABE__INCLUDED_)

