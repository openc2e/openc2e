// URGEncyDlg.h : header file
//

#if !defined(AFX_URGENCYDLG_H__5DAFA0E7_F28C_11D2_A768_0060B07B6A6F__INCLUDED_)
#define AFX_URGENCYDLG_H__5DAFA0E7_F28C_11D2_A768_0060B07B6A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SliderEx.h"
#include "StaticNew.h"
#include "../../common/GameInterface.h"

#include "../../common/Catalogue.h"

/////////////////////////////////////////////////////////////////////////////
// CURGEncyDlg dialog

class CURGEncyDlg : public CDialog
{
// Construction
private:
	CGameInterface gInterface;
	HRGN displayregion;

	Catalogue cat;
public:
	CURGEncyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CURGEncyDlg)
	enum { IDD = IDD_URGENCY_DIALOG };
	CStatic	m_PausedButton;
	CStatic	m_PauseButton;
	CStatic	m_Titlebar;
	CComboBox	m_VerbCombo;
	CComboBox	m_NounCombo;
	CSliderEx	m_NounSlider;
	CSliderEx	m_VerbSlider;
	CString	m_Meaning;
	CString	m_NornThought;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CURGEncyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CURGEncyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelchangeVerbcombo();
	afx_msg void OnSelchangeNouncombo();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAboutbox();
	afx_msg void OnQuit();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnExecute();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnQuitbutton();
	afx_msg void OnMinimbutton();
	afx_msg void OnPause();
	afx_msg void OnEmergency();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URGENCYDLG_H__5DAFA0E7_F28C_11D2_A768_0060B07B6A6F__INCLUDED_)

