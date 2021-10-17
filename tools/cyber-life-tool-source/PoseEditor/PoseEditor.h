// PoseEditor.h : main header file for the POSEEDITOR application
//

#if !defined(AFX_POSEEDITOR_H__FC4A1FCB_A95B_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_POSEEDITOR_H__FC4A1FCB_A95B_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorApp:
// See PoseEditor.cpp for the implementation of this class
//

class CPoseEditorApp : public CWinApp
{
public:
	CPoseEditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoseEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CPoseEditorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNamesImportposenames();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_PoseNameFile;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSEEDITOR_H__FC4A1FCB_A95B_11D2_9D30_0090271EEABE__INCLUDED_)

