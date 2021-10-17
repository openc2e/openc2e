#if !defined(AFX_BLUERADIO_H__09C2A9C8_AC71_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_BLUERADIO_H__09C2A9C8_AC71_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlueRadio.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlueRadio window

class CBlueRadio : public CButton
{
// Construction
public:
	CBlueRadio();
	void Init( UINT id, CWnd *wnd );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlueRadio)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBlueRadio();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBlueRadio)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_Bitmap;
	CBitmap m_Bitmap2;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLUERADIO_H__09C2A9C8_AC71_11D2_9D30_0090271EEABE__INCLUDED_)

