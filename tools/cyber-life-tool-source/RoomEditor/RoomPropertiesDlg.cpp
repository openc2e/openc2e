// RoomPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "RoomPropertiesDlg.h"
#include "C2ERoomEditorDoc.h"
#include "C2ERoomEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoomPropertiesDlg dialog


CRoomPropertiesDlg::CRoomPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoomPropertiesDlg::IDD, pParent), m_View( 0 ), m_Updating( false )
{
	m_PropertyList.SetListOwner( this );
	//{{AFX_DATA_INIT(CRoomPropertiesDlg)
	//}}AFX_DATA_INIT
}


void CRoomPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoomPropertiesDlg)
	DDX_Control(pDX, IDC_DROP_BUTTON, m_DropButton);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_PROPERTY_LIST, m_PropertyList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoomPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CRoomPropertiesDlg)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	ON_EN_KILLFOCUS(IDC_EDIT, OnKillfocusEdit)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_DROP_BUTTON, OnDropButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomPropertiesDlg message handlers

void CRoomPropertiesDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pDI) 
{
	if( nIDCtl == IDC_DROP_BUTTON )
	{
		DrawFrameControl( pDI->hDC, &pDI->rcItem, DFC_SCROLL, DFCS_SCROLLDOWN );
		return;
	}
	COLORREF oldTextColour;
	int oldMode = SetBkMode( pDI->hDC, TRANSPARENT );
	CRect rectLabel( pDI->rcItem ), rectData( pDI->rcItem );
	rectLabel.right = ( rectLabel.left + rectLabel.right ) / 2;
	rectData.left = rectLabel.right;
	if( pDI->itemState & ODS_SELECTED )
	{
		FillRect( pDI->hDC, &rectLabel, GetSysColorBrush( COLOR_HIGHLIGHT ) );
		oldTextColour = SetTextColor( pDI->hDC, RGB( 255, 255, 255 ) );
	}
	else
	{
		FillRect( pDI->hDC, &rectLabel, (HBRUSH)GetStockObject( WHITE_BRUSH ) );
		oldTextColour = SetTextColor( pDI->hDC, RGB( 0, 0, 0 ) );
	}
	FillRect( pDI->hDC, &rectData, (HBRUSH)GetStockObject( WHITE_BRUSH ) );
	if( pDI->itemData < m_Types.size() ) 
		TextOut( pDI->hDC, pDI->rcItem.left, pDI->rcItem.top + 1,
			m_Types[ pDI->itemData ].m_Name,
			m_Types[ pDI->itemData ].m_Name.GetLength() );

	SetTextColor( pDI->hDC, RGB( 0, 0, 0 ) );
	if( int(pDI->itemID) >= 0 ) 
	{
		CString value = GetValueString( pDI->itemID );
		TextOut( pDI->hDC, rectData.left + 1, rectData.top + 1,
			value, value.GetLength() );
	}
	SetTextColor( pDI->hDC, oldTextColour );

	oldMode = SetBkMode( pDI->hDC, oldMode );


	CPen aPen( PS_SOLID, 0, RGB( 192, 192, 192 ) );
	HGDIOBJ oldPen = SelectObject( pDI->hDC, aPen );
	MoveToEx( pDI->hDC, pDI->rcItem.left, pDI->rcItem.bottom - 1, 0 );
	LineTo( pDI->hDC, pDI->rcItem.right, pDI->rcItem.bottom - 1 );
//	MoveToEx( pDI->hDC, pDI->rcItem.left, pDI->rcItem.top, 0 );
//	LineTo( pDI->hDC, pDI->rcItem.right, pDI->rcItem.top );
	MoveToEx( pDI->hDC, rectLabel.right, pDI->rcItem.top, 0 );
	LineTo( pDI->hDC, rectLabel.right, pDI->rcItem.bottom );
	SelectObject( pDI->hDC, oldPen );

	rectData.DeflateRect( 1, 1 );

	if( pDI->itemState & ODS_SELECTED && (int(pDI->itemID) != -1 ) )
	{
		if( m_Types[ pDI->itemData ].m_Enumerated )
		{
			rectData.left = rectData.right - rectData.Height();
			DrawFrameControl( pDI->hDC, &rectData, DFC_SCROLL, DFCS_SCROLLDOWN );
			m_PropertyList.ClientToScreen( &rectData );
			ScreenToClient( &rectData );
			m_DropButton.MoveWindow( &rectData );
			if( (pDI->itemAction & ODA_SELECT) )
			{
//				m_Combo.SetWindowText( m_Values[ pDI->itemID ] );
				m_DropButton.ShowWindow( SW_RESTORE );
				m_DropButton.EnableWindow( TRUE );
			}
			m_Edit.ShowWindow( SW_HIDE );
			m_Edit.EnableWindow( FALSE );
		}
		else
		{
			m_PropertyList.ClientToScreen( &rectData );
			ScreenToClient( &rectData );
			m_Edit.MoveWindow( &rectData );
			if( (pDI->itemAction & ODA_SELECT) )
			{
				m_Edit.SetWindowText( GetValueString( pDI->itemID ) );
				m_Edit.ShowWindow( SW_RESTORE );
				m_Edit.EnableWindow( TRUE );
			}
			m_DropButton.ShowWindow( SW_HIDE );
			m_DropButton.EnableWindow( FALSE );
		}
	}
	//	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CRoomPropertiesDlg::ReflectSelection( CC2ERoomEditorView *view, std::vector< CPropertyType > const &types )
{
	m_View = view;
	if( m_View )
	{
		m_Edit.ShowWindow( SW_HIDE );
		m_Edit.EnableWindow( FALSE );
		m_DropButton.ShowWindow( SW_HIDE );
		m_DropButton.EnableWindow( FALSE );
		if( !m_Updating ) UpdatePropertyList( types );
		for( int i = 0; i < m_IntValues.size(); ++i )
			m_Nulls[i] = !m_View->GetSelectionProperty( m_PropertyList.GetItemData( i ), &m_IntValues[i] );
	}
}

void CRoomPropertiesDlg::UpdatePropertyList( std::vector< CPropertyType > const &types )
{
	m_Types = types;
	m_IntValues.clear();
	m_Nulls.clear();
	m_PropertyList.ResetContent();
	for( int i = 0; i < types.size(); ++i )
		if( types[i].m_Name != "" )
		{
			m_PropertyList.AddString( (LPCTSTR) i );
			m_IntValues.push_back( 0 );
			m_Nulls.push_back( true );
		}
}

void CRoomPropertiesDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT pMI ) 
{
	CClientDC dc( this );
	CSize size = dc.GetTextExtent( "Sample Text" );
	pMI->itemHeight = size.cy + 1;
	pMI->itemWidth = size.cx;
//	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CRoomPropertiesDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if( m_PropertyList.m_hWnd )
	{
		CRect rect;
		GetClientRect( &rect );
		m_PropertyList.MoveWindow( &rect );
		m_PropertyList.Invalidate();
	}
}

void CRoomPropertiesDlg::OnChangeEdit() 
{
}

void CRoomPropertiesDlg::OnKillfocusEdit() 
{
	CString string;
	m_Edit.GetWindowText( string );
	int sel = m_PropertyList.GetCurSel();
	if( sel != -1 )
	{
		if( GetValueString( sel ) != string )
		{
			SetValueString( sel, string );
			SetSelectionProperty();
		}
	}
//	m_Edit.ShowWindow( SW_HIDE );	
//	m_Edit.EnableWindow( FALSE );
}

void CRoomPropertiesDlg::OnListScroll( CPropertyListBox *list )
{
	if( list == &m_PropertyList )
	{
		m_Edit.ShowWindow( SW_HIDE );	
		m_Edit.EnableWindow( FALSE );
		m_DropButton.ShowWindow( SW_HIDE );	
		m_DropButton.EnableWindow( FALSE );
	}
}

int CRoomPropertiesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ComboList.Create( this );	
	return 0;
}

void CRoomPropertiesDlg::OnDropButton() 
{
	int i = m_PropertyList.GetCurSel();
	if( i != -1 )
	{
		int item = m_PropertyList.GetItemData( i );
		m_ComboList.ResetContent();
		for( int j = m_Types[item].m_Min; j <= m_Types[item].m_Max; ++j )
			m_ComboList.AddString( GetEnumName( i, j ) );

		// Find correct rectangle to display
		int height = (m_Types[item].m_Max - m_Types[item].m_Min + 1) * m_ComboList.GetItemHeight( 0 ) + 2;
		if( height > 150 ) height = 150;
		CRect rect;
		m_PropertyList.GetItemRect( i, &rect );
		m_PropertyList.ClientToScreen( &rect );
		rect.left = (rect.left + rect.right) / 2;
		rect.top = rect.bottom;
		rect.bottom += height;
		m_ComboList.MoveWindow( &rect );
		if( !m_Nulls[ i ] ) m_ComboList.SetCurSel( m_IntValues[ i ] );
		m_ComboList.ShowWindow( SW_RESTORE );
	
	}
}

CString CRoomPropertiesDlg::GetEnumName( int index, int enumVal )
{
	int item = m_PropertyList.GetItemData( index );
	CString itemString;
	itemString.Format( "%d", enumVal );
	CString enums = m_Types[item].m_EnumNames + "|";

	int start = 0, len = enums.GetLength();
	while( start < len && enumVal )
		if( enums[start++] == '|' )
			--enumVal;

	int end = start;
	while( end < len && enums[end] != '|' )
		++end;

	if( end > start )
		itemString += " - " + enums.Mid( start, end - start );

	return itemString;
}

BOOL CRoomPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ComboList.SetFont( GetFont() );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CRoomPropertiesDlg::GetValueString( int index )
{
	if( m_Nulls[index] ) return "";
	int item = m_PropertyList.GetItemData( index );
	char buff[32];
	if( m_Types[item].m_Enumerated )
		return GetEnumName( index, m_IntValues[ index ] );
	else
		return CString( itoa( m_IntValues[ index ], buff, 10 ) );
}

void CRoomPropertiesDlg::SetValueString( int index, CString newValue )
{
	if( newValue == "" )
	{
		m_Nulls[index] = true;
		m_IntValues[ index ] = 0;
		return;
	}
	m_Nulls[index] = false;

	int item = m_PropertyList.GetItemData( index );
	if( m_Types[item].m_Enumerated )
	{
		for( int i = m_Types[item].m_Min; i <= m_Types[item].m_Max; ++i )
		{
			if( newValue == GetEnumName( index, m_IntValues[ index ] ) )
			{
				m_IntValues[ index ] = i;
				return;
			}
			m_Nulls[index] = true;
			m_IntValues[ index ] = 0;
		}
	}
	else
		m_IntValues[ index ] = atoi( newValue );
}

void CRoomPropertiesDlg::OnChangeSelection( CComboList *list, int selection )
{
	int i = m_PropertyList.GetCurSel();
	if( i != -1 && selection != -1 )
	{
		m_IntValues[ i ] = selection;
		m_Nulls[ i ] = false;
		CRect rect;
		m_PropertyList.GetItemRect( i, &rect );
		m_PropertyList.InvalidateRect( &rect );
		SetSelectionProperty();
	}
}

void CRoomPropertiesDlg::SetSelectionProperty()
{
	m_Updating = true;
	int sel = m_PropertyList.GetCurSel();
	if( sel != -1 && m_View )
		m_View->SetSelectionProperty( m_PropertyList.GetItemData( sel ), m_IntValues[ sel ] );
	m_Updating = false;
}

