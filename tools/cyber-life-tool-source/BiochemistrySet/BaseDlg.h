#ifndef BASE_DLG_H
#define BASE_DLG_H

#pragma once

// BaseDlg.h : header file
// Base class for savable chem-monitor windows

#include <vector>
#include "resource.h"

void UnMapDialogRect(CRect& rect);
void OurMapDialogRect(CRect& rect);

/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog

class CBaseDlg : public CDialog
{
	typedef CDialog base;

// Construction
public:
	CBaseDlg(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBaseDlg)
	//}}AFX_DATA

protected:
	int m_iTime;
	UINT m_uTimerID;
	bool bInTimer;

	// pameters to save
	CDialogSpec m_spec;

	bool m_bChemicalSample;
	bool m_bError;

public:
	virtual CDialogSpec GetSpec();
	virtual void ChangeSpec(const CDialogSpec& spec);
	virtual void PreSample(bool bError, CString sMessage);
	virtual void Sample(int i, float val) {} ;
	virtual void PostSample() {};
	virtual void GeneralTimer() {};

	virtual UINT MenuResourceID() {	return IDR_GENERIC_MENU; };
	virtual void MenuChange(CMenu* pPopup) {};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnCancel();
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBaseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGraph();
	afx_msg void OnInject();
	afx_msg void OnLog();
	afx_msg void OnProperties();
	afx_msg void OnFavourite();
	afx_msg void OnOrgan();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void RefreshWindowText();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // BASE_DLG_H

