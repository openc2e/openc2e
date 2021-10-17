// MainFrame.h : interface of the CMainFrame class


#if !defined(AFX_MAINFRAME_H__D09269D8_32C3_11D2_A804_0060B05E3B36__INCLUDED_)
#define AFX_MAINFRAME_H__D09269D8_32C3_11D2_A804_0060B05E3B36__INCLUDED_

#pragma once

#include <string>

class CCaosApp; // forward declaration

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

private:
	CToolBar		m_wndToolBar;
	int				m_NextID;
	int				m_NextX, m_NextY;
	CCaosChildFrame *m_CaosFrameList[500];
	int				m_CaosFrameCount;

	void CreateErrorWindow();
	void CreateOutputWindow();
	bool CreateScriptoriumWindow();
	bool DoClose();
	bool DoSave(CCaosChildFrame *CaosFrame);
	bool DoSaveAs(CCaosChildFrame *CaosFrame);
	void DoFetchScript
		(CCaosChildFrame *CaosFrame, 
		HTREEITEM hNode,
		int FGSEValues[4]);
	void DoFetchScripts
		(CCaosChildFrame *CaosFrame, 
		HTREEITEM hNode,
		int FGSEValues[4]);
	CCaosChildFrame *DoNew();
	void DoInject(int InjectType);
	BOOL InjectOneScript(CPtrArray& ScriptList, int script_no, std::string script_name);

	void OutputText(CString& Contents, bool bNoPopup = false);

public:

	BOOL					m_SearchDirectionDown;
	CString					m_SearchText;
	DWORD					m_SearchFlags;
	CString					m_ReplaceText;
	BOOL					m_Cancelled;
	CChildFrame				*m_ErrorFrame;
	CChildFrame				*m_OutputFrame;
	CScriptoriumChildFrame	*m_ScriptoriumFrame;
	CStatusBar				m_wndStatusBar;
	CString					m_defaultTitleString;


	CMainFrame();
	virtual ~CMainFrame();

	void RemoveCaosFrameFromFrameList(int ID);
	void DoOpen(CString Filename, BOOL AddSyntaxErrorsToEnd);
	bool DoGenericSave(CCaosChildFrame *CaosFrame);
	void DoGenericFetch(HTREEITEM hNode);

	bool CheckCaosFrameActive();
	void CheckCaosFrameActive(CCmdUI *pCmdUI);
	void CheckScriptoriumFrameActive(CCmdUI *pCmdUI);
	void CheckCaosFramesExist(CCmdUI *pCmdUI);
	void CheckCaosFrameHasSelection(CCmdUI *pCmdUI);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void ShowHelp(bool bAlphabetic);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnNewDocument();
	afx_msg void OnOpenDocument();
	afx_msg void OnCloseDocument();
	afx_msg void OnCloseAllDocuments();
	afx_msg void OnSaveDocument();
	afx_msg void OnSaveDocumentAs();
	afx_msg void OnSaveAllDocuments();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnSelectAll();
	afx_msg void OnFind();
	afx_msg void OnFindNext();
	afx_msg void OnReplace();
	afx_msg void OnGoto();
	afx_msg void OnIndent();
	afx_msg void OnOutdent();
	afx_msg void OnComment();
	afx_msg void OnUncomment();
	afx_msg void OnViewErrorWindow();
	afx_msg void OnViewOutputWindow();
	afx_msg void OnViewScriptoriumWindow();
	afx_msg void OnInjectInstall();
	afx_msg void OnInjectEvent();
	afx_msg void OnInjectInstallAndEvent();
	afx_msg void OnInjectRemove();
	afx_msg void OnCollapse();
	afx_msg void OnExpand();
	afx_msg void OnRefresh();
	afx_msg void OnWrite();
	afx_msg void OnDelete();
	afx_msg void OnFetch();
	afx_msg void OnUpdateSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSaveAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateClose(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCloseAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReplace(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFindNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGoto(CCmdUI* pCmdUI);
	afx_msg void OnUpdateComment(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUncomment(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutdent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReformat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewErrorWindow(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewOutputWindow(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewScriptoriumWindow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInjectEvent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInjectInstall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInjectRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInjectInstallAndEvent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCollapse(CCmdUI* pCmdUI);	
	afx_msg void OnUpdateExpand(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRefresh(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFetch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWrite(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnHelpAlpha();
	afx_msg void OnHelpCateg();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnInjectEditquickmacros();
	afx_msg void OnInjectQuickmacro1();
	afx_msg void OnInjectQuickmacro2();
	afx_msg void OnInjectQuickmacro3();
	afx_msg void OnInjectQuickmacro4();
	afx_msg void OnInjectQuickmacro5();
	afx_msg void OnInjectQuickmacro6();
	afx_msg void OnInjectQuickmacro7();
	afx_msg void OnInjectQuickmacro8();
	afx_msg void OnInjectQuickmacro9();
	afx_msg void OnInjectQuickmacro0();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void InjectQuickMacro(std::string name);

	void AutoSave(CCaosChildFrame * CaosFrame);

	CCaosApp* GetApp() const;

	CString myDefaultLoadDirectory;

public:
	afx_msg void OnReformat();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAME_H__D09269D8_32C3_11D2_A804_0060B05E3B36__INCLUDED_)

