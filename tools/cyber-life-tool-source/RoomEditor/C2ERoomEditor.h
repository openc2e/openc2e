// C2E Room Editor.h : main header file for the C2E ROOM EDITOR application
//

#if !defined(AFX_C2EROOMEDITOR_H__A50773A5_B9D4_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_C2EROOMEDITOR_H__A50773A5_B9D4_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorApp:
// See C2E Room Editor.cpp for the implementation of this class
//

class CC2ERoomEditorApp : public CWinApp
{
public:
	CC2ERoomEditorApp();
	CString GetPreviewDirectory() { return m_PreviewDir; }

	enum Validation {
		None,
		Auto,
		AutoUndo
	};
	int GetValidationOption() { return m_ValidationOption; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CC2ERoomEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CC2ERoomEditorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnToolsOptions();
	afx_msg void OnFileLoadFromGame();
	afx_msg void OnHelpContents();
	afx_msg void OnCDNWebsite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ShowTipAtStartup(void);
	void ShowTipOfTheDay(void);
	CString m_PreviewDir;
	int m_ValidationOption;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_C2EROOMEDITOR_H__A50773A5_B9D4_11D2_9D30_0090271EEABE__INCLUDED_)

