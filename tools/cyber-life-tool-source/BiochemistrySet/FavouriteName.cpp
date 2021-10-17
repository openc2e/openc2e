// FavouriteName.cpp : implementation file
//

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "FavouriteName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFavouriteName dialog


CFavouriteName::CFavouriteName(CWnd* pParent /*=NULL*/)
	: CDialog(CFavouriteName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFavouriteName)
	m_sName = _T("");
	//}}AFX_DATA_INIT
}


void CFavouriteName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFavouriteName)
	DDX_Text(pDX, IDC_NAME, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFavouriteName, CDialog)
	//{{AFX_MSG_MAP(CFavouriteName)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavouriteName message handlers

