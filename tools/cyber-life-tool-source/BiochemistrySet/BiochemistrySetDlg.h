// BiochemistrySetDlg.h : header file
//

#ifndef BIOCHEMISTRY_SET_DLG_H
#define BIOCHEMISTRY_SET_DLG_H

#pragma once

#include "DialogSpec.h"
#include "FavouriteClass.h"

const int INJECTION_DEFAULT_SAMPLE_RATE = 500;
const int ORGAN_DEFAULT_SAMPLE_RATE = 1000;

/////////////////////////////////////////////////////////////////////////////
// CBiochemistrySetDlg dialog

class CBiochemistrySetDlg : public CDialog
{
	typedef CDialog base;

public:
	static CFavouriteClass favclass; // set of favourite windows

// Construction
public:
	CBiochemistrySetDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBiochemistrySetDlg)
	enum { IDD = IDD_BIOCHEMISTRYSET_DIALOG };
	CButton	m_ctrlDeleteFav;
	CButton	m_ctrlOpenFav;
	CComboBox	m_ctrlFavouriteCombo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBiochemistrySetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hBigIcon;
	HICON m_hSmallIcon;

	// Generated message map functions
	//{{AFX_MSG(CBiochemistrySetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGraph();
	afx_msg void OnOrgan();
	afx_msg void OnLog();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnOpenFav();
	afx_msg void OnDeleteFav();
	afx_msg void OnSelchangeFavCombo();
	afx_msg void OnInject();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void RefreshEnabling();
	CString GetWindowConfigFilename();
	bool QueryFilenameIfNecessary(CDialogSpec& localspec);

public:
	void QueryChemWindow(const CDialogSpec& spec, bool bResetSize, bool bNoQuery = false);
	void CreateChemWindow(const CDialogSpec& spec);
	void AddFavouriteSpec(const CDialogSpec& spec);
	void RefreshFavouritesList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // BIOCHEMISTRY_SET_DLG_H

