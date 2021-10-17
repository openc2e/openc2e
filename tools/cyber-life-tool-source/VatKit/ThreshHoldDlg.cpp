#include "ThreshHoldDlg.h"
#include "math.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

HWND ThreshHoldDlg::ourHWnd = NULL;

ThreshHoldDlg::ThreshHoldDlg(const HINSTANCE hInstance, const HWND hWndOwner, BrainViewport::ThreshHoldStyle &threshHoldStyle, float &threshHold) :  
myThreshHoldStyle(threshHoldStyle),
myThreshHold(threshHold),
myOwner(hWndOwner)
{

	if(ourHWnd)
	{
		BringWindowToTop( ourHWnd );
		throw Wnd::FailConstructorException();
	}
	else
	{

		ourHWnd = myHWnd = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_DIALOGTHRESH), hWndOwner, (DLGPROC) Wnd::MsgHandler);
		SetRadio();

		SendMessage(GetDlgItem(myHWnd, IDC_SLIDER1), TBM_SETRANGE, (WPARAM) TRUE,(LPARAM) MAKELONG(0, 1000)); 
		SendMessage(GetDlgItem(myHWnd, IDC_SLIDER1), TBM_SETPAGESIZE, 0, (LPARAM) 1);
		SendMessage(GetDlgItem(myHWnd, IDC_SLIDER1), TBM_SETTICFREQ, (WPARAM) 25, (LPARAM)0);
		SendMessage(GetDlgItem(myHWnd, IDC_SLIDER1), TBM_SETPOS, (WPARAM) TRUE, (int)(500+(threshHold*500)));

		GetSlider();

		ShowWindow(myHWnd, SW_SHOW);
		UpdateWindow(myHWnd);
	}
};



ThreshHoldDlg::~ThreshHoldDlg()
{
	ourHWnd = NULL;
}



LRESULT ThreshHoldDlg::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	case WM_HSCROLL:
		GetSlider();
		break;

	case WM_CLOSE:
		delete this;

	default:
		// call default superclass handler
		return(DlgWnd::WndProc(hwnd, msg, wParam, lParam));	

	}
	return TRUE;
};



BOOL ThreshHoldDlg::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{
	switch(id)
	{
	case IDC_RADIONOTHRESH:
		myThreshHoldStyle = BrainViewport::NO_THRESHHOLD;
		SetRadio();
		break;
	case IDC_RADIONONZERO:
		myThreshHoldStyle = BrainViewport::NON_ZERO;
		SetRadio();
		break;
	case IDC_RADIOABOVETHRESH:
		myThreshHoldStyle = BrainViewport::ABOVE_THRESHHOLD;
		SetRadio();
		break;
	case IDC_RADIOBELOWTHRESH:
		myThreshHoldStyle = BrainViewport::BELOW_THRESHHOLD;
		SetRadio();
		break;

	case IDOK:
		delete this;
		break;

	default:
		return FALSE;

	}
	return TRUE;
};




void ThreshHoldDlg::SetRadio()
{
	Button_SetState(GetDlgItem(myHWnd, IDC_RADIONOTHRESH), BST_UNCHECKED);
	Button_SetState(GetDlgItem(myHWnd, IDC_RADIONONZERO), BST_UNCHECKED);
	Button_SetState(GetDlgItem(myHWnd, IDC_RADIOABOVETHRESH), BST_UNCHECKED);
	Button_SetState(GetDlgItem(myHWnd, IDC_RADIOBELOWTHRESH), BST_UNCHECKED);

	switch(myThreshHoldStyle)
	{
	case BrainViewport::NO_THRESHHOLD:
		Button_SetState(GetDlgItem(myHWnd, IDC_RADIONOTHRESH), BST_CHECKED);
		break;
	case BrainViewport::NON_ZERO:
		Button_SetState(GetDlgItem(myHWnd, IDC_RADIONONZERO), BST_CHECKED);
		break;
	case BrainViewport::ABOVE_THRESHHOLD:
		Button_SetState(GetDlgItem(myHWnd, IDC_RADIOABOVETHRESH), BST_CHECKED);
		break;
	case BrainViewport::BELOW_THRESHHOLD:
		Button_SetState(GetDlgItem(myHWnd, IDC_RADIOBELOWTHRESH), BST_CHECKED);
		break;
	}
}


void ThreshHoldDlg::GetSlider()
{
	myThreshHold = ((float)(-2.0f)+(500.0f + (float)SendMessage(GetDlgItem(myHWnd, IDC_SLIDER1), TBM_GETPOS, 0, 0))/500.0f);
	char buffer[10];
	sprintf(buffer, "%1.3f", myThreshHold);
	SendMessage(GetDlgItem(myHWnd, IDC_THRESHLABEL), WM_SETTEXT, 0, (LPARAM)buffer);
	RedrawWindow(myOwner, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
}

