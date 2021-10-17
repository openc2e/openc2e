#ifndef PARAMETERS_DLG_H
#define PARAMETERS_DLG_H

#pragma once

// ParametersDlg.h : header file
//

#include "CreatureMoniker.h"

/////////////////////////////////////////////////////////////////////////////
// CParametersDlg dialog

class CParametersDlg : public CDialog
{
	typedef CDialog base;

// Construction
public:
	CParametersDlg(CString title, CDialogSpec& spec, CWnd* pParent = NULL);   // standard constructor
	~CParametersDlg();

// Dialog Data
	//{{AFX_DATA(CParametersDlg)
	enum { IDD = IDD_PARAMETERS };
	CStatic	m_ctrlChemicalsStatic;
	CButton	m_ctrlOK;
	CEdit	m_ctrlTimeSpan;
	CStatic	m_ctrlTimeSpanStatic;
	CListBox	m_ctrlCreatureList;
	CListCtrl	m_ctrlChemicalListView;
	float	m_fSampleRate;
	float	m_fTimeSpan;
	int		m_nCreatureListSel;
	//}}AFX_DATA

	CDialogSpec& m_spec;
	bool m_bInit;
	CString m_title;

	CCreatureNames* m_apCreatureNames;

	void SetCheck(WPARAM ItemIndex, BOOL bCheck);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParametersDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParametersDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemchangedChemListview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSelchangeCreatureList();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTickAll();
	afx_msg void OnUntickAll();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void RefreshEnabling();
	void SaveWindowState();
	bool NoChemControl();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // PARAMETERS_DLG_H

