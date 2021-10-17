// GaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PoseEditor.h"
#include "GaitDlg.h"

#include "PoseEditorDoc.h"
#include "Appearance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGaitDlg dialog


CGaitDlg::CGaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGaitDlg::IDD, pParent), m_Gene( 0 )
{
	//{{AFX_DATA_INIT(CGaitDlg)
	m_Selected = -1;
	m_PoseIndex = -1;
	m_Friction = FALSE;
	//}}AFX_DATA_INIT
	m_Friction = TRUE;
	m_Selected = 0;
}


void CGaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGaitDlg)
	DDX_Control(pDX, IDC_ANGLE, m_Angle);
	DDX_Control(pDX, ID_APPLY, m_Apply);
	DDX_Control(pDX, IDC_POSE, m_PoseList);
	DDX_Radio(pDX, IDC_CAPTION0, m_Selected);
	DDX_CBIndex(pDX, IDC_POSE, m_PoseIndex);
	DDX_Check(pDX, IDC_FRICTION, m_Friction);
	DDX_Control(pDX, IDC_CURRENT, m_CurrentPic);
	DDX_Control(pDX, IDC_ANIMATOR, m_Animator);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PIC0, m_Pic[0]);
	DDX_Control(pDX, IDC_PIC1, m_Pic[1]);
	DDX_Control(pDX, IDC_PIC2, m_Pic[2]);
	DDX_Control(pDX, IDC_PIC3, m_Pic[3]);
	DDX_Control(pDX, IDC_PIC4, m_Pic[4]);
	DDX_Control(pDX, IDC_PIC5, m_Pic[5]);
	DDX_Control(pDX, IDC_PIC6, m_Pic[6]);
	DDX_Control(pDX, IDC_PIC7, m_Pic[7]);
}


BEGIN_MESSAGE_MAP(CGaitDlg, CDialog)
	//{{AFX_MSG_MAP(CGaitDlg)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_POSE, OnSelchangePose)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_FRICTION, OnFriction)
	ON_EN_CHANGE(IDC_ANGLE, OnChangeAngle)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CAPTION0, OnChangeSel)
	ON_BN_CLICKED(IDC_CAPTION1, OnChangeSel)
	ON_BN_CLICKED(IDC_CAPTION2, OnChangeSel)
	ON_BN_CLICKED(IDC_CAPTION3, OnChangeSel)
	ON_BN_CLICKED(IDC_CAPTION4, OnChangeSel)
	ON_BN_CLICKED(IDC_CAPTION5, OnChangeSel)
	ON_BN_CLICKED(IDC_CAPTION6, OnChangeSel)
	ON_BN_CLICKED(IDC_CAPTION7, OnChangeSel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGaitDlg message handlers

BOOL CGaitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText( GetPoseDescription( m_GeneCopy.m_Data[0] + 256 ).c_str() );
	CAppearance const &app = m_Doc->GetAppearance();
	UpdateAppearance( app );

	UpdatePoseList();
	for( int i = 0; i < 8; ++i )
		m_Pic[i].SetScale( 0.3 );
	UpdatePics();
	OnChangeSel();
	m_Angle.SetWindowText( "0" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HGene CGaitDlg::NearestPose( BYTE pose )
{
	HGene gene(0);
	int BestAge = 100;
	int Age = m_Doc->GetAppearance().m_Age;
	CGenome *genome = m_Doc->GetGenome();

	for( CGeneIterator itorPose = genome->GeneTypeBegin( 2, 3 );
		itorPose != genome->GeneEnd();
		++itorPose )
		if( itorPose->GetObj()->m_Data[0] == pose )
		{
			BYTE thisAge = itorPose->GetObj()->m_SwitchOnTime;
			if( thisAge == Age || (BestAge > Age && thisAge < BestAge) || ( thisAge > BestAge && thisAge < Age ) )
			{
				BestAge = thisAge;
				gene = *itorPose;
			}
		}
	return gene;
}

void CGaitDlg::OnClose() 
{
	m_Doc->CloseGaitDlg( this );	
}

void CGaitDlg::OnCancel() 
{
	m_Doc->CloseGaitDlg( this );	
}

void CGaitDlg::OnOK() 
{
	OnApply();
	m_Doc->CloseGaitDlg( this );	
}

void CGaitDlg::OnChangeSel() 
{
	UpdateData();
	m_CurrentPic.SetPoseString( 0, m_Pic[ m_Selected ].GetPoseString( 0 ) );
	std::vector< BYTE >::iterator itor =
		std::find( m_PoseNums.begin(), m_PoseNums.end(), m_GeneCopy.m_Data[m_Selected + 1] );
	if( itor != m_PoseNums.end() )
		m_PoseIndex = itor - m_PoseNums.begin();
	UpdateData( FALSE );
}

BEGIN_EVENTSINK_MAP(CGaitDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CGaitDlg)
	ON_EVENT(CGaitDlg, IDC_PIC0, -600 /* Click */, OnClickPic0, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGaitDlg::OnClickPic0() 
{
	// TODO: Add your control notification handler code here
	
}

void CGaitDlg::UpdatePoseList()
{
	std::vector< bool > poseExists( 256, false );

	CGenome *genome = m_Doc->GetGenome();
	CGeneIterator itorPose = genome->GeneTypeBegin( 2, 3 );

	for( itorPose = genome->GeneTypeBegin( 2, 3 );
		itorPose != genome->GeneEnd();
		++itorPose )
		poseExists[ itorPose->GetObj()->m_Data[0] ] = true;

	m_PoseNums.clear();
	m_PoseList.ResetContent();
	for( int i = 0; i < 256; ++i )
		if( poseExists[i] )
		{
			m_PoseNums.push_back( i );
			m_PoseList.AddString( GetPoseDescription( i ).c_str() );
		}

}

void CGaitDlg::OnSelchangePose() 
{
	UpdateData();
	m_GeneCopy.m_Data[ m_Selected + 1 ] = m_PoseNums[ m_PoseIndex ];
	UpdatePics();
	m_Apply.EnableWindow( TRUE );
}

void CGaitDlg::UpdatePics() 
{
	int nPoses = 0;
	for( int i = 0; i < 8; ++i )
	{
		if( m_GeneCopy.m_Data[i + 1] )
		{
			HGene gene = NearestPose( m_GeneCopy.m_Data[i + 1] );
			if( gene.GetObj() )
			{
				CString poseString;
				for( int j = 1; j < 16; ++j )
					poseString += gene->m_Data[j];
				m_Pic[i].SetPoseString( 0, poseString );
				m_Animator.SetPoseString( i, poseString );
				++nPoses;
			}
		}
		else
			m_Pic[i].SetPoseString( 0, "" );
	}
	m_Animator.SetPoseCount( nPoses );
}

void CGaitDlg::UpdateAppearance( CAppearance const &app )
{
	for( int i = 0; i < 8; ++i )
		m_Pic[i].SetAppearance( app.m_Head, app.m_HeadVar,
			app.m_Body, app.m_BodyVar, 
			app.m_Legs, app.m_LegsVar, 
			app.m_Arms, app.m_ArmsVar, 
			app.m_Tail, app.m_TailVar,
			app.m_Ears, app.m_EarsVar,
			app.m_Hair, app.m_HairVar,
			app.m_Sex, app.m_Age );

	m_Animator.SetAppearance( app.m_Head, app.m_HeadVar,
		app.m_Body, app.m_BodyVar, 
		app.m_Legs, app.m_LegsVar, 
		app.m_Arms, app.m_ArmsVar, 
		app.m_Tail, app.m_TailVar,
		app.m_Ears, app.m_EarsVar,
		app.m_Hair, app.m_HairVar,
		app.m_Sex, app.m_Age );
	m_CurrentPic.SetAppearance( app.m_Head, app.m_HeadVar,
		app.m_Body, app.m_BodyVar, 
		app.m_Legs, app.m_LegsVar, 
		app.m_Arms, app.m_ArmsVar, 
		app.m_Tail, app.m_TailVar,
		app.m_Ears, app.m_EarsVar,
		app.m_Hair, app.m_HairVar,
		app.m_Sex, app.m_Age );
	UpdatePics();
	OnChangeSel();
}

void CGaitDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CString text;
	m_Angle.GetWindowText( text );
	int angle = int( atof( text ) );

	if( pNMUpDown->iDelta == -1 )
		angle -= 1;
	else if( pNMUpDown->iDelta == 1 )
		angle += 1;
	text.Format( "%d", angle );
	m_Angle.SetWindowText( text );
	m_Animator.SetAngle( double( angle ) );


	*pResult = 0;
}

void CGaitDlg::OnFriction() 
{
	UpdateData();
	m_Animator.SetFriction( m_Friction );
}

void CGaitDlg::OnChangeAngle() 
{
	CString text;
	m_Angle.GetWindowText( text );
	double angle = atof( text );
	m_Animator.SetAngle( angle );
}

void CGaitDlg::OnRemove() 
{
	for( int i = m_Selected; i < 7; ++i )
		m_GeneCopy.m_Data[ i + 1 ] = m_GeneCopy.m_Data[ i + 2 ];
	m_GeneCopy.m_Data[7] = 0;
	UpdatePics();
	OnChangeSel();
	m_Apply.EnableWindow( TRUE );
}

void CGaitDlg::OnAdd() 
{
	for( int i = 7; i > m_Selected; --i )
		m_GeneCopy.m_Data[ i + 1 ] = m_GeneCopy.m_Data[ i ];
	UpdatePics();
	OnChangeSel();
	m_Apply.EnableWindow( TRUE );
}

void CGaitDlg::OnApply() 
{
	m_Doc->ModifyGene( m_GeneCopy );
	m_Apply.EnableWindow( FALSE );
}

void CGaitDlg::Refresh( CGene const &gene )
{
	UpdatePics();
	OnChangeSel();
}

