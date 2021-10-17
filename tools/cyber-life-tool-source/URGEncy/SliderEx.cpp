// SliderEx.cpp : implementation file
//

#include "stdafx.h"
#include "URGEncy.h"
#include "SliderEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSliderEx

CSliderEx::CSliderEx()
{

	backmap.LoadBitmap(IDB_BACKGROUND);
}

CSliderEx::~CSliderEx()
{
	
}


BEGIN_MESSAGE_MAP(CSliderEx, CSliderCtrl)
	//{{AFX_MSG_MAP(CSliderEx)
		ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSliderEx message handlers

BOOL CSliderEx::OnEraseBkgnd(CDC* pDC)
{
	//Erase Background by plotting bitmap onto DC
	return true;
}


void CSliderEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CDC imageDC;
	imageDC.CreateCompatibleDC(&dc);
	CBitmap* oldmap = imageDC.SelectObject(&backmap);
	CRect rec;
	GetWindowRect(rec);
	GetParent()->ScreenToClient(&rec);
	//bearing in mind that the backdrop repeats every 64px
	
	int xofs = rec.left % 64;
	int yofs = rec.top % 64;
	for(int x=0;x<((rec.Width() / 64)+1);x++)
		dc.BitBlt((x*64),0,64,64,&imageDC,xofs,yofs,SRCCOPY);
	imageDC.SelectObject(oldmap);
	
	//Draw Line here


	CRect lineRect;

	GetChannelRect(&lineRect);
	dc.Draw3dRect(lineRect,RGB(50,50,50),RGB(200,200,200));

	GetThumbRect(&lineRect);
	dc.Draw3dRect(lineRect,RGB(250,250,250),RGB(0,0,0));
	lineRect.DeflateRect(1,1);
	dc.Draw3dRect(lineRect,RGB(200,200,200),RGB(50,50,50));
	lineRect.DeflateRect(1,1);
	CBrush meh(RGB(150,150,150));
	dc.FillRect(lineRect,&meh);
	// Do not call CSliderCtrl::OnPaint() for painting messages
}

