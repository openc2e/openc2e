//{{AFX_INCLUDES()
#include "creaturepic2.h"
//}}AFX_INCLUDES
#if !defined(AFX_GAITDLG_H__4AFB1724_ABB7_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_GAITDLG_H__4AFB1724_ABB7_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GaitDlg.h : header file
//

class CPoseEditorDoc;
#include "genome.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CGaitDlg dialog

class CAppearance;

class CGaitDlg : public CDialog
{
// Construction
public:
	CGaitDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CPoseEditorDoc *doc, HGene const &gene, CWnd* pParent = NULL)
	{
		m_Doc = doc;
		m_Gene = gene;
		m_GeneCopy = *gene;
		return CDialog::Create(IDD_GAIT, pParent);
	}
	HGene GetGene()
	{
		return m_Gene;
	}
	HGene NearestPose( BYTE pose );
	void UpdatePoseList();
	void UpdatePics();
	void UpdateAppearance( CAppearance const &app );

	void Refresh( CGene const &gene );


// Dialog Data
	//{{AFX_DATA(CGaitDlg)
	enum { IDD = IDD_GAIT };
	CEdit	m_Angle;
	CButton	m_Apply;
	CComboBox	m_PoseList;
	int		m_Selected;
	int		m_PoseIndex;
	BOOL	m_Friction;
	CCreaturePicture2	m_CurrentPic;
	CCreaturePicture2	m_Animator;
	//}}AFX_DATA

	CCreaturePicture2 m_Pic[8];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGaitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGaitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClickPic0();
	afx_msg void OnSelchangePose();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFriction();
	afx_msg void OnChangeAngle();
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	afx_msg void OnApply();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg void OnChangeSel();
	DECLARE_MESSAGE_MAP()
	CPoseEditorDoc *m_Doc;
	HGene m_Gene;
	CGene m_GeneCopy;
	std::vector< BYTE > m_PoseNums;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAITDLG_H__4AFB1724_ABB7_11D2_9D30_0090271EEABE__INCLUDED_)

