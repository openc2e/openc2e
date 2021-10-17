// CLoggerDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "LoggerDlg.h"
#include "ChemNames.h"
#include <iostream>

/////////////////////////////////////////////////////////////////////////////
// CLoggerDlg dialog


CLoggerDlg::CLoggerDlg(CWnd* pParent /*=NULL*/)
	: base(CLoggerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoggerDlg)
	m_ctrlFilename = _T("");
	//}}AFX_DATA_INIT
}

void CLoggerDlg::DoDataExchange(CDataExchange* pDX)
{
	base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoggerDlg)
	DDX_Text(pDX, IDC_FILENAME, m_ctrlFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoggerDlg, base)
	//{{AFX_MSG_MAP(CLoggerDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoggerDlg message handlers

void CLoggerDlg::ChangeSpec(const CDialogSpec& spec)
{
	if ((spec.filename != m_spec.filename) && ologstr.is_open())
		ologstr.close();

	base::ChangeSpec(spec);
	ASSERT(m_spec.type == CDialogSpec::LOG_DIALOG);

	if (!ologstr.is_open())
		ologstr.open(m_spec.filename.c_str());
	
	if (!ologstr.is_open())
	{
		MessageBox(_T("Failed to open file - \n%s") + CString(m_spec.filename.c_str()), _T("Biochemistry Set"), MB_OKCANCEL | MB_ICONINFORMATION);
	}

	ologstr << _T("Time,");
	int n = m_spec.vChems.size();
	for (int i = 0; i < n; ++i)
	{	
		ologstr << (LPCSTR)(CChemNames::GetChemName(m_spec.vChems[i])) << _T(",");
	}
	ologstr << std::endl;

	m_ctrlFilename = m_spec.filename.c_str();
	UpdateData(false);
}

BOOL CLoggerDlg::OnInitDialog() 
{
	base::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoggerDlg::OnClose() 
{
	if (ologstr.is_open())
		ologstr.close();
	
	base::OnClose();
}

void CLoggerDlg::PreSample(bool bError, CString sMessage)
{
	base::PreSample(bError, sMessage);
	if (bError)
	{
		ologstr << (LPCTSTR)sMessage << std::endl;
	}
	else
	{
		float val = float(m_iTime * m_spec.nSampleRate) / 1000.0f;
		ologstr << val << _T(",");
	}
}

void CLoggerDlg::Sample(int i, float val)
{
	ASSERT(ologstr.is_open());
	ologstr << val << _T(",");
}

void CLoggerDlg::PostSample()
{
	ASSERT(ologstr.is_open());
	ologstr << std::endl;
	ologstr.flush();
}

