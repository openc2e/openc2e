// InjectionDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "InjectionDlg.h"
#include "ChemNames.h"
#include <strstrea.h>
#include <string>
#include "../../common/GameInterface.h"

/////////////////////////////////////////////////////////////////////////////
// CInjectionDlg dialog


CInjectionDlg::CInjectionDlg(CWnd* pParent /*=NULL*/)
	: base(CInjectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInjectionDlg)
	m_fLevelNumber = 0.0f;
	//}}AFX_DATA_INIT
}


void CInjectionDlg::DoDataExchange(CDataExchange* pDX)
{
	base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInjectionDlg)
	DDX_Control(pDX, IDC_SET, m_ctrlSetButton);
	DDX_Control(pDX, IDC_CHEMLIST, m_ctrlChemList);
	DDX_Control(pDX, IDC_LEVEL_SLIDER, m_ctrlLevelSlider);
	DDX_Control(pDX, IDC_LEVEL_NUMBER, m_ctrlLevelNumber);
	DDX_Text(pDX, IDC_LEVEL_NUMBER, m_fLevelNumber);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInjectionDlg, base)
	//{{AFX_MSG_MAP(CInjectionDlg)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_LEVEL_NUMBER, OnChangeLevelNumber)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CHEMLIST, OnItemchangedChemlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInjectionDlg message handlers

void CInjectionDlg::ChangeSpec(const CDialogSpec& spec)
{
	base::ChangeSpec(spec);
	ASSERT(m_spec.type == CDialogSpec::INJECTION_DIALOG);

	m_ctrlChemList.DeleteAllItems();
	int n = m_spec.vChems.size();
	for (int i = 0; i < n; ++i)
	{	
		m_ctrlChemList.InsertItem(LVIF_TEXT, i, CChemNames::GetChemName(m_spec.vChems[i]), 0, 0, 0, 0);
	}
	m_ctrlChemList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlChemList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
}

const int CInjectionDlg::slidemax = 1000;

BOOL CInjectionDlg::OnInitDialog() 
{
	base::OnInitDialog();

	m_ctrlChemList.InsertColumn(0, _T("Chemical"), LVCFMT_LEFT, -1, -1);	
	m_ctrlChemList.InsertColumn(1, _T("Concentration"), LVCFMT_LEFT, -1, -1);
	m_ctrlLevelSlider.SetRange(0, slidemax);
	m_ctrlLevelSlider.SetPageSize(slidemax / 10);
	m_ctrlLevelSlider.SetLineSize(slidemax / 100);
	m_ctrlLevelSlider.SetTicFreq(slidemax / 10);
	m_ctrlLevelSlider.SetPos(slidemax);
	
	RefreshEditBox();
	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInjectionDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	RefreshEditBox();
	base::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInjectionDlg::RefreshEditBox()
{
	m_fLevelNumber = ((float)(slidemax - m_ctrlLevelSlider.GetPos()) / (float)slidemax);
	UpdateData(false);
}

void CInjectionDlg::RefreshSlider()
{
	UpdateData();
	int slider = slidemax - (int)(m_fLevelNumber * slidemax);
	if (slider > slidemax)
		slider = slidemax;
	if (slider < 0)
		slider = 0;

	m_ctrlLevelSlider.SetPos(slider);
}

void CInjectionDlg::OnChangeLevelNumber() 
{
	RefreshSlider();
}

void CInjectionDlg::Sample(int i, float val)
{
	ASSERT(i >= 0);
	ASSERT(i < m_ctrlChemList.GetItemCount());
	CString fmt;
	fmt.Format(_T("%.3f"), val);
	m_ctrlChemList.SetItem(i, 1, LVIF_TEXT, fmt, 0, 0, 0, 0);
}

void CInjectionDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	const int maxwidth = 200;
	const int maxheight = 172;
	int width = lpRect->right - lpRect->left;
	int height = lpRect->bottom - lpRect->top;
	if (width < maxwidth) 
		lpRect->right = lpRect->left + maxwidth;
	if (height < maxheight) 
		lpRect->bottom = lpRect->top + maxheight;

	base::OnSizing(nSide, lpRect);
}

void CInjectionDlg::OnSize(UINT nType, int cx, int cy) 
{
	base::OnSize(nType, cx, cy);
	
	if (m_ctrlChemList && m_ctrlLevelSlider && m_ctrlSetButton && m_ctrlLevelNumber)
	{
		CRect fullrect;
		GetClientRect(fullrect);
		CRect chemrect;
		m_ctrlChemList.GetWindowRect(chemrect);
		ScreenToClient(chemrect);
		CRect sliderect;
		m_ctrlLevelSlider.GetWindowRect(sliderect);
		ScreenToClient(sliderect);
		CRect setrect;
		m_ctrlSetButton.GetWindowRect(setrect);
		ScreenToClient(setrect);
		CRect editrect;
		m_ctrlLevelNumber.GetWindowRect(editrect);
		ScreenToClient(editrect);

		int slide_width = sliderect.Width();
		int set_width = setrect.Width();
		int edit_width = editrect.Width();
		int extra_width = max(max(slide_width, set_width), edit_width);
		int set_height = setrect.Height();
		int edit_height = editrect.Height();

		int vert_margin = chemrect.top - fullrect.top;
		int horz_margin = chemrect.left - fullrect.left;
		chemrect.bottom = fullrect.bottom - vert_margin;
		chemrect.right = chemrect.left + fullrect.Width() - (horz_margin * 3) - extra_width;

		sliderect.left = chemrect.right + horz_margin + (extra_width - slide_width) / 2;
		sliderect.right = sliderect.left + slide_width;
		sliderect.top = chemrect.top;
		sliderect.bottom = sliderect.top + fullrect.Height() - (vert_margin * 4) - set_height - edit_height;
		
		editrect.left = chemrect.right + horz_margin + (extra_width - edit_width) / 2;
		editrect.right = editrect.left + edit_width;
		editrect.top = sliderect.bottom + vert_margin;
		editrect.bottom = editrect.top + edit_height;

		setrect.left = chemrect.right + horz_margin + (extra_width - set_width) / 2;
		setrect.right = setrect.left + set_width;
		setrect.top = editrect.bottom + vert_margin;
		setrect.bottom = setrect.top + set_height;

		m_ctrlChemList.MoveWindow(chemrect);
		m_ctrlLevelSlider.MoveWindow(sliderect);
		m_ctrlSetButton.MoveWindow(setrect);
		m_ctrlLevelNumber.MoveWindow(editrect);

		m_ctrlChemList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		m_ctrlChemList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	}	
}

void CInjectionDlg::OnSet() 
{
	UpdateData();
	float new_chem_level = m_fLevelNumber;
	if (new_chem_level > 1.0f)
		new_chem_level = 1.0f;
	if (new_chem_level < 0.0f)
		new_chem_level = 0.0f;

	POSITION pos = m_ctrlChemList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		CString fullcommand = m_spec.monik.TargCAOS();
		while (pos)
		{
			int nItem = m_ctrlChemList.GetNextSelectedItem(pos);
			int nChemical = m_spec.vChems[nItem];
			// perform injection here	
			CString command;
			// the -2 first ensures we zero the chemical 
			// before adding in the amount that we want
			command.Format(_T("chem %d -2.0 chem %d %f "), 
				nChemical, nChemical, new_chem_level);
			fullcommand += command;
		}
		std::string reply;
		bool bOK = theGameInterface.Inject((LPCTSTR)fullcommand, reply);
		if (!bOK)
		{
			MessageBox(CString(_T("Connection Failed - ")) + reply.c_str(), _T("Biochemistry Set"), MB_OK | MB_ICONINFORMATION);
		}
	}
}

void CInjectionDlg::OnItemchangedChemlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	EnableControls();
	
	*pResult = 0;
}

void CInjectionDlg::EnableControls()
{
	POSITION pos = m_ctrlChemList.GetFirstSelectedItemPosition();
	bool bEnable = (pos != NULL);
	m_ctrlSetButton.EnableWindow(bEnable);
}

