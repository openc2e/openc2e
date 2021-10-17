// NewPoseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PoseEditor.h"
#include "NewPoseDlg.h"
#include "Gene.h"
#include "PoseEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewPoseDlg dialog


CNewPoseDlg::CNewPoseDlg( CPoseEditorDoc *Doc, CWnd* pParent /*=NULL*/)
	: m_Doc( Doc ), CDialog(CNewPoseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewPoseDlg)
	m_SwitchOn = -1;
	m_Pose = -1;
	m_PoseOpt = -1;
	m_PoseName = _T("");
	//}}AFX_DATA_INIT
	m_SwitchOn = 0;
	m_Pose = 0;

	m_Gene.m_Type = 2;
	m_Gene.m_SubType = 3;
	m_Gene.m_Generation = 0;
	m_Gene.m_Mutability = 0;
	m_Gene.m_Flags = 0;
	m_Gene.m_ExpressionVariant = 0;
	m_Gene.m_Data.resize( 16, '0' );


}


void CNewPoseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewPoseDlg)
	DDX_Control(pDX, IDC_POSE_OPT, m_PoseOptButton);
	DDX_Control(pDX, IDC_POSE_NAME, m_PoseEdit);
	DDX_Control(pDX, IDC_AGE, m_AgeList);
	DDX_Control(pDX, IDC_POSE, m_PoseList);
	DDX_CBIndex(pDX, IDC_AGE, m_SwitchOn);
	DDX_CBIndex(pDX, IDC_POSE, m_Pose);
	DDX_Radio(pDX, IDC_POSE_OPT, m_PoseOpt);
	DDX_Text(pDX, IDC_POSE_NAME, m_PoseName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewPoseDlg, CDialog)
	//{{AFX_MSG_MAP(CNewPoseDlg)
	ON_CBN_SELCHANGE(IDC_POSE, OnSelchangePose)
	ON_BN_CLICKED(IDC_POSE_OPT, OnPoseOpt)
	ON_BN_CLICKED(IDC_POSE_OPT2, OnPoseOpt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPoseDlg message handlers

BOOL CNewPoseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	std::vector<bool> poseExists( 256, false );

	CGenome *genome = m_Doc->GetGenome();
	for( CGeneIterator itorPose = genome->GeneTypeBegin( 2, 3 );
		itorPose != genome->GeneEnd();
		++itorPose )
		poseExists[ itorPose->GetObj()->m_Data[0] ] = true;

	m_Pose = 255;
	int i;
	for( i = 0; i < 256; ++i )
	{
		if( poseExists[i] )
		{
			m_PoseList.AddString( GetPoseDescription( i ).c_str() );
			m_PoseNumbers.push_back( i );
		}
	}
	m_NewPoseNumber = 256;
	for( i = 0; i < 256; ++i )
	{
		if( !poseExists[i] )
		{
			m_NewPoseNumber = i;
			m_PoseName = GetPoseDescription( i ).c_str();
			break;
		}
	}
	if( m_NewPoseNumber == 256 )
	{
		m_PoseOptButton.EnableWindow( FALSE );
		m_PoseOpt = 1;
	}
	else
		m_PoseOpt = 0;
	UpdateData( FALSE );
	OnPoseOpt();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewPoseDlg::OnOK() 
{
	UpdateData();
	CGenome *genome = m_Doc->GetGenome();
	if( m_PoseOpt )	m_NewPoseNumber = m_PoseNumbers[ m_Pose ];
	else SetPoseDescription( m_NewPoseNumber, std::string( m_PoseName ) );

	for( CGeneIterator itorPose = genome->GeneTypeBegin( 2, 3 );
		itorPose != genome->GeneEnd();
		++itorPose )
		if( itorPose->GetObj()->m_Data[0] == m_NewPoseNumber && itorPose->GetObj()->m_SwitchOnTime == m_SwitchOn )
		{
			MessageBox( "Pose already exists with this switch on time.", "Error" );
			return;
		}

	m_Gene.m_ID = genome->NewID( 2, 3 );
	
	m_Gene.m_Data[0] = m_NewPoseNumber;
	m_Gene.m_SwitchOnTime = m_SwitchOn;

	m_Doc->AddGene( m_Gene );
	CDialog::OnOK();
}

void CNewPoseDlg::OnSelchangePose() 
{
	UpdateData();

	std::vector<bool> poseExists( 7, false );

	if( m_PoseOpt )
	{
		CGenome *genome = m_Doc->GetGenome();
		for( CGeneIterator itorPose = genome->GeneTypeBegin( 2, 3 );
			itorPose != genome->GeneEnd();
			++itorPose )
			if( itorPose->GetObj()->m_Data[0] == m_Pose ) poseExists[ itorPose->GetObj()->m_SwitchOnTime ] = true;
	}
	m_AgeList.ResetContent();
	for( int i = 0; i < 7; ++i )
	{
		CString entry;
		if( poseExists[ i ] ) entry = "* ";
		entry += CGene::GetAgeName( i ).c_str();
		m_AgeList.AddString( entry );
	}
	UpdateData( FALSE );
}

void CNewPoseDlg::OnPoseOpt() 
{
	UpdateData();
	m_PoseEdit.EnableWindow( m_PoseOpt == 0 );
	m_PoseList.EnableWindow( m_PoseOpt == 1 );
	OnSelchangePose();
}

