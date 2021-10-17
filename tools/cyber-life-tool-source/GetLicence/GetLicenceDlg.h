// GetLicenceDlg.h : header file
//

#if !defined(AFX_GETLICENCEDLG_H__FF144D27_3D56_11D5_A26B_005004A81EDC__INCLUDED_)
#define AFX_GETLICENCEDLG_H__FF144D27_3D56_11D5_A26B_005004A81EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGetLicenceDlg dialog

class CGetLicenceDlg : public CDialog
{
// Construction
public:
	CGetLicenceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGetLicenceDlg)
	enum { IDD = IDD_GETLICENCE_DIALOG };
	CEdit	m_editkeycheck;
	CEdit	m_editkeyname;
	CEdit	m_editkeyserial;
	CEdit	m_editname;
	CEdit	m_editkeyprod;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetLicenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	bool isLicenceFormatCorrect(CString &keyprod, CString &keyname, CString &keyserial, CString &keycheck);
	void setRegForProduct(CString &completekey, CString &username);
	void setRegForInstaller();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGetLicenceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeEditName();
	afx_msg void OnChangeEditKeyProd();
	afx_msg void OnChangeEditKeyCheck();
	afx_msg void OnChangeEditKeyName();
	afx_msg void OnChangeEditKeySerial();
	virtual void OnOK();
	afx_msg void OnButtonhelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETLICENCEDLG_H__FF144D27_3D56_11D5_A26B_005004A81EDC__INCLUDED_)
