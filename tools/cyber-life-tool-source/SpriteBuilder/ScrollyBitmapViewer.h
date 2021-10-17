#if !defined(AFX_SCROLLYBITMAPVIEWER_H__5136D4C3_2D5E_11D3_88A0_0060B05E3B36__INCLUDED_)
#define AFX_SCROLLYBITMAPVIEWER_H__5136D4C3_2D5E_11D3_88A0_0060B05E3B36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FIBitmap.h"
#include "SpriteCutter.h"

typedef CTypedPtrArray<CPtrArray, CRect*> CRectArray;

// ScrollyBitmapViewer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScrollyBitmapViewer window

class ScrollyBitmapViewer : public CDialog
{
// Construction
public:
	ScrollyBitmapViewer(CFIBitmap* bmp);
	BOOL OnInitDialog();
	void AutoCut();
// Attributes
public:
	int x_c_pos;
	int y_c_pos;
	SpriteCutter* myCutter;
	CFIBitmap* m_fiBitmap;
	CSprite* m_pSprites;
	int inspos;
	CRectArray* m_rects;
	bool m_zoomer;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScrollyBitmapViewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ScrollyBitmapViewer();

	// Generated message map functions
protected:
	//{{AFX_MSG(ScrollyBitmapViewer)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLYBITMAPVIEWER_H__5136D4C3_2D5E_11D3_88A0_0060B05E3B36__INCLUDED_)

