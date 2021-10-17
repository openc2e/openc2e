#if !defined(AFX_FLOORVALUESDLG_H__86A15524_EC29_11D2_9D31_0090271EEABE__INCLUDED_)
#define AFX_FLOORVALUESDLG_H__86A15524_EC29_11D2_9D31_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FloorValuesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFloorValuesDlg dialog

class CFloorValuesDlg : public CDialog
{
// Construction
public:
	CFloorValuesDlg(CWnd* pParent = NULL);   // standard constructor
	float m_M;
	float m_C;

// Dialog Data
	//{{AFX_DATA(CFloorValuesDlg)
	enum { IDD = IDD_FLOOR_VALUES };
	CEdit	m_Y;
	CEdit	m_X;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFloorValuesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFloorValuesDlg)
	afx_msg void OnCalculate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOORVALUESDLG_H__86A15524_EC29_11D2_9D31_0090271EEABE__INCLUDED_)

