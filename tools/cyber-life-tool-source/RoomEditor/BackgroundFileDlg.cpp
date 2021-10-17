// BackgroundFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "BackgroundFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackgroundFileDlg dialog


CBackgroundFileDlg::CBackgroundFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBackgroundFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBackgroundFileDlg)
	m_Source = _T("");
	m_Max = 0;
	m_Min = 0;
	m_Preview = FALSE;
	m_Sprite = FALSE;
	//}}AFX_DATA_INIT
	m_Preview = TRUE;
	m_Sprite = TRUE;
}


void CBackgroundFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackgroundFileDlg)
	DDX_Control(pDX, IDC_MIN, m_MinEdit);
	DDX_Control(pDX, IDC_MAX, m_MaxEdit);
	DDX_Text(pDX, IDC_SOURCE, m_Source);
	DDX_Text(pDX, IDC_MAX, m_Max);
	DDV_MinMaxInt(pDX, m_Max, 0, 255);
	DDX_Text(pDX, IDC_MIN, m_Min);
	DDV_MinMaxInt(pDX, m_Min, 0, 255);
	DDX_Check(pDX, IDC_MAKE_PREVIEW_FILE, m_Preview);
	DDX_Check(pDX, IDC_MAKE_SPRITE_FILE, m_Sprite);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackgroundFileDlg, CDialog)
	//{{AFX_MSG_MAP(CBackgroundFileDlg)
	ON_BN_CLICKED(IDC_MAKE_PREVIEW_FILE, OnMakePreviewFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackgroundFileDlg message handlers

void CBackgroundFileDlg::OnMakePreviewFile() 
{
	UpdateData();
	m_MinEdit.EnableWindow( m_Preview );
	m_MaxEdit.EnableWindow( m_Preview );
	
}

