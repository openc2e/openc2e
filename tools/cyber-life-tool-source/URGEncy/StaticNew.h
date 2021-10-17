#if !defined(AFX_STATICNEW_H__06413061_D8F4_11D1_9529_0060B07BEC58__INCLUDED_)
#define AFX_STATICNEW_H__06413061_D8F4_11D1_9529_0060B07BEC58__INCLUDED_
#define _STATICNEW_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StaticNew.h : header file
//

//
// A CStatic extension.
//
//
// Features:
//
// - Draws text using TRANSPARENT mode (so can be used over fancy bitmap backgrounds).
// - Allows for an icon to be drawn alongside the text (either left or right).
//
// TODO:
//
// Change drawing to allow text centering, right justified text etc...
// (should use existing CStatic window flags)
//


/////////////////////////////////////////////////////////////////////////////
// CStaticNew window

class CStaticNew : public CStatic
{
// Construction
public:
	CStaticNew();

// Attributes
public:

// Operations
public:
	enum{ iconRight, iconLeft };

	void SetIconImage( UINT nIDResource, int iPositioning=iconRight );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticNew)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticNew();

protected:
	int m_iIconPosition;
	HICON	m_hIcon;
	int		m_iIconW;
	int		m_iIconH;

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticNew)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICNEW_H__06413061_D8F4_11D1_9529_0060B07BEC58__INCLUDED_)

