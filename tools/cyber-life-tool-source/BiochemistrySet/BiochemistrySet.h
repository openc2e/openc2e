// BiochemistrySet.h : main header file for the BIOCHEMISTRYSET application
//

#if !defined(AFX_BIOCHEMISTRYSET_H__17EC13E6_D53B_11D2_AC73_00105A56C7E5__INCLUDED_)
#define AFX_BIOCHEMISTRYSET_H__17EC13E6_D53B_11D2_AC73_00105A56C7E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBiochemistrySetApp:
// See BiochemistrySet.cpp for the implementation of this class
//

extern bool g_bSineTest;

class CBiochemistrySetApp : public CWinApp
{
public:
	CBiochemistrySetApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBiochemistrySetApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBiochemistrySetApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BIOCHEMISTRYSET_H__17EC13E6_D53B_11D2_AC73_00105A56C7E5__INCLUDED_)

