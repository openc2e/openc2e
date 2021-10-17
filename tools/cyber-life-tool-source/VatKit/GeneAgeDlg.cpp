#include "GeneAgeDlg.h"
#include "math.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

GeneAgeDlg::GeneAgeDlg(const HINSTANCE hInstance, const HWND hWndOwner, int &GeneAge) :  
myGeneAge(GeneAge),
myOwner(hWndOwner)
{


	myHWnd = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_DIALOG_GENEAGE), hWndOwner, (DLGPROC) Wnd::MsgHandler);

	std::string text;
	text = "Embryo";
	ListBox_AddString(GetDlgItem(myHWnd, IDC_AGE_LIST), text.c_str());
	ListBox_AddString(GetDlgItem(myHWnd, IDC_AGE_LIST), "Child");
	ListBox_AddString(GetDlgItem(myHWnd, IDC_AGE_LIST), "Adolescent");
	ListBox_AddString(GetDlgItem(myHWnd, IDC_AGE_LIST), "Youth");
	ListBox_AddString(GetDlgItem(myHWnd, IDC_AGE_LIST), "Adult");
	ListBox_AddString(GetDlgItem(myHWnd, IDC_AGE_LIST), "Old");
	ListBox_AddString(GetDlgItem(myHWnd, IDC_AGE_LIST), "Senile");

//	ComboBox_SetDroppedHeight(GetDlgItem(myHWnd, IDC_COMBO_GENEAGE), 1000);

	if(myGeneAge < 0 || myGeneAge > 6)
		myGeneAge = 0;

	ListBox_SetCurSel(GetDlgItem(myHWnd, IDC_AGE_LIST), myGeneAge);

	
	ShowWindow(myHWnd, SW_SHOW);
	UpdateWindow(myHWnd);
};



GeneAgeDlg::~GeneAgeDlg()
{
}



LRESULT GeneAgeDlg::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{

	case WM_COMMAND:
		return HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	case WM_INITDIALOG:
		return TRUE;
		break;

	case WM_CLOSE:
		delete this;

	default:
		// call default superclass handler
		return(DlgWnd::WndProc(hwnd, msg, wParam, lParam));	

	}
	return TRUE;
};



BOOL GeneAgeDlg::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	switch(id)
	{

	case IDOK:
		myGeneAge = ListBox_GetCurSel(GetDlgItem(myHWnd, IDC_AGE_LIST));
		PostQuitMessage(1);
		break;

	default:
		return FALSE;

	}
	return TRUE;
};


