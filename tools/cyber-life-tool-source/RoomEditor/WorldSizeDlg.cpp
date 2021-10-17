// WorldSizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "WorldSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldSizeDlg dialog


CWorldSizeDlg::CWorldSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorldSizeDlg)
	m_Height = 0;
	m_Width = 0;
	m_MetaroomBase = 0;
	m_RoomBase = 0;
	//}}AFX_DATA_INIT
}


void CWorldSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorldSizeDlg)
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDV_MinMaxInt(pDX, m_Height, 128, 1000000);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	DDV_MinMaxInt(pDX, m_Width, 128, 1000000);
	DDX_Text(pDX, IDC_METAROOM_BASE, m_MetaroomBase);
	DDX_Text(pDX, IDC_ROOM_BASE, m_RoomBase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorldSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CWorldSizeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldSizeDlg message handlers

