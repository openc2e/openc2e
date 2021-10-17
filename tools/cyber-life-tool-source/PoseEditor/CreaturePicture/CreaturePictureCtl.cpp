// CreaturePictureCtl.cpp : Implementation of the CCreaturePictureCtrl ActiveX Control class.

#include "stdafx.h"
#include "CreaturePicture.h"
#include "CreaturePictureCtl.h"
#include "CreaturePicturePpg.h"
#include "../../../common/WhichEngine.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

std::list< CDisplayerMapEntry > CCreaturePictureCtrl::m_DisplayerMap;


IMPLEMENT_DYNCREATE(CCreaturePictureCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CCreaturePictureCtrl, COleControl)
	//{{AFX_MSG_MAP(CCreaturePictureCtrl)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CCreaturePictureCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CCreaturePictureCtrl)
	DISP_PROPERTY_NOTIFY(CCreaturePictureCtrl, "Scale", m_scale, OnScaleChanged, VT_R4)
	DISP_PROPERTY_NOTIFY(CCreaturePictureCtrl, "Friction", m_friction, OnFrictionChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CCreaturePictureCtrl, "Angle", m_angle, OnAngleChanged, VT_R8)
	DISP_PROPERTY_EX(CCreaturePictureCtrl, "PoseCount", GetPoseCount, SetPoseCount, VT_I2)
	DISP_FUNCTION(CCreaturePictureCtrl, "SetAppearance", SetAppearance, VT_EMPTY, VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2)
	DISP_PROPERTY_PARAM(CCreaturePictureCtrl, "PoseString", GetPoseString, SetPoseString, VT_BSTR, VTS_I2)
	DISP_STOCKPROP_BORDERSTYLE()
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CCreaturePictureCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CCreaturePictureCtrl, COleControl)
	//{{AFX_EVENT_MAP(CCreaturePictureCtrl)
	EVENT_STOCK_CLICK()
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CCreaturePictureCtrl, 1)
	PROPPAGEID(CCreaturePicturePropPage::guid)
END_PROPPAGEIDS(CCreaturePictureCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CCreaturePictureCtrl, "CREATUREPICTURE2.CreaturePicture2Ctrl.1",
	0x843eace9, 0xd2d6, 0x11d2, 0x9d, 0x30, 0, 0x90, 0x27, 0x1e, 0xea, 0xbe)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CCreaturePictureCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DCreaturePicture =
		{ 0x843eace7, 0xd2d6, 0x11d2, { 0x9d, 0x30, 0, 0x90, 0x27, 0x1e, 0xea, 0xbe } };
const IID BASED_CODE IID_DCreaturePictureEvents =
		{ 0x843eace8, 0xd2d6, 0x11d2, { 0x9d, 0x30, 0, 0x90, 0x27, 0x1e, 0xea, 0xbe } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwCreaturePictureOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CCreaturePictureCtrl, IDS_CREATUREPICTURE, _dwCreaturePictureOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::CCreaturePictureCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CCreaturePictureCtrl

BOOL CCreaturePictureCtrl::CCreaturePictureCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_CREATUREPICTURE,
			IDB_CREATUREPICTURE,
			afxRegApartmentThreading,
			_dwCreaturePictureOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

std::vector<int> StringToDescriptor( CString const &poseString )
{
	std::vector<int> poseDescriptor( 15, 0 );
	int PosLen = poseString.GetLength();
	for( int i = 0; i < 15; ++i )
	{
		if( PosLen > i )
		{
			if( isdigit( (unsigned char)(poseString[ i ]) ) )
				poseDescriptor[ i ] = poseString[ i ] - '0';
			else if( poseString[ i ] == 'X' )
				poseDescriptor[ i ] = POSE_NO_CHANGE;
			else if( poseString[ i ] == '?' )
				poseDescriptor[ i ] = POSE_TO_IT;
			else if( poseString[ i ] == '!' )
				poseDescriptor[ i ] = POSE_FROM_IT;
		}
	}
	return poseDescriptor;
}



/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::CCreaturePictureCtrl - Constructor

CCreaturePictureCtrl::CCreaturePictureCtrl() : m_PoseCount( 1 ), m_Prepared( false ), m_scale( 1.0 ), m_angle( 0 ), m_friction( TRUE ), m_Timer( 0 )
{
	InitializeIIDs(&IID_DCreaturePicture, &IID_DCreaturePictureEvents);

/*
	m_ImageDir = "c:\\program files\\creatures 2\\images\\";
	m_BodyDataDir = "c:\\program files\\creatures 2\\body data\\";

	HKEY key1, key2, key3, key4;
	if( RegOpenKey( HKEY_LOCAL_MACHINE, "Software", &key1 ) == ERROR_SUCCESS )
	{
		if( RegOpenKey( key1, "CyberLife Technology", &key2 ) == ERROR_SUCCESS )
		{
			if( RegOpenKey( key2, "c2e", &key3 ) == ERROR_SUCCESS )
			{
				if( RegOpenKey( key3, "0.0", &key4 ) == ERROR_SUCCESS )
				{
					DWORD Res, Type, Len;
					if( RegQueryValueEx( key4, "Images Directory", &Res, &Type, 0, &Len ) == ERROR_SUCCESS )
					{
						RegQueryValueEx( key4, "Images Directory", &Res, &Type, (unsigned char *)m_ImageDir.GetBuffer( Len ), &Len );
						m_ImageDir.ReleaseBuffer();
					}
					if( RegQueryValueEx( key4, "Body Data Directory", &Res, &Type, 0, &Len ) == ERROR_SUCCESS )
					{
						RegQueryValueEx( key4, "Body Data Directory", &Res, &Type, (unsigned char *)m_BodyDataDir.GetBuffer( Len ), &Len );
						m_BodyDataDir.ReleaseBuffer();
					}
					RegCloseKey( key4 );
				}
				RegCloseKey( key3 );
			}
			RegCloseKey( key2 );
		}
		RegCloseKey( key1 );
	}
*/
	WhichEngine which;
	m_ImageDir = which.GetStringKey( "Images Directory" ).c_str();
	m_BodyDataDir = which.GetStringKey( "Body Data Directory" ).c_str();
	m_Displayer = GetDisplayer( m_Appearance );
}

CCreatureDisplayer *CCreaturePictureCtrl::GetDisplayer( CAppearance const &app )
{
	CString Error;
	if( !m_DisplayerMap.size() )
	{
		CCreatureDisplayer *disp = new CCreatureDisplayer();
		disp->Initialize( m_ImageDir, m_BodyDataDir,
		0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0,
		Error );
		CDisplayerMapEntry entry;
		entry.m_Appearance = CAppearance();
		entry.m_Count = 1;
		entry.m_Displayer = handle< CCreatureDisplayer >( disp );
		m_DisplayerMap.push_back( entry );
	}
	std::list< CDisplayerMapEntry >::iterator itor;
	for( itor = m_DisplayerMap.begin(); itor != m_DisplayerMap.end(); ++itor )
	{
		if( itor->m_Appearance == app )
		{
			itor->m_Count++;
			return itor->m_Displayer.GetObj();
		}
	}

	CCreatureDisplayer *disp = new CCreatureDisplayer();
	disp->Initialize(  m_ImageDir, m_BodyDataDir,
		app.m_Head, app.m_Body , app.m_Legs, app.m_Arms, app.m_Tail, app.m_Ears, app.m_Hair,
		app.m_Sex, app.m_Age,
		app.m_HeadVar, app.m_BodyVar , app.m_LegsVar, app.m_ArmsVar, app.m_TailVar, app.m_EarsVar, app.m_HairVar,
		Error );
	CDisplayerMapEntry entry;
	entry.m_Appearance = app;
	entry.m_Count = 1;
	entry.m_Displayer = handle< CCreatureDisplayer >( disp );
	m_DisplayerMap.push_back( entry );
	return disp;
}

void CCreaturePictureCtrl::UnlinkDisplayer( CCreatureDisplayer *pCD )
{
	std::list< CDisplayerMapEntry >::iterator itor;
	for( itor = m_DisplayerMap.begin(); itor != m_DisplayerMap.end(); ++itor )
	{
		if( itor->m_Displayer.GetObj() == pCD )
		{
			itor->m_Count--;
			if( itor->m_Count == 0 )
				m_DisplayerMap.erase( itor );
			return;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::~CCreaturePictureCtrl - Destructor

CCreaturePictureCtrl::~CCreaturePictureCtrl()
{
	UnlinkDisplayer( m_Displayer );
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::OnDraw - Drawing function

void CCreaturePictureCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if( m_hWnd )
	{
		Prepare();
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap( pdc, rcBounds.Width(), rcBounds.Height());

		if( m_PoseCount == 1 )
		{
			if( m_PoseString[0] == "" )
			{
				pdc->FillSolidRect( &rcBounds, RGB( 255, 255, 255 ) );
				return;
			}
			else
				m_Displayer->DisplayPose( rcBounds, &bitmap, StringToDescriptor( m_PoseString[0] ).begin(), m_scale, &m_AD );
		}
		else
			if( m_friction )
				m_Displayer->UpdateWalk( rcBounds, &bitmap, &m_AD );
			else
				m_Displayer->UpdateAnimate( rcBounds, &bitmap, m_angle, &m_AD );

		CDC dc;
		dc.CreateCompatibleDC( pdc );
		CBitmap *oldBitmap = dc.SelectObject( &bitmap );
		pdc->BitBlt( rcBounds.left, rcBounds.top, rcBounds.Width(), rcBounds.Height(), &dc, 0, 0, SRCCOPY );
		dc.SelectObject( oldBitmap );
	}
//	DoSuperclassPaint(pdc, rcBounds);
}

/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::DoPropExchange - Persistence support

void CCreaturePictureCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::OnResetState - Reset control to default state

void CCreaturePictureCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::AboutBox - Display an "About" box to the user

void CCreaturePictureCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_CREATUREPICTURE);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CCreaturePictureCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::IsSubclassedControl - This is a subclassed control

BOOL CCreaturePictureCtrl::IsSubclassedControl()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl::OnOcmCommand - Handle command messages

LRESULT CCreaturePictureCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl message handlers

DWORD CCreaturePictureCtrl::GetControlFlags() 
{
	return COleControl::GetControlFlags();
}

BSTR CCreaturePictureCtrl::GetPoseString(short Index) 
{
	CString strResult;
	if( Index >= 0 && Index <= 7 ) strResult = m_PoseString[Index];

	return strResult.AllocSysString();
}

void CCreaturePictureCtrl::SetPoseString(short Index, LPCTSTR lpszNewValue) 
{
	if( Index >= 0 && Index <= 7 ) m_PoseString[Index] = lpszNewValue;
	m_Prepared = false;
	Invalidate();
	SetModifiedFlag();
}

short CCreaturePictureCtrl::GetPoseCount() 
{
	return m_PoseCount;
}

void CCreaturePictureCtrl::SetPoseCount(short nNewValue) 
{
	if( nNewValue < 1 ) nNewValue = 1;
	if( nNewValue > 8 ) nNewValue = 8;
	m_Prepared = false;
	m_PoseCount = nNewValue;
	Invalidate();
	SetModifiedFlag();
}

void CCreaturePictureCtrl::Prepare() 
{
	if( !m_Prepared )
	{
		if( m_Timer ) KillTimer( IDT_TIMER1 );
		m_Timer = 0;
		if( m_PoseCount > 1 )
		{
			int RoughPoseDescriptors[MAX_ROUGH_POSE_COUNT][POSE_DESCRIPTOR_SIZE];
			for( int i = 0; i < m_PoseCount; ++i )
			{
				std::vector< int > temp = StringToDescriptor( m_PoseString[ i ] );
				for( int j = 0; j < POSE_DESCRIPTOR_SIZE; ++j )
					RoughPoseDescriptors[i][j] = temp[j];
			}
			if( m_friction )
			{
				CRect rect;
				GetClientRect( &rect );
				m_Displayer->PrepareWalk( rect, RoughPoseDescriptors, m_PoseCount, m_scale, m_angle, &m_AD);
			}
			else
				m_Displayer->PrepareAnimate( RoughPoseDescriptors, m_PoseCount, m_scale, &m_AD);
			m_Timer = SetTimer( IDT_TIMER1, 100, 0 );
		}
		m_Prepared = true;
	}
}

void CCreaturePictureCtrl::OnTimer(UINT nIDEvent) 
{
	Invalidate();	
	COleControl::OnTimer(nIDEvent);
}

void CCreaturePictureCtrl::OnDestroy() 
{
	COleControl::OnDestroy();
	
	if( m_Timer ) KillTimer( IDT_TIMER1 );
	
}

void CCreaturePictureCtrl::SetAppearance(short HeadGenus, short HeadVariant,
										 short BodyGenus, short BodyVariant,
										 short LegsGenus, short LegsVariant,
										 short ArmsGenus, short ArmsVariant,
										 short TailGenus, short TailVariant,
										 short EarGenus, short EarVariant,
										 short HairGenus, short HairVariant,
										 short Sex, short Age) 
{
	if( m_Timer ) KillTimer( IDT_TIMER1 );
	m_Timer = 0;
	CAppearance app( Sex, Age, HeadGenus, HeadVariant, BodyGenus, BodyVariant,
		LegsGenus, LegsVariant, ArmsGenus, ArmsVariant, TailGenus, TailVariant,
		EarGenus, EarVariant, HairGenus, HairVariant );
	UnlinkDisplayer( m_Displayer );
	m_Displayer = GetDisplayer( app );
	m_Prepared = false;
	Invalidate();	
}

void CCreaturePictureCtrl::OnScaleChanged() 
{
	Invalidate();
	m_Prepared = false;
	SetModifiedFlag();
}

void CCreaturePictureCtrl::OnFrictionChanged() 
{
	Invalidate();
	m_Prepared = false;
	SetModifiedFlag();
}

void CCreaturePictureCtrl::OnAngleChanged() 
{
	Invalidate();
	m_Prepared = false;
	SetModifiedFlag();
}

