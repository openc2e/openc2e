#ifndef GeneAgeDlg_H
#define GeneAgeDlg_H
#include "DlgWnd.h"
#include "Resource.h"
#include "BrainViewport.h"

class GeneAgeDlg : public DlgWnd
{
protected:
	
	LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	BOOL On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode);
		
private:

	int &myGeneAge;
	const HWND myOwner;

	
public:
	GeneAgeDlg(const HINSTANCE hInstance, const HWND hWndOwner, int &GeneAge);
	~GeneAgeDlg();
};
#endif //ExperimentSetup_H

