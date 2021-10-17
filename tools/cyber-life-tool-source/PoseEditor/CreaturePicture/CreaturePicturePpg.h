#if !defined(AFX_CREATUREPICTUREPPG_H__11DF2DE2_A937_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_CREATUREPICTUREPPG_H__11DF2DE2_A937_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CreaturePicturePpg.h : Declaration of the CCreaturePicturePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CCreaturePicturePropPage : See CreaturePicturePpg.cpp.cpp for implementation.

class CCreaturePicturePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CCreaturePicturePropPage)
	DECLARE_OLECREATE_EX(CCreaturePicturePropPage)

// Constructor
public:
	CCreaturePicturePropPage();

// Dialog Data
	//{{AFX_DATA(CCreaturePicturePropPage)
	enum { IDD = IDD_PROPPAGE_CREATUREPICTURE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CCreaturePicturePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATUREPICTUREPPG_H__11DF2DE2_A937_11D2_9D30_0090271EEABE__INCLUDED)

