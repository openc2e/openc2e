#if !defined(AFX_SWITCHMETAROOMDLG_H__1F46AD7B_DABF_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_SWITCHMETAROOMDLG_H__1F46AD7B_DABF_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SwitchMetaroomDlg.h : header file
//

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CSwitchMetaroomDlg dialog

class CWorld;

class CSwitchMetaroomDlg : public CDialog
{
// Construction
public:
	CSwitchMetaroomDlg(CWorld *world, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSwitchMetaroomDlg)
	enum { IDD = IDD_SWITCH_METAROOM };
	CComboBox	m_Metaroom;
	CComboBox	m_Background;
	BOOL	m_DisplayMap;
	int		m_Metacomboindex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwitchMetaroomDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSwitchMetaroomDlg)
	virtual void OnOK();
	afx_msg void OnApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMetaroom();
	//}}AFX_MSG
	std::vector<int> myMetaLookup;
	DECLARE_MESSAGE_MAP()
	void UpdateBackgrounds( int index ) ;
	CWorld *m_World;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWITCHMETAROOMDLG_H__1F46AD7B_DABF_11D2_9D30_0090271EEABE__INCLUDED_)

