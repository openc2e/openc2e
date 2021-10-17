#if !defined(AFX_CAPROPERTIESDLG_H__9F6A7DA1_E1CC_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_CAPROPERTIESDLG_H__9F6A7DA1_E1CC_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAPropertiesDlg.h : header file
//

#include "CARates.h"
#include <vector>
#include <set>

/////////////////////////////////////////////////////////////////////////////
// CCAPropertiesDlg dialog

class CCAPropertiesDlg : public CDialog
{
// Construction
public:
	CCAPropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	void SetRoomTypes( std::vector< CString > const &types ) { m_RoomTypes = types; }
	void SetCANames( std::vector< CString > const &names ) { m_CANames = names; }
	void SetCARates( std::vector< std::vector<CCARates> > const &rates ) { m_CARates = rates; }

	std::vector< std::vector<CCARates> > const &GetCARates() const { return m_CARates; }
// Dialog Data
	//{{AFX_DATA(CCAPropertiesDlg)
	enum { IDD = IDD_CA_PROPERTIES };
	CListBox	m_RoomTypeList;
	CListBox	m_CAList;
	CString	m_Diffusion;
	CString	m_Gain;
	CString	m_Loss;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCAPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCAPropertiesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeRoomType();
	afx_msg void OnSelchangeCaProperties();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void StoreSelectedRates();
	std::vector< std::vector<CCARates> > m_CARates;
	std::vector< CString > m_RoomTypes;
	std::vector< CString > m_CANames;
	std::set<int> m_SelectedRoomTypes;
	std::set<int> m_SelectedCAs;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPROPERTIESDLG_H__9F6A7DA1_E1CC_11D2_9D30_0090271EEABE__INCLUDED_)

