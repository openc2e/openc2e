#ifndef LobeGraphDlg_H
#define LobeGraphDlg_H

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <vector>

#include "Wnd.h"
#include "../../engine/Creature/Brain/BrainAccess.h"
#include "BrainDlg.h"
#include "GraphDlg.h"
#include "LobeNames.h"

class BrainDlgContainer;

class LobeGraphDlg : public BrainAccess, public GraphDlg, public BrainDlg
{	
public:

	LobeGraphDlg(HINSTANCE hInstance, HWND hWndOwner, Lobe &lobe, int var, LobeNames &lobeNames, BrainDlgContainer &container);
	~LobeGraphDlg();
	HWND Create();
	void Activate() {SetActiveWindow(myHWnd);};
	void Update();
	const void *LinkedTo(){return (void *)myLobe;};
	bool Updatable() {return true;};


private:

	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];
	Lobe &myLobe;
	int myVariable;
	LobeNames &myLobeNames;
	BrainDlgContainer &myContainer;

	LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	void On_RButtonDown(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags);
	void On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode);

};

#endif

