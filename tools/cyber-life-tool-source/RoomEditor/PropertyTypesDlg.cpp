// PropertyTypesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "PropertyTypesDlg.h"
#include "PropertyTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyTypesDlg dialog


CPropertyTypesDlg::CPropertyTypesDlg( std::vector< CPropertyType > const &types, CWnd* pParent /*=NULL*/)
	: m_Types( types ), CDialog(CPropertyTypesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertyTypesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropertyTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyTypesDlg)
	DDX_Control(pDX, IDC_PROPERTY_TYPES_LIST, m_TypesList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyTypesDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertyTypesDlg)
	ON_LBN_DBLCLK(IDC_PROPERTY_TYPES_LIST, OnDblclkPropertyTypesList)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyTypesDlg message handlers

BOOL CPropertyTypesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int tabs[3] = {131, 155, 181};
	m_TypesList.SetTabStops( 3, tabs );

	for( int i = 0; i < m_Types.size(); ++i )
	{
		m_TypesList.AddString( GetListString( i ) );
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyTypesDlg::OnDblclkPropertyTypesList() 
{
	OnEdit();
}

CString CPropertyTypesDlg::GetListString( int i )
{
	CString item;
	CString name = m_Types[i].m_Name;
	if( name == "" ) name = "[unused]";
	item.Format( "%d - %s\t%d\t%d\t%s", i,
		(const char *)name,
		m_Types[i].m_Min, m_Types[i].m_Max,
		m_Types[i].m_Enumerated ? "y" : "n" );
	return item;
}

void CPropertyTypesDlg::OnEdit() 
{
	CPropertyTypeDlg dlg;
	int i = m_TypesList.GetCurSel();
	if( i == -1 ) return;
	dlg.m_Name = m_Types[i].m_Name;
	dlg.m_Min = m_Types[i].m_Min;
	dlg.m_Max = m_Types[i].m_Max;
	dlg.m_Enumerated = m_Types[i].m_Enumerated;
	dlg.m_EnumValues = m_Types[i].m_EnumNames;
	dlg.m_EnumValues.Replace( "|", "\r\n" );
	if( dlg.DoModal() == IDOK )
	{
		m_Types[i].m_Name = dlg.m_Name;
		m_Types[i].m_Min = dlg.m_Min;
		m_Types[i].m_Max = dlg.m_Max;
		m_Types[i].m_Enumerated = dlg.m_Enumerated == TRUE;
		m_Types[i].m_EnumNames = dlg.m_EnumValues;
		m_Types[i].m_EnumNames.Replace( "\r\n", "|" );
		m_TypesList.DeleteString( i );
		m_TypesList.InsertString( i, GetListString( i ) );
		m_TypesList.SetCurSel( i );
	}
}

