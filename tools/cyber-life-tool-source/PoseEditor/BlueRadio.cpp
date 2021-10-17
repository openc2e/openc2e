// BlueRadio.cpp : implementation file
//

#include "stdafx.h"
#include "PoseEditor.h"
#include "BlueRadio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlueRadio

CBlueRadio::CBlueRadio()
{
}

CBlueRadio::~CBlueRadio()
{
}


BEGIN_MESSAGE_MAP(CBlueRadio, CButton)
	//{{AFX_MSG_MAP(CBlueRadio)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBlueRadio::Init( UINT id, CWnd *wnd )
{
	static CBitmap bitmapAll;
	static bool loaded = false;
	if( !loaded )
	{
		bitmapAll.LoadBitmap( IDB_BUTTON_FACE );
		loaded = true;
	}

	SubclassDlgItem( id, wnd );
	ModifyStyle( 0, BS_BITMAP );
	CDC dc;
	dc.CreateCompatibleDC( NULL );
	int test = dc.GetDeviceCaps( BITSPIXEL );
	CRect rect;
	CString caption;
	GetWindowText( caption );
	GetClientRect( rect );

//	bitmapAll.
	m_Bitmap.CreateCompatibleBitmap( &dc, rect.Width(), rect.Height() );
	CBitmap *oldBitmap = dc.SelectObject( &m_Bitmap );
	dc.FillSolidRect( rect, RGB( 192, 0, 0 ) );
	dc.SetTextAlign( TA_CENTER | TA_BOTTOM );
	dc.TextOut( rect.Width() /2 , rect.Height(), caption );
	dc.SelectObject( oldBitmap );

	m_Bitmap2.CreateCompatibleBitmap( &dc, rect.Width(), rect.Height() );
	dc.SelectObject( &m_Bitmap2 );
//	dc.FillSolidRect( rect, RGB( 0, 0, 192 ) );
	dc.SetTextAlign( TA_CENTER | TA_BOTTOM );
	dc.SetTextColor( RGB( 255, 255, 255 ) );
	dc.SetBkMode( TRANSPARENT );
	dc.TextOut( rect.Width() /2 , rect.Height(), caption );
	dc.SelectObject( oldBitmap );

	SetBitmap( (HBITMAP)m_Bitmap.m_hObject );
}


/////////////////////////////////////////////////////////////////////////////
// CBlueRadio message handlers

void CBlueRadio::OnClicked() 
{
	SetBitmap( (HBITMAP)m_Bitmap2.m_hObject );
}

