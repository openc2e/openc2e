// URGEncy.h : main header file for the URGENCY application
//

#if !defined(AFX_URGENCY_H__5DAFA0E5_F28C_11D2_A768_0060B07B6A6F__INCLUDED_)
#define AFX_URGENCY_H__5DAFA0E5_F28C_11D2_A768_0060B07B6A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CURGEncyApp:
// See URGEncy.cpp for the implementation of this class
//

class CURGEncyApp : public CWinApp
{
public:
	CURGEncyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CURGEncyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CURGEncyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

extern CURGEncyApp theApp;

#endif // !defined(AFX_URGENCY_H__5DAFA0E5_F28C_11D2_A768_0060B07B6A6F__INCLUDED_)

