// C2E Room EditorView.cpp : implementation of the CC2ERoomEditorView class
//

#include "stdafx.h"
#include "C2ERoomEditor.h"

#include "C2ERoomEditorDoc.h"
#include "C2ERoomEditorView.h"
#include "REException.h"
#include "MainFrm.h"
#include "BackgroundDlg.h"
#include "utils.h"
#include "FloorValuesDlg.h"
#include "Game.h"
#include "MusicDlg.h"
#include "HeightCheckDlg.h"
#include <strstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorView

IMPLEMENT_DYNCREATE(CC2ERoomEditorView, CView)

BEGIN_MESSAGE_MAP(CC2ERoomEditorView, CView)
	//{{AFX_MSG_MAP(CC2ERoomEditorView)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_TOOLS_ADD_METAROOM, OnToolsAddMetaroom)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ADD_METAROOM, OnUpdateToolsAddMetaroom)
	ON_COMMAND(ID_TOOLS_ADD_ROOM, OnToolsAddRoom)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ADD_ROOM, OnUpdateToolsAddRoom)
	ON_COMMAND(ID_TOOLS_SELECT_METAROOM, OnToolsSelectMetaroom)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECT_METAROOM, OnUpdateToolsSelectMetaroom)
	ON_COMMAND(ID_TOOLS_SELECT_ROOM, OnToolsSelectRoom)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECT_ROOM, OnUpdateToolsSelectRoom)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_TOOLS_ZOOM, OnToolsZoom)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ZOOM, OnUpdateToolsZoom)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_VIEW_ZOOM_WORLD, OnViewZoomWorld)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_ZOOM_FORWARD, OnViewZoomForward)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_FORWARD, OnUpdateViewZoomForward)
	ON_COMMAND(ID_VIEW_ZOOM_LAST, OnViewZoomLast)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_LAST, OnUpdateViewZoomLast)
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_EDIT_BACKGROUND, OnEditBackground)
	ON_UPDATE_COMMAND_UI(ID_EDIT_BACKGROUND, OnUpdateEditBackground)
	ON_COMMAND(ID_FILE_VALIDATE, OnFileValidate)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_TOOLS_CHEESE, OnToolsCheese)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHEESE, OnUpdateToolsCheese)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_SHOW_BACKGROUND, OnViewShowBackground)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_BACKGROUND, OnUpdateViewShowBackground)
	ON_COMMAND(ID_TOOLS_RUN_CA, OnToolsRunCa)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RUN_CA, OnUpdateToolsRunCa)
	ON_COMMAND(ID_EDIT_FLOORCEILINGVALUES, OnEditFloorceilingvalues)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FLOORCEILINGVALUES, OnUpdateEditFloorceilingvalues)
	ON_COMMAND(ID_TOOLS_UPDATE_CA_FROM_GAME, OnToolsUpdateCaFromGame)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_UPDATE_CA_FROM_GAME, OnUpdateToolsUpdateCaFromGame)
	ON_COMMAND(ID_EDIT_MUSIC, OnEditMusic)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MUSIC, OnUpdateEditMusic)
	ON_COMMAND(ID_EDIT_CHECKHEIGHTS, OnEditCheckheights)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND_RANGE(ID_VIEW_COLOURROOMS_CA0, ID_VIEW_COLOURROOMS_CA19, OnViewCA )
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_COLOURROOMS_CA0, ID_VIEW_COLOURROOMS_CA19, OnUpdateViewCA )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorView construction/destruction

CC2ERoomEditorView::CC2ERoomEditorView()
	: m_SelectedMetaroom( -1 ),
	  m_ColourRoomOpt( -1 ),
	  m_CheeseRadius( 10 ),
	  m_Timer( 0 ),
	  m_ShowBackground( true ),
	  m_RunCA( false ),
	  m_UpdateCA( false )
{
	m_ToolAddRoom.SetView( this );
	m_ToolAddMetaroom.SetView( this );
	m_ToolSelectRoom.SetView( this );
	m_ToolSelectMetaroom.SetView( this );
	m_ToolZoom.SetView( this );
	m_ToolCheese.SetView( this );
	m_Tool = &m_ToolSelectRoom;
}

CC2ERoomEditorView::~CC2ERoomEditorView()
{
	if( GetPropertiesDlg()->GetView() == this )
		GetPropertiesDlg()->ReflectSelection( 0, std::vector< CPropertyType >() );
}

BOOL CC2ERoomEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, 0/*, GetSysColorBrush(COLOR_WINDOW)*/);
	cs.style |= WS_HSCROLL | WS_VSCROLL;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorView drawing

void CC2ERoomEditorView::OnDraw(CDC* pDC)
{
	CC2ERoomEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CDC dc;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;

	CRect Rect;
	CViewParams params = GetViewParams();
	if( m_ColourRoomOpt != -1 )
	{
		int pMin = pDoc->GetPropertyType( m_ColourRoomOpt + 7 ).m_Min;
		int pMax = pDoc->GetPropertyType( m_ColourRoomOpt + 7 ).m_Max;
		CString range;
//		range.Format( "{%d,0,0,0} {%d,255,255,255}", pMin, pMax );
		range.Format( "{0,0,0,255} {64,0,255,255} {128,0,255,0} {192,255,255,0} {256,255,0,0}", pMin, pMax );
		params.SetParamRange( range );
	}
	if( pDC->IsPrinting() )
	{
		Rect = CRect( 0, 0, pDC->GetDeviceCaps( HORZRES ),pDC->GetDeviceCaps( VERTRES ) );
		params = CViewParams( m_ViewArea, Rect );
		pDrawDC = pDC;
	}
	else
	{
		GetClientRect( Rect );
		// draw to offscreen bitmap for fast looking repaints
		if (dc.CreateCompatibleDC(pDC))
		{
			if (bitmap.CreateCompatibleBitmap(pDC, Rect.Width(), Rect.Height()))
			{
				pDrawDC = &dc;
				pOldBitmap = dc.SelectObject(&bitmap);
			}
		}
		pDrawDC->FillSolidRect(Rect, RGB(255, 255, 192) );
	}

	pDoc->GetWorld()->Draw( pDrawDC, &params );


	if (pDrawDC != pDC)
	{
		pDC->BitBlt(Rect.left, Rect.top, Rect.Width(), Rect.Height(),
			&dc, 0, 0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
	}
	DrawCheese( pDC );
	HighlightSelection( pDC );
}

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorView printing

BOOL CC2ERoomEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CC2ERoomEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CC2ERoomEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorView diagnostics

#ifdef _DEBUG
void CC2ERoomEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CC2ERoomEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CC2ERoomEditorDoc* CC2ERoomEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CC2ERoomEditorDoc)));
	return (CC2ERoomEditorDoc*)m_pDocument;
}
#endif //_DEBUG

CViewParams CC2ERoomEditorView::GetViewParams() const
{
	CRect rect;
	GetClientRect( &rect );
	CViewParams params( m_ViewArea, rect, m_ColourRoomOpt, true, true, m_ShowBackground );
	return params;
}

void CC2ERoomEditorView::SetViewArea( CRect const &rect )
{
	m_BackRects.push( m_ViewArea );
	while( !m_ForwardRects.empty() ) m_ForwardRects.pop();
	m_ViewArea = rect;
	m_ViewArea.NormalizeRect();
	UpdateScrollInfo();
	Invalidate();
}

void CC2ERoomEditorView::ClearSelection()
{
	CClientDC dc( this );

	HighlightSelection( &dc );

	m_SelectedRooms.clear();
	m_SelectedDoors.clear();
	m_SelectedMetaroom = -1;
}

void CC2ERoomEditorView::SetSelectedMetaroom( int index )
{
	CClientDC dc( this );

	ClearSelection();

	m_SelectedMetaroom = index;

	HighlightSelection( &dc );

	ReflectSelection();
}

void CC2ERoomEditorView::SetSelectedRoom( int index )
{
	CClientDC dc( this );

	ClearSelection();

	if( index != -1 ) m_SelectedRooms.insert( index );

	HighlightSelection( &dc );

	ReflectSelection();
}

void CC2ERoomEditorView::SetSelectedRooms( CRect const &rect )
{
	CClientDC dc( this );

	ClearSelection();

	CRect rectNorm = rect;
	rectNorm.NormalizeRect();

	GetDocument()->GetWorld()->FindRooms( rectNorm, m_SelectedRooms );

	HighlightSelection( &dc );

	ReflectSelection();
}

void CC2ERoomEditorView::SetSelectedDoor( HDoor door )
{
	CClientDC dc( this );

	ClearSelection();

	if( door.obj() ) m_SelectedDoors.insert( door );

	HighlightSelection( &dc );

	ReflectSelection();
}

void CC2ERoomEditorView::ToggleSelectedRoom( int index )
{
	CClientDC dc( this );
	HighlightSelection( &dc );

	m_SelectedMetaroom = -1;
	m_SelectedDoors.clear();
	if( index != -1 )
	{
		std::set< int >::iterator itor = m_SelectedRooms.find( index );
		if( itor == m_SelectedRooms.end() )
			m_SelectedRooms.insert( index );
		else
			m_SelectedRooms.erase( itor );
	}

	HighlightSelection( &dc );

	ReflectSelection();
}

void CC2ERoomEditorView::ToggleSelectedDoor( HDoor door )
{
	CClientDC dc( this );
	HighlightSelection( &dc );

	m_SelectedMetaroom = -1;
	m_SelectedRooms.clear();
	if( door.obj() )
	{
		CDoorSet::iterator itor = m_SelectedDoors.find( door );
		if( itor == m_SelectedDoors.end() )
			m_SelectedDoors.insert( door );
		else
			m_SelectedDoors.erase( itor );
	}

	HighlightSelection( &dc );

	ReflectSelection();
}

int CC2ERoomEditorView::GetSelectedMetaroom() const
{
	return m_SelectedMetaroom;
}

bool CC2ERoomEditorView::GetSelectionProperty( int propertyIndex, int *pValue )
{
	if( m_SelectedRooms.size() )
	{
		std::set<int>::const_iterator itor = m_SelectedRooms.begin();
		*pValue = GetDocument()->GetWorld()->GetRoom( *itor )->GetProperty( propertyIndex );

		while( ++itor != m_SelectedRooms.end() )
			if( GetDocument()->GetWorld()->GetRoom( *itor )->GetProperty( propertyIndex ) != *pValue )
				return false;
		return true;
	}
	else if( m_SelectedDoors.size() )
	{
		CDoorSet::const_iterator itor = m_SelectedDoors.begin();
		*pValue = (*itor)->GetOpening();

		while( ++itor != m_SelectedDoors.end() )
			if( (*itor)->GetOpening() != *pValue )
				return false;
		return true;
	}
	return false;
}

void CC2ERoomEditorView::SetSelectionProperty( int propertyIndex, int value )
{
	if( m_SelectedRooms.size() )
		GetDocument()->Execute( HAction( new CActionSetRoomProperty( m_SelectedRooms, propertyIndex, value ) ) );
	else if( m_SelectedDoors.size() )
		GetDocument()->Execute( HAction( new CActionSetDoorProperty( m_SelectedDoors, propertyIndex, value ) ) );
}

void CC2ERoomEditorView::HighlightSelection( CDC *dc )
{
	CViewParams viewParams = GetViewParams();
	CWorld *world = GetDocument()->GetWorld();

	if( m_SelectedMetaroom != -1 ) {
		world->GetMetaroom( m_SelectedMetaroom )->
			DrawHandles( dc, &viewParams );
	}
	std::set<int>::const_iterator itor;
	int mapMode = dc->SetMapMode( MM_TEXT );
	::SetBkMode( dc->m_hDC, OPAQUE );

	int oldOP = dc->SetROP2( R2_XORPEN );
	dc->SetBkColor( RGB( 0, 0, 0 ) );
	dc->SetTextColor( RGB( 0, 0, 0 ) );
	dc->SetPolyFillMode( WINDING );

	CPen nullPen( PS_NULL, 0, RGB( 0, 0, 0 ) );
	CBrush brush( HS_DIAGCROSS, RGB( 255, 255, 255 ) );
	CPen *oldPen = dc->SelectObject( &nullPen );
	CBrush *oldBrush = dc->SelectObject( &brush );
	for( itor = m_SelectedRooms.begin(); itor != m_SelectedRooms.end(); ++itor )
	{
		world->GetRoom( *itor )->DrawHandles( dc, &viewParams );
//		int code = ::SetBkMode( dc->m_hDC, TRANSPARENT );
		world->GetRoom( *itor )->Fill( dc, &viewParams );
	}
	dc->SelectObject( oldBrush );

	CPen pen( PS_SOLID, 3, RGB( 255, 255, 255 ) );
	dc->SelectObject( &pen );
	CDoorSet::const_iterator itorDoor;
	for( itorDoor = m_SelectedDoors.begin(); itorDoor != m_SelectedDoors.end(); ++itorDoor )
	{
		dc->MoveTo( viewParams.WorldToWindow( (*itorDoor)->GetPoint1() ) );
		dc->LineTo( viewParams.WorldToWindow( (*itorDoor)->GetPoint2() ) );
	}

	dc->SelectObject( oldPen );
	dc->SetROP2( oldOP );
}

void CC2ERoomEditorView::DrawCheese( CDC *dc, bool inc )
{
	CViewParams viewParams = GetViewParams();
	std::vector< CCheese > const &cheeses = GetDocument()->GetCheeses();

	int oldOP = dc->SetROP2( R2_XORPEN );
	CBrush brush;
	brush.CreateStockObject( NULL_BRUSH );
	CBrush *oldBrush = dc->SelectObject( &brush );
	CPen pen( PS_SOLID, 0, RGB( 255, 255, 255 ) );
	CPen *oldPen = dc->SelectObject( &pen );

	std::vector< CCheese >::const_iterator itor;
	int j = 0;
	for( itor = cheeses.begin(); itor != cheeses.end(); ++itor )
	{
		CPoint p = viewParams.WorldToWindow( itor->GetPoint() );
		for( int i = 0; i < 20; i+=5 )
		{
			int r = ( m_CheeseRadius + i + j ) % 20;
			dc->Ellipse( p.x - r, p.y - r,
				p.x + r, p.y + r );
			if( inc )
			{
				int r = ( m_CheeseRadius + i + j + 1) % 20;
				dc->Ellipse( p.x - r, p.y - r,
					p.x + r, p.y + r );
			}
		}
		++j;
	}
	dc->SelectObject( oldPen );
	dc->SelectObject( oldBrush );
	dc->SetROP2( oldOP );
}

/////////////////////////////////////////////////////////////////////////////
// CC2ERoomEditorView message handlers

void CC2ERoomEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CViewParams viewParams = GetViewParams();
	SetCurrentPoint( viewParams.WindowToWorld( point ) );
	m_Tool->MouseMove( point, (nFlags & MK_SHIFT) != 0, (nFlags & MK_CONTROL) != 0 );	
	
	CView::OnMouseMove(nFlags, point);
}

void CC2ERoomEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_Tool->MouseMove( point, (nFlags & MK_SHIFT) != 0, (nFlags & MK_CONTROL) != 0 );	
	m_Tool->MouseDown( (nFlags & MK_SHIFT) != 0, (nFlags & MK_CONTROL) != 0 );	
	CView::OnLButtonDown(nFlags, point);
}

void CC2ERoomEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_Tool->MouseUp( (nFlags & MK_SHIFT) != 0, (nFlags & MK_CONTROL) != 0 );	
	
	CView::OnLButtonUp(nFlags, point);
}

void CC2ERoomEditorView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	m_ViewArea = CRect( CPoint(0,0), GetDocument()->GetWorld()->GetSize() );
	int expand = max( m_ViewArea.Height(), m_ViewArea.Width() ) / 20;
	m_ViewArea.InflateRect( expand, expand );
	UpdateScrollInfo();

	m_Timer = SetTimer( ID_TIMER, 1000, NULL );
}

void CC2ERoomEditorView::OnToolsAddMetaroom() 
{
	m_Tool->StopUsing();
	m_Tool = &m_ToolAddMetaroom;
	m_Tool->StartUsing();
}

void CC2ERoomEditorView::OnUpdateToolsAddMetaroom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_Tool == &m_ToolAddMetaroom );
}

void CC2ERoomEditorView::OnToolsAddRoom() 
{
	m_Tool->StopUsing();
	m_Tool = &m_ToolAddRoom;
	m_Tool->StartUsing();
}

void CC2ERoomEditorView::OnUpdateToolsAddRoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_Tool == &m_ToolAddRoom );
}

void CC2ERoomEditorView::OnToolsSelectMetaroom() 
{
	m_Tool->StopUsing();
	m_Tool = &m_ToolSelectMetaroom;
	m_Tool->StartUsing();
}

void CC2ERoomEditorView::OnUpdateToolsSelectMetaroom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_Tool == &m_ToolSelectMetaroom );
}

void CC2ERoomEditorView::OnToolsSelectRoom() 
{
	m_Tool->StopUsing();
	m_Tool = &m_ToolSelectRoom;
	m_Tool->StartUsing();
}

void CC2ERoomEditorView::OnUpdateToolsSelectRoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_Tool == &m_ToolSelectRoom );
}

void CC2ERoomEditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
// Must deselect any rooms or metarooms which are about to be deleted
	if( lHint == CAction::DESELECT_METAROOM )
	{
		int *mr = (int *)pHint;
		if( m_SelectedMetaroom == *mr ) m_SelectedMetaroom = -1;
		std::set< int >::iterator itor = m_SelectedRooms.begin();
		while( itor != m_SelectedRooms.end() )
		{
			if( GetDocument()->GetWorld()->GetRoomMetaroom( *itor ) == *mr )
				itor = m_SelectedRooms.erase( itor );
			else
				++itor;
		}
	}
	else if( lHint == CAction::DESELECT_ROOM )
	{
		int *mr = (int *)pHint;
		std::set< int >::iterator itor = m_SelectedRooms.find( *mr );
		if( itor != m_SelectedRooms.end() ) m_SelectedRooms.erase( itor );
	}
	else if( lHint == CAction::REFLECT_ROOM_PROPERTIES )
	{
		if( GetPropertiesDlg()->GetView() == this )
			ReflectSelection();
	}
	else
		CView::OnUpdate( pSender, lHint, pHint );
}

void CC2ERoomEditorView::OnEditDelete() 
{
	if( m_SelectedMetaroom != -1 )
		GetDocument()->Execute( HAction( new CActionRemoveMetaroom( m_SelectedMetaroom ) ) );
	if( !m_SelectedRooms.empty() )
		GetDocument()->Execute( HAction( new CActionRemoveRoom( m_SelectedRooms ) ) );
}

void CC2ERoomEditorView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_SelectedMetaroom != -1 || !m_SelectedRooms.empty() );
}

void CC2ERoomEditorView::OnToolsZoom() 
{
	m_Tool->StopUsing();
	m_Tool = &m_ToolZoom;
	m_Tool->StartUsing();
}

void CC2ERoomEditorView::OnUpdateToolsZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_Tool == &m_ToolZoom );
}

void CC2ERoomEditorView::UpdateScrollInfo()
{
	CSize size = GetDocument()->GetWorld()->GetSize();
	CRect VisibleRect = GetWorldVisible();
	SCROLLINFO sInfo;
	sInfo.cbSize = sizeof(SCROLLINFO);
	sInfo.fMask = SIF_ALL;
	sInfo.nMin = 0;
	sInfo.nMax = size.cx;
	sInfo.nPage = VisibleRect.Width() + 1;
	sInfo.nPos = VisibleRect.left;
	SetScrollInfo( SB_HORZ, &sInfo );
	sInfo.cbSize = sizeof(SCROLLINFO);
	sInfo.fMask = SIF_ALL;
	sInfo.nMin = 0;
	sInfo.nMax = size.cy;
	sInfo.nPage = VisibleRect.Height() + 1;
	sInfo.nPos = VisibleRect.top;
	SetScrollInfo( SB_VERT, &sInfo );
}

CRect CC2ERoomEditorView::GetWorldVisible() const
{
	return GetViewParams().GetWorldRect();
}


void CC2ERoomEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_ViewArea = GetWorldVisible();
	int Height = m_ViewArea.Height();

	int Min, Max;
	GetScrollRange( SB_VERT, &Min, &Max );

	switch( nSBCode	)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_ViewArea.top = nPos;
		m_ViewArea.bottom = nPos + Height;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_PAGELEFT:
		m_ViewArea.top = std::_MAX( m_ViewArea.top - Height, Min );
		m_ViewArea.bottom = m_ViewArea.top + Height;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_PAGERIGHT:
		m_ViewArea.bottom = std::_MIN( m_ViewArea.bottom + Height, Max );
		m_ViewArea.top = m_ViewArea.bottom - Height;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_LINELEFT:
		m_ViewArea.top = std::_MAX( m_ViewArea.top - Height/5, Min );
		m_ViewArea.bottom = m_ViewArea.top + Height;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_LINERIGHT:
		m_ViewArea.bottom = std::_MIN( m_ViewArea.bottom + Height/5, Max );
		m_ViewArea.top = m_ViewArea.bottom - Height;
		UpdateScrollInfo();
		Invalidate();
		break;
	}

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CC2ERoomEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_ViewArea = GetWorldVisible();
	int Width = m_ViewArea.Width();

	int Min, Max;
	GetScrollRange( SB_HORZ, &Min, &Max );

	switch( nSBCode	)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_ViewArea.left = nPos;
		m_ViewArea.right = nPos + Width;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_PAGELEFT:
		m_ViewArea.left = std::_MAX( m_ViewArea.left - Width, Min );
		m_ViewArea.right = m_ViewArea.left + Width;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_PAGERIGHT:
		m_ViewArea.right = std::_MIN( m_ViewArea.right + Width, Max );
		m_ViewArea.left = m_ViewArea.right - Width;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_LINELEFT:
		m_ViewArea.left = std::_MAX( m_ViewArea.left - Width/5, Min );
		m_ViewArea.right = m_ViewArea.left + Width;
		UpdateScrollInfo();
		Invalidate();
		break;
	case SB_LINERIGHT:
		m_ViewArea.right = std::_MIN( m_ViewArea.right + Width/5, Max );
		m_ViewArea.left = m_ViewArea.right - Width;
		UpdateScrollInfo();
		Invalidate();
		break;
	}

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CC2ERoomEditorView::OnViewZoomWorld() 
{
	CRect rect = CRect( CPoint(0,0), GetDocument()->GetWorld()->GetSize() );
	int expand = max( rect.Height(), rect.Width() ) / 20;
	rect.InflateRect( expand, expand );
	SetViewArea( rect );
}

void CC2ERoomEditorView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_Tool->MouseMenu( point );	
}

void CC2ERoomEditorView::OnViewZoomForward() 
{
	m_BackRects.push( m_ViewArea );
	m_ViewArea = m_ForwardRects.top();
	m_ForwardRects.pop();
	UpdateScrollInfo();
	Invalidate();
}

void CC2ERoomEditorView::OnUpdateViewZoomForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !m_ForwardRects.empty() );
}

void CC2ERoomEditorView::OnViewZoomLast() 
{
	m_ForwardRects.push( m_ViewArea );
	m_ViewArea = m_BackRects.top();
	m_BackRects.pop();
	UpdateScrollInfo();
	Invalidate();
}

void CC2ERoomEditorView::OnUpdateViewZoomLast(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !m_BackRects.empty() );
}

void CC2ERoomEditorView::OnViewZoomIn() 
{
	CViewParams vp = GetViewParams();
	CRect rect = vp.GetWorldRect();
	rect.DeflateRect( rect.Width() / 4, rect.Height() / 4 );
	SetViewArea( rect );
}

void CC2ERoomEditorView::OnViewZoomOut() 
{
	CViewParams vp = GetViewParams();
	CRect rect = vp.GetWorldRect();
	rect.InflateRect( rect.Width() / 2, rect.Height() / 2 );
	SetViewArea( rect );
}

void CC2ERoomEditorView::ReflectSelection()
{
	const std::vector< CPropertyType > emptyTypes;
	std::vector< CPropertyType > const *types = &emptyTypes;

	if( m_SelectedRooms.size() )
		types = &(GetDocument()->GetPropertyTypes());
	else if( m_SelectedDoors.size() )
		types = &(GetDocument()->GetDoorPropertyTypes());

	GetPropertiesDlg()->ReflectSelection( this, *types );
}

void CC2ERoomEditorView::OnEditBackground() 
{
	CBackgroundDlg dlg( GetDocument(), m_SelectedMetaroom );
//	dlg.SetStringList( GetDocument()->GetWorld()->GetMetaroom( m_SelectedMetaroom )->GetBackgroundList() );
	if( dlg.DoModal() == IDOK )
	{
	}
}

void CC2ERoomEditorView::OnUpdateEditBackground(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_SelectedMetaroom != -1 );	
}

void CC2ERoomEditorView::OnFileValidate() 
{
	CC2ERoomEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ClearSelection();
	try
	{
		pDoc->GetWorld()->Validate( &m_SelectedRooms, &m_SelectedDoors );
		AfxMessageBox( "Map is 100% wholesome and pure.", MB_OK | MB_ICONEXCLAMATION );
	}
	catch( CREException const &exept)
	{
		AfxMessageBox( exept.what(), MB_OK | MB_ICONEXCLAMATION );
	}
	catch( ... )
	{
	}

	if( m_SelectedDoors.size() )
	{
		SetViewArea( CRect( (*m_SelectedDoors.begin())->GetPoint1(), (*m_SelectedDoors.begin())->GetPoint2() ) );
	}

	CClientDC dc( this );
	HighlightSelection( &dc );

	ReflectSelection();
}

void CC2ERoomEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_Tool->KeyPress( nChar );	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CC2ERoomEditorView::OnViewCA( UINT nID )
{
	if( m_ColourRoomOpt == nID - ID_VIEW_COLOURROOMS_CA0 )
		m_ColourRoomOpt = -1;
	else
		m_ColourRoomOpt = nID - ID_VIEW_COLOURROOMS_CA0;
	Invalidate();
}

void CC2ERoomEditorView::OnUpdateViewCA(CCmdUI* pCmdUI)
{
	UINT nID = pCmdUI->m_nID;
	pCmdUI->SetCheck( nID - ID_VIEW_COLOURROOMS_CA0 == m_ColourRoomOpt );
}

void CC2ERoomEditorView::OnToolsCheese() 
{
	m_Tool->StopUsing();
	m_Tool = &m_ToolCheese;
	m_Tool->StartUsing();
}

void CC2ERoomEditorView::OnUpdateToolsCheese(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_Tool == &m_ToolCheese );
}

void CC2ERoomEditorView::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == ID_TIMER )
	{
		static bool inTimer = false;
		static inTimeCount = 0;
		if( inTimer )
		{
			++inTimeCount;
		}
		else
		{
			inTimer = true;
			if( inTimeCount )
				--inTimeCount;
			else
			{
				CClientDC dc( this );
				DrawCheese( &dc, true );
				m_CheeseRadius = ( m_CheeseRadius + 1 ) % 20;

				if( m_RunCA )
				{
					CC2ERoomEditorDoc* pDoc = GetDocument();
					ASSERT_VALID(pDoc);
					pDoc->StepCA( );
				}
				if( m_UpdateCA && m_ColourRoomOpt != -1 )
				{
					CC2ERoomEditorDoc* pDoc = GetDocument();
					ASSERT_VALID(pDoc);
					CWorld *world = pDoc->GetWorld();
					try
					{
						CGame game;
						int nRooms = world->GetRoomCount();
						int roomIndexBase = world->GetRoomIndexBase();
/*
						CString res = game.Execute( "SETV VA00 %d\n"
							"REPS %d\n"
							"OUTV PROP VA00 %d\n"
							"OUTS \"\\n\"\n"
							"ADDV VA00 1\n"
							"REPE", roomIndexBase, nRooms, m_ColourRoomOpt );
*/
						CString command;
						command.Format( "SETV VA00 %d\n", m_ColourRoomOpt );
						world->GetCACommand( command );
						CString res = game.Execute( command );
						std::istrstream stream( res );
						world->ReadCAValues( stream, m_ColourRoomOpt );
						Invalidate();
				/*
						for( int i = 0; i < nRooms; ++i )
						{
							HRoom room = m_World.GetRoom( i );
							stream >> room->m_CAValue;
						}
				*/
					}
					catch( CREException &except )
					{
						AfxMessageBox( except.what(), MB_OK | MB_ICONSTOP );
					}
					catch( ... )
					{
						AfxMessageBox( "Error updating CA", MB_OK | MB_ICONEXCLAMATION );
					}
				}
			}
			inTimer = false;
		}
	}
	CView::OnTimer(nIDEvent);
}

void CC2ERoomEditorView::OnDestroy() 
{
	if( m_Timer ) KillTimer( ID_TIMER );
	CView::OnDestroy();
}

void CC2ERoomEditorView::OnViewShowBackground() 
{
	m_ShowBackground = !m_ShowBackground;
	Invalidate();
}

void CC2ERoomEditorView::OnUpdateViewShowBackground(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_ShowBackground );
}

void CC2ERoomEditorView::OnToolsRunCa() 
{
	m_RunCA = !m_RunCA;
	if( m_RunCA ) m_UpdateCA = false;
}

void CC2ERoomEditorView::OnUpdateToolsRunCa(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_RunCA );
}

void CC2ERoomEditorView::OnEditFloorceilingvalues() 
{
	CFloorValuesDlg dlg;
	HDoor door = *m_SelectedDoors.begin();
	double m, c;
	LineEquation( door->GetPoint1(), door->GetPoint2(), &m, &c );
	dlg.m_M = m;
	dlg.m_C = c;
	dlg.DoModal();
}

void CC2ERoomEditorView::OnUpdateEditFloorceilingvalues(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_SelectedDoors.size() == 1 &&
			( (*m_SelectedDoors.begin())->GetDirection() == CDoor::Dir::Up ||
			  (*m_SelectedDoors.begin())->GetDirection() == CDoor::Dir::Down )
		);
}

void CC2ERoomEditorView::OnToolsUpdateCaFromGame() 
{
	m_UpdateCA = !m_UpdateCA;
	if( m_UpdateCA ) m_RunCA = false;
}

void CC2ERoomEditorView::OnUpdateToolsUpdateCaFromGame(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_UpdateCA );
}

void CC2ERoomEditorView::OnEditMusic() 
{
	CC2ERoomEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CMusicDlg dlg;
	CString music;
	if(  m_SelectedMetaroom != -1 )
		music = pDoc->GetWorld()->GetMetaroom( m_SelectedMetaroom )->GetMusic();
	else if ( !m_SelectedRooms.empty() )
	{
		music = pDoc->GetWorld()->GetRoom( *(m_SelectedRooms.begin()) )->GetMusic();
		std::set< int >::iterator room;
		for( room = m_SelectedRooms.begin(); room != m_SelectedRooms.end(); ++room )
			if( music != pDoc->GetWorld()->GetRoom( *room )->GetMusic() )
				music = "";
	}
	dlg.m_Music = music;
	if( dlg.DoModal() == IDOK )
	{
		if(  m_SelectedMetaroom != -1 )
			GetDocument()->Execute( HAction( new CActionChangeMetaroomMusic( m_SelectedMetaroom, dlg.m_Music ) ) );
		else
			GetDocument()->Execute( HAction( new CActionChangeRoomMusic( m_SelectedRooms, dlg.m_Music ) ) );
		pDoc->SetModifiedFlag();
	}
}

void CC2ERoomEditorView::OnUpdateEditMusic(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_SelectedMetaroom != -1 || !m_SelectedRooms.empty() );
}

void CC2ERoomEditorView::OnEditCheckheights() 
{
	CHeightCheckDlg dlg;
	if( dlg.DoModal() == IDOK )
	{
		CC2ERoomEditorDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		ClearSelection();
		pDoc->SelectRoomWronguns( dlg.m_Permiability, dlg.m_Height, m_SelectedRooms );
		CClientDC dc( this );
		HighlightSelection( &dc );
	}
}

