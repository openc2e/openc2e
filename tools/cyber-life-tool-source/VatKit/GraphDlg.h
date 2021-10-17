#ifndef GraphDlg_H
#define GraphDlg_H
#include "Wnd.h"
#include "../../common/Grapher.h"
#include "Tips.h"
#include <cstring>


class GraphDlg : public Grapher, public Wnd
{	
public:


	GraphDlg(HINSTANCE hInstance, HWND hWndOwner);
	HWND Create();

protected:

	Grapher myGraph;
	std::vector<GraphTrace> myTestTrace;
	int myITime;

	void Reset(void);
	void ChangeSpec(int historySize, int sampleRate, int traceSize, std::string names[], std::string labels[]);
	void PostSample();
	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	
private:

	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];
	int myHistorySize;
	Tips *myTips;

	void On_Paint();
	void TipsTrace(HWND hwnd, int x, int y, UINT keyflags);
};

#endif

