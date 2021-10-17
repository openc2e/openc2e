// CreaturePicturePpg.cpp : Implementation of the CCreaturePicturePropPage property page class.

#include "stdafx.h"
#include "CreaturePicture.h"
#include "CreaturePicturePpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCreaturePicturePropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CCreaturePicturePropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CCreaturePicturePropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CCreaturePicturePropPage, "CREATUREPICTURE.CreaturePicturePropPage.1",
	0x843eacea, 0xd2d6, 0x11d2, 0x9d, 0x30, 0, 0x90, 0x27, 0x1e, 0xea, 0xbe)


/////////////////////////////////////////////////////////////////////////////
// CCreaturePicturePropPage::CCreaturePicturePropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CCreaturePicturePropPage

BOOL CCreaturePicturePropPage::CCreaturePicturePropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CREATUREPICTURE_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePicturePropPage::CCreaturePicturePropPage - Constructor

CCreaturePicturePropPage::CCreaturePicturePropPage() :
	COlePropertyPage(IDD, IDS_CREATUREPICTURE_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CCreaturePicturePropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePicturePropPage::DoDataExchange - Moves data between page and properties

void CCreaturePicturePropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CCreaturePicturePropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePicturePropPage message handlers

