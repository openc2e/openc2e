// StaticNew.cpp : implementation file
//

#include "stdafx.h"
#include "StaticNew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticNew

CStaticNew::CStaticNew()
{
	m_hIcon = NULL;
}

CStaticNew::~CStaticNew()
{
}


BEGIN_MESSAGE_MAP(CStaticNew, CStatic)
	//{{AFX_MSG_MAP(CStaticNew)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CStaticNew message handlers

void CStaticNew::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	CString str;
	CWnd* pOwner;
	CFont* pOldFont;

	GetClientRect( rect );

	/////////////////////////
	// draw the icon, if any


	if( m_hIcon )
	{
//		dc.DrawIcon( rect.right-m_iIconW,rect.top, m_hIcon );

		if( m_iIconPosition == iconLeft )
		{
			DrawIconEx( dc,
				rect.left,
				rect.top, m_hIcon,
				0,0,
				0,
				NULL,
				DI_NORMAL );
			rect.left += m_iIconW + 2;	// allow 2 pixels spacing before text
		}
		else		// iconRight
		{
			DrawIconEx( dc,
				rect.right-m_iIconW,
				rect.top, m_hIcon,
				0,0,
				0,
				NULL,
				DI_NORMAL );
			rect.right -= m_iIconW;
		}
	}

	/////////////////////////
	// draw the text

	GetWindowText( str );

	pOwner = GetParentOwner();
	if( pOwner )
		pOldFont = dc.SelectObject( pOwner->GetFont() );

	dc.SetBkMode( TRANSPARENT );
	dc.DrawText( str, rect, DT_LEFT );

	if( pOwner && pOldFont )
		dc.SelectObject( pOldFont );

	// Do not call CStatic::OnPaint() for painting messages
}


void CStaticNew::SetIconImage( UINT nIDResource, int iPositioning )
{
	ASSERT( iPositioning == iconLeft || iPositioning == iconRight );

	m_iIconW = GetSystemMetrics( SM_CXSMICON );
	m_iIconH = GetSystemMetrics( SM_CYSMICON );

	m_iIconPosition = iPositioning;
	m_hIcon = (HICON)LoadImage( AfxGetInstanceHandle(),
		MAKEINTRESOURCE( nIDResource ),
		IMAGE_ICON,
		0,
		0,
		0);
}

