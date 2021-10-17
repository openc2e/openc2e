// GetLicence.h : main header file for the GETLICENCE application
//

#if !defined(AFX_GETLICENCE_H__FF144D25_3D56_11D5_A26B_005004A81EDC__INCLUDED_)
#define AFX_GETLICENCE_H__FF144D25_3D56_11D5_A26B_005004A81EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGetLicenceApp:
// See GetLicence.cpp for the implementation of this class
//

class CGetLicenceApp : public CWinApp
{
public:
	CGetLicenceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetLicenceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGetLicenceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETLICENCE_H__FF144D25_3D56_11D5_A26B_005004A81EDC__INCLUDED_)
