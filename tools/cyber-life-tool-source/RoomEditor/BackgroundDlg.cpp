// BackgroundDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "c2eroomeditordoc.h"
#include "BackgroundDlg.h"
#include "BackgroundFileDlg.h"
#include "utils.h"
#include "BMPToSprite.h"
#include "REException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackgroundDlg dialog


CBackgroundDlg::CBackgroundDlg(CC2ERoomEditorDoc *pDoc, int metaroom, CWnd* pParent /*=NULL*/)
	: CDialog(CBackgroundDlg::IDD, pParent), m_Doc( pDoc ), m_Metaroom( metaroom )
{
	//{{AFX_DATA_INIT(CBackgroundDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBackgroundDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackgroundDlg)
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackgroundDlg, CDialog)
	//{{AFX_MSG_MAP(CBackgroundDlg)
	ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackgroundDlg message handlers

BOOL CBackgroundDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HMetaroom metaroom = m_Doc->GetWorld()->GetMetaroom( m_Metaroom );
	std::vector< CString > const &list = metaroom->GetBackgroundList();
	std::vector< CString >::const_iterator itor;
	for( itor = list.begin(); itor != list.end(); ++itor )
		m_ListBox.AddString( GetFileTitle( *itor ) );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBackgroundDlg::OnProperties() 
{
	int sel = m_ListBox.GetCurSel();
	if( sel != -1 )
	{
		CBackgroundFileDlg dlg;
		dlg.m_Source = m_Doc->GetWorld()->GetMetaroom( m_Metaroom )->GetBackgroundList()[sel];
		dlg.m_Min = 64;
		dlg.m_Max = 192;
		if( dlg.DoModal() == IDOK )
		{
			CString source, dest, preview;
			source = dlg.m_Source;
			if( dlg.m_Sprite ) dest = GetImageDirectory() + GetFileTitle( source ) + ".blk";
			if( dlg.m_Preview ) preview = GetPreviewDirectory() + GetFileTitle( source ) + ".bmp";

			try
			{
				BMPToSprite( source, dest, preview, true, dlg.m_Min, dlg.m_Max );
				m_Doc->Execute( HAction( new CActionChangeMetaroomBackground( m_Metaroom, sel, source ) ) );
			}
			catch( CREException &except )
			{
				::AfxMessageBox( except.what(), MB_OK | MB_ICONEXCLAMATION );
			}
		}
	}
}

void CBackgroundDlg::OnDblclkList() 
{
	OnProperties();	
}

void CBackgroundDlg::OnRemove() 
{
	int sel = m_ListBox.GetCurSel();
	if( sel != -1 )
	{
		if( m_Doc->Execute( HAction( new CActionRemoveMetaroomBackground( m_Metaroom, sel ) ) ) )
			m_ListBox.DeleteString( sel );
	}
}

void CBackgroundDlg::OnAdd() 
{
	CFileDialog fileDlg( TRUE, ".bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Bitmaps (*.bmp)|*.bmp||" );

	if( fileDlg.DoModal() == IDOK )
	{
		CString imageDir = GetImageDirectory();
		CBackgroundFileDlg dlg;
		dlg.m_Source = fileDlg.GetPathName();
		dlg.m_Min = 64;
		dlg.m_Max = 192;
		if( dlg.DoModal() == IDOK )
		{
			CString source, dest, preview;
			source = dlg.m_Source;
			if( dlg.m_Sprite ) dest = GetImageDirectory() + GetFileTitle( source ) + ".blk";
			if( dlg.m_Preview ) preview = GetPreviewDirectory() + GetFileTitle( source ) + ".bmp";

			try
			{
				BMPToSprite( source, dest, preview, true, dlg.m_Min, dlg.m_Max );
				if( m_Doc->Execute( HAction( new CActionAddMetaroomBackground( m_Metaroom, source ) ) ) )
					m_ListBox.AddString( GetFileTitle( source ) );
			}
			catch( CREException &except )
			{
				::AfxMessageBox( except.what(), MB_OK | MB_ICONEXCLAMATION );
			}
		}
	}
}

void CBackgroundDlg::OnSet() 
{	
	int sel = m_ListBox.GetCurSel();
	if( sel != -1 )
	{
		CString preview;
		m_ListBox.GetText( sel, preview );
		preview = GetPreviewDirectory() + preview + ".bmp";
		m_Doc->Execute( HAction( new CActionSetMetaroomBackground( m_Metaroom, preview ) ) );
	}
}

void CBackgroundDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CBackgroundDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

