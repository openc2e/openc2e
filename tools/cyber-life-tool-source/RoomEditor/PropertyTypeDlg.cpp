// PropertyTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "PropertyTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyTypeDlg dialog


CPropertyTypeDlg::CPropertyTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertyTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertyTypeDlg)
	m_EnumValues = _T("");
	m_Enumerated = FALSE;
	m_Max = 0;
	m_Min = 0;
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void CPropertyTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyTypeDlg)
	DDX_Control(pDX, IDC_ENUM_VALUES, m_EnumValuesCtl);
	DDX_Text(pDX, IDC_ENUM_VALUES, m_EnumValues);
	DDX_Check(pDX, IDC_ENUMERATED, m_Enumerated);
	DDX_Text(pDX, IDC_MAX, m_Max);
	DDX_Text(pDX, IDC_MIN, m_Min);
	DDX_Text(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertyTypeDlg)
	ON_BN_CLICKED(IDC_ENUMERATED, OnEnumerated)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyTypeDlg message handlers

void CPropertyTypeDlg::OnEnumerated() 
{
	UpdateData();
	m_EnumValuesCtl.EnableWindow( m_Enumerated );
}

BOOL CPropertyTypeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnEnumerated();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

