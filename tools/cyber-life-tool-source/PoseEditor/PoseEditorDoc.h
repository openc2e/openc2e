// PoseEditorDoc.h : interface of the CPoseEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSEEDITORDOC_H__FC4A1FD3_A95B_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_POSEEDITORDOC_H__FC4A1FD3_A95B_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Genome.h"
#include "PoseDlg.h"
#include "GaitDlg.h"
#include "PoseCommand.h"
#include <stack>

#include "Appearance.h"

class CPoseEditorDoc : public CDocument
{
protected: // create from serialization only
	CPoseEditorDoc();
	DECLARE_DYNCREATE(CPoseEditorDoc)

// Attributes
public:
	CGenome *GetGenome() { return &m_Genome; }

	void UpdateAppearance( CAppearance const &app );
	CAppearance const &GetAppearance() {return m_Appearance;}

	void EditPose( HGene const &gene );
	void ClosePoseDlg( CPoseDlg *dlg );

	void EditGait( HGene const &gene );
	void CloseGaitDlg( CGaitDlg *dlg );

	void RefreshGeneDlgs( CGene const &gene );
	void RemoveGeneDlgs( CGene const &gene );
	void ModifyGene( CGene const &gene );
	void AddGene( CGene const &gene );
	void RemoveGene( CGene const &gene );
	void Execute( HPoseCommand const &command );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoseEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPoseEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPoseEditorDoc)
	afx_msg void OnEditRedo();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnFileNamesImportgaitnames();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CGenome m_Genome;
	CAppearance m_Appearance;
	std::list< handle< CPoseDlg > > m_PoseDlgs;
	std::list< handle< CGaitDlg > > m_GaitDlgs;
	std::stack< HPoseCommand > m_Undo;
	std::stack< HPoseCommand > m_Redo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSEEDITORDOC_H__FC4A1FD3_A95B_11D2_9D30_0090271EEABE__INCLUDED_)

