#if !defined(AFX_COMBOLIST_H__E4A980C5_C0F5_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_COMBOLIST_H__E4A980C5_C0F5_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboList window

class CComboList;

class CComboListOwner
{
	friend class CComboList;
	virtual void OnChangeSelection( CComboList *list, int selection ) = 0; 
};

class CComboList : public CListBox
{
// Construction
public:
	CComboList();
	BOOL Create( CComboListOwner *owner );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboList)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	CComboListOwner *m_ListOwner;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOLIST_H__E4A980C5_C0F5_11D2_9D30_0090271EEABE__INCLUDED_)

