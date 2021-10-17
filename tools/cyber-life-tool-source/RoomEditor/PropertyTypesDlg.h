#if !defined(AFX_PROPERTYTYPESDLG_H__E0028AA1_C008_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_PROPERTYTYPESDLG_H__E0028AA1_C008_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyTypesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyTypesDlg dialog

#include <vector>
#include "PropertyType.h"

class CPropertyTypesDlg : public CDialog
{
// Construction
public:
	CPropertyTypesDlg( std::vector< CPropertyType > const &types, CWnd* pParent = NULL);   // standard constructor

	std::vector< CPropertyType > const &GetPropertyTypes() {return m_Types;}
// Dialog Data
	//{{AFX_DATA(CPropertyTypesDlg)
	enum { IDD = IDD_PROPERY_TYPES };
	CListBox	m_TypesList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyTypesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertyTypesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkPropertyTypesList();
	afx_msg void OnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString GetListString( int index );
	std::vector< CPropertyType > m_Types;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYTYPESDLG_H__E0028AA1_C008_11D2_9D30_0090271EEABE__INCLUDED_)

