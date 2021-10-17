// GetLicenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GetLicence.h"
#include "GetLicenceDlg.h"
#include "LicenseKey\CTL_License.h"
#include "InstallLib\Registry.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRODCODE 350

/////////////////////////////////////////////////////////////////////////////
// CGetLicenceDlg dialog

CGetLicenceDlg::CGetLicenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetLicenceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetLicenceDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_CAOS);
}

void CGetLicenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetLicenceDlg)
	DDX_Control(pDX, IDC_EDITKEYCHECK, m_editkeycheck);
	DDX_Control(pDX, IDC_EDITKEYNAME, m_editkeyname);
	DDX_Control(pDX, IDC_EDITKEYSERIAL, m_editkeyserial);
	DDX_Control(pDX, IDC_EDITNAME, m_editname);
	DDX_Control(pDX, IDC_EDITKEYPROD, m_editkeyprod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGetLicenceDlg, CDialog)
	//{{AFX_MSG_MAP(CGetLicenceDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDITNAME, OnChangeEditName)
	ON_EN_CHANGE(IDC_EDITKEYPROD, OnChangeEditKeyProd)
	ON_EN_CHANGE(IDC_EDITKEYCHECK, OnChangeEditKeyCheck)
	ON_EN_CHANGE(IDC_EDITKEYNAME, OnChangeEditKeyName)
	ON_EN_CHANGE(IDC_EDITKEYSERIAL, OnChangeEditKeySerial)
	ON_BN_CLICKED(IDC_BUTTONHELP, OnButtonhelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetLicenceDlg message handlers

BOOL CGetLicenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	GotoDlgCtrl(GetDlgItem(IDC_EDITNAME));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGetLicenceDlg::OnPaint() 
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
HCURSOR CGetLicenceDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGetLicenceDlg::OnChangeEditName() 
{
	
	// TODO: Add your control notification handler code here
	
}

// This section effectivly auto-tabs to the next section of
// the licence when the current section has been filled in
void CGetLicenceDlg::OnChangeEditKeyProd() 
{
	CString prod;
	m_editkeyprod.GetWindowText(prod);
	if (prod.GetLength() > 2)
	{
		NextDlgCtrl();
	}
}

void CGetLicenceDlg::OnChangeEditKeyName() 
{
	CString name;
	m_editkeyname.GetWindowText(name);
	if (name.GetLength() > 4)
	{
		NextDlgCtrl();
	}
}

void CGetLicenceDlg::OnChangeEditKeySerial() 
{
	CString serial;
	m_editkeyserial.GetWindowText(serial);
	if (serial.GetLength() > 4)
	{
		NextDlgCtrl();
	}
}

void CGetLicenceDlg::OnChangeEditKeyCheck() 
{
	CString check;
	m_editkeycheck.GetWindowText(check);
	if (check.GetLength() > 1)
	{
		NextDlgCtrl();
	}
}

void CGetLicenceDlg::OnOK() 
{
	CString username, keyprod, keyname, keyserial, keycheck, completekey;
	m_editname.GetWindowText(username);
	m_editkeyprod.GetWindowText(keyprod);
	m_editkeyname.GetWindowText(keyname);
	m_editkeyserial.GetWindowText(keyserial);
	m_editkeycheck.GetWindowText(keycheck);

	completekey = "";
	completekey = keyprod + "-" + keyname + "-" + keyserial + "-" + keycheck;

	// Check key is entered correctly
	if (isLicenceFormatCorrect(keyprod, keyname, keyserial, keycheck))
	{
		// Check key is valid
		std::string usr = username;
		std::string com = completekey;
		if (CTL_ValidKey((char*)usr.c_str(), PRODCODE, (char*)com.c_str()))
		{
			MessageBox("The license key is valid.", "Licence", MB_ICONINFORMATION | MB_OK);
			setRegForProduct(completekey, username);
			setRegForInstaller();
			CDialog::OnOK();
		}
		else
		{
			MessageBox("The license key is not valid.", "Licence", MB_ICONINFORMATION | MB_OK);
		}
	}	
}

// This section does some basic checking that the licence key has been entered
// in the correct format.
bool CGetLicenceDlg::isLicenceFormatCorrect(CString &keyprod, CString &keyname, CString &keyserial, CString &keycheck)
{
	int length;
	int i;
	char ch;
	bool anyErrors = FALSE;
	CString message;

	// Product key section
	if (!anyErrors)
	{
		length = keyprod.GetLength();
		// Check length
		if (length != 3)
		{
			MessageBox("The first section of the licence key should be 3 characters.",
					   "Licence", MB_ICONINFORMATION | MB_OK);
			GotoDlgCtrl(GetDlgItem(IDC_EDITKEYPROD));
			anyErrors = TRUE;
		}
		else
		{
			// Check the first section is all numbers
			for (i = 0; i < length; i++)
			{
				ch = keyprod.GetAt(i);
				if (((int)ch < 48) || ((int)ch > 57))
				{
					int num = i + 49;
					char numchar = (char)num;
					MessageBox("The character at position " + (CString)numchar + " in the first\n" +
							  "section of the licence key is not a valid number.",
							  "Licence", MB_ICONINFORMATION | MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_EDITKEYPROD));
					anyErrors = TRUE;
					break;
				}
			}
		}
	}

	// Name section
	if (!anyErrors)
	{
		length = keyname.GetLength();
		// Check length
		if (length != 5)
		{
			MessageBox("The second section of the licence key should be 5 characters.",
					   "Licence", MB_ICONINFORMATION | MB_OK);
			GotoDlgCtrl(GetDlgItem(IDC_EDITKEYNAME));
			anyErrors = TRUE;
		}
		else
		{
			for (i = 0; i < (length - 1); i++)
			{
				ch = keyname.GetAt(i);
				if (((int)ch < 48) || ((int)ch > 57))
				{
					int num = i + 49;
					char numchar = (char)num;
					MessageBox("The character at position " + (CString)numchar + " in the second\n" +
								  "section of the licence key is not a valid number.", 
								  "Licence", MB_ICONINFORMATION | MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_EDITKEYNAME));
					anyErrors = TRUE;
					break;
				}
			}
			if (!anyErrors)
			{
				ch = keyname.GetAt(length-1);
				if (((int)ch < 65) || ((int)ch > 90))
				{
					int num = (length - 1) + 49;
					char numchar = (char)num;
					MessageBox("The character at position " + (CString)numchar + " in the second\n" +
							   "section of the licence key is not a valid letter.", 
							   "Licence", MB_ICONINFORMATION | MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_EDITKEYNAME));
					anyErrors = TRUE;
				}
			}
		}
	}

	// Serial section
	if (!anyErrors)
	{
		length = keyserial.GetLength();
		// Check length
		if (length != 5)
		{
			MessageBox("The third section of the licence key should be 5 characters.", 
					   "Licence", MB_ICONINFORMATION | MB_OK);
			GotoDlgCtrl(GetDlgItem(IDC_EDITKEYSERIAL));
			anyErrors = TRUE;
		}
		else
		{
			for (i = 0; i < (length - 1); i++)
			{
				ch = keyserial.GetAt(i);
				if (((int)ch < 48) || ((int)ch > 57))
				{
					int num = i + 49;
					char numchar = (char)num;
					MessageBox("The character at position " + (CString)numchar + " in the third\n" +
							   "section of the licence key is not a valid number.", 
							   "Licence", MB_ICONINFORMATION | MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_EDITKEYSERIAL));
					anyErrors = TRUE;
					break;
				}
			}
			if (!anyErrors)
			{
				ch = keyserial.GetAt(length-1);
				if (((int)ch < 65) || ((int)ch > 90))
				{
					int num = (length - 1) + 49;
					char numchar = (char)num;
					MessageBox("The character at position " + (CString)numchar + " in the third\n" +
							   "section of the licence key is not a valid letter.", 
							   "Licence", MB_ICONINFORMATION | MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_EDITKEYSERIAL));
					anyErrors = TRUE;
				}
			}
		}
	}

	// Checksum section
	if (!anyErrors)
	{
		length = keycheck.GetLength();
		// Check length
		if (length != 2)
		{
			MessageBox("The last section of the licence key should be 2 characters.", 
					   "Licence", MB_ICONINFORMATION | MB_OK);
			GotoDlgCtrl(GetDlgItem(IDC_EDITKEYCHECK));
			anyErrors = TRUE;
		}
		else
		{
			// Check the first section is all numbers
			for (i = 0; i < length; i++)
			{
				ch = keycheck.GetAt(i);
				if (((int)ch < 65) || ((int)ch > 90))
				{
					int num = i + 49;
					char numchar = (char)num;
					MessageBox("The character at position " + (CString)numchar + " in the last\n" +
							   "section of the licence key is not a valid letter.", 
							   "Licence", MB_ICONINFORMATION | MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_EDITKEYCHECK));
					anyErrors = TRUE;
					break;
				}
			}
		}
	}

	return !anyErrors;
}

void CGetLicenceDlg::setRegForProduct(CString &completekey, CString &username)
{
	InstallLib::Registry prodReg("Software\\Cyberlife Technology\\CECAOS\\1.0");
	prodReg.CreateValue("License", (std::string)completekey, HKEY_LOCAL_MACHINE);
	prodReg.CreateValue("User", (std::string)username, HKEY_LOCAL_MACHINE);
}

void CGetLicenceDlg::setRegForInstaller()
{
	InstallLib::Registry installReg("Software\\Creature Labs\\NSIS");
	installReg.CreateValue("State", (std::string)"ok", HKEY_LOCAL_MACHINE);
}

void CGetLicenceDlg::OnButtonhelp() 
{
	MessageBox((CString)"The licence key should be entered in the following format:\n\n" +
			   "N = Number\n" +
			   "L = Letter\n\n" +
			   "NNN-NNNNL-NNNNL-LL", "Licence", MB_ICONINFORMATION | MB_OK);
}
