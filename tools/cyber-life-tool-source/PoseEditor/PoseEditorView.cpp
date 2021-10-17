// PoseEditorView.cpp : implementation of the CPoseEditorView class
//

#include "stdafx.h"
#include "PoseEditor.h"

#include "PoseEditorDoc.h"
#include "PoseEditorView.h"
#include "Appearance.h"
#include "NewPoseDlg.h"
#include "NewGaitDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorView

extern char UserGuideFile[512];
extern char CurrentDirectory[512];

IMPLEMENT_DYNCREATE(CPoseEditorView, CFormView)

BEGIN_MESSAGE_MAP(CPoseEditorView, CFormView)
	//{{AFX_MSG_MAP(CPoseEditorView)
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_GAIT_LIST, OnDblclkGaitList)
	ON_NOTIFY(NM_DBLCLK, IDC_POSE_LIST_VIEW, OnDblclkPoseListView)
	ON_BN_CLICKED(IDC_ADD_GAIT, OnAddGait)
	ON_BN_CLICKED(IDC_ADD_POSE, OnAddPose)
	ON_BN_CLICKED(IDC_REMOVE_POSE, OnRemovePose)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_BN_CLICKED(IDC_REMOVE_GAIT, OnRemoveGait)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_BN_CLICKED(IDC_EDIT_GAIT, OnEditGait)
	ON_BN_CLICKED(IDC_EDIT_POSE, OnEditPose)
	ON_COMMAND(ID_HELP_USERGUIDE, OnHelpUserguide)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_CBN_SELCHANGE(IDC_SEX, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_AGE, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_GENUS, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_HEAD, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_BODY, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_LEGS, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_ARMS, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_TAIL, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_EARS, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_HAIR, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_HEAD_VAR, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_BODY_VAR, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_LEGS_VAR, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_ARMS_VAR, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_TAIL_VAR, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_EARS_VAR, OnSelchangeAppearance)
	ON_CBN_SELCHANGE(IDC_HAIR_VAR, OnSelchangeAppearance)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorView construction/destruction

CPoseEditorView::CPoseEditorView()
	: CFormView(CPoseEditorView::IDD)
{
	//{{AFX_DATA_INIT(CPoseEditorView)
	m_Age = -1;
	m_Arms = -1;
	m_ArmsVar = -1;
	m_Body = -1;
	m_BodyVar = -1;
	m_Genus = -1;
	m_Head = -1;
	m_HeadVar = -1;
	m_Legs = -1;
	m_LegsVar = -1;
	m_Sex = -1;
	m_Tail = -1;
	m_TailVar = -1;
	m_Ears = -1;
	m_EarsVar = -1;
	m_Hair = -1;
	m_HairVar = -1;
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CPoseEditorView::~CPoseEditorView()
{
}

void CPoseEditorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPoseEditorView)
	DDX_Control(pDX, IDC_POSE_LIST_VIEW, m_PoseListView);
	DDX_Control(pDX, IDC_GAIT_LIST, m_GaitList);
	DDX_CBIndex(pDX, IDC_AGE, m_Age);
	DDX_CBIndex(pDX, IDC_ARMS, m_Arms);
	DDX_CBIndex(pDX, IDC_ARMS_VAR, m_ArmsVar);
	DDX_CBIndex(pDX, IDC_BODY, m_Body);
	DDX_CBIndex(pDX, IDC_BODY_VAR, m_BodyVar);
	DDX_CBIndex(pDX, IDC_GENUS, m_Genus);
	DDX_CBIndex(pDX, IDC_HEAD, m_Head);
	DDX_CBIndex(pDX, IDC_HEAD_VAR, m_HeadVar);
	DDX_CBIndex(pDX, IDC_LEGS, m_Legs);
	DDX_CBIndex(pDX, IDC_LEGS_VAR, m_LegsVar);
	DDX_CBIndex(pDX, IDC_SEX, m_Sex);
	DDX_CBIndex(pDX, IDC_TAIL, m_Tail);
	DDX_CBIndex(pDX, IDC_TAIL_VAR, m_TailVar);
	DDX_CBIndex(pDX, IDC_EARS, m_Ears);
	DDX_CBIndex(pDX, IDC_EARS_VAR, m_EarsVar);
	DDX_CBIndex(pDX, IDC_HAIR, m_Hair);
	DDX_CBIndex(pDX, IDC_HAIR_VAR, m_HairVar);
	//}}AFX_DATA_MAP
}

BOOL CPoseEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CFormView::PreCreateWindow(cs);
	cs.style = cs.style & ~WS_THICKFRAME;
	cs.dwExStyle = cs.dwExStyle & ~WS_EX_CLIENTEDGE;
}

/*
bool operator<( HGene const &p1, HGene const &p2 ) const
{
	return p1.GetObj()->m_Data[0] > p2.GetObj()->m_Data[0];
}
*/

void CPoseEditorView::OnInitialUpdate()
{
	if (!UpdateData(FALSE))
		TRACE0("UpdateData failed during formview initial update.\n");

	CRect rect;
	m_PoseListView.GetClientRect( &rect );
	int width = rect.Width() - GetSystemMetrics( SM_CXVSCROLL );
	int ageWidth = width /3;
	m_PoseListView.InsertColumn( 0, "Pose Name", LVCFMT_LEFT, width - ageWidth );
	m_PoseListView.InsertColumn( 1, "Age", LVCFMT_LEFT, ageWidth );

	CFormView::OnInitialUpdate();


	ResizeParentToFit( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorView printing

BOOL CPoseEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPoseEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPoseEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CPoseEditorView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorView diagnostics

#ifdef _DEBUG
void CPoseEditorView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPoseEditorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPoseEditorDoc* CPoseEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPoseEditorDoc)));
	return (CPoseEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorView message handlers


int CPoseEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetWindowLong( m_hWnd, GWL_STYLE, GetWindowLong( m_hWnd, GWL_STYLE ) & ~WS_THICKFRAME );
	return 0;
}

void CPoseEditorView::OnDblclkGaitList() 
{
	int Sel = m_GaitList.GetCurSel();
	if( Sel >= 0 )
	{
		CHGeneList::iterator itor = m_GaitGenes.begin();
		while( Sel-- ) ++itor;
		GetDocument()->EditGait( *itor );
	}
}

void CPoseEditorView::OnDblclkPoseListView(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEditPose();
	*pResult = 0;
}


void CPoseEditorView::OnSelchangeAppearance() 
{
	UpdateData();
	CAppearance app( m_Sex, m_Age, m_Head, m_HeadVar,
		m_Body, m_BodyVar, m_Legs, m_LegsVar,
		m_Arms, m_ArmsVar, m_Tail, m_TailVar,
		m_Ears, m_EarsVar, m_Hair, m_HairVar );
	GetDocument()->UpdateAppearance( app );
}

void CPoseEditorView::OnAddGait() 
{
	CNewGaitDlg dlg( GetDocument() );
	dlg.DoModal();
}

void CPoseEditorView::OnAddPose() 
{
	CNewPoseDlg dlg( GetDocument() );
	dlg.DoModal();
}

void CPoseEditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if( lHint == 1 )
	{
		HGene *pHGene = (HGene *)pHint;
		if( pHGene->GetObj()->m_Type == 2 && pHGene->GetObj()->m_SubType == 3 )
			AddPoseToList( *pHGene );
		if( pHGene->GetObj()->m_Type == 2 && pHGene->GetObj()->m_SubType == 4 )
			AddGaitToList( *pHGene );
		return;
	}
	else if( lHint == 2 )
	{
		HGene *pHGene = (HGene *)pHint;
		if( pHGene->GetObj()->m_Type == 2 && pHGene->GetObj()->m_SubType == 3 )
			RemovePoseFromList( *pHGene );
		if( pHGene->GetObj()->m_Type == 2 && pHGene->GetObj()->m_SubType == 4 )
			RemoveGaitFromList( *pHGene );
		return;
	}

	m_GaitList.ResetContent();
	m_PoseListView.DeleteAllItems();
	m_PoseGenes.clear();
	m_GaitGenes.clear();
	
	CGenome *genome = GetDocument()->GetGenome();

	CGeneIterator itorPose = genome->GeneTypeBegin( 2, 3 );

	for( itorPose = genome->GeneTypeBegin( 2, 3 );
		itorPose != genome->GeneEnd();
		++itorPose )
		AddPoseToList( *itorPose );

	for( itorPose = genome->GeneTypeBegin( 2, 4 );
		itorPose != genome->GeneEnd();
		++itorPose )
		AddGaitToList( *itorPose );

	CAppearance const &app = GetDocument()->GetAppearance();
	m_Genus = genome->GetGenus();
	m_Sex = app.m_Sex;
	m_Age = app.m_Age;
	m_Head = app.m_Head;
	m_HeadVar = app.m_HeadVar;
	m_Body = app.m_Body;
	m_BodyVar = app.m_BodyVar;
	m_Legs = app.m_Legs;
	m_LegsVar = app.m_LegsVar;
	m_Arms = app.m_Arms;
	m_ArmsVar = app.m_ArmsVar;
	m_Tail = app.m_Tail;
	m_TailVar = app.m_TailVar;
	m_Ears = app.m_Ears;
	m_EarsVar = app.m_EarsVar;
	m_Hair = app.m_Hair;
	m_HairVar = app.m_HairVar;
	UpdateData( FALSE );
}

void CPoseEditorView::AddPoseToList( HGene const &gene )
{
	int i = 0;
	CHGeneList::iterator itor;
	for( itor = m_PoseGenes.begin();
		itor != m_PoseGenes.end();
		++itor )
	{
		if( *gene < *( itor->GetObj() ) )
		{
			AddPoseToList( gene, itor, i );
			return;
		}
		++i;
	}
	AddPoseToList( gene, itor, i );
}

void CPoseEditorView::AddPoseToList( HGene const &gene, CHGeneList::iterator itor, int i )
{
	m_PoseGenes.insert( itor, gene );
	CString caption;
	caption.Format( "(%d) %s", gene->m_Data[0], GetPoseDescription( gene->m_Data[0] ).c_str() );
	m_PoseListView.InsertItem( i, caption );
	m_PoseListView.SetItemText( i, 1, gene->GetAgeName().c_str() );
}

void CPoseEditorView::AddGaitToList( HGene const &gene )
{
	int i = 0;
	CHGeneList::iterator itor;
	for( itor = m_GaitGenes.begin();
		itor != m_GaitGenes.end();
		++itor )
	{
		if( *gene < *( itor->GetObj() ) )
		{
			AddGaitToList( gene, itor, i );
			return;
		}
		++i;
	}
	AddGaitToList( gene, itor, i );
}

void CPoseEditorView::AddGaitToList( HGene const &gene, CHGeneList::iterator itor, int i )
{
	m_GaitGenes.insert( itor, gene );
	CString caption;
	caption.Format( "(%d) %s", gene->m_Data[0], GetPoseDescription( gene->m_Data[0] + 256 ).c_str() );
	m_GaitList.InsertString( i, caption );
}

void CPoseEditorView::OnRemovePose() 
{
	POSITION pos = m_PoseListView.GetFirstSelectedItemPosition();
	while( pos )
	{
		int Sel = m_PoseListView.GetNextSelectedItem(pos);
		CHGeneList::iterator itor = m_PoseGenes.begin();
		while( Sel-- ) ++itor;
		GetDocument()->RemoveGene( *(*itor) );
	}
}

void CPoseEditorView::RemovePoseFromList( HGene const &gene )
{
	int i = 0;
	CHGeneList::iterator itor;
	for( itor = m_PoseGenes.begin();
		itor != m_PoseGenes.end();
		++itor )
	{
		if( gene->m_ID == itor->GetObj()->m_ID )
		{
			m_PoseListView.DeleteItem( i );
			m_PoseGenes.erase( itor );
			return;
		}
		++i;
	}
}

void CPoseEditorView::RemoveGaitFromList( HGene const &gene )
{
	int i = 0;
	CHGeneList::iterator itor;
	for( itor = m_GaitGenes.begin();
		itor != m_GaitGenes.end();
		++itor )
	{
		if( gene->m_ID == itor->GetObj()->m_ID )
		{
			m_GaitList.DeleteString( i );
			m_GaitGenes.erase( itor );
			return;
		}
		++i;
	}
}

void CPoseEditorView::OnEditCopy() 
{
	if( OpenClipboard() && EmptyClipboard() )
	{
		if( GetFocus() == &m_PoseListView && m_PoseListView.GetSelectedCount() )
		{
			POSITION pos = m_PoseListView.GetFirstSelectedItemPosition();
			if( pos )
			{
				int Sel = m_PoseListView.GetNextSelectedItem(pos);
				CHGeneList::iterator itor = m_PoseGenes.begin();
				while( Sel-- ) ++itor;
				itor->GetObj()->WriteClipboard();
			}
		}
		else if( GetFocus() == &m_GaitList && m_GaitList.GetCurSel() != -1 )
		{
			int Sel = m_GaitList.GetCurSel();
			CHGeneList::iterator itor = m_GaitGenes.begin();
			while( Sel-- ) ++itor;
			itor->GetObj()->WriteClipboard();
		}
		CloseClipboard();
	}
}

void CPoseEditorView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	BOOL Enabled = FALSE;
	if( ( GetFocus() == &m_PoseListView && m_PoseListView.GetSelectedCount() ) ||
		( GetFocus() == &m_GaitList && m_GaitList.GetCurSel() != -1 ) )
		Enabled = TRUE;
	pCmdUI->Enable( Enabled );
}

void CPoseEditorView::OnEditPaste() 
{
	CGene gene;
	if( OpenClipboard() )
	{
		if( gene.ReadClipboard() )
		{
			if( gene.m_Type == 2 && gene.m_SubType == 4 )
			{
				CNewGaitDlg dlg( GetDocument() );
				dlg.SetGene( gene );
				dlg.DoModal();
			}
			else if( gene.m_Type == 2 && gene.m_SubType == 3 )
			{
				CNewPoseDlg dlg( GetDocument() );
				dlg.SetGene( gene );
				dlg.DoModal();
			}
		}
		CloseClipboard();
	}
}

void CPoseEditorView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( CGene::CanPaste() );
}

void CPoseEditorView::OnRemoveGait() 
{
	int pos = m_GaitList.GetCurSel();
	if( pos >= 0 )
	{
		CHGeneList::iterator itor = m_GaitGenes.begin();
		while( pos-- ) ++itor;
		GetDocument()->RemoveGene( *(*itor) );
	}
}

void CPoseEditorView::OnEditCut() 
{
	if( OpenClipboard() && EmptyClipboard() )
	{
		if( GetFocus() == &m_PoseListView && m_PoseListView.GetSelectedCount() )
		{
			POSITION pos = m_PoseListView.GetFirstSelectedItemPosition();
			if( pos )
			{
				int Sel = m_PoseListView.GetNextSelectedItem(pos);
				CHGeneList::iterator itor = m_PoseGenes.begin();
				while( Sel-- ) ++itor;
				itor->GetObj()->WriteClipboard();
				GetDocument()->RemoveGene( *(*itor) );
			}
		}
		else if( GetFocus() == &m_GaitList && m_GaitList.GetCurSel() != -1 )
		{
			int Sel = m_GaitList.GetCurSel();
			CHGeneList::iterator itor = m_GaitGenes.begin();
			while( Sel-- ) ++itor;
			itor->GetObj()->WriteClipboard();
			GetDocument()->RemoveGene( *(*itor) );
		}
		CloseClipboard();
	}
}

void CPoseEditorView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	BOOL Enabled = FALSE;
	if( ( GetFocus() == &m_PoseListView && m_PoseListView.GetSelectedCount() ) ||
		( GetFocus() == &m_GaitList && m_GaitList.GetCurSel() != -1 ) )
		Enabled = TRUE;
	pCmdUI->Enable( Enabled );
}

void CPoseEditorView::OnEditGait() 
{
	OnDblclkGaitList();	
}

void CPoseEditorView::OnEditPose() 
{
	POSITION pos = m_PoseListView.GetFirstSelectedItemPosition();
	while( pos )
	{
		int Sel = m_PoseListView.GetNextSelectedItem(pos);
		CHGeneList::iterator itor = m_PoseGenes.begin();
		while( Sel-- ) ++itor;
		GetDocument()->EditPose( *itor );
	}
}

void CPoseEditorView::OnHelpUserguide() 
{
	DWORD Status;
	CString Message;

	// Launch the default HTML viewer on the file
	Status = (DWORD)ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),
        NULL, UserGuideFile, NULL, CurrentDirectory, SW_SHOWNORMAL);

	if (Status <= 32) {
		::Beep(450, 100);
		Message.Format("Unable to view the user guide [error code %d]", Status);
		::AfxMessageBox(Message, MB_OK | MB_ICONWARNING);
	}
	
}

