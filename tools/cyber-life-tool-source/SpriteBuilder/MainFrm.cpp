// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SpriteBuilder.h"

#include "MainFrm.h"
#include "SpriteBuilderView.h"

#include "ScrollyBitmapViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Popup menu.
	m_Menu.LoadMenu(IDR_POP_UP);

    // Restore previously saved state.
    LoadBarState(_T(REG_CONTROL_BAR));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/*********************************************************************
* Protected: OnContextMenu.
*********************************************************************/
void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CSpriteBuilderView* pView = (CSpriteBuilderView*)GetActiveView();
	if (pView->Drag())
		return;

	CMenu* pSubMenu = m_Menu.GetSubMenu(0);
	ASSERT(pSubMenu);

	if (pView->SelectedBitmap() != -1)
	{
		pSubMenu->EnableMenuItem(ID_REPLACE_BITMAP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_EXPORT_BITMAP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	else
	{
		pSubMenu->EnableMenuItem(ID_REPLACE_BITMAP, MF_BYCOMMAND | MF_ENABLED);
		pSubMenu->EnableMenuItem(ID_EXPORT_BITMAP, MF_BYCOMMAND | MF_ENABLED);
	}

	pSubMenu->TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd(),
		NULL);
}



void CMainFrame::OnClose() 
{
    // Store toolbar states.
    SaveBarState(_T(REG_CONTROL_BAR));
	
	CFrameWnd::OnClose();
}

