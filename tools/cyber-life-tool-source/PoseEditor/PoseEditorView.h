// PoseEditorView.h : interface of the CPoseEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSEEDITORVIEW_H__FC4A1FD5_A95B_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_POSEEDITORVIEW_H__FC4A1FD5_A95B_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPoseEditorView : public CFormView
{
protected: // create from serialization only
	CPoseEditorView();
	DECLARE_DYNCREATE(CPoseEditorView)

public:
	//{{AFX_DATA(CPoseEditorView)
	enum { IDD = IDD_POSEEDITOR_FORM };
	CListCtrl	m_PoseListView;
	CListBox	m_GaitList;
	int		m_Age;
	int		m_Arms;
	int		m_ArmsVar;
	int		m_Body;
	int		m_BodyVar;
	int		m_Genus;
	int		m_Head;
	int		m_HeadVar;
	int		m_Legs;
	int		m_LegsVar;
	int		m_Sex;
	int		m_Tail;
	int		m_TailVar;
	int		m_Ears;
	int		m_EarsVar;
	int		m_Hair;
	int		m_HairVar;
	//}}AFX_DATA

// Attributes
public:
	CPoseEditorDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoseEditorView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPoseEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPoseEditorView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclkGaitList();
	afx_msg void OnDblclkPoseListView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddGait();
	afx_msg void OnAddPose();
	afx_msg void OnRemovePose();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnRemoveGait();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditGait();
	afx_msg void OnEditPose();
	afx_msg void OnHelpUserguide();
	//}}AFX_MSG
	afx_msg void OnSelchangeAppearance();
	DECLARE_MESSAGE_MAP()
private:
	CHGeneList m_PoseGenes;
	CHGeneList m_GaitGenes;

	void AddPoseToList( HGene const &gene );
	void AddPoseToList( HGene const &gene, CHGeneList::iterator itor, int i );
	void AddGaitToList( HGene const &gene );
	void AddGaitToList( HGene const &gene, CHGeneList::iterator itor, int i );
	void RemovePoseFromList( HGene const &gene );
	void RemoveGaitFromList( HGene const &gene );
};

#ifndef _DEBUG  // debug version in PoseEditorView.cpp
inline CPoseEditorDoc* CPoseEditorView::GetDocument()
   { return (CPoseEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSEEDITORVIEW_H__FC4A1FD5_A95B_11D2_9D30_0090271EEABE__INCLUDED_)

