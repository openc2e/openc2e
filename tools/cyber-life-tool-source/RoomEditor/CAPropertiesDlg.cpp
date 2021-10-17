// CAPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "CAPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCAPropertiesDlg dialog


CCAPropertiesDlg::CCAPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCAPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCAPropertiesDlg)
	m_Diffusion = _T("");
	m_Gain = _T("");
	m_Loss = _T("");
	//}}AFX_DATA_INIT
}


void CCAPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCAPropertiesDlg)
	DDX_Control(pDX, IDC_ROOM_TYPE, m_RoomTypeList);
	DDX_Control(pDX, IDC_CA_PROPERTIES, m_CAList);
	DDX_Text(pDX, IDC_DIFFUSION, m_Diffusion);
	DDX_Text(pDX, IDC_GAIN, m_Gain);
	DDX_Text(pDX, IDC_LOSS, m_Loss);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCAPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CCAPropertiesDlg)
	ON_LBN_SELCHANGE(IDC_ROOM_TYPE, OnSelchangeRoomType)
	ON_LBN_SELCHANGE(IDC_CA_PROPERTIES, OnSelchangeCaProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCAPropertiesDlg message handlers

BOOL CCAPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i;
	for( i = 0; i < m_RoomTypes.size(); ++i ) m_RoomTypeList.AddString( m_RoomTypes[i] );
	for( i = 0; i < m_CANames.size(); ++i ) m_CAList.AddString( m_CANames[i] );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCAPropertiesDlg::OnSelchangeRoomType() 
{
	OnSelchangeCaProperties();	
}

void CCAPropertiesDlg::OnSelchangeCaProperties() 
{
	StoreSelectedRates();

	int i;

	m_SelectedRoomTypes.clear();
	for( i = 0; i < m_RoomTypes.size(); ++i )
		if( m_RoomTypeList.GetSel( i ) ) m_SelectedRoomTypes.insert( i );
	
	m_SelectedCAs.clear();
	for( i = 0; i < m_CANames.size(); ++i )
		if( m_CAList.GetSel( i ) ) m_SelectedCAs.insert( i );

	m_Gain = "";
	m_Loss = "";
	m_Diffusion = "";
	if( m_SelectedRoomTypes.size() && m_SelectedCAs.size() )
	{
		int rt = *m_SelectedRoomTypes.begin();
		int ca = *m_SelectedCAs.begin();
		float gain = m_CARates[ rt ][ ca ].GetGain();
		float loss = m_CARates[ rt ][ ca ].GetLoss();
		float diff = m_CARates[ rt ][ ca ].GetDiffusion();
		for( std::set< int >::const_iterator i = m_SelectedRoomTypes.begin();
			i != m_SelectedRoomTypes.end(); ++i )
			for( std::set< int >::const_iterator j = m_SelectedCAs.begin();
				j != m_SelectedCAs.end(); ++j )
				{
					if( gain != m_CARates[ *i ][ *j ].GetGain() ) gain = -1;
					if( loss != m_CARates[ *i ][ *j ].GetLoss() ) loss = -1;
					if( diff != m_CARates[ *i ][ *j ].GetDiffusion() ) diff = -1;
				}
		if( gain >= 0 ) m_Gain.Format( "%.4f", gain );
		if( loss >= 0 ) m_Loss.Format( "%.4f", loss );
		if( diff >= 0 ) m_Diffusion.Format( "%.4f", diff );
	}
	UpdateData(FALSE);

}

void CCAPropertiesDlg::StoreSelectedRates()
{
	UpdateData();
	if( m_Gain != "" )
	{
		float gain = float( atof( m_Gain ) );
		for( std::set< int >::const_iterator i = m_SelectedRoomTypes.begin();
			i != m_SelectedRoomTypes.end(); ++i )
			for( std::set< int >::const_iterator j = m_SelectedCAs.begin();
				j != m_SelectedCAs.end(); ++j )
				m_CARates[ *i ][ *j ].SetGain( gain );
	}
	if( m_Loss != "" )
	{
		float loss = float( atof( m_Loss ) );
		for( std::set< int >::const_iterator i = m_SelectedRoomTypes.begin();
			i != m_SelectedRoomTypes.end(); ++i )
			for( std::set< int >::const_iterator j = m_SelectedCAs.begin();
				j != m_SelectedCAs.end(); ++j )
				m_CARates[ *i ][ *j ].SetLoss( loss );
	}
	if( m_Diffusion != "" )
	{
		float diffusion = float( atof( m_Diffusion ) );
		for( std::set< int >::const_iterator i = m_SelectedRoomTypes.begin();
			i != m_SelectedRoomTypes.end(); ++i )
			for( std::set< int >::const_iterator j = m_SelectedCAs.begin();
				j != m_SelectedCAs.end(); ++j )
				m_CARates[ *i ][ *j ].SetDiffusion( diffusion );
	}
}

void CCAPropertiesDlg::OnOK() 
{
	OnSelchangeCaProperties();	
	CDialog::OnOK();
}

