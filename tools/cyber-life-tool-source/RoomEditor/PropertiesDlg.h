#if !defined(AFX_PROPERTIESDLG_H__50A20525_BC1B_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_PROPERTIESDLG_H__50A20525_BC1B_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesDlg dialog

#include <vector>
#include <set>

#include "PropertyType.h"
#include "handle.h"
#include "EmptyChildDlg.h"

class CC2ERoomEditorView;

class CPropertiesDlg : public CDialog
{
// Construction
public:
	CPropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	void ReflectSelection( CC2ERoomEditorView *view );

// Dialog Data
	//{{AFX_DATA(CPropertiesDlg)
	enum { IDD = IDD_PROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// m_CtlParent must appear before m_Ctls (so that is is destroyed last)
	CEmptyChildDlg m_CtlParent;
	std::vector< handle< CWnd > > m_Ctls;
	std::set< int > m_UsedProperties;

	void UpdatePropertyList( std::vector< CPropertyType > const &types );
	void SizeCtlParent();
	// Generated message map functions
	//{{AFX_MSG(CPropertiesDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESDLG_H__50A20525_BC1B_11D2_9D30_0090271EEABE__INCLUDED_)

