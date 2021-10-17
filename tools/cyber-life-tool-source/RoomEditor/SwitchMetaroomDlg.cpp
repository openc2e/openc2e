// SwitchMetaroomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "SwitchMetaroomDlg.h"
#include "World.h"
#include "Game.h"
#include "utils.h"
#include "REException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSwitchMetaroomDlg dialog


CSwitchMetaroomDlg::CSwitchMetaroomDlg(CWorld *world, CWnd* pParent /*=NULL*/)
	: m_World( world ), CDialog(CSwitchMetaroomDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSwitchMetaroomDlg)
	m_DisplayMap = TRUE;
	m_Metacomboindex = -1;
	//}}AFX_DATA_INIT
}


void CSwitchMetaroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSwitchMetaroomDlg)
	DDX_Control(pDX, IDC_METAROOM, m_Metaroom);
	DDX_Control(pDX, IDC_BACKGROUND, m_Background);
	DDX_Check(pDX, IDC_DISPLAY_MAP, m_DisplayMap);
	DDX_CBIndex(pDX, IDC_METAROOM, m_Metacomboindex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSwitchMetaroomDlg, CDialog)
	//{{AFX_MSG_MAP(CSwitchMetaroomDlg)
	ON_BN_CLICKED(IDB_APPLY, OnApply)
	ON_CBN_SELCHANGE(IDC_METAROOM, OnSelchangeMetaroom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSwitchMetaroomDlg message handlers

void CSwitchMetaroomDlg::OnOK() 
{
	OnApply();
	CDialog::OnOK();
}

void CSwitchMetaroomDlg::OnApply() 
{
	UpdateData();
	if( m_Metacomboindex == CB_ERR ) return;
	try
	{
		CGame game;
		CRect rect = m_World->GetMetaroom(myMetaLookup[m_Metacomboindex])->GetRect();

		game.Execute( "meta gmap %d %d -1 -1 0", rect.CenterPoint().x, rect.CenterPoint().y );
		int back = m_Background.GetCurSel();
		if( back != CB_ERR )
		{
			CString str;
			m_Background.GetLBText( back, str );
			game.Execute( "bkgd gmap %d %d \"%s\" 0", rect.CenterPoint().x, rect.CenterPoint().y, (const char *)str );
		}
		int disp = m_DisplayMap ? 1 : 0;
		game.Execute( "dmap %d", disp );
	}
	catch( CREException const &exept)
	{
		AfxMessageBox( exept.what(), MB_OK | MB_ICONEXCLAMATION );
	}
	catch( ... )
	{
		AfxMessageBox( "An unknown error occurred", MB_OK | MB_ICONEXCLAMATION );
	}
}

BOOL CSwitchMetaroomDlg::OnInitDialog() 
{
	
	CDialog::OnInitDialog();

	myMetaLookup = m_World->GetMetaroomIndices();

	for( int i = 0; i < myMetaLookup.size(); ++i )
	{
		CString str;
		str.Format( "%d", myMetaLookup[i] );
		m_Metaroom.AddString( str );
	}

	UpdateBackgrounds( 0 );
	m_Metacomboindex = 0;
	UpdateData( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSwitchMetaroomDlg::UpdateBackgrounds( int index ) 
{
	m_Background.ResetContent();
	std::vector< CString > const &bList = m_World->GetMetaroom( myMetaLookup[index] )->GetBackgroundList();
	std::vector< CString >::const_iterator i;
	for( i = bList.begin(); i != bList.end(); ++i )
	{
		m_Background.AddString( GetFileTitle( *i ) );
	}
	m_Background.SetCurSel( 0 );
}	

void CSwitchMetaroomDlg::OnSelchangeMetaroom() 
{
	UpdateData();
	if( m_Metacomboindex != CB_ERR )
		UpdateBackgrounds( myMetaLookup[m_Metacomboindex] );
}

