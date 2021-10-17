// QuickNornDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuickNorn.h"
#include "QuickNornDlg.h"
#include "DirDlg.h"
#include "NornConfig.h"

#include <fstream>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickNornDlg dialog

CQuickNornDlg::CQuickNornDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickNornDlg::IDD, pParent), m_DataLoaded( false )
{
	//{{AFX_DATA_INIT(CQuickNornDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Picture.SetClient( this );

	ClearData();
}

void CQuickNornDlg::ClearData()
{
	int part, set, position;

	m_Points.clear();
	m_Points.resize( CNornConfig::NumParts() );
	for( part = 0; part < CNornConfig::NumParts(); ++part )
	{
		m_Points[part].resize( CNornConfig::NumSets( part ) );
		for( set = 0; set < CNornConfig::NumSets( part ); ++set )
		{
			m_Points[part][set].resize( CNornConfig::NumPositions() );
			for( position = 0; position < CNornConfig::NumPositions(); ++position )
				m_Points[part][set][position].clear();
		}
	}
}

void CQuickNornDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickNornDlg)
	DDX_Control(pDX, IDC_SET, m_SetCombo);
	DDX_Control(pDX, IDC_DIRECTION_LIST, m_DirectionList);
	DDX_Control(pDX, IDC_BODY_PART_LIST, m_BodyPartList);
	DDX_Control(pDX, IDC_POINT_LIST, m_PointList);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CQuickNornDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickNornDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DIR, OnDir)
	ON_LBN_SELCHANGE(IDC_BODY_PART_LIST, OnSelchangeBodyPartList)
	ON_LBN_SELCHANGE(IDC_DIRECTION_LIST, OnSelchangeDirectionList)
	ON_CBN_SELCHANGE(IDC_SET, OnSelchangeSet)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_AUTO_ONE, OnAutoOne)
	ON_BN_CLICKED(IDC_AUTO_PART, OnAutoPart)
	ON_BN_CLICKED(IDC_AUTO_ALL, OnAutoAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickNornDlg message handlers

BOOL CQuickNornDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString title = "Quick Norn - ";
	title += GameName;
	title += " version";
	SetWindowText(title);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	int i;
	for( i = 0; i < CNornConfig::NumParts(); ++i )
		m_BodyPartList.AddString( CNornConfig::GetPartName(i) );
	for( i = 0; i < CNornConfig::NumPositions(); ++i )
		m_DirectionList.AddString( CNornConfig::GetPositionName(i) );

	{
	#ifdef ADVENTURES
		char const *keyName = "Software\\CyberLife Technology\\Creatures Adventures";
	#else
		char const *keyName = "Software\\CyberLife Technology\\Creatures 3";
	#endif
		char const *valueName = "Body Data Directory";

		HKEY key;
		if( RegOpenKey( HKEY_LOCAL_MACHINE, keyName, &key ) == ERROR_SUCCESS )
		{
			DWORD Res, Type, Len;
			if( RegQueryValueEx( key, valueName, &Res, &Type, 0, &Len ) == ERROR_SUCCESS )
			{
				
				RegQueryValueEx( key, valueName, &Res, &Type, (unsigned char *)m_AttatchmentDir.GetBuffer( Len ), &Len );
				m_AttatchmentDir.ReleaseBuffer();
			}
		}
		RegCloseKey(key);
	}

	{
		char const *keyName = "Software\\CyberLife Technology\\QuickNorn";
		char const *valueName = "Bitmaps Directory";

		HKEY key;
		LONG ret = RegOpenKey( HKEY_CURRENT_USER, keyName, &key );
		if( ret == ERROR_SUCCESS )
		{
			DWORD Res, Type, Len;
			if( RegQueryValueEx( key, valueName, &Res, &Type, 0, &Len ) == ERROR_SUCCESS )
			{
				
				RegQueryValueEx( key, valueName, &Res, &Type, (unsigned char *)m_BitmapDir.GetBuffer( Len ), &Len );
				m_BitmapDir.ReleaseBuffer();
			}
		}
		RegCloseKey(key);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CQuickNornDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQuickNornDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CQuickNornDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CQuickNornDlg::OnDir() 
{
	CDirDlg dlg;
	dlg.m_Bitmaps = m_BitmapDir;
	dlg.m_BodyData = m_AttatchmentDir;
	dlg.m_Suffix = m_Suffix;
	if( dlg.DoModal() == IDOK )
	{
		m_BitmapDir = dlg.m_Bitmaps;
		m_AttatchmentDir = dlg.m_BodyData;

		// store bitmap directory in registry
		{
			char const *keyName = "Software\\CyberLife Technology\\QuickNorn";
			char const *valueName = "Bitmaps Directory";

			HKEY key;
			int result = RegOpenKey(HKEY_CURRENT_USER,keyName,&key);
			if (result != ERROR_SUCCESS)
				result = RegCreateKey(HKEY_CURRENT_USER,keyName,&key);
			if( result == ERROR_SUCCESS )
			{
				DWORD Res, Type;
				Type = REG_EXPAND_SZ;
				Res = 0;
				RegSetValueEx( key, valueName, Res, Type, (const unsigned char *)(LPCTSTR)m_BitmapDir, m_BitmapDir.GetLength() );
			}
			else
				ASSERT(false);
			RegCloseKey(key);
		}

		m_Suffix = dlg.m_Suffix;
		ClearData();
		for( int part = 0; part < CNornConfig::NumParts(); ++part )
		{
			if( CNornConfig::NumSets( part ) )
				ReadAttatchmentFile( part, m_Points[part][0] );
		}
		m_DataLoaded = true;
	}

}

void CQuickNornDlg::ReadAttatchmentFile( int part,
										std::vector< std::vector< CPoint  > > &points )
{
	CString Filename = m_AttatchmentDir +
		CNornConfig::BodyID( part ) + m_Suffix + ".att";
	std::ifstream file( Filename );
	if( file )
	{
		int position = 0;
		while( !file.eof() && position < CNornConfig::NumPositions() )
		{
			std::string line;
			std::getline( file, line );
			std::istringstream stream( line );
			CPoint point;
			while( stream >> point.x >> point.y )
			{
				points[ position ].push_back( point );
			}
			++position;
		}
	}
}

void CQuickNornDlg::WriteAttatchmentFile( int part,
										std::vector< std::vector< CPoint  > > &points )
{
	CString Filename = m_AttatchmentDir +
		CNornConfig::BodyID( part ) + m_Suffix + ".att";
	std::ofstream file( Filename );
	if( file )
	{
		for( int position = 0; position < CNornConfig::NumPositions(); ++position )
		{
			int n = points[ position ].size();
			for( int i = 0; i < n; ++i )
			{
				file << points[ position ][ i ].x << ' ' <<
					points[ position ][ i ].y << ' ';
			}
			file << '\n';
		}
	}
}

void CQuickNornDlg::OnSelchangeBitmapDir() 
{
}

void CQuickNornDlg::OnPictureClick( CPictureEx *picture, CPoint point )
{
	CString temp;
	temp.Format( "%d %d", point.x, point.y );
	m_PointList.AddString( temp );
	m_Points[m_Part][m_Set][m_Position].push_back( point );
}

void CQuickNornDlg::ShowPicture()
{
	m_Part = m_BodyPartList.GetCurSel();
	m_Position = m_DirectionList.GetCurSel();
	m_Set = m_SetCombo.GetCurSel();
	int maxSet = CNornConfig::NumSets( m_Part ) - 1;
	if( m_Set < 0 ) m_Set = 0;
	if( m_Set > maxSet ) m_Set = maxSet;

	if( m_Part != -1 && m_Position != -1 && m_Set != -1 )
	{
		CString filename = m_BitmapDir + "\\" + CNornConfig::GetFilename( m_Part, m_Position, m_Set );
		HANDLE handleBmp = LoadImage( 0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
		DeleteObject( m_Picture.SetBitmap( (HBITMAP)handleBmp ) );
		m_PointList.ResetContent();
		std::vector< CPoint > points;
		for( int i = 0; i < m_Points[m_Part][m_Set][m_Position].size(); ++i )
		{
			CString temp;
			temp.Format( "%d %d", m_Points[m_Part][m_Set][m_Position][i].x,
				m_Points[m_Part][m_Set][m_Position][i].y );
			m_PointList.AddString( temp );
			points.push_back( m_Points[m_Part][m_Set][m_Position][i] );
		}
		m_Picture.SetPoints( points );

		// Get the pixel data (just don't ask!)

		HBITMAP aBitmap = (HBITMAP) LoadImage( 0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );

		BITMAP info;
		GetObject(aBitmap, sizeof(info), &info);

		CBitmap* cbmp = CBitmap::FromHandle(aBitmap);

		m_Width = info.bmWidth;
		m_PaddedWidth = m_Width * 3;
		if (m_PaddedWidth % 2 == 1)
			++m_PaddedWidth;
	
		m_Height = info.bmHeight;
		m_PixelData.resize(m_PaddedWidth * m_Height, -1);
		DWORD res = cbmp->GetBitmapBits(m_PixelData.size(), &m_PixelData[0]);

		DeleteObject(aBitmap);
	}
}

void CQuickNornDlg::OnSelchangeBodyPartList() 
{
	ShowPicture();
}

void CQuickNornDlg::OnSelchangeDirectionList() 
{
	ShowPicture();
}

void CQuickNornDlg::OnSelchangeSet() 
{
	ShowPicture();
}

void CQuickNornDlg::OnSave() 
{
	for( int part = 0; part < CNornConfig::NumParts(); ++part )
	{
		if( CNornConfig::NumSets( part ) )
			WriteAttatchmentFile( part, m_Points[part][0] );
	}
	m_DataLoaded = true;
}

void CQuickNornDlg::OnClear() 
{
	if( m_Part != -1 && m_Position != -1 && m_Set != -1)
		m_Points[m_Part][m_Set][m_Position].clear();
	ShowPicture();
}

void CQuickNornDlg::OnCancel() 
{
	CDialog::OnCancel();
}

// otherwise return quits the dialog
void CQuickNornDlg::OnOK() 
{
}

int CQuickNornDlg::PixelB(int x, int y) 
{
	return m_PixelData[x * 3 + m_PaddedWidth * y];
}

int CQuickNornDlg::PixelG(int x, int y) 
{
	return m_PixelData[x * 3 + 1 + m_PaddedWidth * y];
}

int CQuickNornDlg::PixelR(int x, int y) 
{
	return m_PixelData[x * 3 + 2 + m_PaddedWidth * y];
}

void CQuickNornDlg::OnAutoOne() 
{
	if( m_Part == -1 || m_Position == -1 )
		return;

	CWaitCursor waitCursor;	
	OnClear();

	int nAPs = CNornConfig::NumAPs(m_Part);
	for (int ap = 0; ap < nAPs; ++ap)
	{
		COLOUR col = (COLOUR)CNornConfig::AttachPointColour(m_Part, ap);
		ASSERT(col != -1);
		CPoint point = FindColourPixel(col);
		if (point == CPoint(-1, -1))
		{
			// couldn't find obvious colour...

			if (CString(CNornConfig::GetPartName(m_Part)) == "Body")
			{
				// sideways body parts definitely overlap, so catch those cases
				if (col == YELLOW)
					col = MAGENTA;
				else if (col == MAGENTA)
					col = YELLOW;
				else if (col == CYAN)
					col = BLUE;
				else if (col == BLUE)
					col = CYAN;
				// sometimes from front on, tail and neck overlap
				else if (col == RED)
					col = GREEN;
				else if (col == GREEN)
					col = RED;

				point = FindColourPixel(col);
			}
			else if (nAPs == 2 || CNornConfig::AttachPointColour(m_Part, 2) == PADDING)
			{
				// sometimes other parts overlap, so we catch the least case we
				// know about that easily covers this

				point = FindColourPixel(ANY);
				if (point != CPoint(-1, -1))
					::MessageBox(NULL, "Warning: Main colour not found, but only two points and arbitary colour found.\nProbably just an overlap clash, but please check.", "CQuickNornDlg::OnAutoOne", MB_OK);
			}

			if (point == CPoint(-1, -1))
			{
				::MessageBox(NULL, "Failed to find coloured pixel", "CQuickNornDlg::OnAutoOne", MB_OK);
				return;
			}
		}
		OnPictureClick( NULL, point );
	}
}

CPoint CQuickNornDlg::FindColourPixel(COLOUR col)
{
	if (col == PADDING)
		return CPoint(0,0);

	// values above this count as coloured pixels
	const int threshold = 10;
	CPoint brightestPoint(0, 0);
	int brightestBrightness = -1;

	int seekR = ourColours[col].R;
	int seekG = ourColours[col].G;
	int seekB = ourColours[col].B;
	for (int i = 0; i < m_Height; ++i)
	{
		for (int j = 0; j < m_Width; ++j)
		{
			int R = PixelR(j, i);
			int G = PixelG(j, i);
			int B = PixelB(j, i);

			if (
				(col == ANY && (R >= threshold || G >= threshold || B >= threshold)) ||	
				(((seekR > 0 && R >= threshold) || (seekR == 0 && R < threshold))
				&& ((seekG > 0 && G >= threshold) || (seekG == 0 && G < threshold))
					&& ((seekB > 0 && B >= threshold) || (seekB == 0 && B < threshold)))
				)
			{
				int brightness = R + G + B;
				if (brightness > brightestBrightness)
				{
					brightestBrightness = brightness;
					brightestPoint = CPoint(j, i);
				}
			}
		}
	}

	if (brightestBrightness == -1)
		return CPoint(-1, -1);

	return brightestPoint;
}

void CQuickNornDlg::OnAutoPart() 
{
	if( m_Part == -1 )
		return;

	for (int pos = 0; pos < CNornConfig::NumPositions(); ++pos)
	{
		m_DirectionList.SetCurSel(pos);
		ShowPicture();
		OnAutoOne();
	}
}

void CQuickNornDlg::OnAutoAll() 
{
	for (int part = 0; part < CNornConfig::NumParts(); ++part)
	{
		m_BodyPartList.SetCurSel(part);
		ShowPicture();
		OnAutoPart();
	}	
}

