// SpriteBuilderView.cpp : implementation of the CSpriteBuilderView class
//

#include "stdafx.h"
#include "SpriteBuilder.h"

#include "SpriteBuilderDoc.h"
#include "CntrItem.h"
#include "SpriteBuilderView.h"
#include "Limits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderView

IMPLEMENT_DYNCREATE(CSpriteBuilderView, CScrollView)

BEGIN_MESSAGE_MAP(CSpriteBuilderView, CScrollView)
	//{{AFX_MSG_MAP(CSpriteBuilderView)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderView construction/destruction

CSpriteBuilderView::CSpriteBuilderView() :
	m_SelectedRect(),
	m_TargetRect(),
	m_Font()
{
	m_pSelection = NULL;
	// TODO: add construction code here
	m_iSelectedBitmap = -1;
	m_iPrevSelectedBitmap = -1;
	m_iTargetBitmap = -1;
	m_iPrevTargetBitmap = -1;
	m_bDrag = FALSE;

	m_iRowCount = 0;

}

CSpriteBuilderView::~CSpriteBuilderView()
{
}

BOOL CSpriteBuilderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderView drawing

void CSpriteBuilderView::OnDraw(CDC* pDC)
{
	// Obtain sprite.
	CSpriteBuilderDoc* pDoc = GetDocument();
	ASSERT(pDoc);
	ASSERT_VALID(pDoc);

	// _BC_
	// changed the ASSERT to a return to avoid the palette-not-found
	// crash... (Caused by an AfxMessageBox() call during CSprite Construction).

//	ASSERT(pDoc->m_pSprite);
	if( !(pDoc->m_pSprite) )
		return;

	CSprite& rSprite = *(pDoc->m_pSprite);

	CRectArray& rRectArray = GetDocument()->m_RectArray;

	// Array never initialised.
	if (rRectArray.GetSize() == 0)
	{
		CalcBitmapPositions();
	}

	int iNumberOfBitmaps = rSprite.NumberOfBitmaps();
	int i;

	// Select font.
	void* pOldFont = pDC->SelectObject(m_Font);
	pDC->SetBkMode(TRANSPARENT);

	CDC MemDC;
	VERIFY(MemDC.CreateCompatibleDC(pDC));

	// Draw all the bitmaps...
	for (i = 0; i < iNumberOfBitmaps; i++)
	{
		if (i != m_iSelectedBitmap)
		{
			DrawBitmap(pDC, &MemDC, rSprite, i);
		}
	}

	// Draw targetting hilight...
	if (m_iTargetBitmap != -1)
	{
		CRect rectSpace;
		DrawTarget(pDC, FALSE, m_SelectedRect.CenterPoint());
	}

	// Target changed...
	if (m_iPrevTargetBitmap != -1 && m_iPrevTargetBitmap != m_iTargetBitmap)
	{
		// Erase hilight...
		CRect rectSpace;
		DrawTarget(pDC, TRUE, m_SelectedRect.CenterPoint());
		m_iPrevTargetBitmap = -1;
	}

	// Draw selected bitmap last - so it appears on top.
	if (m_iSelectedBitmap != -1)
	{
		DrawBitmap(pDC, &MemDC, rSprite, m_iSelectedBitmap);
		DrawBitmapBorder(pDC, FALSE, m_SelectedRect);
	}

	// Selection changed...
	if (m_iPrevSelectedBitmap != -1 && m_iPrevSelectedBitmap < rRectArray.GetSize())
	{
		// Erase border.
		DrawBitmapBorder(pDC, TRUE, *rRectArray[m_iPrevSelectedBitmap]);
		m_iPrevSelectedBitmap = -1;
	}

	// Clean up DC.
	pDC->SelectObject(pOldFont);


	// TODO: also draw all OLE items in the document

	// Draw the selection at an arbitrary position.  This code should be
	//  removed once your real drawing code is implemented.  This position
	//  corresponds exactly to the rectangle returned by CSpriteBuilderCntrItem,
	//  to give the effect of in-place editing.

	// TODO: remove this code when final draw code is complete.
	/*
	if (m_pSelection == NULL)
	{
		POSITION pos = pDoc->GetStartPosition();
		m_pSelection = (CSpriteBuilderCntrItem*)pDoc->GetNextClientItem(pos);
	}
	if (m_pSelection != NULL)
		m_pSelection->Draw(pDC, CRect(10, 10, 210, 210));
		*/
}


/*********************************************************************
* Protected: DrawBitmap.
*********************************************************************/
void CSpriteBuilderView::DrawBitmap(CDC* pDC, CDC* pMemDC, CSprite& rSprite, int iIndex)
{
	CRect* pR;
	CRectArray& rRectArray = GetDocument()->m_RectArray;

	if (iIndex == m_iSelectedBitmap)
	{
		pR = &m_SelectedRect;
	}
	else
	{
		pR = rRectArray[iIndex];
	}

	// Draw number label.
	CString str;
	str.Format("(%d)\n %d\nx\n%d", iIndex,pR->Width(),pR->Height());
	//Drawing text...
	CRect pos;
	pos.left = pR->left - X_SPACING;
	pos.right = pos.left + X_SPACING;
	pos.top = pR->top;
	pos.bottom = pos.top + pDC->DrawText(str,pos,DT_CALCRECT | DT_CENTER);
	pDC->DrawText(str,pos,DT_CENTER);

	// Draw bitmap.
	HBITMAP hBitmap = *rSprite[iIndex];
	ASSERT(hBitmap);
	CBitmap* pOldBitmap = pMemDC->SelectObject(CBitmap::FromHandle(hBitmap));
	ASSERT(pOldBitmap);

	VERIFY(pDC->BitBlt(
		pR->left, pR->top,
		pR->Width(), pR->Height(), 
		pMemDC,
		0, 0,
		SRCCOPY));

	VERIFY(pMemDC->SelectObject(pOldBitmap));
}

/*********************************************************************
* Protected: DrawBitmapBorder.
*********************************************************************/
void CSpriteBuilderView::DrawBitmapBorder(CDC* pDC, BOOL bErase, CRect Rect)
{
	DWORD dwBorderColour;
	DWORD dwHiLightColour;
	DWORD dwShadowColour;

	// Select colours...
	if (bErase)
	{
		dwBorderColour = ::GetSysColor(COLOR_WINDOW);
		dwHiLightColour = dwBorderColour;
		dwShadowColour = dwBorderColour;
	}
	else
	{
		dwBorderColour  = ::GetSysColor(COLOR_3DFACE);
		dwHiLightColour = ::GetSysColor(COLOR_BTNHILIGHT);
		dwShadowColour = ::GetSysColor(COLOR_BTNSHADOW);
	}

	CBrush Brush(dwBorderColour);
	CBrush* pOldBrush = pDC->SelectObject(&Brush);

	CPen Pen(PS_SOLID, 0, dwBorderColour);
	CPen* pOldPen = pDC->SelectObject(&Pen);
	
	// Left.
	CRect rectBorder;
	rectBorder.SetRect(Rect.left - BW, Rect.top - BW, Rect.left, Rect.bottom + BW);
	pDC->Rectangle(&rectBorder);

	// Top.
	rectBorder.SetRect(Rect.left - BW, Rect.top - BW, Rect.right + BW, Rect.top);
	pDC->Rectangle(&rectBorder);

	// Right.
	rectBorder.SetRect(Rect.right, Rect.top - BW, Rect.right + BW, Rect.bottom + BW);
	pDC->Rectangle(&rectBorder);

	// Bottom.
	rectBorder.SetRect(Rect.left - BW, Rect.bottom, Rect.right + BW, Rect.bottom + BW);
	pDC->Rectangle(&rectBorder);

	Rect.InflateRect(BW, BW);
	pDC->Draw3dRect(Rect, dwHiLightColour, dwShadowColour);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}


/*********************************************************************
* Protected: DrawTarget.
*********************************************************************/
void CSpriteBuilderView::DrawTarget(CDC* pDC, BOOL bErase, CPoint ptPoint)
{
	CRect rectSpace;
	int iIndex = NearTest(ptPoint, rectSpace);

	DWORD dwHiLightColour;
	DWORD dwShadowColour;

	// Select colours...
	if (bErase)
	{
		dwHiLightColour = ::GetSysColor(COLOR_WINDOW);
		dwShadowColour = dwHiLightColour;
	}
	else
	{
		dwHiLightColour = ::GetSysColor(COLOR_BTNHILIGHT);
		dwShadowColour = ::GetSysColor(COLOR_BTNSHADOW);
	}

	const int iTH = TARGET_HEIGHT / 2;
	CRectArray& rRectArray = GetDocument()->m_RectArray;
	CPoint ptCenter = rectSpace.CenterPoint();
	CRect rectLine(
		ptCenter.x - 1, rectSpace.top - iTH, 
		ptCenter.x + 1, rectSpace.top + rRectArray[m_iSelectedBitmap]->Height() + iTH);
	rectLine.OffsetRect(-2, 0);
	pDC->Draw3dRect(rectLine, dwHiLightColour, dwShadowColour);
	rectLine.OffsetRect(4, 0);
	pDC->Draw3dRect(rectLine, dwHiLightColour, dwShadowColour);

	m_TargetRect = rectLine;
	m_TargetRect.InflateRect(4, 0);
}


/*********************************************************************
* Protected: CalculateBitmapPositions.
*********************************************************************/
void CSpriteBuilderView::CalcBitmapPositions()
{
	// Obtain document.
	CSpriteBuilderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// Obtain sprite.
	if (!pDoc->m_pSprite)
		return;

	// Reset 'map' information.
	m_iRowCount = 0;
	for (int w = 0; w < MAX_ROWS; w++)
		m_iColumnCount[w] = 0;

	CSprite& rSprite = *(pDoc->m_pSprite);
	int iNumberOfBitmaps = rSprite.NumberOfBitmaps();
	int x = X_MARGIN; // Starting point for drawing.
	int y = Y_MARGIN;
	int iMaxHeightOfRow = 0;
	CRectArray& rRectArray = GetDocument()->m_RectArray;
	BOOL bFirstTime = (rRectArray.GetSize() == 0);
	CRect rectClient;
	GetClientRect(&rectClient);

	// Run thru' all the bitmaps...
	for (int i = 0; i < iNumberOfBitmaps; i++)
	{
		x += X_SPACING;

		CFIBitmap* pBitmap = rSprite[i];

		if (bFirstTime)
		{
			rRectArray.Add(new CRect(x, y, x + pBitmap->Width(), y + pBitmap->Height()));
		}
		else
		{
			rRectArray[i]->SetRect(x, y, x + pBitmap->Width(), y + pBitmap->Height());
		}

		// Add to map.
		m_iColumnCount[m_iRowCount]++;

		if (i == m_iSelectedBitmap)
		{
			m_SelectedRect.SetRect(x, y, x + pBitmap->Width(), y + pBitmap->Height());
		}

		// Calculate position of next bitmap...
		if (rSprite[i]->Height() > iMaxHeightOfRow)
		{
			iMaxHeightOfRow = rSprite[i]->Height();
		}

		x += pBitmap->Width() + X_SPACING;

		// Check for end of row...
		if (i < iNumberOfBitmaps - 1)
		{
			if (x + rSprite[i+1]->Width() + X_SPACING > rectClient.right)
			{
				// New row:
				x = X_MARGIN;
				y += iMaxHeightOfRow + Y_SPACING;
				iMaxHeightOfRow = 0;

				// Count rows.
				m_iRowCount++;
			}
		}
	}

	// Must add the last one.
	m_iRowCount++;

	CSize sizeTotal(rectClient.Width(), y + iMaxHeightOfRow + Y_MARGIN);
	CSize sizePage(0, SCROLL_PAGE);
	CSize sizeLine(0, SCROLL_LINE);
	SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);
}


void CSpriteBuilderView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

    // Create required font.
	if ((HFONT)m_Font == NULL)
	{
		m_Font.CreatePointFont(80, "MS Sans Serif");
	}

	CRectArray& rRectArray = GetDocument()->m_RectArray;
    int iCnt = rRectArray.GetSize();

    for(int i = 0; i < iCnt; i++)
    {
        CRect* pTmp = rRectArray[0];
		rRectArray.RemoveAt(0);
		delete pTmp;
		pTmp = NULL;
	}

	m_iSelectedBitmap = -1;
	m_iPrevSelectedBitmap = -1;

	// TODO: remove this code when final selection model code is written
	//m_pSelection = NULL;    // initialize selection
}

void CSpriteBuilderView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.
   CScrollView::OnDestroy();
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
}


/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CSpriteBuilderView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CSpriteBuilderCntrItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.

	// TODO: implement this function that tests for a selected OLE client item

	return pDocItem == m_pSelection;
}

void CSpriteBuilderView::OnInsertObject()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new CSpriteBuilderCntrItem object.
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	CSpriteBuilderCntrItem* pItem = NULL;
	TRY
	{
		// Create new item connected to this document.
		CSpriteBuilderDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CSpriteBuilderCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// Initialize the item from the dialog data.
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // any exception will do
		ASSERT_VALID(pItem);

		// If item created from class list (not from file) then launch
		//  the server to edit the item.
		if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);

		// As an arbitrary user interface design, this sets the selection
		//  to the last item inserted.

		// TODO: reimplement selection as appropriate for your application

		m_pSelection = pItem;   // set selection to last inserted item
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void CSpriteBuilderView::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void CSpriteBuilderView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // don't call the base class
			return;
		}
	}

	CScrollView::OnSetFocus(pOldWnd);
}

void CSpriteBuilderView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();

	CalcBitmapPositions();
}

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderView diagnostics

#ifdef _DEBUG
void CSpriteBuilderView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSpriteBuilderView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CSpriteBuilderDoc* CSpriteBuilderView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpriteBuilderDoc)));
	return (CSpriteBuilderDoc*)m_pDocument;
}
#endif //_DEBUG


/*********************************************************************
* Protected: HitTest.
*********************************************************************/
int CSpriteBuilderView::HitTest(CPoint point)
{
	CRectArray& rRectArray = GetDocument()->m_RectArray;
	int iCnt = rRectArray.GetSize();

	for(int i = 0; i < iCnt; i++)
	{
		if (rRectArray[i]->PtInRect(point))
		{
			TRACE("CSpriteBuilderView::HitTest(%d, %d) = %d\n", point.x, point.y, i);
			return i;
		}
	}


	return -1;
}


/*********************************************************************
* Protected: NearTest.
*********************************************************************/
int CSpriteBuilderView::NearTest(CPoint point, CRect& rectSpace)
{
	CRectArray& rRectArray = GetDocument()->m_RectArray;
	int iCnt = rRectArray.GetSize();
	CPoint ptCandidate;
	int iProximityX, iProximityY;
	int iMinProximityX = 1024;
	int iMinProximityY = 1024;
	int iClosestFound = -1;
	int iIndex = 0;

	int iNumberOfBitmaps = rRectArray.GetSize();

	for (int r = 0; r < m_iRowCount; r++)
	{
		for(int c = 0; c < m_iColumnCount[r] + 1; c++)
		{
			CRect rectCandidate;

			if (iIndex == m_iSelectedBitmap || iIndex == m_iSelectedBitmap + 1)
			{
				// Targetting the selected object.
				rectCandidate = *rRectArray[m_iSelectedBitmap];
			}
			else if (c == 0)
			{
				// LHS of row.
				rectCandidate.SetRect(
					rRectArray[iIndex]->left - X_SPACING, rRectArray[iIndex]->top,
					rRectArray[iIndex]->left, rRectArray[iIndex]->bottom);
			}
			else if (c == m_iColumnCount[r])
			{
				// RHS of row.
				rectCandidate.SetRect(
					rRectArray[iIndex - 1]->right, rRectArray[iIndex - 1]->top,
					rRectArray[iIndex - 1]->right + X_SPACING, rRectArray[iIndex - 1]->bottom);
			}
			else
			{
				// Middle of row.
				rectCandidate.SetRect(
					rRectArray[iIndex - 1]->right, rRectArray[iIndex - 1]->top,
					rRectArray[iIndex]->left, rRectArray[iIndex]->bottom);
			}

			// Calculate distance.
			ptCandidate = rectCandidate.CenterPoint();

			iProximityX = abs(point.x - ptCandidate.x);
			iProximityY = abs(point.y - ptCandidate.y);

			int dDistance = (iProximityX * iProximityX) + (iProximityY * iProximityY);
			int dMinDistance = (iMinProximityX * iMinProximityX) + (iMinProximityY * iMinProximityY);

			if (dDistance < dMinDistance)
			{
				iClosestFound = iIndex;
				rectSpace = rectCandidate;
				iMinProximityX = iProximityX;
				iMinProximityY = iProximityY;
			}

			if (c != m_iColumnCount[r])
				iIndex++;
		}
	}

	return iClosestFound;
}


/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderView message handlers
/*********************************************************************
* Protected: OnLButtonDown.
*********************************************************************/
void CSpriteBuilderView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Convert from device coords into MM_TEXT.
	CClientDC DC(this);
	OnPrepareDC(&DC);
	DC.DPtoLP(&point);

	SelectBitmap(point);

	// Start drag.
	m_bDrag = TRUE;
	SetCapture();
	
	// Convert back to device coords.
	DC.LPtoDP(&point);

	CScrollView::OnLButtonDown(nFlags, point);
}


/*********************************************************************
* Protected: OnLButtonUp.
*********************************************************************/
void CSpriteBuilderView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// Convert from device coords into MM_TEXT.
	CClientDC DC(this);
	OnPrepareDC(&DC);
	DC.DPtoLP(&point);

	// A drop?
	if (m_iSelectedBitmap != -1 && m_iTargetBitmap != -1)
	{
		// Obtain sprite.
		CSpriteBuilderDoc* pDoc = GetDocument();
		ASSERT(pDoc);
		ASSERT_VALID(pDoc);
		ASSERT(pDoc->m_pSprite);
		CSprite& rSprite = *(pDoc->m_pSprite);
		CRectArray& rRectArray = GetDocument()->m_RectArray;

		// Store state before change.
		pDoc->StoreForUndo();

		// Rearrange bitmaps.
		CFIBitmap* pBitmap = rSprite[m_iSelectedBitmap];
		rSprite.RemoveAt(m_iSelectedBitmap);

		if (m_iSelectedBitmap < m_iTargetBitmap)
		{
			// All the bitmaps will have shuffled down one.
			m_iTargetBitmap--;
		}

		rSprite.InsertAt(m_iTargetBitmap, pBitmap);

		// Reset selection & targeting.
		m_iSelectedBitmap = m_iTargetBitmap;
		m_SelectedRect = rRectArray[m_iSelectedBitmap];
		m_iPrevTargetBitmap = m_iTargetBitmap;
		m_iTargetBitmap = -1;

		// Redraw entire window.
		Invalidate();
	}

	m_bDrag = FALSE;
	ReleaseCapture();

	// Convert back to device coords.
	DC.LPtoDP(&point);
	CScrollView::OnLButtonUp(nFlags, point);
}


/*********************************************************************
* Protected: OnMouseMove.
*********************************************************************/
void CSpriteBuilderView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint ptDevice = point;
	//TRACE("moved\n");
	// Convert from device coords into MM_TEXT.
	CClientDC DC(this);
	OnPrepareDC(&DC);
	DC.DPtoLP(&point);

	static CPoint ptPrevPoint = point;

	if (m_bDrag && m_iSelectedBitmap != -1)
	{
		// Erase previous position...
		CRect rect = m_SelectedRect;
		rect.InflateRect(BW + X_SPACING, BW);
		InvalidateRect(&rect);

		// Move selected bitmap...
		m_SelectedRect.OffsetRect(point - ptPrevPoint);

		// Redraw selected...
		rect = m_SelectedRect;
		rect.InflateRect(BW + X_SPACING, BW);
		InvalidateRect(&rect);
		
		// Handle target.
		CRect PrevTargetRect(m_TargetRect);
		m_iPrevTargetBitmap = m_iTargetBitmap;
		m_iTargetBitmap = NearTest(m_SelectedRect.CenterPoint(), m_TargetRect);

		// Erase previous target.
		if (m_iPrevTargetBitmap != m_iTargetBitmap)
		{
			rect = PrevTargetRect;
			rect.InflateRect(0, TARGET_HEIGHT);
			InvalidateRect(&rect);
			TRACE("Erasing rect = %d %d %d %d\n",
				rect.left, rect.top, rect.right, rect.bottom);
		}

		// Draw new target.
		rect = m_TargetRect;
		rect.InflateRect(0, TARGET_HEIGHT);
		InvalidateRect(&rect);

		// Update status bar message.
		CString sMessage = "";
		if (m_iTargetBitmap != -1)
		{
			sMessage.Format("Insert bitmap at position %d.\n", m_iTargetBitmap);
		}

		((CFrameWnd*)AfxGetMainWnd())->GetControlBar(AFX_IDW_STATUS_BAR)->SetWindowText(sMessage);

		// Scroll necessary?
		CRect ClientRect;
		GetClientRect(&ClientRect);
		CPoint ptScrollPos = GetScrollPosition();

		if (ptDevice.y > ClientRect.bottom)
		{
			ptScrollPos.y += SCROLL_LINE;
			ScrollToPosition(ptScrollPos);
		}
		else if (ptDevice.y < ClientRect.top)
		{
			TRACE("Need to scroll!\n");
			ptScrollPos.y -= SCROLL_LINE;
			ScrollToPosition(ptScrollPos);
		}

		
	}
	
	ptPrevPoint = point;

	// Convert back to device coords.
	DC.LPtoDP(&point);
	CScrollView::OnMouseMove(nFlags, point);
}


/*********************************************************************
* Protected: OnRButton.
*********************************************************************/
void CSpriteBuilderView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bDrag)
		return;

	// Convert from device coords into MM_TEXT.
	CClientDC DC(this);
	OnPrepareDC(&DC);
	DC.DPtoLP(&point);

	SelectBitmap(point);
	
	// Convert back to device coords.
	DC.LPtoDP(&point);
	CScrollView::OnRButtonDown(nFlags, point);
}


/*********************************************************************
* Protected: SelectBitmap.
*********************************************************************/
void CSpriteBuilderView::SelectBitmap(CPoint Point)
{
	SelectBitmap(HitTest(Point));
}

void CSpriteBuilderView::SelectBitmap(int newSelection)
{
	CRectArray& rRectArray = GetDocument()->m_RectArray;
	int iCnt = rRectArray.GetSize();

	if (newSelection == iCnt)
		newSelection--;
	
	DoSelect(newSelection);
}

void CSpriteBuilderView::DoSelect(int newSelection)
{
	CRectArray& rRectArray = GetDocument()->m_RectArray;
	int iCnt = rRectArray.GetSize();
	if (newSelection < -1 || newSelection >= iCnt)
		return;

	// Find sprite under cursor.
	m_iPrevSelectedBitmap = m_iSelectedBitmap;
	m_iSelectedBitmap = newSelection;

	if (m_iPrevSelectedBitmap != m_iSelectedBitmap)
	{
		CRectArray& rRectArray = GetDocument()->m_RectArray;

		if (m_iPrevSelectedBitmap != -1 && m_iPrevSelectedBitmap < rRectArray.GetSize())
		{
			*rRectArray[m_iPrevSelectedBitmap] = m_SelectedRect;

			CRect rect = rRectArray[m_iPrevSelectedBitmap];
			rect.InflateRect(BW, BW);
			InvalidateRect(rect);
		}

		if (m_iSelectedBitmap != -1)
		{
			CRect rect = *rRectArray[m_iSelectedBitmap];
			m_SelectedRect = rect;
			rect.InflateRect(BW, BW);
			InvalidateRect(rect);
		}
	}
}


/*********************************************************************
* Protected: InvalidateRect.
*********************************************************************/
void CSpriteBuilderView::InvalidateRect(LPCRECT lpRect, BOOL bErase)
{
	// Convert from device coords into MM_TEXT.
	CClientDC DC(this);
	OnPrepareDC(&DC);
	CRect rect = *lpRect;
	DC.LPtoDP(&rect);
	CScrollView::InvalidateRect(rect);
}


/*********************************************************************
* Protected: Invalidate.
*********************************************************************/
void CSpriteBuilderView::Invalidate(BOOL bErase, int iSelected)
{
	if (iSelected != -2)
	{
		DoSelect(iSelected);
	}

	EnsureVisible();

	CScrollView::Invalidate(bErase);

	if (iSelected > 0)
	{
		DoSelect(-1);
		DoSelect(iSelected);
	}
}


void CSpriteBuilderView::EnsureVisible()
{
	CalcBitmapPositions();

	// Ensure that any newly selected bitmap is visible in the view.
	if (m_iSelectedBitmap >= 0)
	{
		CRect ClientRect;
		GetClientRect(&ClientRect);

		CRect IntersectRect;
		CRect SelRect = m_SelectedRect;
		CPoint ptScrollPos = GetScrollPosition();
		SelRect.OffsetRect(- ptScrollPos);
		IntersectRect.IntersectRect(&ClientRect, &SelRect);

		if (IntersectRect != SelRect)
		{
			CPoint pt = m_SelectedRect.TopLeft();
			pt.y -= 10;
			ScrollToPosition(pt);
		}
	}
}

	/*********************************************************************
* Public: Cut.
*********************************************************************/
void CSpriteBuilderView::Cut(int iCut)
{
	int oldSel = m_iSelectedBitmap;

	if (iCut == m_iPrevSelectedBitmap)
		m_iPrevSelectedBitmap = -1;

	if (iCut == m_iSelectedBitmap)
		m_iSelectedBitmap = -1;
}

void CSpriteBuilderView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}

void CSpriteBuilderView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRectArray& rRectArray = GetDocument()->m_RectArray;
	int entries = rRectArray.GetSize();
	int newSelection = m_iSelectedBitmap;
	if (nChar == VK_RIGHT)
		newSelection++;
	else if (nChar == VK_LEFT)
		newSelection--;
	else if (nChar == VK_UP || nChar == VK_DOWN)
	{
		CalcBitmapPositions();

		if (nChar == VK_DOWN)
		{
			// start of next row
			do
			{
				++newSelection;
			}
			while (newSelection < entries - 1 &&
				m_SelectedRect.top == rRectArray[newSelection]->top);

			// find horizontal position
			int nextRow = newSelection;
			if (newSelection < entries - 1)
			{
				int rowTop = rRectArray[newSelection]->top; 
				while (newSelection < entries - 1 &&
					rowTop == rRectArray[newSelection]->top &&
					m_SelectedRect.left > rRectArray[newSelection]->right)
				{
					++newSelection;
				}
				if (newSelection >= entries || rowTop != rRectArray[newSelection]->top)
					--newSelection;
			}
		}
		else
		{
			if (newSelection == -1)
				newSelection = entries - 1;

			// end of prev row
			do
			{
				--newSelection;
			}
			while (newSelection > 0 &&
				m_SelectedRect.top == rRectArray[newSelection]->top);

			// find horizontal position
			int prevRow = newSelection;
			if (newSelection > 0)
			{
				int rowTop = rRectArray[newSelection]->top; 
				while (newSelection > 0 &&
					rowTop == rRectArray[newSelection]->top &&
					m_SelectedRect.right < rRectArray[newSelection]->left)
				{
					--newSelection;
				}
				if (newSelection < 0 || rowTop != rRectArray[newSelection]->top)
					++newSelection;
			}
			if (newSelection == -1)
				newSelection = 0;
		}

	}

	DoSelect(newSelection);
	EnsureVisible();
	
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

