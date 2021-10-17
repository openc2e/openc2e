// MusicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "MusicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMusicDlg dialog


CMusicDlg::CMusicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMusicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMusicDlg)
	m_Music = _T("");
	//}}AFX_DATA_INIT
}


void CMusicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMusicDlg)
	DDX_Text(pDX, IDC_MUSIC, m_Music);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMusicDlg, CDialog)
	//{{AFX_MSG_MAP(CMusicDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMusicDlg message handlers

