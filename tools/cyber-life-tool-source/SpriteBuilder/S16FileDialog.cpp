// S16FileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteBuilder.h"
#include "S16FileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CS16FileDialog

IMPLEMENT_DYNAMIC(CS16FileDialog, CFileDialog)

CS16FileDialog::CS16FileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CS16FileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CS16FileDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CS16FileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	CButton* pChk1 = (CButton *)GetDlgItem(chx1);
	if (pChk1 == NULL)
		pChk1 = (CButton *)GetParent()->GetDlgItem(chx1);
	ASSERT(pChk1);

	if (m_bOpenFileDialog)
	{
		pChk1->SetWindowText("Old S16 format");
	}
	else
	{
		pChk1->SetWindowText("565 Format");
		pChk1->SetCheck(1); // Check the box.
		pChk1->EnableWindow(false);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

