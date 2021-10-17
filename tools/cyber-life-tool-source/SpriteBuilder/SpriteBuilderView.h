// SpriteBuilderView.h : interface of the CSpriteBuilderView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITEBUILDERVIEW_H__A049CFCC_51D9_11D1_8791_0060B07BFA18__INCLUDED_)
#define AFX_SPRITEBUILDERVIEW_H__A049CFCC_51D9_11D1_8791_0060B07BFA18__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "afxtempl.h"
#include "SpriteBuilderDoc.h"

#define X_MARGIN	16
#define Y_MARGIN	16
#define X_SPACING	48
#define Y_SPACING	64
#define BW			4
#define TARGET_HEIGHT	16
#define MAX_ROWS	256

#define SCROLL_LINE (Y_MARGIN / 2)
#define SCROLL_PAGE (SCROLL_LINE * 2)


class CSpriteBuilderCntrItem;

class CSpriteBuilderView : public CScrollView
{
protected: // create from serialization only
	CSpriteBuilderView();
	DECLARE_DYNCREATE(CSpriteBuilderView)

// Attributes
public:
	CSpriteBuilderDoc* GetDocument();
	// m_pSelection holds the selection to the current CSpriteBuilderCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CSpriteBuilderCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CSpriteBuilderCntrItem* m_pSelection;

	int SelectedBitmap()
	{	return m_iSelectedBitmap; }
	BOOL Drag() 
	{	return m_bDrag; }

	void Cut(int iCut);

// Operations
public:
	void Invalidate(BOOL bErase = TRUE, int iSelected = -2);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpriteBuilderView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpriteBuilderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void SelectBitmap(int newSelection);

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSpriteBuilderView)
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DrawBitmap(CDC* pDC, CDC* pMemDC, CSprite& rSprite, int iIndex);
	void DrawBitmapBorder(CDC* pDC, BOOL bErase, CRect Rect);
	void DrawTarget(CDC* pDC, BOOL bErase, CPoint ptPoint);
	void CalcBitmapPositions();

	void SelectBitmap(CPoint Point);

	int HitTest(CPoint point);
	int NearTest(CPoint point, CRect& rectSpace);
	void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);

	void DoSelect(int newSelection);
	void EnsureVisible();

private:
	BOOL m_bDrag;

	int m_iColumnCount[MAX_ROWS];
	int m_iRowCount;

	CRect m_SelectedRect;
	int m_iSelectedBitmap;
	int m_iPrevSelectedBitmap;

	CRect m_TargetRect;
	int m_iTargetBitmap;
	int m_iPrevTargetBitmap;

	CFont m_Font;
};

#ifndef _DEBUG  // debug version in SpriteBuilderView.cpp
inline CSpriteBuilderDoc* CSpriteBuilderView::GetDocument()
   { return (CSpriteBuilderDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPRITEBUILDERVIEW_H__A049CFCC_51D9_11D1_8791_0060B07BFA18__INCLUDED_)

