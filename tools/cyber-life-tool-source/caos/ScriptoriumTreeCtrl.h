////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SCRIPTORIUMTREECTRL_H__08CCDB91_5158_11D2_A807_0060B05E3B36__INCLUDED_)
#define AFX_SCRIPTORIUMTREECTRL_H__08CCDB91_5158_11D2_A807_0060B05E3B36__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScriptoriumTreeCtrl.h : header file
//

//
// Extra messages
//

// Sent to parent when the Enter key is pressed over a node
#define WM_SCRIPT_FETCH		(WM_USER+4)

// Sent to parent when the Delete key is pressed over a node
#define WM_SCRIPT_DELETE	(WM_USER+5)

// Sent to parent when the selection moves to another node
#define WM_NODE_CHANGED		(WM_USER+6)



/////////////////////////////////////////////////////////////////////////////
// CScriptoriumTreeCtrl window

class CScriptoriumTreeCtrl : public CTreeCtrl
{
public:
	CScriptoriumTreeCtrl();
	virtual ~CScriptoriumTreeCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptoriumTreeCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CScriptoriumTreeCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNodeChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTORIUMTREECTRL_H__08CCDB91_5158_11D2_A807_0060B05E3B36__INCLUDED_)

