#ifndef INJECTION_DLG_H
#define INJECTION_DLG_H

#pragma once

// InjectionDlg.h : header file
//

#include "DialogSpec.h"
#include "BaseDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CInjectionDlg dialog

class CInjectionDlg : public CBaseDlg
{
	typedef CBaseDlg base;

private:
	static const int slidemax;

// Construction
public:
	CInjectionDlg(CWnd* pParent = NULL);   // standard constructor

	virtual void ChangeSpec(const CDialogSpec& spec);
	virtual void Sample(int i, float val);

// Dialog Data
	//{{AFX_DATA(CInjectionDlg)
	enum { IDD = IDD_INJECTION_DIALOG };
	CButton	m_ctrlSetButton;
	CListCtrl	m_ctrlChemList;
	CSliderCtrl	m_ctrlLevelSlider;
	CEdit	m_ctrlLevelNumber;
	float	m_fLevelNumber;
	//}}AFX_DATA

// Overrides
	virtual UINT MenuResourceID() {	return IDR_INJECTER_MENU; };

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInjectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInjectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeLevelNumber();
	afx_msg void OnSet();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnItemchangedChemlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls();
	void RefreshSlider();
	void RefreshEditBox();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // INJECTION_DLG_H

