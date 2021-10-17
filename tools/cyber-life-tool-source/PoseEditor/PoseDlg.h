//{{AFX_INCLUDES()
#include "creaturepic2.h"
//}}AFX_INCLUDES
#if !defined(AFX_POSEDLG_H__FC4A1FE0_A95B_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_POSEDLG_H__FC4A1FE0_A95B_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PoseDlg.h : header file
//
class CPoseEditorDoc;
#include "genome.h"
#include "BlueRadio.h"

/////////////////////////////////////////////////////////////////////////////
// CPoseDlg dialog
class CAppearance;

class CPoseDlg : public CDialog
{
// Construction
public:
	CPoseDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CPoseEditorDoc *doc, HGene const &gene, CWnd* pParent = NULL)
	{
		m_Doc = doc;
		m_Gene = gene;
		return CDialog::Create(IDD_POSE, pParent);
	}
	HGene GetGene()
	{
		return m_Gene;
	}

	void Refresh();
	void UpdateAppearance( CAppearance const &app );
	CString m_PoseString;
// Dialog Data
	//{{AFX_DATA(CPoseDlg)
	enum { IDD = IDD_POSE };
	CEdit	m_StringCtl;
	CButton	m_Apply;
	CCreaturePicture2	m_CreaturePic;
	CString	m_String;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPoseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int	m_Part[15];
	CPoseEditorDoc *m_Doc;
	HGene m_Gene;
	std::list< handle< CBlueRadio > > m_RadioButtons;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSEDLG_H__FC4A1FE0_A95B_11D2_9D30_0090271EEABE__INCLUDED_)

