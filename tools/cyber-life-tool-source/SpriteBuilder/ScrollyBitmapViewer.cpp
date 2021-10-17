// ScrollyBitmapViewer.cpp : implementation file
//

#include "stdafx.h"
#include "spritebuilder.h"
#include "ScrollyBitmapViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScrollyBitmapViewer

ScrollyBitmapViewer::ScrollyBitmapViewer(CFIBitmap* bmp)
: CDialog(IDD_CUTTER_DIALOG)
{
	m_fiBitmap = bmp;
}

ScrollyBitmapViewer::~ScrollyBitmapViewer()
{
}


BEGIN_MESSAGE_MAP(ScrollyBitmapViewer, CDialog)
	//{{AFX_MSG_MAP(ScrollyBitmapViewer)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZING()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL ScrollyBitmapViewer::OnInitDialog()
{
	CRect rec;
	GetWindowRect(rec);
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nPage = rec.Width();
	si.nMax = m_fiBitmap->Width();
	
	SetScrollInfo(SB_HORZ,&si);

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = m_fiBitmap->Height();
	si.nPage = rec.Height();
	SetScrollInfo(SB_VERT,&si);
	
	return CDialog::OnInitDialog();
}

/////////////////////////////////////////////////////////////////////////////
// ScrollyBitmapViewer message handlers

void ScrollyBitmapViewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	HBITMAP hbmp = *m_fiBitmap;
	CBitmap* srcbmp = CBitmap::FromHandle(hbmp);
	CDC srcdc;
	srcdc.CreateCompatibleDC(NULL);
	CBitmap* oldbmp = srcdc.SelectObject(srcbmp);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(SB_HORZ,&si);
	int xpos = si.nPos;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(SB_VERT,&si);
	int ypos = si.nPos;

	if (!m_zoomer)
		dc.BitBlt(-xpos,-ypos,m_fiBitmap->Width(),m_fiBitmap->Height(),&srcdc,0,0,SRCCOPY);
	else
		dc.StretchBlt(-(2*xpos),-(2*ypos),m_fiBitmap->Width() * 2, m_fiBitmap->Height() * 2,
						&srcdc, 0, 0, m_fiBitmap->Width(), m_fiBitmap->Height(), SRCCOPY);

	srcdc.SelectObject(oldbmp);
	// Do not call CWnd::OnPaint() for painting messages
	dc.SelectStockObject(WHITE_PEN);
	dc.SelectStockObject(HOLLOW_BRUSH);
	for(int i = 0; i < m_rects->GetSize(); i++)
	{
		CRect rec(m_rects->GetAt(i));
		if (m_zoomer)
		{
			int w = rec.Width() * 2;
			int h = rec.Height() * 2;
			rec.left = rec.left * 2;
			rec.top = rec.top * 2;
			rec.right = rec.left + w;
			rec.bottom = rec.top + h;
		}
		rec.OffsetRect(-((m_zoomer?2:1)*xpos),-((m_zoomer?2:1)*ypos));
		dc.Rectangle(rec);
	}
}

int ScrollyBitmapViewer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	
	return 0;
}

void ScrollyBitmapViewer::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int dPos = 0;
	bool absolut = false; // Yum!!! Vodka!

	switch ( nSBCode )
	{
	case SB_LINEDOWN:
		dPos +=5;
		break;
	case SB_LINEUP:
		dPos -=5;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		dPos = nPos;
		absolut = true;
		break;

	}

	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = nPos;
	GetScrollInfo(SB_HORZ,&si);
	int oPos = si.nPos;
	if (!absolut)
		si.nPos += (dPos);
	else
		si.nPos = dPos;
	SetScrollInfo(SB_HORZ,&si,TRUE);
	//DrawScene();
	Invalidate();
}

void ScrollyBitmapViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int dPos = 0;
	bool absolut = false; // Yum!!! Vodka!

	switch ( nSBCode )
	{
	case SB_LINEDOWN:
		dPos +=5;
		break;
	case SB_LINEUP:
		dPos -=5;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		dPos = nPos;
		absolut = true;
		break;

	}

	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = nPos;
	GetScrollInfo(SB_VERT,&si);
	int oPos = si.nPos;
	if (!absolut)
		si.nPos += (dPos);
	else
		si.nPos = dPos;
	SetScrollInfo(SB_VERT,&si,TRUE);
	//DrawScene();
	Invalidate();
}

void ScrollyBitmapViewer::OnSize(UINT nType, int cx, int cy) 
{
	

	CRect rec;
	// Right then... manage sizing issues...
	if ((cx > (m_fiBitmap->Width() * (m_zoomer?2:1))) || (cy > (m_fiBitmap->Height() * (m_zoomer?2:1))))
	{
		cx = (rec.Width() * (m_zoomer?2:1));
		cy = (rec.Height() * (m_zoomer?2:1));
		
	}
	CDialog::OnSize(nType, cx, cy);

	GetWindowRect(rec);
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = (m_zoomer?2:1)*m_fiBitmap->Width();
	si.nPage = rec.Width();
	
	SetScrollInfo(SB_HORZ,&si);

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = (m_zoomer?2:1)*m_fiBitmap->Height();
	si.nPage = rec.Height();
	SetScrollInfo(SB_VERT,&si);
	
	// TODO: Add your message handler code here
	Invalidate();
}

BOOL ScrollyBitmapViewer::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rec;
	GetWindowRect(rec);
	rec.OffsetRect(-rec.left,-rec.top);
	pDC->SelectStockObject(WHITE_PEN);
	// pDC->SelectStockObject(SOLID_BRUSH);
	pDC->Rectangle(rec);
	return 1;
	// return CDialog::OnEraseBkgnd(pDC);
}

void ScrollyBitmapViewer::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CDialog::OnSizing(fwSide, pRect);
	
	// TODO: Add your message handler code here
	
}

void ScrollyBitmapViewer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	x_c_pos = point.x;
	y_c_pos = point.y;
	CDialog::OnLButtonDown(nFlags, point);
}

void ScrollyBitmapViewer::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ((abs(x_c_pos-point.x) < 3) &&
		(abs(y_c_pos-point.y) < 3))
	{
		// Let's do a cut :)
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(SB_HORZ,&si);
		int xpos = si.nPos;
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(SB_VERT,&si);
		int ypos = si.nPos;
		
		if (m_zoomer)
		{
			point.x = (point.x>>1) + (xpos);
			point.y = (point.y>>1) + (ypos);
		}
		else
		{
			point.x += xpos;
			point.y += ypos;
		}
		if (myCutter->CutAtPosition(point))
			m_rects->InsertAt(myCutter->Count-1,new CRect(myCutter->GetFoundRect()));
		Invalidate(false);
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void ScrollyBitmapViewer::AutoCut()
{
	while(myCutter->CutAtAnyPosition())
	{
		m_rects->InsertAt(myCutter->Count-1,new CRect(myCutter->GetFoundRect()));
	}
}

