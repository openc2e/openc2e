// BiochemistrySetDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "BiochemistrySetDlg.h"
#include "GrapherDlg.h"
#include "LoggerDlg.h"
#include "ParametersDlg.h"
#include "FavouriteName.h"
#include "InjectionDlg.h"
#include "OrganDlg.h"
#include "../../common/WindowState.h"
#include "../../common/WhichEngine.h"

#include <shlwapi.h> // path manipulation

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
// CBiochemistrySetDlg dialog

CFavouriteClass CBiochemistrySetDlg::favclass;

CBiochemistrySetDlg::CBiochemistrySetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBiochemistrySetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBiochemistrySetDlg)
	//}}AFX_DATA_INIT
	m_hBigIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hSmallIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME_SMALL);
}

void CBiochemistrySetDlg::DoDataExchange(CDataExchange* pDX)
{
	base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBiochemistrySetDlg)
	DDX_Control(pDX, IDC_DELETE_FAV, m_ctrlDeleteFav);
	DDX_Control(pDX, IDC_OPEN_FAV, m_ctrlOpenFav);
	DDX_Control(pDX, IDC_FAV_COMBO, m_ctrlFavouriteCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBiochemistrySetDlg, CDialog)
	//{{AFX_MSG_MAP(CBiochemistrySetDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GRAPH, OnGraph)
	ON_BN_CLICKED(IDC_ORGAN, OnOrgan)
	ON_BN_CLICKED(IDC_LOG, OnLog)
	ON_WM_INITMENUPOPUP()
	ON_BN_CLICKED(IDC_OPEN_FAV, OnOpenFav)
	ON_BN_CLICKED(IDC_DELETE_FAV, OnDeleteFav)
	ON_CBN_SELCHANGE(IDC_FAV_COMBO, OnSelchangeFavCombo)
	ON_BN_CLICKED(IDC_INJECT, OnInject)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBiochemistrySetDlg message handlers

CString GetModulePath(HMODULE hModule)
{
	CString strModulePath;

	TCHAR pszModuleFilename[_MAX_PATH];
	if (GetModuleFileName(hModule, pszModuleFilename, _MAX_PATH))
	{
		PathRemoveFileSpec(pszModuleFilename);
		strModulePath = pszModuleFilename;
	}

	return strModulePath;
}

BOOL CBiochemistrySetDlg::OnInitDialog()
{
	base::OnInitDialog();

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

	SetIcon(m_hBigIcon, TRUE);			// Set big icon
	SetIcon(m_hSmallIcon, FALSE);		// Set small icon
		
	// Add game name to title
	char old_title[256];
	GetWindowText(old_title, 255);
	std::string title = old_title;
	title += " - ";
	title += theWhichEngine.GameName();
	SetWindowText(title.c_str());

	favclass.Load(GetWindowConfigFilename());
	RefreshFavouritesList();
	if (m_ctrlFavouriteCombo.GetCount() > 0)
		m_ctrlFavouriteCombo.SetCurSel(0);

	RefreshEnabling();

	WindowState::Load("Windows", "Main", m_hWnd, true);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBiochemistrySetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		base::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBiochemistrySetDlg::OnPaint() 
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
		dc.DrawIcon(x, y, m_hBigIcon);
	}
	else
	{
		base::OnPaint();
	}
}

HCURSOR CBiochemistrySetDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hBigIcon;
}

// Add COMMAND_UI handling to pulldown
void CBiochemistrySetDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu) 
{
	base::OnInitMenuPopup(pMenu, nIndex, bSysMenu);	

	if (bSysMenu)
		return;
	
	bool bAutoMenuEnable = true;
	ASSERT(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = GetTopLevelParent();
			// child windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, bAutoMenuEnable && state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CBiochemistrySetDlg::OnGraph() 
{
	CDialogSpec spec;
	spec.type = CDialogSpec::GRAPH_DIALOG;
	QueryChemWindow(spec, true);
}

void CBiochemistrySetDlg::OnOrgan() 
{
	CDialogSpec spec;
	spec.type = CDialogSpec::ORGAN_DIALOG;
	spec.nSampleRate = ORGAN_DEFAULT_SAMPLE_RATE;
	QueryChemWindow(spec, true);
}

void CBiochemistrySetDlg::OnLog() 
{
	CDialogSpec spec;
	spec.type = CDialogSpec::LOG_DIALOG;
	QueryChemWindow(spec, true);
}

void CBiochemistrySetDlg::OnInject() 
{
	CDialogSpec spec;
	spec.type = CDialogSpec::INJECTION_DIALOG;
	spec.nSampleRate = INJECTION_DEFAULT_SAMPLE_RATE;
	QueryChemWindow(spec, true);
}

void CBiochemistrySetDlg::QueryChemWindow(const CDialogSpec& spec, bool bResetSize, bool bNoQuery)
{
	CDialogSpec localspec = spec;
	CString window_type = spec.TypeString();

	if (bResetSize)
	{
		// Note: These rectangles are measured in dialog units
		UINT id;
		if (localspec.type == CDialogSpec::GRAPH_DIALOG)
			id = IDD_GRAPHER_DIALOG;
		else if (localspec.type == CDialogSpec::INJECTION_DIALOG)
			id = IDD_INJECTION_DIALOG;
		else if (localspec.type == CDialogSpec::LOG_DIALOG)
			id = IDD_LOG_DIALOG;
		else if (localspec.type == CDialogSpec::ORGAN_DIALOG)
			id = IDD_ORGAN_DIALOG;

		CDialog test_size_dlg;
		test_size_dlg.Create(id, NULL);
		CRect rect;
		test_size_dlg.GetWindowRect(rect);
		//	CRect oldrect = rect;
		UnMapDialogRect(rect);
		int width = rect.Width();
		int height = rect.Height();
		//	OurMapDialogRect(rect);
		//	ASSERT(rect == oldrect); // fires too often due to off by one errors

		localspec.rectPosition.right = localspec.rectPosition.left + width;
		localspec.rectPosition.bottom = localspec.rectPosition.top + height;
	}
		
	bool bOK = true;

	if (!bNoQuery)
	{
		CParametersDlg dlgParams(_T("New ") + window_type + _T(" Window"), localspec);
		bOK = (dlgParams.DoModal() == IDOK);
	}

	if (bOK)
		CreateChemWindow(localspec);
}

bool CBiochemistrySetDlg::QueryFilenameIfNecessary(CDialogSpec& localspec)
{
	if (localspec.type == CDialogSpec::LOG_DIALOG)
	{
		CFileDialog dlgFile(false, _T("csv"), localspec.filename.c_str(), OFN_OVERWRITEPROMPT, _T("Comma Separated Values (*.csv)|*.csv|All Files (*.*)|*.*||"));
		if (dlgFile.DoModal() == IDCANCEL)
			return false;
		localspec.filename = dlgFile.GetPathName();
	}
	return true;
}

void CBiochemistrySetDlg::CreateChemWindow(const CDialogSpec& spec)
{
	CDialogSpec localspec = spec;
	localspec.name = localspec.MakeName();
	if (!QueryFilenameIfNecessary(localspec))
		return;

	if (localspec.type == CDialogSpec::GRAPH_DIALOG)
	{
		CGrapherDlg* pGrapher = new CGrapherDlg;
		if (!pGrapher->Create(IDD_GRAPHER_DIALOG))
		{
			ASSERT(false);
			TRACE(_T("Failed to create grapher dialog\n"));
			delete pGrapher;
			return;
		}
		pGrapher->ChangeSpec(localspec);
		pGrapher->ShowWindow(SW_SHOW);
	}
	else if (localspec.type == CDialogSpec::LOG_DIALOG)
	{
		CLoggerDlg* pLogger = new CLoggerDlg;
		if (!pLogger->Create(IDD_LOG_DIALOG))
		{
			ASSERT(false);
			TRACE(_T("Failed to create logger dialog\n"));
			delete pLogger;
			return;
		}
		pLogger->ChangeSpec(localspec);
		pLogger->ShowWindow(SW_SHOW);
	}
	else if (localspec.type == CDialogSpec::INJECTION_DIALOG)
	{
		CInjectionDlg* pInjecter = new CInjectionDlg;
		if (!pInjecter->Create(IDD_INJECTION_DIALOG))
		{
			ASSERT(false);
			TRACE(_T("Failed to create injection dialog\n"));
			delete pInjecter;
			return;
		}
		pInjecter->ChangeSpec(localspec);
		pInjecter->ShowWindow(SW_SHOW);
	}
	else if (localspec.type == CDialogSpec::ORGAN_DIALOG)
	{
		COrganDlg* pOrganer = new COrganDlg;
		if (!pOrganer->Create(IDD_ORGAN_DIALOG))
		{
			ASSERT(false);
			TRACE(_T("Failed to create organ dialog\n"));
			delete pOrganer;
			return;
		}
		pOrganer->ChangeSpec(localspec);
		pOrganer->ShowWindow(SW_SHOW);
	}
	else
		ASSERT(false);
}

void CBiochemistrySetDlg::AddFavouriteSpec(const CDialogSpec& spec)
{
	CFavouriteName dlgName;
	dlgName.m_sName = spec.name.c_str();
	if (dlgName.DoModal() == IDOK)
	{
		CDialogSpec localspec = spec;
		localspec.name = dlgName.m_sName;
		if (localspec.name.empty())
			localspec.name = _T("<unnamed>");
		favclass.Add(localspec);
		RefreshFavouritesList();
		m_ctrlFavouriteCombo.SetCurSel(m_ctrlFavouriteCombo.GetCount() - 1);
		RefreshEnabling();
	}
}

void CBiochemistrySetDlg::RefreshFavouritesList()
{
	m_ctrlFavouriteCombo.ResetContent();

	int n = favclass.size();
	for (int i = 0; i < n; ++i)
	{
		CDialogSpec localspec = favclass.GetSpec(i);
		m_ctrlFavouriteCombo.AddString(localspec.name.c_str());
	}
	ASSERT(m_ctrlFavouriteCombo.GetCount() == favclass.size());
	RefreshEnabling();

	favclass.Save(GetWindowConfigFilename());
}

CString CBiochemistrySetDlg::GetWindowConfigFilename()
{
	return GetModulePath(GetModuleHandle(NULL)) + "\\BiochemWindows.cfg";
}

void CBiochemistrySetDlg::OnOpenFav() 
{
	ASSERT(m_ctrlFavouriteCombo.GetCount() == favclass.size());
	int sel = m_ctrlFavouriteCombo.GetCurSel();
	if (sel != LB_ERR)
	{
		ASSERT(sel >= 0 && sel < favclass.size());
		CreateChemWindow(favclass.GetSpec(sel));
	}		
}

void CBiochemistrySetDlg::OnDeleteFav() 
{
	ASSERT(m_ctrlFavouriteCombo.GetCount() == favclass.size());
	int sel = m_ctrlFavouriteCombo.GetCurSel();
	if (sel != LB_ERR)
	{
		ASSERT(sel >= 0 && sel < favclass.size());
		CString message;
		message.Format(_T("Delete saved window \"%s\"?"), favclass.GetSpec(sel).name.c_str());
		if (MessageBox(message, _T("Biochemistry Set"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			favclass.Delete(sel);
			RefreshFavouritesList();
			if (m_ctrlFavouriteCombo.GetCount() > 0)
				m_ctrlFavouriteCombo.SetCurSel(0);
			RefreshEnabling();
		}
	}	
}

void CBiochemistrySetDlg::OnSelchangeFavCombo() 
{	
	RefreshEnabling();
}

void CBiochemistrySetDlg::RefreshEnabling()
{
	int sel = m_ctrlFavouriteCombo.GetCurSel();
	bool bEnable = (sel != LB_ERR);
	m_ctrlOpenFav.EnableWindow(bEnable);
	m_ctrlDeleteFav.EnableWindow(bEnable);
}

// otherwise esc quits the dialog
void CBiochemistrySetDlg::OnCancel() 
{
}

// otherwise return quits the dialog
void CBiochemistrySetDlg::OnOK() 
{
}

void CBiochemistrySetDlg::OnClose() 
{
	WindowState::Save("Windows", "Main", m_hWnd);
		
	// destory ourselves, as we've stopped OnCancel
	DestroyWindow();
}

