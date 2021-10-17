#ifndef LOGGER_DLG_H
#define LOGGER_DLG_H

#pragma once
#include "DialogSpec.h"
#include "BaseDlg.h"

// CLoggerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoggerDlg dialog

class CLoggerDlg : public CBaseDlg
{
	typedef CBaseDlg base;

private:
	// Construction

public:
	CLoggerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoggerDlg)
	enum { IDD = IDD_LOG_DIALOG };
	CString	m_ctrlFilename;
	//}}AFX_DATA

	std::ofstream ologstr;

	virtual void PreSample(bool bError, CString sMessage);
	virtual void Sample(int i, float val);
	virtual void PostSample();

// Overrides
	virtual UINT MenuResourceID() {	return IDR_LOGGER_MENU; };

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoggerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoggerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void ChangeSpec(const CDialogSpec& spec);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // LOGGER_DLG_H

