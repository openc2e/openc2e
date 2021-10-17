// DirDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuickNorn.h"
#include "DirDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirDlg dialog


CDirDlg::CDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDirDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDirDlg)
	m_Bitmaps = _T("");
	m_BodyData = _T("");
	m_Suffix = _T("");
	//}}AFX_DATA_INIT
}


void CDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirDlg)
	DDX_Text(pDX, IDC_BITMAPS, m_Bitmaps);
	DDX_Text(pDX, IDC_BODY_DATA, m_BodyData);
	DDX_Text(pDX, IDC_SUFFIX, m_Suffix);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirDlg, CDialog)
	//{{AFX_MSG_MAP(CDirDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirDlg message handlers

