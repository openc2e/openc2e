#if !defined(AFX_NEWGAITDLG_H__6F1EA1C1_B5CC_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_NEWGAITDLG_H__6F1EA1C1_B5CC_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewGaitDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewGaitDlg dialog
#include "Gene.h"

class CPoseEditorDoc;

class CNewGaitDlg : public CDialog
{
// Construction
public:
	CNewGaitDlg( CPoseEditorDoc *Doc, CWnd* pParent = NULL);   // standard constructor
	void SetGene( CGene const &gene ) { m_Gene = gene; }
// Dialog Data
	//{{AFX_DATA(CNewGaitDlg)
	enum { IDD = IDD_NEW_GAIT };
	CString	m_GaitName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewGaitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewGaitDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_NewGaitNumber;
	CPoseEditorDoc *m_Doc;
	CGene m_Gene;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWGAITDLG_H__6F1EA1C1_B5CC_11D2_9D30_0090271EEABE__INCLUDED_)

