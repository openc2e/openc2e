#if !defined(AFX_ROOMPROPERTIESDLG_H__EA9AFA61_C044_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_ROOMPROPERTIESDLG_H__EA9AFA61_C044_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RoomPropertiesDlg.h : header file
//

#include <vector>
#include <set>

#include "PropertyType.h"
#include "handle.h"

#include "PropertyListBox.h"
#include "ComboList.h"

class CC2ERoomEditorView;

/////////////////////////////////////////////////////////////////////////////
// CRoomPropertiesDlg dialog

class CRoomPropertiesDlg : public CDialog, CPropertyListBoxOwner, CComboListOwner
{
// Construction
public:
	CRoomPropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	void ReflectSelection( CC2ERoomEditorView *view, std::vector< CPropertyType > const &types );
	CC2ERoomEditorView const *GetView() const { return m_View; }

// Dialog Data
	//{{AFX_DATA(CRoomPropertiesDlg)
	enum { IDD = IDD_ROOM_PROPERTIES };
	CButton	m_DropButton;
	CEdit	m_Edit;
	CPropertyListBox	m_PropertyList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdatePropertyList( std::vector< CPropertyType > const &types );

	// Generated message map functions
	//{{AFX_MSG(CRoomPropertiesDlg)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeEdit();
	afx_msg void OnKillfocusEdit();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	std::vector< CPropertyType > m_Types;
//	std::vector< CString > m_Values;
	std::vector< int > m_IntValues;
	std::vector< bool > m_Nulls;
	virtual void OnListScroll( CPropertyListBox *list ); 
	virtual void OnChangeSelection( CComboList *list, int selection ); 
	CComboList m_ComboList;
	CString GetEnumName( int index, int enumVal );
	CString GetValueString( int index );
	void SetValueString( int index, CString newValue );
	CC2ERoomEditorView *m_View;
	bool m_Updating;
	void SetSelectionProperty();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROOMPROPERTIESDLG_H__EA9AFA61_C044_11D2_9D30_0090271EEABE__INCLUDED_)

