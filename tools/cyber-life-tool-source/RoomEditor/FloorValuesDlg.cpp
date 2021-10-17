// FloorValuesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "FloorValuesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFloorValuesDlg dialog


CFloorValuesDlg::CFloorValuesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFloorValuesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFloorValuesDlg)
	//}}AFX_DATA_INIT
}


void CFloorValuesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFloorValuesDlg)
	DDX_Control(pDX, IDC_Y, m_Y);
	DDX_Control(pDX, IDC_X, m_X);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFloorValuesDlg, CDialog)
	//{{AFX_MSG_MAP(CFloorValuesDlg)
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloorValuesDlg message handlers

void CFloorValuesDlg::OnCalculate() 
{
	CString temp;
	m_X.GetWindowText(temp);
	float x = (float)atof( temp );
	float y = m_M * x + m_C;
	temp.Format( "%.6f", y );
	m_Y.SetWindowText( temp );
}

