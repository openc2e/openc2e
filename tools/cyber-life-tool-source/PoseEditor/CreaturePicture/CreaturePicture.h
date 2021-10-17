#if !defined(AFX_CREATUREPICTURE_H__11DF2DD8_A937_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_CREATUREPICTURE_H__11DF2DD8_A937_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CreaturePicture.h : main header file for CREATUREPICTURE.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureApp : See CreaturePicture.cpp for implementation.

class CCreaturePictureApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATUREPICTURE_H__11DF2DD8_A937_11D2_9D30_0090271EEABE__INCLUDED)

