// GrapherDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "DialogSpec.h"
#include "GrapherDlg.h"
#include "ChemNames.h"
#include "BiochemistrySetDlg.h"

const int nDeflateBorder = 0;

/////////////////////////////////////////////////////////////////////////////
// CGrapherDlg dialog


CGrapherDlg::CGrapherDlg(CWnd* pParent /*=NULL*/)
	: base(CGrapherDlg::IDD, pParent), 	m_wndLabel(0, 1000)
{
	//{{AFX_DATA_INIT(CGrapherDlg)
	//}}AFX_DATA_INIT
}


void CGrapherDlg::DoDataExchange(CDataExchange* pDX)
{
	base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrapherDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrapherDlg, base)
	//{{AFX_MSG_MAP(CGrapherDlg)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_COMMAND(IDC_AXES, OnAxes)
	ON_COMMAND(IDC_WHITE_PAGE, OnWhitePage)
	ON_WM_MOVE()
	ON_COMMAND(IDC_INJECT, OnInject)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrapherDlg message handlers

void CGrapherDlg::OnSize(UINT nType, int cx, int cy) 
{
	base::OnSize(nType, cx, cy);
	MyRefresh();
}

void CGrapherDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	const int maxwidth = 200;
	const int maxheight = 120;
	int width = lpRect->right - lpRect->left;
	int height = lpRect->bottom - lpRect->top;
	if (width < maxwidth) 
		lpRect->right = lpRect->left + maxwidth;
	if (height < maxheight) 
		lpRect->bottom = lpRect->top + maxheight;
	base::OnSizing(nSide, lpRect);
}

BOOL CGrapherDlg::OnInitDialog() 
{
	base::OnInitDialog();
	m_Graph.OnCreate();
	m_wndLabel.Create(this);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGrapherDlg::Sample(int i, float val)
{
	ASSERT(i >= 0);
	ASSERT(i < m_vTestTrace.size());
	m_vTestTrace[i].AddSample(val);
}

void CGrapherDlg::PostSample()
{
	if( m_iTime > m_Graph.GetHSpan() )
		m_Graph.SetTime( m_iTime );
	else
		m_Graph.SetTime( m_Graph.GetHSpan() );
	MyRefresh();
}

void CGrapherDlg::ChangeSpec(const CDialogSpec& spec)
{
	base::ChangeSpec(spec);

	ASSERT(m_spec.type == CDialogSpec::GRAPH_DIALOG);

	m_Graph.m_vLabel[0] = "0.0 ";
	m_Graph.m_vLabel[1] = "0.5 ";
	m_Graph.m_vLabel[2] = "1.0 ";

	m_Graph.m_bDrawKey = m_spec.bDrawKey;
	m_Graph.SetWhiteColour(m_spec.bWhiteColour);

	m_Graph.SetHSpan( m_spec.nHistorySize );
	m_Graph.SetHUnits( m_spec.nSampleRate );

	m_Graph.SetTraceSize(m_spec.vChems.size());
	m_vTestTrace.resize(m_spec.vChems.size());
	int n = m_vTestTrace.size();
	for (int i = 0; i < n; ++i)
	{	
		m_vTestTrace[i].SetName(std::string(CChemNames::GetChemName(m_spec.vChems[i])));
		m_vTestTrace[i].Reset(m_spec.nHistorySize);
		m_Graph.SetTrace(i, &m_vTestTrace[i]);
	}
	
	// the following must be done after settrace
	m_Graph.SetHistorySize( m_spec.nHistorySize );

	MyRefresh();
}

CDialogSpec CGrapherDlg::GetSpec()
{
	m_spec.bDrawKey = m_Graph.m_bDrawKey;
	m_spec.bWhiteColour = m_Graph.m_bWhiteColour;
	return base::GetSpec();
}

void CGrapherDlg::MenuChange(CMenu* pPopup)
{
	if (m_Graph.m_bDrawKey)
		pPopup->CheckMenuItem(IDC_AXES, MF_BYCOMMAND | MF_CHECKED);
	if (m_Graph.m_bWhiteColour)
		pPopup->CheckMenuItem(IDC_WHITE_PAGE, MF_BYCOMMAND | MF_CHECKED);
}

void CGrapherDlg::OnAxes() 
{
	m_Graph.m_bDrawKey = !m_Graph.m_bDrawKey;
	MyRefresh();
}

void CGrapherDlg::OnWhitePage() 
{
	m_Graph.SetWhiteColour(!m_Graph.m_bWhiteColour);	
	MyRefresh();
}

void CGrapherDlg::OnMove(int x, int y) 
{
	base::OnMove(x, y);

	m_wndLabel.GoAway();
}

void CGrapherDlg::OnInject() 
{
	CDialogSpec localspec = GetSpec();

	localspec.rectPosition.OffsetRect(20, 20);
	localspec.type = CDialogSpec::INJECTION_DIALOG;
	localspec.nSampleRate = INJECTION_DEFAULT_SAMPLE_RATE;

	dynamic_cast<CBiochemistrySetDlg*>(AfxGetMainWnd())->QueryChemWindow(localspec, true, true);	
}

void CGrapherDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(rect);
	rect.DeflateRect(nDeflateBorder, nDeflateBorder);
	m_Graph.DoPaint(dc, rect);
}

void CGrapherDlg::MyRefresh()
{
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE );
}

void CGrapherDlg::LabelTrace(CPoint point)
{
	const int within = 16; // number of pixels to be near trace by

	CPoint snap;
	float value;
	std::vector<int> hits = m_Graph.HitTestTraces( point, snap, within, value);

	if( hits.size() > 0 )
	{
		int i = hits[0];
		
		CString str = CChemNames::GetChemNameAndNumber(m_spec.vChems[i]);
		CString sValue;
		sValue.Format(" = %.3f", value);
		str = str + sValue;

		CRect rect;
		bool bWasVisible = m_wndLabel.m_bVisible;
		if (m_wndLabel.m_bVisible)
			m_wndLabel.GetWindowRect(rect);
		m_wndLabel.PopUp( str, snap, true, true, m_Graph.GetTraceColour(i));

		// refresh if the label has moved
		if (bWasVisible && (snap.x != rect.left || snap.y != rect.top))
			MyRefresh();
	}
}


void CGrapherDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	LabelTrace(point);
	
	base::OnMouseMove(nFlags, point);
}

