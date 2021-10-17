// BiochemistrySet.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "BiochemistrySetDlg.h"
#include "GrapherDlg.h"
#include "../../common/GameInterface.h"
#include "ChemNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool g_bSineTest;

/////////////////////////////////////////////////////////////////////////////
// CBiochemistrySetApp

BEGIN_MESSAGE_MAP(CBiochemistrySetApp, CWinApp)
	//{{AFX_MSG_MAP(CBiochemistrySetApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBiochemistrySetApp construction

CBiochemistrySetApp::CBiochemistrySetApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBiochemistrySetApp object

CBiochemistrySetApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBiochemistrySetApp initialization

BOOL CBiochemistrySetApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// SPARKY SetRegistryKey(_T(COMPANY));

	CString sCmd(m_lpCmdLine);

	// command line help.
	if (sCmd.Find("?") != -1 || sCmd.Find("help") != -1)
	{
		MessageBox(NULL, _T("Command line parameters:\n/static - connect to game at startup rather than dynamically\n/sin - don't connect to game, and just draw sine curves"), _T("Biochemistry Set"), MB_OK | MB_ICONINFORMATION);
		exit(1);
	}
	g_bSineTest = (sCmd.Find("/sin") != -1);
	bool bDynConnect = (sCmd.Find("/static") == -1);
	theGameInterface.SetDynConnect(bDynConnect);

	if (!g_bSineTest && !bDynConnect)
	{
		int result;
		do
		{
			bool bConnect = theGameInterface.Connect();
			if (bConnect)
				break;
			result = MessageBox(NULL, _T("Failed to connect to game.\n\nDon't use /static on command line if you\nwant to connect dynamically."), _T("Biochemistry Set"), MB_RETRYCANCEL | MB_ICONINFORMATION);
			if (result == IDCANCEL)
				exit(1);
		}
		while (result == IDRETRY);
	}

	CChemNames::Init();

	CBiochemistrySetDlg dlg;	
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	if (!g_bSineTest && !bDynConnect)
		theGameInterface.Disconnect();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}

