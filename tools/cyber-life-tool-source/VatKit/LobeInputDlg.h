#ifndef InputEdit_H
#define InputEdit_H

#include "BrainDlg.h"
#include "../../engine/Creature/Brain/BrainAccess.h"
#include "LobeNames.h"
#include "Wnd.h"

class BrainDlgContainer;


class LobeInputDlg : public Wnd, public BrainAccess, public BrainDlg
{	
public:

	LobeInputDlg(HINSTANCE hInstance, HWND hWndOwner, Lobe &lobe, LobeNames& lobeNames, BrainDlgContainer &container);
	~LobeInputDlg();
	HWND Create();

	void Activate();
	const void *LinkedTo(){return (void *)myLobe;};
	void DumpToLobe();



private:
	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];
	float *myInputValues;
	char **myNeuronNames;
	int myCurrentSel;
	HFONT myFont;

	
	BrainDlgContainer &myContainer;
	Lobe &myLobe;
	LobeNames &myLobeNames;
	HWND myHWndInputList, myHWndTrackbarLabel, myHWndInputTrack, myHWndCloseButton;
	
	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	void On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode);
	void On_ScrollTrackbar();
	void SetTrackbar(float val);

};

#endif

