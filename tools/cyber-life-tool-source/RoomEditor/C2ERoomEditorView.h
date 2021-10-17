// C2E Room EditorView.h : interface of the CC2ERoomEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2EROOMEDITORVIEW_H__A50773AF_B9D4_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_C2EROOMEDITORVIEW_H__A50773AF_B9D4_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tool.h"
#include "ViewParams.h"
#include "Door.h"

#include <set>
#include <stack>

class CC2ERoomEditorDoc;

class CC2ERoomEditorView : public CView
{
protected: // create from serialization only
	CC2ERoomEditorView();
	DECLARE_DYNCREATE(CC2ERoomEditorView)

// Attributes
public:
	CC2ERoomEditorDoc* GetDocument();

	CViewParams GetViewParams() const;
	void SetViewArea( CRect const &rect );
	void UpdateScrollInfo();
	CRect GetWorldVisible() const;
	bool GetSelectionProperty( int propertyIndex, int *pValue );
	void SetSelectionProperty( int propertyIndex, int value );

	void ClearSelection();
	// ---------------------------------------------------------------------
	// Method:		SetSelectedMetaRoom
	// Arguments:	index - new selection
	// Returns:		None
	// Description:	Deselects any Room selected.
	// ---------------------------------------------------------------------
	void SetSelectedMetaroom( int index );

	// ---------------------------------------------------------------------
	// Method:		SetSelectedRoom
	// Arguments:	index - new selection
	// Returns:		None
	// Description:	Deselects any Room selected.
	// ---------------------------------------------------------------------
	void SetSelectedRooms( CRect const &rect );
	void SetSelectedRoom( int index );
	std::set< int > const &GetSelectedRooms() { return m_SelectedRooms; }
	void ToggleSelectedRoom( int index );

	void ToggleSelectedDoor( HDoor door );
	void SetSelectedDoor( HDoor door );
	// ---------------------------------------------------------------------
	// Method:		GetSelectedMetaRoom
	// Arguments:	None
	// Returns:		Selected Metaroom index or -1 if no selection
	// Description:	Deselects any Room selected.
	// ---------------------------------------------------------------------
	int GetSelectedMetaroom() const;

	// ---------------------------------------------------------------------
	// Method:		HighlightSelection
	// Arguments:	dc - pointer to device context
	// Returns:		none
	// Description:	Shows selected objects
	// ---------------------------------------------------------------------
	void HighlightSelection( CDC *dc );
	void DrawCheese( CDC *dc, bool inc = false );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CC2ERoomEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CC2ERoomEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
public:
	//{{AFX_MSG(CC2ERoomEditorView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnToolsAddMetaroom();
	afx_msg void OnUpdateToolsAddMetaroom(CCmdUI* pCmdUI);
	afx_msg void OnToolsAddRoom();
	afx_msg void OnUpdateToolsAddRoom(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectMetaroom();
	afx_msg void OnUpdateToolsSelectMetaroom(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectRoom();
	afx_msg void OnUpdateToolsSelectRoom(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnToolsZoom();
	afx_msg void OnUpdateToolsZoom(CCmdUI* pCmdUI);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnViewZoomWorld();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewZoomForward();
	afx_msg void OnUpdateViewZoomForward(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomLast();
	afx_msg void OnUpdateViewZoomLast(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnEditBackground();
	afx_msg void OnUpdateEditBackground(CCmdUI* pCmdUI);
	afx_msg void OnFileValidate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnToolsCheese();
	afx_msg void OnUpdateToolsCheese(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnViewShowBackground();
	afx_msg void OnUpdateViewShowBackground(CCmdUI* pCmdUI);
	afx_msg void OnToolsRunCa();
	afx_msg void OnUpdateToolsRunCa(CCmdUI* pCmdUI);
	afx_msg void OnEditFloorceilingvalues();
	afx_msg void OnUpdateEditFloorceilingvalues(CCmdUI* pCmdUI);
	afx_msg void OnToolsUpdateCaFromGame();
	afx_msg void OnUpdateToolsUpdateCaFromGame(CCmdUI* pCmdUI);
	afx_msg void OnEditMusic();
	afx_msg void OnUpdateEditMusic(CCmdUI* pCmdUI);
	afx_msg void OnEditCheckheights();
	//}}AFX_MSG
	afx_msg void OnViewCA( UINT nID );
	afx_msg void OnUpdateViewCA(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
private:
	void ReflectSelection();
	
	CTool *m_Tool;
	CToolAddRoom m_ToolAddRoom;
	CToolAddMetaroom m_ToolAddMetaroom;
	CToolSelectRoom m_ToolSelectRoom;
	CToolSelectMetaroom m_ToolSelectMetaroom;
	CToolZoom m_ToolZoom;
	CToolCheese m_ToolCheese;
	CRect m_ViewArea;
	int m_SelectedMetaroom;
	std::set< int > m_SelectedRooms;
	CDoorSet m_SelectedDoors;
	std::stack< CRect > m_BackRects;
	std::stack< CRect > m_ForwardRects;
	int m_ColourRoomOpt;
	int m_CheeseRadius;
	UINT m_Timer;
	bool m_ShowBackground;
	bool m_RunCA;
	bool m_UpdateCA;
};

#ifndef _DEBUG  // debug version in C2E Room EditorView.cpp
inline CC2ERoomEditorDoc* CC2ERoomEditorView::GetDocument()
   { return (CC2ERoomEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_C2EROOMEDITORVIEW_H__A50773AF_B9D4_11D2_9D30_0090271EEABE__INCLUDED_)

