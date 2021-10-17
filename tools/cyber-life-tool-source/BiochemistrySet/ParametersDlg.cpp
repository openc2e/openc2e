// ParametersDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "BiochemistrySetDlg.h"
#include "ParametersDlg.h"
#include "../../common/WindowState.h"
#include "ChemNames.h"
#include "BaseDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CParametersDlg dialog


CParametersDlg::CParametersDlg(CString title, CDialogSpec& spec, CWnd* pParent /*=NULL*/)
	: base(CParametersDlg::IDD, pParent), m_spec(spec), m_title(title),
	m_apCreatureNames(NULL)
{
	//{{AFX_DATA_INIT(CParametersDlg)
	m_fSampleRate = (float)m_spec.nSampleRate / 1000.0f;
	m_fTimeSpan = (float)m_spec.nHistorySize * m_fSampleRate;
	m_nCreatureListSel = -1;
	//}}AFX_DATA_INIT
	m_bInit = false;
}

CParametersDlg::~CParametersDlg()
{
	// should be auto_ptr, but MFC STL doesn't have it in (growl)
	ASSERT(m_apCreatureNames);
	if (m_apCreatureNames)
		delete m_apCreatureNames;
}

void CParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParametersDlg)
	DDX_Control(pDX, IDC_CHEMICALS_STATIC, m_ctrlChemicalsStatic);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_TIME_SPAN, m_ctrlTimeSpan);
	DDX_Control(pDX, IDC_TIME_SPAN_STATIC, m_ctrlTimeSpanStatic);
	DDX_Control(pDX, IDC_CREATURE_LIST, m_ctrlCreatureList);
	DDX_Control(pDX, IDC_CHEM_LISTVIEW, m_ctrlChemicalListView);
	DDX_Text(pDX, IDC_SAMPLE_RATE, m_fSampleRate);
	DDV_MinMaxFloat(pDX, m_fSampleRate, 0.001f, 100000.0f);
	DDX_Text(pDX, IDC_TIME_SPAN, m_fTimeSpan);
	DDV_MinMaxFloat(pDX, m_fTimeSpan, 0.01f, 100000.0f);
	DDX_LBIndex(pDX, IDC_CREATURE_LIST, m_nCreatureListSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParametersDlg, base)
	//{{AFX_MSG_MAP(CParametersDlg)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CHEM_LISTVIEW, OnItemchangedChemListview)
	ON_WM_MOVE()
	ON_LBN_SELCHANGE(IDC_CREATURE_LIST, OnSelchangeCreatureList)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TICK_ALL, OnTickAll)
	ON_COMMAND(ID_UNTICK_ALL, OnUntickAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParametersDlg message handlers

BOOL CParametersDlg::OnInitDialog() 
{
	base::OnInitDialog();

	SetWindowText(m_title);
		
	// Turn controls on and off
	if ((m_spec.type == CDialogSpec::LOG_DIALOG)
		|| (m_spec.type == CDialogSpec::INJECTION_DIALOG)
		|| (m_spec.type == CDialogSpec::ORGAN_DIALOG))
	{
		m_ctrlTimeSpan.ShowWindow(SW_HIDE);
		m_ctrlTimeSpanStatic.ShowWindow(SW_HIDE);
	}

	if (NoChemControl())
	{
		m_ctrlChemicalListView.ShowWindow(SW_HIDE);
		m_ctrlChemicalsStatic.ShowWindow(SW_HIDE);
	}

	// Fill in chemical list
	for (int i = 1; i < CChemNames::size(); ++i)
	{
		m_ctrlChemicalListView.InsertItem(i, CChemNames::GetChemName(i));
	}

	// Set tick state of chemical list
	ListView_SetExtendedListViewStyle(m_ctrlChemicalListView.m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	{
		{
			for (int i = 1; i < CChemNames::size(); ++i)
				SetCheck(i, false);
		}
		int n = m_spec.vChems.size();
		for (int i = 0; i < n; ++i)
		{
			SetCheck(m_spec.vChems[i], true);
		}
	}

	// Fill in creatures list
	{
		ASSERT(!m_apCreatureNames); // add code to delete old one if you call this twice
		m_apCreatureNames = CCreatureNames::SnapshotCreatureNames();
		int sel = -1;
		int n = m_apCreatureNames->size();
		for (int i = 0; i < n; ++i)
		{
			m_ctrlCreatureList.AddString(m_apCreatureNames->GetCreatureName(i));
			CCreatureMoniker currmonik = m_apCreatureNames->GetMoniker(i);
			if (currmonik == m_spec.monik)
			{
				ASSERT(sel == -1); // should only find once!
				sel = i; // select moniker which matches
			}
		}
		if (sel >= 0)
			m_ctrlCreatureList.SetCurSel(sel);
	}

	RefreshEnabling();

	m_bInit = true;

	// Get last position from registry
	WindowState::Load("Windows", "Parameters", m_hWnd);

	// Fixed width and height if no chemical selection control
	if (NoChemControl())
	{
		CRect rect(0, 0, 160, 95);
		OurMapDialogRect(rect);
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE);
	}

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);			// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME_SMALL), FALSE);		// Set small icon

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CParametersDlg::SetCheck(WPARAM ItemIndex, BOOL bCheck)
{
	m_ctrlChemicalListView.SetCheck(ItemIndex - 1, bCheck);
}

void CParametersDlg::OnSize(UINT nType, int cx, int cy) 
{
	base::OnSize(nType, cx, cy);
	
	if (m_ctrlChemicalListView)
	{
		CRect rect;
		m_ctrlChemicalListView.GetWindowRect(rect);
		ScreenToClient(rect);
		CRect outer;
		GetWindowRect(outer);
		ScreenToClient(outer);

		int spacing = (rect.left - outer.left);
		int width = (outer.Width()) - (spacing * 2);
		rect.right = rect.left + width;
		rect.bottom = outer.bottom - spacing;
		m_ctrlChemicalListView.MoveWindow(rect);
	}
}

void CParametersDlg::OnMove(int x, int y) 
{
	base::OnMove(x, y);
}

int CParametersDlg::DoModal() 
{
	int ret = base::DoModal();

	if (ret == IDOK)
	{
		m_spec.nSampleRate = ((m_fSampleRate * 1000) + 0.5);
		m_spec.nHistorySize = (m_fTimeSpan / m_fSampleRate) + 1;
		ASSERT(m_nCreatureListSel != LB_ERR);
		m_spec.monik = m_apCreatureNames->GetMoniker(m_nCreatureListSel);		 
	}

	return ret;
}

void CParametersDlg::OnItemchangedChemListview(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_bInit)
	{
		m_spec.vChems.clear();
		for (int i = 1; i < CChemNames::size(); ++i)
		{
			int iChecked = m_ctrlChemicalListView.GetCheck(i - 1);
			if (iChecked)
			{
				m_spec.vChems.push_back(i);
			}
		}
	}
	
	*pResult = 0;
}

void CParametersDlg::RefreshEnabling()
{
	m_ctrlOK.EnableWindow(m_ctrlCreatureList.GetCurSel() != LB_ERR);
}


void CParametersDlg::OnSelchangeCreatureList()
{
	RefreshEnabling();
}

void CParametersDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_PROPERTIES_MENU));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL); 
	SetActiveWindow();
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);	
}

void CParametersDlg::OnTickAll() 
{
	m_spec.vChems.resize(CChemNames::size());
	for (int i = 1; i < CChemNames::size(); ++i)
	{
		SetCheck(i, true);	
		m_spec.vChems[i] = i;
	}
}

void CParametersDlg::OnUntickAll() 
{
	m_spec.vChems.clear();
	for (int i = 1; i < CChemNames::size(); ++i)
		SetCheck(i, false);	
}

void CParametersDlg::SaveWindowState()
{
	if (!NoChemControl())
		WindowState::Save("Windows", "Parameters", m_hWnd);
}

void CParametersDlg::OnCancel() 
{
	SaveWindowState();
	
	base::OnCancel();
}

void CParametersDlg::OnOK() 
{
	SaveWindowState();
	
	base::OnOK();
}

bool CParametersDlg::NoChemControl()
{
	return (m_spec.type == CDialogSpec::ORGAN_DIALOG);
}

