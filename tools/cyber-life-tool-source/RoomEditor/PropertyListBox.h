#if !defined(AFX_PROPERTYLISTBOX_H__E6B95525_C0CD_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_PROPERTYLISTBOX_H__E6B95525_C0CD_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox window
class CPropertyListBox;

class CPropertyListBoxOwner
{
	friend class CPropertyListBox;
	virtual void OnListScroll( CPropertyListBox *list ) = 0; 
};


class CPropertyListBox : public CListBox
{
// Construction
public:
	CPropertyListBox();
	void SetListOwner( CPropertyListBoxOwner *owner ) { m_ListOwner = owner; }
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyListBox)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	 CPropertyListBoxOwner *m_ListOwner;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLISTBOX_H__E6B95525_C0CD_11D2_9D30_0090271EEABE__INCLUDED_)

