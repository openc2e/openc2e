// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "C2ERoomEditor.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_INITMENU()
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDS_CURRENT_POINT,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() : m_PropertiesDlg( this )
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
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

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_PropertiesDlg.Create( IDD_ROOM_PROPERTIES, this );
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnViewProperties() 
{
	m_PropertiesDlg.ShowWindow( 
		m_PropertiesDlg.IsWindowVisible() ? SW_HIDE : SW_RESTORE );
}

void CMainFrame::OnUpdateViewProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_PropertiesDlg.IsWindowVisible() );	
}

void CMainFrame::SetCurrentPoint( CPoint const &point )
{
	CString str;
	if( point.x >= 0 && point.y >= 0 )
		str.Format( "%d, %d", point.x, point.y );
	m_wndStatusBar.SetPaneText( 1, str );
}

CRoomPropertiesDlg *GetPropertiesDlg()
{
	return ((CMainFrame *)AfxGetMainWnd())->GetPropertiesDlg();
}

void SetCurrentPoint( CPoint const &point )
{
	((CMainFrame *)AfxGetMainWnd())->SetCurrentPoint( point );
}

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
   CMDIFrameWnd::OnInitMenu(pMenu);
  
}

