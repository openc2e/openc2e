// HeightCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "HeightCheckDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeightCheckDlg dialog


CHeightCheckDlg::CHeightCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHeightCheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHeightCheckDlg)
	m_Height = 0;
	m_Permiability = 0;
	//}}AFX_DATA_INIT
}


void CHeightCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHeightCheckDlg)
	DDX_Text(pDX, IDC_MIN_HEIGHT, m_Height);
	DDV_MinMaxInt(pDX, m_Height, 0, 500);
	DDX_Text(pDX, IDC_MIN_PERM, m_Permiability);
	DDV_MinMaxInt(pDX, m_Permiability, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHeightCheckDlg, CDialog)
	//{{AFX_MSG_MAP(CHeightCheckDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeightCheckDlg message handlers

