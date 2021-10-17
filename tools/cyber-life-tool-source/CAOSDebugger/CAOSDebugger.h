// CAOSDebugger.h : main header file for the CAOSDEBUGGER application
//

#ifndef CAOS_DEBUGGER_H
#define CAOS_DEBUGGER_H

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "../../common/catalogue.h"
extern Catalogue theCatalogue;

#include "resource.h"       // main symbols
#include "../../engine/classifier.h"
#include <map>
#include "agenttree.h"

/////////////////////////////////////////////////////////////////////////////
// CCAOSDebuggerApp:
// See CAOSDebugger.cpp for the implementation of this class
//

class CCAOSDebuggerApp : public CWinApp
{
public:
	CCAOSDebuggerApp();
	void CreateWatch(const FGSU& fgsu, const std::string& non_fgsu_prefix = "", const std::string& non_fgsu_title = "Unspecified");
	std::string GetScript(Classifier c, int& poffset);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCAOSDebuggerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

	bool DoRefreshSyntax();

public:
	//{{AFX_MSG(CCAOSDebuggerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnGlobalWatch();
	afx_msg void OnRefreshSyntax();
	afx_msg void OnHotWatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // CAOS_DEBUGGER_H

