
#ifndef _FLAT_TOOLBAR
#define _FLAT_TOOLBAR

#ifndef TBSTYLE_FLAT
#define TBSTYLE_FLAT 0x0800	// (in case you don't have the new commctrl.h)
#endif


// FlatToolBar.h
// (c) 1997, Roger Onslow
class CFlatToolBar : public CToolBar 
{
	DECLARE_DYNAMIC(CFlatToolBar);

public:
    BOOL LoadToolBar(LPCTSTR lpszResourceName);
	BOOL LoadToolBar(UINT nIDResource)
		{ return LoadToolBar(MAKEINTRESOURCE(nIDResource)); }

    BOOL LoadBitmap(LPCTSTR lpszResourceName);
    BOOL LoadBitmap(UINT nIDResource )
		{ return LoadBitmap(MAKEINTRESOURCE(nIDResource)); }

        void SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage);

protected:
	void RepaintBackground();
	void DrawSeparators();
	void DrawSeparators(CClientDC* pDC);
	void EraseNonClient();
	void DrawGripper(CWindowDC *pDC, CRect& rectWindow);
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatToolBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL
	// Message Handlers
protected:
	//{{AFX_MSG(CFlatToolBar)
	afx_msg void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS*	lpncsp );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
};

#endif

