// PictureEx.cpp : implementation file
//

#include "stdafx.h"
#include "QuickNorn.h"
#include "PictureEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPictureEx

CPictureEx::CPictureEx() : m_Client( 0 )
{
}

CPictureEx::~CPictureEx()
{
}


BEGIN_MESSAGE_MAP(CPictureEx, CStatic)
	//{{AFX_MSG_MAP(CPictureEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureEx message handlers

void CPictureEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_Client ) m_Client->OnPictureClick( this, point );
	CStatic::OnLButtonDown(nFlags, point);
}

BOOL CPictureEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_CROSSHAIR ) );
	return TRUE;
}

void CPictureEx::OnPaint() 
{
//	CPaintDC dc(this); // device context for painting

	CStatic::OnPaint();
	CClientDC dc( this );
	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255, 0, 0 ) );
	for( int i = 0; i != m_Points.size(); ++i )
	{
		char buff[10];
		dc.SetPixel( m_Points[i], RGB( 255, 0, 0 ) );
		dc.TextOut( m_Points[i].x, m_Points[i].y, itoa( i, buff, 10 )  );
	}
	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages
}

