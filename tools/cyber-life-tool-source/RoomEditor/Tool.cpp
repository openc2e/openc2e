#include "stdafx.h"
#include "resource.h"

#include "Tool.h"

#include "C2ERoomEditorView.h"
#include "C2ERoomEditorDoc.h"
#include "BMPToSprite.h"
#include "BackgroundFileDlg.h"
#include "REException.h"
#include "utils.h"
#include "mmsystem.h"
#include <algorithm>

void CTool::MouseMove( CPoint const &point, bool shift, bool ctrl )
{
	SetCursor();
	if( m_View->GetCapture() == m_View ) {
		CRect lastRect( m_PointDown, m_PointLast );
		CRect newRect( m_PointDown, point );
		lastRect.NormalizeRect();
		newRect.NormalizeRect();

		CClientDC dc( m_View );
		dc.DrawDragRect( newRect, CSize( 2,2 ), lastRect, CSize( 2, 2 ), dc.GetHalftoneBrush(), dc.GetHalftoneBrush() );
	}

	m_PointLast = point;
}

void CTool::SetCursor()
{
	::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
}

void CTool::MouseDown( bool shift, bool ctrl )
{
	m_TimeDown = timeGetTime();
	m_PointDown = m_PointLast;
	m_View->SetCapture();
}

void CTool::MouseUp( bool shift, bool ctrl )
{
	if( m_View->GetCapture() == m_View ) {
		CRect lastRect( m_PointDown, m_PointLast );
		lastRect.NormalizeRect();

		CClientDC dc( m_View );
		dc.DrawDragRect( lastRect, CSize( 2,2 ), NULL, CSize( 2, 2 ), dc.GetHalftoneBrush(), dc.GetHalftoneBrush() );
		ReleaseCapture();
	}
}

void CTool::DrawSelRect( CRect const &rect )
{
	CRect aRect = rect;
	aRect.NormalizeRect();

	CClientDC dc( m_View );
	dc.DrawDragRect( aRect, CSize( 2,2 ), NULL, CSize( 2, 2 ), dc.GetHalftoneBrush(), dc.GetHalftoneBrush() );
}

bool CTool::Dragging()
{
	DWORD timeNow = timeGetTime();
	return (timeNow - m_TimeDown > 250) || (Magnitude( m_PointLast - m_PointDown ) > 4);
}


void CToolAddRoom::MouseMove( CPoint const &point, bool shift, bool ctrl )
{
	DrawRoom();
	m_Points[ m_PointCount ] = m_View->GetViewParams().WindowToWorld( point );
	if( shift && m_PointCount == 3 )
	{
		//Snap either to the previous or first point
		if( abs( m_Points[ 3 ].y - m_Points[ 0 ].y ) < abs( m_Points[ 3 ].y - m_Points[ 2 ].y ) )
			m_Points[ 3 ].y = m_Points[ 0 ].y;
		else
			m_Points[ 3 ].y = m_Points[ 2 ].y;
	}
	else if( shift && m_PointCount )
	{
		m_Points[ m_PointCount ].y = m_Points[ m_PointCount - 1 ].y;
	}
	DrawRoom();
	::SetCursor( AfxGetApp()->LoadCursor( IDC_ADD_ROOM ) );
}

void CToolAddRoom::DrawRoom()
{
	if( m_PointCount ) {
		CViewParams params = m_View->GetViewParams();
		CClientDC dc( m_View );
		dc.SetROP2( R2_NOT );
		CPoint points[5];
		int n = FormRoomShape( m_Points, points, m_PointCount + 1, 4 );
		if( m_PointCount + 1 == 4 )
		{
			CRoom room( points );
			m_View->GetDocument()->GetWorld()->SnapRoom( &room, 0, params.WindowToWorld( 4 ) );
			room.Draw( &dc, &params );
		}
		else
		{
			for( int i = 0; i < n; ++i ) points[i] = params.WorldToWindow( points[i] );
			dc.Polyline( points, n );
		}
	}
}


int CToolAddRoom::FormRoomShape( CPoint *in, CPoint *out, int nPoints, int Tol )
{
	int i;

	for( i = 0; i < nPoints; ++i )
		out[i] = in[i];
	out[nPoints] = out[0];

	if( nPoints > 1 )
	{
		CPoint diff = out[1] - out[0];
		bool firstVertical = abs( diff.x ) < abs( diff.y ) / 10;
		if( nPoints > 2 )
		{
			CPoint diff2 = out[2] - out[1];
			firstVertical = abs( diff.y * diff2.x ) > abs( diff2.y * diff.x );
		}
		if( firstVertical )
		{
			out[1].x = out[0].x;
			if( nPoints > 3 )
				out[3].x = out[2].x;
		}
		else
		{
			if( nPoints > 2 )
				out[2].x = out[1].x;
			if( nPoints > 3 )
				out[3].x = out[0].x;
		}
	}


	if( nPoints > 2 )
		return nPoints + 1;
	else
		return nPoints;
}

void CToolAddRoom::MouseDown( bool shift, bool ctrl )
{
	DrawRoom();
	if( m_PointCount < 3 ) {
		++m_PointCount;
		m_Points[ m_PointCount ] = m_Points[ m_PointCount - 1 ];
	}
	else {
		CPoint points[5];
		FormRoomShape( m_Points, points, m_PointCount + 1, 4 );
		m_PointCount = 0;
		CRoom room( points );
		m_View->GetDocument()->GetWorld()->SnapRoom( &room, 0,
			m_View->GetViewParams().WindowToWorld( 4 ) );
		points[0] = room.GetShape().BottomLeft();
		points[1] = room.GetShape().TopLeft();
		points[2] = room.GetShape().TopRight();
		points[3] = room.GetShape().BottomRight();
		m_View->GetDocument()->
			Execute( HAction( new CActionAddRoom( points ) ) );
	}
	DrawRoom();
}

void CToolAddRoom::MouseUp( bool shift, bool ctrl )
{
}

void CToolAddRoom::StartUsing()
{
	m_PointCount = 0;
}

void CToolAddRoom::StopUsing()
{
	DrawRoom();
}

void CToolAddRoom::KeyPress( UINT key )
{
	if( key == VK_BACK && m_PointCount > 0 )
	{
		DrawRoom();
		--m_PointCount;
		DrawRoom();
	}
}

void CToolAddMetaroom::MouseUp( bool shift, bool ctrl )
{
	if( m_View->GetCapture() == m_View )
	{
		CTool::MouseUp( shift, ctrl );
		CFileDialog fileDlg( TRUE, ".bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"Bitmaps (*.bmp)|*.bmp||" );

		CRect rect( m_PointDown, m_PointLast );
		rect.NormalizeRect();
		rect = m_View->GetViewParams().WindowToWorld( rect );
		

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
					m_View->GetDocument()->Execute( HAction( new CActionAddMetaroom( source, rect ) ) );
				}
				catch( CREException &except )
				{
					::AfxMessageBox( except.what(), MB_OK | MB_ICONEXCLAMATION );
				}
			}
		}
	}
}

void CToolAddMetaroom::SetCursor()
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_ADD_METAROOM ) );
}

void CToolZoom::MouseUp( bool shift, bool ctrl )
{
	if( m_View->GetCapture() == m_View )
	{
		CRect rect( m_PointDown, m_PointLast );
		rect.NormalizeRect();
		rect = m_View->GetViewParams().WindowToWorld( rect );
		m_View->SetViewArea( rect );
	}
	CTool::MouseUp( shift, ctrl );
}

void CToolZoom::SetCursor()
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_ZOOM ) );
}

void CToolZoom::MouseMenu( CPoint const &point )
{
	CMenu menu;
	if( menu.LoadMenu( IDR_ZOOM_MENU ) )
	{
		CMenu *popup = menu.GetSubMenu( 0 );
		if( popup )
		{
			popup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, AfxGetMainWnd());
		}
	}
}


void CToolSelectMetaroom::MouseUp( bool shift, bool ctrl )
{
	if( m_View->GetCapture() == m_View )
	{
		ReleaseCapture();
		if( m_OverSelection && Dragging() ) {
			m_View->GetDocument()->
				Execute( HAction( new CActionMoveMetaroom( m_View->GetSelectedMetaroom(), m_MoveRect ) ) );
		}
		else {
			CPoint point = m_View->GetViewParams().WindowToWorld( m_PointLast );
			int tol = m_View->GetViewParams().WindowToWorld( 4 );
			m_View->SetSelectedMetaroom( m_View->GetDocument()->GetWorld()->FindMetaroom( point, tol ) );
		}
	}
}

void CToolSelectMetaroom::MouseMove( CPoint const &point, bool shift, bool ctrl )
{
	CViewParams params = m_View->GetViewParams(); 
	CPoint pointWorld = params.WindowToWorld( point );

	int sel = m_View->GetSelectedMetaroom();

	if( m_View->GetCapture() == m_View ) {
		if( m_OverSelection ) {
			DrawSelRect( params.WorldToWindow( m_MoveRect ) );
			m_MoveRect += pointWorld - params.WindowToWorld( m_PointLast );
			if( m_MoveRect.left < 0 )
			{
				m_MoveRect.right = m_MoveRect.Width();
				m_MoveRect.left = 0;
			}
			if( m_MoveRect.top < 0 )
			{
				m_MoveRect.bottom = m_MoveRect.Height();
				m_MoveRect.top = 0;
			}
			DrawSelRect( params.WorldToWindow( m_MoveRect ) );
		}
	}
	else {
		m_OverSelection = false;
		if( sel != -1 ) {
			m_MoveRect = m_View->GetDocument()->GetWorld()->GetMetaroom( sel )->GetRect();
			m_OverSelection = m_MoveRect.PtInRect( pointWorld ) != 0;
		}
	}
	::SetCursor( ::LoadCursor( NULL, m_OverSelection ? IDC_SIZEALL : IDC_ARROW ) );
	m_PointLast = point;

}

void CToolSelectRoom::MouseUp( bool shift, bool ctrl )
{
	CViewParams params = m_View->GetViewParams(); 
	CPoint pointWorldDown = params.WindowToWorld( m_PointDown );
	CPoint pointWorldLast = params.WindowToWorld( m_PointLast );

	if( m_View->GetCapture() == m_View )
	{
		int tol = params.WindowToWorld( 4 );
		ReleaseCapture();
		if( !m_MoveMode ) DrawSelRect( CRect( m_PointDown, m_PointLast ) );
		if( m_MoveMode && Dragging() )
		{
			CPoint move = pointWorldLast - pointWorldDown;
			int tol = params.WindowToWorld( 4 );
			m_View->GetDocument()->	Execute( HAction(
				new CActionMoveRoom( m_View->GetSelectedRooms(), move, m_MoveMode,
				*m_View->GetDocument()->GetWorld(), tol ) ) );
		}
		else if( Dragging() )
		{
			m_View->SetSelectedRooms( CRect( pointWorldDown, pointWorldLast ) );
		}
		else
		{
			HDoor door = m_View->GetDocument()->GetWorld()->FindDoor( pointWorldLast, tol );
			if( door.obj() )
			{
				if( ctrl )
					m_View->ToggleSelectedDoor( door );
				else
					m_View->SetSelectedDoor( door );
			}
			else
			{
				int room = m_View->GetDocument()->GetWorld()->FindRoom( pointWorldLast, 0 );
				if( ctrl )
					m_View->ToggleSelectedRoom( room );
				else
					m_View->SetSelectedRoom( room );
			}
		}
	}
}

void CToolSelectRoom::MouseMove( CPoint const &point, bool shift, bool ctrl )
{
	CViewParams params = m_View->GetViewParams(); 
	CPoint pointWorld = params.WindowToWorld( point );
	CPoint pointWorldDown = params.WindowToWorld( m_PointDown );
	CPoint pointWorldLast = params.WindowToWorld( m_PointLast );

	std::set< int > const &sel = m_View->GetSelectedRooms();
	CWorld const *world = m_View->GetDocument()->GetWorld();

	if( m_View->GetCapture() == m_View ) {
		if( m_MoveMode == 0 ) {
			DrawSelRect( CRect( m_PointDown, m_PointLast ) );
			DrawSelRect( CRect( m_PointDown, point ) );
		}
		else if( m_MoveMode ) {
			CClientDC dc( m_View );
			dc.SetROP2( R2_NOT );
			std::set< int >::const_iterator itor;
			for( itor = sel.begin(); itor != sel.end(); ++itor )
			{
				CRoom room = *world->GetRoom( *itor );
				room.Move( pointWorldLast - pointWorldDown, m_MoveMode );
				world->SnapRoom( &room, &sel, params.WindowToWorld( 4 ), world->GetRoomMetaroom( *itor ) );
				room.Draw( &dc, &params );

				room = *world->GetRoom( *itor );
				room.Move( pointWorld - pointWorldDown, m_MoveMode );
				world->SnapRoom( &room, &sel, params.WindowToWorld( 4 ), world->GetRoomMetaroom( *itor ) );
				room.Draw( &dc, &params );
			}
		}
	}
	else {
		m_MoveMode = 0;
		if( sel.size() == 1 )
		{
			HRoom room = world->GetRoom( *sel.begin() );
			if( params.PtNear( room->GetShape().TopLeft(), pointWorld ) )
				m_MoveMode = CRoomShape::Move_Left | CRoomShape::Move_LeftTop;
			else if( params.PtNear( room->GetShape().BottomLeft(), pointWorld ) )
				m_MoveMode = CRoomShape::Move_Left | CRoomShape::Move_LeftBottom;
			else if( params.PtNear( room->GetShape().TopRight(), pointWorld ) )
				m_MoveMode = CRoomShape::Move_Right | CRoomShape::Move_RightTop;
			else if( params.PtNear( room->GetShape().BottomRight(), pointWorld ) )
				m_MoveMode = CRoomShape::Move_Right | CRoomShape::Move_RightBottom;
		}
		std::set< int >::const_iterator itor;
		if( !m_MoveMode )
			for( itor = sel.begin(); itor != sel.end(); ++itor )
				if( world->GetRoom( *itor )->ContainsPoint( pointWorld, 0 ) )
					m_MoveMode = CRoomShape::Move_All;
	}

	char *cursor;
	switch( m_MoveMode )
	{
	case CRoomShape::Move_Left | CRoomShape::Move_LeftTop:
	case CRoomShape::Move_Right | CRoomShape::Move_RightBottom:
		cursor = IDC_SIZENWSE;
		break;
	case CRoomShape::Move_Left | CRoomShape::Move_LeftBottom:
	case CRoomShape::Move_Right | CRoomShape::Move_RightTop:
		cursor = IDC_SIZENESW;
		break;
	case CRoomShape::Move_All:
		cursor = IDC_SIZEALL;
		break;
	default:
		cursor = IDC_ARROW;
		break;
	}
	::SetCursor( ::LoadCursor( NULL, cursor ) );
	m_PointLast = point;

}

void CToolSelectRoom::MouseMenu( CPoint const &point )
{
	CMenu menu;
	if( menu.LoadMenu( IDR_SELECT_MENU ) )
	{
		CMenu *popup = menu.GetSubMenu( 0 );
		if( popup )
		{
			popup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, AfxGetMainWnd());
		}
	}
}

void CToolCheese::MouseUp( bool shift, bool ctrl )
{
	CViewParams params = m_View->GetViewParams(); 
	CPoint pointWorldDown = params.WindowToWorld( m_PointDown );
	CTool::MouseUp( shift, ctrl );
	m_View->GetDocument()->AddCheese( pointWorldDown );
	m_View->GetDocument()->UpdateAllViews( NULL );
}

