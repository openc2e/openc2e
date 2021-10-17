// PoseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PoseEditor.h"
#include "PoseDlg.h"
#include "PoseEditorDoc.h"
#include "Appearance.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoseDlg dialog


CPoseDlg::CPoseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPoseDlg::IDD, pParent), m_Gene( 0 )
{
	//{{AFX_DATA_INIT(CPoseDlg)
	m_String = _T("");
	//}}AFX_DATA_INIT

	for( int i = 0; i < 15; ++i ) m_Part[i] = 0;
}


void CPoseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPoseDlg)
	DDX_Control(pDX, IDC_STRING, m_StringCtl);
	DDX_Control(pDX, ID_APPLY, m_Apply);
	DDX_Control(pDX, IDC_CREATUREPIC, m_CreaturePic);
	DDX_Text(pDX, IDC_STRING, m_String);
	//}}AFX_DATA_MAP
	const char *ValStrings[15] =
	{ "0123X?!", "012345X?", "0123X", "0123X", "0123X", "0123X", "0123X",
	"0123X", "0123X", "0123X", "0123X", "0123X", "0123X", "0123X", "0123X" };

	if( !pDX->m_bSaveAndValidate )
		for( int i = 0; i < 15; ++i )
			m_Part[i] = std::_MAX( 0, CString( ValStrings[i] ).Find( m_PoseString[i] ) );


	DDX_Radio(pDX, IDC_PART0, m_Part[0]);
	DDX_Radio(pDX, IDC_PART1, m_Part[1]);
	DDX_Radio(pDX, IDC_PART2, m_Part[2]);
	DDX_Radio(pDX, IDC_PART3, m_Part[3]);
	DDX_Radio(pDX, IDC_PART4, m_Part[4]);
	DDX_Radio(pDX, IDC_PART5, m_Part[5]);
	DDX_Radio(pDX, IDC_PART6, m_Part[6]);
	DDX_Radio(pDX, IDC_PART7, m_Part[7]);
	DDX_Radio(pDX, IDC_PART8, m_Part[8]);
	DDX_Radio(pDX, IDC_PART9, m_Part[9]);
	DDX_Radio(pDX, IDC_PART10, m_Part[10]);
	DDX_Radio(pDX, IDC_PART11, m_Part[11]);
	DDX_Radio(pDX, IDC_PART12, m_Part[12]);
	DDX_Radio(pDX, IDC_PART13, m_Part[13]);
	DDX_Radio(pDX, IDC_PART14, m_Part[14]);

	if( pDX->m_bSaveAndValidate )
		for( int i = 0; i < 15; ++i )
			m_PoseString.SetAt( i, ValStrings[i][m_Part[i]] );
}


BEGIN_MESSAGE_MAP(CPoseDlg, CDialog)
	//{{AFX_MSG_MAP(CPoseDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoseDlg message handlers

BOOL CPoseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_CreaturePic.SetPoseString( 0, m_PoseString ) ;
//_StringCtl.SetWindowText( m_PoseString );
	m_CreaturePic.SetScale( float(1.0) );

	SetWindowText( GetPoseDescription( m_Gene->m_Data[0] ).c_str() );
	CAppearance const &app = m_Doc->GetAppearance();
	UpdateAppearance( app );
/*
	CWnd *child = GetWindow( GW_CHILD );
	while( child )
	{
		if( (child->GetStyle() & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON )
		{
			m_RadioButtons.push_back( handle<CBlueRadio>( new CBlueRadio() ) );
			UINT iD = child->GetDlgCtrlID();
			m_RadioButtons.back()->Init( iD, this );

		}
		child = child->GetNextWindow();
	}
*/

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPoseDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CPoseDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UpdateData();
	m_Apply.EnableWindow( TRUE );
	m_CreaturePic.SetPoseString( 0, m_PoseString );
//	m_StringCtl.SetWindowText( m_PoseString );
	return CDialog::OnCommand(wParam, lParam);
}


void CPoseDlg::OnClose() 
{
	m_Doc->ClosePoseDlg( this );	
}

void CPoseDlg::OnCancel() 
{
	m_Doc->ClosePoseDlg( this );	
}

void CPoseDlg::OnOK() 
{
	OnApply();
	m_Doc->ClosePoseDlg( this );	
}

void CPoseDlg::OnApply() 
{
	UpdateData();
	CGene gene = *m_Gene;
	for( int i = 0; i < 15; ++i )
	{
		gene.m_Data[i+1] = m_PoseString[i];
	}
	m_Doc->ModifyGene( gene );
	m_Apply.EnableWindow( FALSE );
}

void CPoseDlg::Refresh()
{
	for( int i = 0; i < 15; ++i )
	{
		m_PoseString.SetAt( i, m_Gene->m_Data[i+1] );
	}
	UpdateData( FALSE );
	m_CreaturePic.SetPoseString( 0, m_PoseString );
	m_StringCtl.SetWindowText( m_PoseString );
}

void CPoseDlg::UpdateAppearance( CAppearance const &app )
{
	m_CreaturePic.SetAppearance( app.m_Head, app.m_HeadVar,
		app.m_Body, app.m_BodyVar, 
		app.m_Legs, app.m_LegsVar, 
		app.m_Arms, app.m_ArmsVar, 
		app.m_Tail, app.m_TailVar,
		app.m_Ears, app.m_EarsVar,
		app.m_Hair, app.m_HairVar,
		app.m_Sex, app.m_Age );
}

int CPoseDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	return 0;
}

