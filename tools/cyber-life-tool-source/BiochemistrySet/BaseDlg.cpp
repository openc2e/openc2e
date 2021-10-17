// GrapherDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "DialogSpec.h"
#include "BiochemistrySet.h"
#include "BiochemistrySetDlg.h"
#include "ParametersDlg.h"
#include "BaseDlg.h"
#include <string>
#include <math.h> // sine
#include "../../common/GameInterface.h"

#include <sstream>

/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog


CBaseDlg::CBaseDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: base(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CBaseDlg)
	//}}AFX_DATA_INIT
	m_uTimerID = 0;
	m_bChemicalSample = true;
	m_bError = false;
	bInTimer = false;
}


void CBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBaseDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBaseDlg, base)
	//{{AFX_MSG_MAP(CBaseDlg)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDC_GRAPH, OnGraph)
	ON_COMMAND(IDC_INJECT, OnInject)
	ON_COMMAND(IDC_LOG, OnLog)
	ON_COMMAND(IDC_PROPERTIES, OnProperties)
	ON_COMMAND(IDC_FAVOURITE, OnFavourite)
	ON_COMMAND(IDC_ORGAN, OnOrgan)
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseDlg message handlers

void CBaseDlg::OnTimer(UINT nIDEvent)
{
	if (!bInTimer && (m_uTimerID == nIDEvent))
	{
		bInTimer = true;

		// Anything needed in general (organ window updates)
		GeneralTimer();

		// Chemical level sample updates
		if (m_bChemicalSample)
		{
			bool bOK;
			std::string reply;
			if (g_bSineTest)
				bOK = true;
			else
			{
				CString command = m_spec.monik.TargCAOS();
				int n = m_spec.vChems.size();
				for (int i = 0; i < n; ++i)
				{
					CString num;
					num.Format("%d", m_spec.vChems[i]);
					command = command + _T("outv chem ") + num + _T("\nouts \"\\n\"\n");
				}
				
				bOK = theGameInterface.Inject((LPCTSTR)command, reply);
			}
			if (!bOK)
			{
				PreSample(true, CString(_T("Connection Failed - ")) + reply.c_str());
				// _T("Creature Not Found");
				// _T("No Dynamic Target");
			}
			else
			{
				std::istringstream strin(reply);

				PreSample(false, _T(""));
				int n = m_spec.vChems.size();
				for (int i = 0; i < n; ++i)
				{
					float sam;
					if (g_bSineTest)
						sam = (sin((float)(m_iTime + i * 10) / 50.0f) + 1.0f) / 2.0f;
					else
						strin >> sam;
					ASSERT(sam >= 0.0f && sam <= 1.0f);
					if (sam < 0)
						sam = 0;
					if (sam > 1.0f)
						sam = 1.0f;
					Sample(i, sam);
				}
				PostSample();
				m_iTime++;
			}			
		}

		bInTimer = false;
	}

	base::OnTimer(nIDEvent);
}

void CBaseDlg::OnClose() 
{
	if (m_uTimerID)
		KillTimer( m_uTimerID );
	
	// destory ourselves, as we've stopped OnCancel
	DestroyWindow();
}

void CBaseDlg::OnDestroy() 
{
	base::OnDestroy();
	
	delete this;	
}

void CBaseDlg::ChangeSpec(const CDialogSpec& spec)
{
	m_spec = spec;

	CRect rect = m_spec.rectPosition;
	OurMapDialogRect(rect);  // convert from dialog units to pixels
	MoveWindow(rect);

	m_iTime = 0;
	if (m_uTimerID)
		KillTimer( m_uTimerID );

	m_uTimerID = SetTimer(1, m_spec.nSampleRate, NULL);

	RefreshWindowText();
}

void CBaseDlg::RefreshWindowText()
{
	SetWindowText((/*std::string(_T("Biochemistry Set - ")) + */ m_spec.name).c_str());
}

// Converts pixels to dialog units
// Not quite the reverse of MapDialogRect as it doesn't
// take into accoutn fonts changes in the dialog template - but
// we don't use those anyway
void UnMapDialogRect(CRect& rect)
{
	LONG baseunits = GetDialogBaseUnits();
	int baseunitY = HIWORD(baseunits);
	int baseunitX = LOWORD(baseunits);

	rect.top = (rect.top * 8) / baseunitY;
	rect.left = (rect.left * 4) / baseunitX;
	rect.bottom = (rect.bottom * 8) / baseunitY;
	rect.right = (rect.right * 4) / baseunitX;
}

// Converts dialog units to pixels
// We don't trust theirs
void OurMapDialogRect(CRect& rect)
{
	LONG baseunits = GetDialogBaseUnits();
	int baseunitY = HIWORD(baseunits);
	int baseunitX = LOWORD(baseunits);

	rect.top = (rect.top * baseunitY) / 8;
	rect.left = (rect.left * baseunitX) / 4;
	rect.bottom = (rect.bottom * baseunitY) / 8;
	rect.right = (rect.right * baseunitX) / 4;
}

CDialogSpec CBaseDlg::GetSpec()
{
	CRect rect;
	GetWindowRect(rect);
	UnMapDialogRect(rect); // convert from pixels to dialog units
	m_spec.rectPosition = rect;

	return m_spec;
}

void CBaseDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	VERIFY(menu.LoadMenu(MenuResourceID()));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL); 
	MenuChange(pPopup);
	SetActiveWindow();
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CBaseDlg::OnGraph() 
{
	CDialogSpec localspec = GetSpec();
	localspec.rectPosition.OffsetRect(20, 20);
	bool bSameType = (localspec.type == CDialogSpec::GRAPH_DIALOG);
	localspec.type = CDialogSpec::GRAPH_DIALOG;
	dynamic_cast<CBiochemistrySetDlg*>(AfxGetMainWnd())->QueryChemWindow(localspec, !bSameType);
}

void CBaseDlg::OnInject() 
{
	CDialogSpec localspec = GetSpec();
	localspec.rectPosition.OffsetRect(20, 20);
	bool bSameType = (localspec.type == CDialogSpec::INJECTION_DIALOG);
	localspec.type = CDialogSpec::INJECTION_DIALOG;
	localspec.nSampleRate = INJECTION_DEFAULT_SAMPLE_RATE;
	dynamic_cast<CBiochemistrySetDlg*>(AfxGetMainWnd())->QueryChemWindow(localspec, !bSameType);
}

void CBaseDlg::OnLog() 
{
	CDialogSpec localspec = GetSpec();
	localspec.rectPosition.OffsetRect(20, 20);
	bool bSameType = (localspec.type == CDialogSpec::LOG_DIALOG);
	localspec.type = CDialogSpec::LOG_DIALOG;
	dynamic_cast<CBiochemistrySetDlg*>(AfxGetMainWnd())->QueryChemWindow(localspec, !bSameType);
}

void CBaseDlg::OnProperties() 
{
	CDialogSpec localspec = GetSpec();
	CParametersDlg dlgParams(localspec.TypeString() + _T(" Properties"), localspec);
	if (dlgParams.DoModal() == IDOK)
	{
		localspec.name = localspec.MakeName();
		ChangeSpec(localspec);
	}	
}

void CBaseDlg::OnFavourite() 
{
	CDialogSpec localspec = GetSpec();
	dynamic_cast<CBiochemistrySetDlg*>(AfxGetMainWnd())->AddFavouriteSpec(localspec);
}


BOOL CBaseDlg::OnInitDialog()
{
	base::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);			// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME_SMALL), FALSE);		// Set small icon

	return true;
}

void CBaseDlg::PreSample(bool bError, CString sMessage)
{
	if (bError)
	{
		SetWindowText(sMessage);
		m_bError = true;
				
	}
	else if (m_bError)
	{
		RefreshWindowText();
		m_bError = false;
	}
}

// otherwise esc quits the dialog
void CBaseDlg::OnCancel()
{
}

// otherwise return quits the dialog
void CBaseDlg::OnOK()
{
}


void CBaseDlg::OnOrgan() 
{
	CDialogSpec localspec = GetSpec();
	localspec.rectPosition.OffsetRect(20, 20);
	bool bSameType = (localspec.type == CDialogSpec::ORGAN_DIALOG);
	localspec.type = CDialogSpec::ORGAN_DIALOG;
	dynamic_cast<CBiochemistrySetDlg*>(AfxGetMainWnd())->QueryChemWindow(localspec, !bSameType);	
}

