#if !defined(AFX_NEWPOSEDLG_H__09C2A9C5_AC71_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_NEWPOSEDLG_H__09C2A9C5_AC71_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewPoseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewPoseDlg dialog

#include <vector>
#include "gene.h"

class CPoseEditorDoc;

class CNewPoseDlg : public CDialog
{
// Construction
public:
	CNewPoseDlg( CPoseEditorDoc *Doc, CWnd* pParent = NULL);   // standard constructor

	void SetGene( CGene const &gene ) { m_Gene = gene; }
// Dialog Data
	//{{AFX_DATA(CNewPoseDlg)
	enum { IDD = IDD_NEW_POSE };
	CButton	m_PoseOptButton;
	CEdit	m_PoseEdit;
	CComboBox	m_AgeList;
	CComboBox	m_PoseList;
	int		m_SwitchOn;
	int		m_Pose;
	int		m_PoseOpt;
	CString	m_PoseName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewPoseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewPoseDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangePose();
	afx_msg void OnPoseOpt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPoseEditorDoc *m_Doc;
	std::vector< int > m_PoseNumbers;
	int m_NewPoseNumber;
	CGene m_Gene;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPOSEDLG_H__09C2A9C5_AC71_11D2_9D30_0090271EEABE__INCLUDED_)

