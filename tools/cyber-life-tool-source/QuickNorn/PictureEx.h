#if !defined(AFX_PICTUREEX_H__901E1CD5_CBD9_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_PICTUREEX_H__901E1CD5_CBD9_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PictureEx.h : header file
//
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CPictureEx window

class CPictureEx;

class CPictureExClient
{
public:
	virtual void OnPictureClick( CPictureEx *picture, CPoint point ) = 0;
};

class CPictureEx : public CStatic
{
// Construction
public:
	CPictureEx();
	void SetClient( CPictureExClient *client ) { m_Client = client; }
	void SetPoints( std::vector< CPoint > const &points ) {m_Points = points;Invalidate();}
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPictureEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPictureEx)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CPictureExClient *m_Client;
	std::vector<CPoint> m_Points;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTUREEX_H__901E1CD5_CBD9_11D2_9D30_0090271EEABE__INCLUDED_)

