#ifndef WATCH_DIALOG_H
#define WATCH_DIALOG_H

#pragma once

#include "../caos/MacroParse.h"
#include "../caos/MacroEditCtrl.h"
#include "splitterbar.h"
#include "WatchStreamer.h"

// WatchDialog.h : header file

/////////////////////////////////////////////////////////////////////////////
// CWatchDialog dialog

class CWatchDialog : public CDialog
{
// Construction
public:
	CWatchDialog(CWnd* pParent = NULL);   // standard constructor

	void RefreshFromGame();
	void RefreshSourceFromGame();
	void SetClassifier(const FGSU& fgsu, const std::string& non_fgsu_prefix, const std::string& non_fgsu_title);
	void CycleClassifier();

// Dialog Data
	//{{AFX_DATA(CWatchDialog)
	enum { IDD = IDD_WATCH };
	CListCtrl	m_ctrlList;
	//}}AFX_DATA

	CMacroEditCtrl	m_ctrlRichEdit;
	CSplitterBar m_wndSplitterBar;
	bool m_bSourcePresence;
	int m_nSourceWidth;

	afx_msg void OnClose();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SetSourceInRichEdit(const char* script);
	void SetSourcePresence(bool bNewPresence);
	int AddNewWatch(int ix, const std::string& name, std::string& caos);
	void DeleteWatch(int ix);
	void EditCaos(int ix);
	bool SourceDisplayPossible();

	// Generated message map functions
	//{{AFX_MSG(CWatchDialog)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnWatchAdd();
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnWatchDelete();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSourceToggle();
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditCaos();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCycleclassifier();
	afx_msg void OnAlwaysOnTop();
	//}}AFX_MSG
	afx_msg void OnSingleStepTack();
	afx_msg void OnWatchSet(UINT nID);
	afx_msg void OnSplitterMoved();
	virtual void OnOK();
	virtual void OnCancel();
	void DoSizing();
	DECLARE_MESSAGE_MAP()

	std::string myTargPrefix;
	std::string myPrettyName;

	std::string myPreviousSource;
	FGSU myFGSU;

	std::vector<std::string> myCAOS;

	WatchStreamer myOptionalWatches;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // WATCH_DIALOG_H

