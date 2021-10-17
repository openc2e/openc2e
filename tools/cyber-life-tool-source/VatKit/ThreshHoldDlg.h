#ifndef ThreshHoldDlg_H
#define ThreshHoldDlg_H
#include "DlgWnd.h"
#include "Resource.h"
#include "BrainViewport.h"

class ThreshHoldDlg : public DlgWnd
{
protected:
	
	LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	BOOL On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode);
		
private:

	BrainViewport::ThreshHoldStyle &myThreshHoldStyle;
	float &myThreshHold;
	const HWND myOwner;

	void GetSlider();
	void SetRadio();
	
	static HWND ourHWnd;

public:
	ThreshHoldDlg(const HINSTANCE hInstance, const HWND hWndOwner, BrainViewport::ThreshHoldStyle &threshHoldStyle, float &threshHold);
	~ThreshHoldDlg();
};
#endif //ExperimentSetup_H

