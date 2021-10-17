// Caos.h : main header file for the CAOS application
//

#if !defined(AFX_CAOS_H__D09269D4_32C3_11D2_A804_0060B05E3B36__INCLUDED_)
#define AFX_CAOS_H__D09269D4_32C3_11D2_A804_0060B05E3B36__INCLUDED_

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "MacroParse.h"
#include "MacroEditCtrl.h"
#include "CaosChildFrame.h"
#include "ChildFrame.h"
#include "GotoDialog.h"
#include "FindDialog.h"
#include "ReplaceDialog.h"
#include "../../common/GameInterface.h"
#include "ScriptoriumTreeCtrl.h"
#include "ScriptoriumChildFrame.h"
#include "MainFrame.h"
#include "CTLLicense.h"


/////////////////////////////////////////////////////////////////////////////
// CCaosApp:
// See Caos.cpp for the implementation of this class
//

class CCaosApp : public CWinApp
{
public:
	CCaosApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaosApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	void RemoveFromRecentFileList(LPCTSTR lpszPathName);
	bool DoRefreshSyntax();

	static void DoBackgroundColour(CRichEditCtrl& edit);

// Implementation

	//{{AFX_MSG(CCaosApp)
	afx_msg void OnAbout();
	afx_msg void OnRefreshSyntax();
	//}}AFX_MSG
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg void OnBindGame();
	afx_msg void OnLaunchEngine();
	CCaosApp::RefreshTitle();
	DECLARE_MESSAGE_MAP()
};

extern CCaosApp theCAOSApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAOS_H__D09269D4_32C3_11D2_A804_0060B05E3B36__INCLUDED_)

