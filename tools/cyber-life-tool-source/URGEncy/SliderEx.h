#if !defined(AFX_SLIDEREX_H__5DAFA0EF_F28C_11D2_A768_0060B07B6A6F__INCLUDED_)
#define AFX_SLIDEREX_H__5DAFA0EF_F28C_11D2_A768_0060B07B6A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SliderEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSliderEx window

class CSliderEx : public CSliderCtrl
{
private:
	CBitmap backmap;
// Construction
public:
	CSliderEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSliderEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSliderEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSliderEx)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDEREX_H__5DAFA0EF_F28C_11D2_A768_0060B07B6A6F__INCLUDED_)

