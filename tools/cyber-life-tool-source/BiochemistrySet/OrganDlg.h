#if !defined(AFX_ORGANDLG_H__2F15B121_DACC_11D2_AC73_00105A56C7E5__INCLUDED_)
#define AFX_ORGANDLG_H__2F15B121_DACC_11D2_AC73_00105A56C7E5__INCLUDED_

#pragma once

// OrganDlg.h : header file
//

#include "DialogSpec.h"
#include "BaseDlg.h"

/////////////////////////////////////////////////////////////////////////////
// COrganDlg dialog

class COrganDlg : public CBaseDlg
{
	typedef CBaseDlg base;

// Construction
public:
	COrganDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COrganDlg)
	enum { IDD = IDD_ORGAN_DIALOG };
	CListCtrl	m_ctrlOrganList;
	//}}AFX_DATA

	virtual void ChangeSpec(const CDialogSpec& spec);
	virtual void GeneralTimer();
	virtual UINT MenuResourceID() {	return IDR_ORGAN_MENU; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrganDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COrganDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int myOrganCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORGANDLG_H__2F15B121_DACC_11D2_AC73_00105A56C7E5__INCLUDED_)

