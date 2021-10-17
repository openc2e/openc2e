// NewGaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PoseEditor.h"
#include "NewGaitDlg.h"

#include "PoseEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewGaitDlg dialog


CNewGaitDlg::CNewGaitDlg(CPoseEditorDoc *Doc, CWnd* pParent /*=NULL*/)
	: m_Doc( Doc ), CDialog(CNewGaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewGaitDlg)
	m_GaitName = _T("");
	//}}AFX_DATA_INIT
	m_Gene.m_Type = 2;
	m_Gene.m_SubType = 4;
	m_Gene.m_Generation = 0;
	m_Gene.m_Mutability = 0;
	m_Gene.m_Flags = 0;
	m_Gene.m_Data.resize( 9, 0 );
	m_Gene.m_SwitchOnTime = 0;
}


void CNewGaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewGaitDlg)
	DDX_Text(pDX, IDC_GAIT_NAME, m_GaitName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewGaitDlg, CDialog)
	//{{AFX_MSG_MAP(CNewGaitDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNewGaitDlg message handlers

void CNewGaitDlg::OnOK() 
{
	UpdateData();

	m_Gene.m_ID = m_Doc->GetGenome()->NewID( 2, 4 );
	
	m_Gene.m_Data[0] = m_NewGaitNumber;

	SetPoseDescription( m_NewGaitNumber + 256, std::string( m_GaitName ) );
	m_Doc->AddGene( m_Gene );

	CDialog::OnOK();
}

BOOL CNewGaitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	std::vector<bool> gaitExists( 256, false );

	CGenome *genome = m_Doc->GetGenome();
	for( CGeneIterator itorGait = genome->GeneTypeBegin( 2, 4 );
		itorGait != genome->GeneEnd();
		++itorGait )
		gaitExists[ itorGait->GetObj()->m_Data[0] ] = true;

	m_NewGaitNumber = 256;
	int i;
	for( i = 0; i < 256; ++i )
	{
		if( !gaitExists[i] )
		{
			m_NewGaitNumber = i;
			m_GaitName = GetPoseDescription( i + 256 ).c_str();
			break;
		}
	}
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

