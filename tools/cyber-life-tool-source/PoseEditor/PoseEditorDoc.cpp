// PoseEditorDoc.cpp : implementation of the CPoseEditorDoc class
//

#include "stdafx.h"
#include "PoseEditor.h"

#include "PoseEditorDoc.h"
#include "Mainfrm.h"
#include "Appearance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorDoc

IMPLEMENT_DYNCREATE(CPoseEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CPoseEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CPoseEditorDoc)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_FILE_NAMES_IMPORTGAITNAMES, OnFileNamesImportgaitnames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorDoc construction/destruction

CPoseEditorDoc::CPoseEditorDoc()
{
	// TODO: add one-time construction code here

}

CPoseEditorDoc::~CPoseEditorDoc()
{
}

BOOL CPoseEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPoseEditorDoc serialization

void CPoseEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorDoc diagnostics

#ifdef _DEBUG
void CPoseEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPoseEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPoseEditorDoc commands

BOOL CPoseEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
			
	if( !m_Genome.Load( lpszPathName ) )
		return FALSE;

	m_Appearance.m_Age = 0;
	m_Appearance.m_Sex = 0;
	m_Genome.GetBodyPart( CGene::BP_Head, &m_Appearance.m_Head, &m_Appearance.m_HeadVar );
	m_Genome.GetBodyPart( CGene::BP_Body, &m_Appearance.m_Body, &m_Appearance.m_BodyVar );
	m_Genome.GetBodyPart( CGene::BP_Legs, &m_Appearance.m_Legs, &m_Appearance.m_LegsVar );
	m_Genome.GetBodyPart( CGene::BP_Arms, &m_Appearance.m_Arms, &m_Appearance.m_ArmsVar );
	m_Genome.GetBodyPart( CGene::BP_Tail, &m_Appearance.m_Tail, &m_Appearance.m_TailVar );

	return TRUE;
}

BOOL CPoseEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	return m_Genome.Save( lpszPathName );
}

void CPoseEditorDoc::EditPose( HGene const &gene )
{

	CPoseDlg *dlg = 0;
	CMainFrame *frm = (CMainFrame *)AfxGetMainWnd();

// see if dlg is open already
	std::list< handle< CPoseDlg > >::iterator itor;
	for( itor = m_PoseDlgs.begin(); itor != m_PoseDlgs.end(); ++itor )
		if( itor->GetObj()->GetGene() == gene ) dlg = itor->GetObj();

	if( !dlg )
	{
		dlg = new CPoseDlg(CWnd::FromHandle(frm->m_hWndMDIClient));
		m_PoseDlgs.push_back( handle<CPoseDlg>( dlg ) );
		for( int i = 1; i < 16; ++i )
			dlg->m_PoseString += gene.GetObj()->m_Data[i];
		dlg->Create( this, gene, CWnd::FromHandle(frm->m_hWndMDIClient) );
	}
	dlg->BringWindowToTop();
}

void CPoseEditorDoc::ClosePoseDlg( CPoseDlg *dlg )
{
	std::list< handle< CPoseDlg > >::iterator itor;
	for( itor = m_PoseDlgs.begin(); itor != m_PoseDlgs.end(); ++itor )
	{
		if( itor->GetObj() == dlg )
		{
			m_PoseDlgs.erase( itor );
			return;
		}
	}
}

void CPoseEditorDoc::EditGait( HGene const &gene )
{

	CGaitDlg *dlg = 0;
	CMainFrame *frm = (CMainFrame *)AfxGetMainWnd();

// see if dlg is open already
	std::list< handle< CGaitDlg > >::iterator itor;
	for( itor = m_GaitDlgs.begin(); itor != m_GaitDlgs.end(); ++itor )
		if( itor->GetObj()->GetGene() == gene ) dlg = itor->GetObj();

	if( !dlg )
	{
		dlg = new CGaitDlg(CWnd::FromHandle(frm->m_hWndMDIClient));
		m_GaitDlgs.push_back( handle<CGaitDlg>( dlg ) );
		dlg->Create( this, gene, CWnd::FromHandle(frm->m_hWndMDIClient) );
	}
	dlg->BringWindowToTop();
}

void CPoseEditorDoc::CloseGaitDlg( CGaitDlg *dlg )
{
	std::list< handle< CGaitDlg > >::iterator itor;
	for( itor = m_GaitDlgs.begin(); itor != m_GaitDlgs.end(); ++itor )
	{
		if( itor->GetObj() == dlg )
		{
			m_GaitDlgs.erase( itor );
			return;
		}
	}
}

void CPoseEditorDoc::ModifyGene( CGene const &gene )
{
	Execute( HPoseCommand( new CPoseCommandModify( gene ) ) );
}

void CPoseEditorDoc::AddGene( CGene const &gene )
{
	Execute( HPoseCommand( new CPoseCommandAdd( gene ) ) );
}

void CPoseEditorDoc::RemoveGene( CGene const &gene )
{
	Execute( HPoseCommand( new CPoseCommandRemove( gene ) ) );
}

void CPoseEditorDoc::Execute( HPoseCommand const &command )
{
	m_Undo.push( command );
	m_Undo.top()->Do( this );
	while( m_Redo.size() ) m_Redo.pop();
}

void CPoseEditorDoc::OnEditRedo() 
{
	m_Redo.top()->Do( this );
	m_Undo.push( m_Redo.top() );
	m_Redo.pop();
}

void CPoseEditorDoc::OnEditUndo() 
{
	m_Undo.top()->Undo( this );
	m_Redo.push( m_Undo.top() );
	m_Undo.pop();
}

void CPoseEditorDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Undo.size() != 0 );
	CString cmdString = "Undo ";
	if( m_Undo.size() ) cmdString += m_Undo.top()->Describe();
	cmdString += "\tCtrl+Z";
	pCmdUI->SetText( cmdString  );
}

void CPoseEditorDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Redo.size() != 0 );
	CString cmdString = "Redo ";
	if( m_Redo.size() ) cmdString += m_Redo.top()->Describe();
	cmdString += "\tCtrl+Y";
	pCmdUI->SetText( cmdString  );
}

void CPoseEditorDoc::RefreshGeneDlgs( CGene const &gene )
{
	std::list< handle< CPoseDlg > >::iterator itor;
	for( itor = m_PoseDlgs.begin(); itor != m_PoseDlgs.end(); ++itor )
		itor->GetObj()->Refresh();

	std::list< handle< CGaitDlg > >::iterator itor2;
	for( itor2 = m_GaitDlgs.begin(); itor2 != m_GaitDlgs.end(); ++itor2 )
		itor2->GetObj()->Refresh( gene );
}

void CPoseEditorDoc::RemoveGeneDlgs( CGene const &gene )
{
	std::list< handle< CPoseDlg > >::iterator itor;
	for( itor = m_PoseDlgs.begin(); itor != m_PoseDlgs.end(); )
		if( itor->GetObj()->GetGene()->SameID( gene ) )
			itor = m_PoseDlgs.erase( itor );
		else
			++itor;

	std::list< handle< CGaitDlg > >::iterator itor2;
	for( itor2 = m_GaitDlgs.begin(); itor2 != m_GaitDlgs.end(); )
		if( itor2->GetObj()->GetGene()->SameID( gene ) )
			itor2 = m_GaitDlgs.erase( itor2 );
		else
			++itor2;
}

void CPoseEditorDoc::UpdateAppearance( CAppearance const &app )
{
	m_Appearance = app;
	std::list< handle< CPoseDlg > >::iterator itor;
	for( itor = m_PoseDlgs.begin(); itor != m_PoseDlgs.end(); ++itor )
		itor->GetObj()->UpdateAppearance( app );

	std::list< handle< CGaitDlg > >::iterator itor2;
	for( itor2 = m_GaitDlgs.begin(); itor2 != m_GaitDlgs.end(); ++itor2 )
		itor2->GetObj()->UpdateAppearance( app );
}

void CPoseEditorDoc::OnFileNamesImportgaitnames() 
{
	CFileDialog dlg( TRUE, ".gno", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Genome files (*.gno)|*.gno" );

	if( dlg.DoModal() == IDOK )
		ImportGaitDescriptions( std::string( dlg.GetPathName() ), GetGenome() );
}

