// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D09269D6_32C3_11D2_A804_0060B05E3B36__INCLUDED_)
#define AFX_STDAFX_H__D09269D6_32C3_11D2_A804_0060B05E3B36__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components

// We have to redefine _RICHEDIT_VER as the only way 
// to force MFC to use the new version
#pragma warning(disable : 4005)
#define _RICHEDIT_VER 0x0200 // for multiple Undo/Redo support
#pragma warning(default : 4005)

#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D09269D6_32C3_11D2_A804_0060B05E3B36__INCLUDED_)

