// EmptyChildDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "EmptyChildDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmptyChildDlg dialog


CEmptyChildDlg::CEmptyChildDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEmptyChildDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmptyChildDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEmptyChildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmptyChildDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmptyChildDlg, CDialog)
	//{{AFX_MSG_MAP(CEmptyChildDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmptyChildDlg message handlers

void CEmptyChildDlg::OnCancel() 
{
}

void CEmptyChildDlg::OnOK() 
{
}

