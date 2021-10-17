// EditQuickMacros.cpp : implementation file
//

#include "stdafx.h"
#include "caos.h"
#include "EditQuickMacros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditQuickMacros dialog


CEditQuickMacros::CEditQuickMacros(CWnd* pParent /*=NULL*/)
	: CDialog(CEditQuickMacros::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditQuickMacros)
	m_Macro1 = _T("");
	m_Macro2 = _T("");
	m_Macro3 = _T("");
	m_Macro4 = _T("");
	m_Macro5 = _T("");
	m_Macro6 = _T("");
	m_Macro7 = _T("");
	m_Macro8 = _T("");
	m_Macro9 = _T("");
	m_Macro0 = _T("");
	//}}AFX_DATA_INIT
}


void CEditQuickMacros::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditQuickMacros)
	DDX_Text(pDX, IDC_MACRO1, m_Macro1);
	DDX_Text(pDX, IDC_MACRO2, m_Macro2);
	DDX_Text(pDX, IDC_MACRO3, m_Macro3);
	DDX_Text(pDX, IDC_MACRO4, m_Macro4);
	DDX_Text(pDX, IDC_MACRO5, m_Macro5);
	DDX_Text(pDX, IDC_MACRO6, m_Macro6);
	DDX_Text(pDX, IDC_MACRO7, m_Macro7);
	DDX_Text(pDX, IDC_MACRO8, m_Macro8);
	DDX_Text(pDX, IDC_MACRO9, m_Macro9);
	DDX_Text(pDX, IDC_MACRO10, m_Macro0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditQuickMacros, CDialog)
	//{{AFX_MSG_MAP(CEditQuickMacros)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditQuickMacros message handlers

BOOL CEditQuickMacros::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Macro1 = theCAOSApp.GetProfileString("Quick Macros","Macro 1");
	m_Macro2 = theCAOSApp.GetProfileString("Quick Macros","Macro 2");
	m_Macro3 = theCAOSApp.GetProfileString("Quick Macros","Macro 3");
	m_Macro4 = theCAOSApp.GetProfileString("Quick Macros","Macro 4");
	m_Macro5 = theCAOSApp.GetProfileString("Quick Macros","Macro 5");
	m_Macro6 = theCAOSApp.GetProfileString("Quick Macros","Macro 6");
	m_Macro7 = theCAOSApp.GetProfileString("Quick Macros","Macro 7");
	m_Macro8 = theCAOSApp.GetProfileString("Quick Macros","Macro 8");
	m_Macro9 = theCAOSApp.GetProfileString("Quick Macros","Macro 9");
	m_Macro0 = theCAOSApp.GetProfileString("Quick Macros","Macro 0");

	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditQuickMacros::OnDestroy() 
{
	CDialog::OnDestroy();	
}


void CEditQuickMacros::OnCancel() 
{	
	CDialog::OnCancel();
}

void CEditQuickMacros::OnOK() 
{	
	UpdateData(true);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 1",m_Macro1);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 2",m_Macro2);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 3",m_Macro3);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 4",m_Macro4);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 5",m_Macro5);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 6",m_Macro6);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 7",m_Macro7);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 8",m_Macro8);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 9",m_Macro9);
	theCAOSApp.WriteProfileString("Quick Macros","Macro 0",m_Macro0);

	CDialog::OnOK();
}

