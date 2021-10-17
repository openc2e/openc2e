#if !defined(AFX_SCRIPTORIUMCHILDFRAME_H__87E8A3F1_4B19_11D2_A806_0060B05E3B36__INCLUDED_)
#define AFX_SCRIPTORIUMCHILDFRAME_H__87E8A3F1_4B19_11D2_A806_0060B05E3B36__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScriptoriumChildFrame.h : header file
//


// Structure used to store additional information about each node
struct NodeStructure {
	int FGSEValues[4];		// Family, Genus, Species, Event IDs
	int Level;				// Node level: 0=Root, 1=Family, 2=Genus
							//             3=Species, 4=Event
};


/////////////////////////////////////////////////////////////////////////////
// CScriptoriumChildFrame frame

class CScriptoriumChildFrame : public CMDIChildWnd
{

private:

	// Private Member Variables

	int m_ScriptListSize;
	int m_ScriptCount;
	CStringArray m_ScriptList;
	CPtrArray m_ScriptNodeList;

	// Private static function
	static int CALLBACK CompareNodes
		(LPARAM _Node1, LPARAM _Node2, LPARAM _ChildIndex);

	// Private Member Functions

	HTREEITEM AddChildNodeToParentNode
		(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex);

	HTREEITEM AddChildNodeInSortedOrder
		(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex);

	HTREEITEM FindChildNode
		(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex);

	void LoadTreeHelper
		(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex);

	void UnloadTreeHelper(HTREEITEM hParentNode);
	void ExpandCollapseHelper(HTREEITEM hParentNode, int Operation);
	void DeleteScriptsHelper(HTREEITEM hParentNode);
	void GetScriptsHelper(HTREEITEM hParentNode);

		
public:
	DECLARE_DYNCREATE(CScriptoriumChildFrame)

	// Public Member Variables

	CImageList m_ImageList;
	CScriptoriumTreeCtrl m_ctrlTree;

	// Public Member Functions

	CScriptoriumChildFrame();      
	virtual ~CScriptoriumChildFrame();

	void RemoveFGSE(int FGSEValues[4]);
	void AddFGSE(int FGSEValues[4]);
	void LoadTree();
	void UnloadTree();
	void Refresh();
	void Collapse();
	void Expand();
	void DeleteScripts();
	void WriteScriptorium(CString Filename);
	void GetScripts
		(HTREEITEM hNode, 
		 CStringArray & ScriptList, 
	     CPtrArray & ScriptNodeList, 
		 int & ScriptCount);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptoriumChildFrame)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CScriptoriumChildFrame)
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LONG OnScriptFetch(UINT a, LONG dummy);
	afx_msg LONG OnNodeChanged(UINT a, LONG b);
	afx_msg LONG OnScriptDelete(UINT a, LONG dummy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTORIUMCHILDFRAME_H__87E8A3F1_4B19_11D2_A806_0060B05E3B36__INCLUDED_)

