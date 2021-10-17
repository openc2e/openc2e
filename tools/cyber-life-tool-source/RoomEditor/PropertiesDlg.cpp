// PropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "PropertiesDlg.h"
#include "C2ERoomEditorView.h"
#include "C2ERoomEditorDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesDlg dialog


CPropertiesDlg::CPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertiesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertiesDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesDlg message handlers

int CPropertiesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect( &rect );
	m_CtlParent.Create( IDD_EMPTY_CHILD, this );
	return 0;
}

void CPropertiesDlg::ReflectSelection( CC2ERoomEditorView *view )
{
	UpdatePropertyList( view->GetDocument()->GetPropertyTypes() );
}

void CPropertiesDlg::UpdatePropertyList( std::vector< CPropertyType > const &types )
{
	const int lHeight = 20;
	m_Ctls.clear();

	CRect rectLabel;
	GetClientRect( &rectLabel );
	rectLabel.bottom = lHeight;
	CRect rectEdit = rectLabel;
	rectLabel.right = rectLabel.right / 2;
	rectEdit.left = rectLabel.right;

	for( int i = 0; i < types.size(); ++i )
		if( types[i].m_Name != "" )
		{
			m_UsedProperties.insert( i );

			CStatic *stat = new CStatic();
			stat->Create( types[i].m_Name, SS_SIMPLE | WS_VISIBLE | WS_BORDER, rectLabel, &m_CtlParent );
			stat->SetFont( GetFont() );
			handle<CWnd> hWndLabel( stat );
			m_Ctls.push_back( hWndLabel );

			if( types[i].m_Enumerated )
			{
				CComboBox *combo = new CComboBox();
				CRect rectCombo = rectEdit;
				rectCombo.bottom += 100;
				combo->Create( CBS_DROPDOWNLIST | WS_VSCROLL | WS_VISIBLE, rectCombo, &m_CtlParent, i + 100 );
				combo->SetFont( GetFont() );
				handle<CWnd> hWndCombo( combo );
				m_Ctls.push_back( hWndCombo );
				CString enums = types[i].m_EnumNames + "|";
				for( int item = types[i].m_Min; item < types[i].m_Max; ++item )
				{
					CString itemString;
					int r = enums.Find( '|' );
					itemString.Format( "%d", item );
					if( r != -1 )
					{
						itemString += " - " + enums.Left( r );
						enums = enums.Right( enums.GetLength() - r - 1 );
					}
					combo->AddString( itemString );
				}
			}
			else
			{
				CEdit *edit = new CEdit();
				edit->Create( WS_VISIBLE | WS_BORDER | WS_TABSTOP, rectEdit, &m_CtlParent, i + 100 );
				edit->SetFont( GetFont() );
				handle<CWnd> hWndEdit( edit );
				m_Ctls.push_back( hWndEdit );
			}

			rectLabel += CPoint( 0, lHeight - 1);
			rectEdit += CPoint( 0, lHeight - 1);
		}

}

void CPropertiesDlg::SizeCtlParent()
{
	
}

void CPropertiesDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if( m_CtlParent.m_hWnd )
	{
		CRect rect;
		GetClientRect( &rect );
		m_CtlParent.MoveWindow( &rect );
	}
	
}

