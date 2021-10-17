// URGEncyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "URGEncy.h"
#include "URGEncyDlg.h"

#include <string>
#include "../../common/WhichEngine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MINIMX 0
#define MINIMY 0

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
// CURGEncyDlg dialog

CURGEncyDlg::CURGEncyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CURGEncyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CURGEncyDlg)
	m_Meaning = _T("");
	m_NornThought = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	gInterface.SetDynConnect(true);
	std::string catpath,aLang;

	catpath = theWhichEngine.CatalogueDir();

	aLang = "en";
	try
	{
		cat.AddDir(catpath, aLang);
	}
	catch ( ... )
	{
		AfxMessageBox("Warning, Could not load the catalogue. Aborting load");
		exit(0);
	}
}

void CURGEncyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CURGEncyDlg)
	DDX_Control(pDX, IDC_PAUSED, m_PausedButton);
	DDX_Control(pDX, IDC_PAUSE, m_PauseButton);
	DDX_Control(pDX, IDC_TITLEBAR, m_Titlebar);
	DDX_Control(pDX, IDC_VERBCOMBO, m_VerbCombo);
	DDX_Control(pDX, IDC_NOUNCOMBO, m_NounCombo);
	DDX_Control(pDX, IIDC_NOUNSLIDER, m_NounSlider);
	DDX_Control(pDX, IIDC_VERBSLIDER, m_VerbSlider);
	DDX_Text(pDX, IDC_DISPLAYMEANING, m_Meaning);
	DDX_Text(pDX, IDC_NORNTHOUGHT, m_NornThought);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CURGEncyDlg, CDialog)
	//{{AFX_MSG_MAP(CURGEncyDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_VERBCOMBO, OnSelchangeVerbcombo)
	ON_CBN_SELCHANGE(IDC_NOUNCOMBO, OnSelchangeNouncombo)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDM_ABOUTBOX, OnAboutbox)
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_EXECUTE, OnExecute)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_QUITBUTTON, OnQuitbutton)
	ON_BN_CLICKED(IDC_MINIMBUTTON, OnMinimbutton)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_PAUSED, OnPause)
	ON_BN_CLICKED(IDC_EMERGENCY, OnEmergency)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CURGEncyDlg message handlers

BOOL CURGEncyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString title;
	title = "URGEncy (c)1999 CyberLife Technology Ltd";
	SetWindowText(title);

	//And now cock up calculations dramatically, by linking our loverly region into
	//the window...

	CRect rec;
	GetWindowRect(&rec);
	displayregion = CreateRoundRectRgn(0,0,rec.Width(),rec.Height(),20,20);
	SetWindowRgn(displayregion,false);

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
	
	// TODO: Add extra initialization here

	//Hmm, now we perhaps must initialise the sliders.

	m_NounSlider.SetRange(0,101);
	m_NounSlider.SetPageSize(10);
	m_NounSlider.SetLineSize(1);
	m_VerbSlider.SetRange(0,101);
	m_VerbSlider.SetPageSize(10);
	m_VerbSlider.SetLineSize(1);

	//Next we initialise the verb combo with the verbs from the verbslist

#ifdef _OLDSTYLELOAD
	CStdioFile fil;
	fil.Open("c:\\verbs.txt",CFile::typeText | CFile::modeRead);
	int i;
	for(i=0;i<15;i++)
	{
		CString astr;
		fil.ReadString(astr);
		m_VerbCombo.AddString(astr);
	}
	fil.Close();
	fil.Open("c:\\nouns.txt",CFile::typeText | CFile::modeRead);
	for(i=0;i<51;i++)
	{
		CString astr;
		fil.ReadString(astr);
		m_NounCombo.AddString(astr);
	}
	fil.Close();
#else

	//New style load into the code...

	m_VerbCombo.AddString("No action urged");
	m_NounCombo.AddString("No attention urged");

	std::string group;
	int nums = 0,loop = 0;
	group = "Creature Actions";

	nums = cat.GetArrayCountForTag(group);

	for(loop=0;loop<nums;loop++)
	{
		CString meh;
		std::string res;
		res = cat.Get(group,loop);
		meh = res.c_str();
		m_VerbCombo.AddString(meh);
	}

	group = "Agent Categories";
	nums = cat.GetArrayCountForTag(group);
	for(loop=0;loop<nums;loop++)
	{
		CString meh;
		std::string res;
		res = cat.Get(group,loop);
		meh = res.c_str();
		m_NounCombo.AddString(meh);
	}

#endif
	m_VerbCombo.SetCurSel(0);
	m_NounCombo.SetCurSel(0);


	m_NornThought = "Please wait, URGEncy initialising...";
	OnSelchangeVerbcombo();

	SetTimer(0,500,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CURGEncyDlg::OnEraseBkgnd(CDC* pDC)
{
	//Here we need to plot our background lotsly..
	return TRUE;
}

UINT CURGEncyDlg::OnNcHitTest(CPoint point)
{
	CRect rec;
	m_Titlebar.GetWindowRect(&rec);
	if (rec.PtInRect(point))
		return HTCAPTION;
	else
		return HTCLIENT;
}

void CURGEncyDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CURGEncyDlg::OnPaint() 
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
		CPaintDC dc(this);
		CRect dlgRect;
		GetWindowRect(&dlgRect);
		int maxx = (dlgRect.Width() / 128) + 1;
		int maxy = (dlgRect.Height() / 128) + 1;
		CBitmap bmp;
		bmp.LoadBitmap(IDB_BACKGROUND);
		CDC imageDC;
		imageDC.CreateCompatibleDC(&dc);
		CBitmap* oldImage = imageDC.SelectObject(&bmp);
		for(int x=0;x<maxx;x++)
			for(int y=0;y<maxy;y++)
				dc.BitBlt(x*128,y*128,128,128,&imageDC,0,0,SRCCOPY);

		imageDC.SelectObject(oldImage);

		//Hmm, let's run around the border and play "hiliter"

		dc.SelectStockObject(NULL_BRUSH);
		dc.RoundRect(0,0,dlgRect.Width()-1,dlgRect.Height()-1,20,20);

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CURGEncyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CURGEncyDlg::OnSelchangeVerbcombo() 
{
	// TODO: Add your control notification handler code here
	//Calc the display text...

	m_Meaning.Format("targ norn urge writ targ %d %1.2G %d %1.2G",m_NounCombo.GetCurSel()-1,(double)((m_NounSlider.GetPos()<101)?(double)m_NounSlider.GetPos()/100.0:1.99),
										m_VerbCombo.GetCurSel()-1,(double)((m_VerbSlider.GetPos()<101)?(double)m_VerbSlider.GetPos()/100.0:1.99));
	UpdateData(false);
}

void CURGEncyDlg::OnSelchangeNouncombo() 
{
	// TODO: Add your control notification handler code here
	OnSelchangeVerbcombo();
}

void CURGEncyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	OnSelchangeVerbcombo();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CURGEncyDlg::OnAboutbox() 
{
	// TODO: Add your control notification handler code here
	OnSysCommand(IDM_ABOUTBOX,0);
}

void CURGEncyDlg::OnQuit() 
{
	PostQuitMessage(0);	
}

void CURGEncyDlg::OnExecute() 
{
	// TODO: Add your control notification handler code here
	std::string command;
	std::string reply;
	command = m_Meaning;
	if (!gInterface.Inject(command,reply,true))
	{
		std::string message = "The \"Execute Macro\" command failed. ";
		message.append(reply);
		AfxMessageBox(message.c_str());
	}

}

void CURGEncyDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	//Hrm... Update norn display I suppose...

	std::string command;
	std::string reply;
	command = "targ norn outv decn outs \":\" outv attn";
	if (gInterface.Inject(command,reply,true))
	{
		//reply should be verb:noun
		int decn,attn;

		sscanf(reply.c_str(),"%d:%d",&decn,&attn);

		std::string sdecn,sattn,group;

		char buffer[1024];

		group = "Creature Actions";

		sprintf(&buffer[0],"%s%s%s",(decn<100)?"":"(",(decn<100 && decn>=0)?(cat.Get(group,decn)):"****",(decn<100)?"":")");
		sdecn.assign(&buffer[0]);

		group = "Agent Categories";
		sprintf(&buffer[0],"%s%s%s",(attn<100)?"":"(",(attn<100 && attn>=0)?(cat.Get(group,attn)):"****",(attn<100)?"":")");
		sattn.assign(&buffer[0]);

		CString ccommand;
		ccommand.Format("Norn is performing \"%s\" on \"%s\"",sdecn.c_str(),sattn.c_str());
		m_NornThought = ccommand;
		UpdateData(false);
	}
	else
	{
		m_NornThought = reply.c_str();
		UpdateData(false);
	}

	command = "outv paws";
	if (!gInterface.Inject(command,reply,true))
		return;

	bool paused = (reply.compare("0") != 0);

	if (paused)
	{
		m_PauseButton.ShowWindow(SW_HIDE);
		m_PausedButton.ShowWindow(SW_SHOW);
	}
	else
	{
		m_PausedButton.ShowWindow(SW_HIDE);
		m_PauseButton.ShowWindow(SW_SHOW);
	}

	CDialog::OnTimer(nIDEvent);
}

void CURGEncyDlg::OnQuitbutton() 
{
	// TODO: Add your control notification handler code here
	OnQuit();
}

void CURGEncyDlg::OnMinimbutton() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_MINIMIZE);
}

void CURGEncyDlg::OnPause() 
{
	// TODO: Add your control notification handler code here
	//Here we do the following...
	//1. read pause state from game
	//2. invert state
	//3. write state back to game
	//4. update display.

	std::string command;
	std::string reply;
	command = "outv paws";
	if (!gInterface.Inject(command,reply,true))
		return;

	bool paused = (reply.compare("0") != 0);

	if (paused)
		command = "dbg: play";
	else
		command = "dbg: paws";
	
	if (!gInterface.Inject(command,reply,true))
		return;

	command = "outv paws";
	if (!gInterface.Inject(command,reply,true))
		return;
	paused = (reply.compare("0") != 0);

	if (paused)
	{
		m_PauseButton.ShowWindow(SW_HIDE);
		m_PausedButton.ShowWindow(SW_SHOW);
	}
	else
	{
		m_PausedButton.ShowWindow(SW_HIDE);
		m_PauseButton.ShowWindow(SW_SHOW);
	}
}

void CURGEncyDlg::OnEmergency() 
{
	// TODO: Add your control notification handler code here
	std::string command;
	std::string reply;
	command = "targ norn urge writ targ 0 2 0 2";
	if (!gInterface.Inject(command,reply,true))
	{
		std::string message = "The \"Emergency Macro\" command failed. ";
		message.append(reply);
		AfxMessageBox(message.c_str());
	}
}

