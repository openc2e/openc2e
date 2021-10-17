#if !defined(AFX_OVERLAYFILESELECTOR_H__3D1D0641_F436_11D3_B79C_00105A4CB2BF__INCLUDED_)
#define AFX_OVERLAYFILESELECTOR_H__3D1D0641_F436_11D3_B79C_00105A4CB2BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OverlayFileSelector.h : header file
//

#include  <Afxcoll.h> 
class SpriteBuilderDoc;

/////////////////////////////////////////////////////////////////////////////
// OverlayFileSelector dialog

class OverlayFileSelector : public CDialog
{
// Construction
public:
	OverlayFileSelector(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(OverlayFileSelector)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OverlayFileSelector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	public:
	CStringArray& GetSelection(){return mySelection;}

	void SetDoc(SpriteBuilderDoc* doc){myDoc = doc;}
	void AddFile(UINT id, CString letter);
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OverlayFileSelector)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CStringArray mySelection;

	SpriteBuilderDoc* myDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERLAYFILESELECTOR_H__3D1D0641_F436_11D3_B79C_00105A4CB2BF__INCLUDED_)

