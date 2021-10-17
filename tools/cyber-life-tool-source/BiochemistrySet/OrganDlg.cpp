// OrganDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "OrganDlg.h"
#include "../../common/GameInterface.h"
#include <sstream>

const int nDeflateBorder = 11;

/////////////////////////////////////////////////////////////////////////////
// COrganDlg dialog


COrganDlg::COrganDlg(CWnd* pParent /*=NULL*/)
	: base(COrganDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COrganDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bChemicalSample = false;
}


void COrganDlg::DoDataExchange(CDataExchange* pDX)
{
	base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrganDlg)
	DDX_Control(pDX, IDC_ORGAN_LIST, m_ctrlOrganList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COrganDlg, base)
	//{{AFX_MSG_MAP(COrganDlg)
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrganDlg message handlers

void COrganDlg::OnSize(UINT nType, int cx, int cy) 
{
	base::OnSize(nType, cx, cy);
	
	if (m_ctrlOrganList)
	{
		CRect rect;
		GetClientRect(rect);
		rect.DeflateRect(nDeflateBorder, nDeflateBorder);
		m_ctrlOrganList.MoveWindow(rect);
	}	
}

void COrganDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	const int maxwidth = 174;
	const int maxheight = 94;
	int width = lpRect->right - lpRect->left;
	int height = lpRect->bottom - lpRect->top;
	if (width < maxwidth) 
		lpRect->right = lpRect->left + maxwidth;
	if (height < maxheight) 
		lpRect->bottom = lpRect->top + maxheight;

	base::OnSizing(nSide, lpRect);
}

void COrganDlg::ChangeSpec(const CDialogSpec& spec)
{
	base::ChangeSpec(spec);
	ASSERT(m_spec.type == CDialogSpec::ORGAN_DIALOG);
	myOrganCount = -1;
}

BOOL COrganDlg::OnInitDialog() 
{
	base::OnInitDialog();
	
	m_ctrlOrganList.InsertColumn(0, "Organ", LVCFMT_LEFT, -1, -1);	
	m_ctrlOrganList.InsertColumn(1, "Receptors", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(2, "Emitters", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(3, "Reactions", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(4, "Clock rate", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(5, "Short term locus", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(6, "Repair factor locus", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(7, "Injury to apply locus", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(8, "Initial life force", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(9, "Short term life force", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(10, "Long term life force", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(11, "Long term damage rate", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(12, "Energy cost", LVCFMT_LEFT, -1, -1);
	m_ctrlOrganList.InsertColumn(13, "Damage when no energy", LVCFMT_LEFT, -1, -1);
  
	int n = 14;
	for (int i = 0; i < n; ++i)
	{
		m_ctrlOrganList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COrganDlg::GeneralTimer()
{
	// refresh all organs

	std::string command = (LPCTSTR)m_spec.monik.TargCAOS();
	command += 
		"outv orgn "
		"outs \"\\n\" "
		"doif orgn < 0 "
			"stop "
		"endi "

		"setv va00 0 "
		"loop "
			"outv orgi va00 0 "
			"outs \" \" "
			"outv orgi va00 1 "
			"outs \" \" "
			"outv orgi va00 2 "
			"outs \" \" "

			"setv va01 0 "
			"loop "
				"outv orgf va00 va01 "
				"outs \" \" "
				"addv va01 1 "
			"untl va01 > 9 "

			"outs \"\\n\" "
			"addv va00 1 "
		"untl va00 >= orgn ";

	std::string reply;

	bool bOK = theGameInterface.Inject(command.c_str(), reply);

	std::istringstream strin(reply);
	int newOrganCount = 0;
	strin >> newOrganCount;

	if (myOrganCount != newOrganCount)
	{
		m_ctrlOrganList.DeleteAllItems();
		myOrganCount = newOrganCount;
		for (int i = 0; i < myOrganCount; ++i)
		{
			std::ostringstream out;
			out << i;
			m_ctrlOrganList.InsertItem(i, out.str().c_str());
		}
	}

	for (int i = 0; i < myOrganCount; ++i)
	{
		for (int col = 1; col < 14; ++col)
		{
			CString fmt;
			if (col >= 4)
			{
				float val = 0;
				strin >> val;
				fmt.Format(_T("%.3f"), val);
			}
			else
			{
				int val = 0;
				strin >> val;
				fmt.Format(_T("%d"), val);
			}

			m_ctrlOrganList.SetItem(i, col, LVIF_TEXT, fmt, 0, 0, 0, 0);
		}
	}
}

