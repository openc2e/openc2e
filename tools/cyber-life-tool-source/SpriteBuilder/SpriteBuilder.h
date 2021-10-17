// SpriteBuilder.h : main header file for the SPRITEBUILDER application
//

#if !defined(AFX_SPRITEBUILDER_H__A049CFC4_51D9_11D1_8791_0060B07BFA18__INCLUDED_)
#define AFX_SPRITEBUILDER_H__A049CFC4_51D9_11D1_8791_0060B07BFA18__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderApp:
// See SpriteBuilder.cpp for the implementation of this class
//

class CSpriteBuilderApp : public CWinApp
{
public:
	CSpriteBuilderApp();
	inline CRecentFileList& RecentFileList()
	{	return *m_pRecentFileList;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpriteBuilderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSpriteBuilderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPRITEBUILDER_H__A049CFC4_51D9_11D1_8791_0060B07BFA18__INCLUDED_)

