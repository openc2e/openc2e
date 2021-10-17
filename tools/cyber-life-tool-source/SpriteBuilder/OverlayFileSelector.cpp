// OverlayFileSelector.cpp : implementation file
//

#include "stdafx.h"
#include "spritebuilder.h"
#include "OverlayFileSelector.h"
#include "SpriteBuilderDoc.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OverlayFileSelector dialog


OverlayFileSelector::OverlayFileSelector(CWnd* pParent /*=NULL*/)
	: CDialog(OverlayFileSelector::IDD, pParent)
{
	//{{AFX_DATA_INIT(OverlayFileSelector)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void OverlayFileSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OverlayFileSelector)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OverlayFileSelector, CDialog)
	//{{AFX_MSG_MAP(OverlayFileSelector)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OverlayFileSelector message handlers

void OverlayFileSelector::OnOK() 
{
	// TODO: Add extra validation here
	CString array("abcdefghijklmnopq");

	UINT id = IDC_CHECK1;

	for(int i =0; i < 17; i++,id++)
	{
		AddFile(id, array.GetAt(i));
	}
//	if(myDoc)
//		myDoc->SetSelection(mySelection);
	CDialog::OnOK();
}


void OverlayFileSelector::AddFile(UINT id, CString letter)
{
	if(((CButton*)GetDlgItem(id))->GetCheck() == 1)
	{
		mySelection.Add(letter);
	}
}

