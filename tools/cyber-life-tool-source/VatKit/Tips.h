#ifndef Tips_H
#define Tips_H

#include <string>
#include "Resource.h"
#include "Wnd.h"

class Tips : public Wnd
{
	enum { border = 2};

public:
	Tips(HINSTANCE hInstance, HWND hWndOwner);
	Tips(HINSTANCE hInstance, HWND hWndOwner, UINT uShowDelay, UINT uHideDelay);
	~Tips();

	void PopUp( LPCTSTR label, POINT pos, bool adjustToCursor = true, bool sticky = false, bool frameColour = false, COLORREF colFrame = 0);
	void GoAway();

	virtual HWND Create();

	bool myVisible;

protected:

	LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);


private:

	HWND myOwnerHWnd;

	std::string	myLabel;
	POINT	myPos;
	bool	myAdjustToCursor;
	bool	mySticky;

	bool	myFrameColour;
	COLORREF myColFrame;
	HFONT myFont;

	UINT	myTimerID;
	UINT	myShowDelay;
	UINT	myHideDelay;



	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];

	void DefineWindow(HINSTANCE hInstance, HWND hWndOwner);
	void On_Paint();
	void On_Timer(HWND hwnd, UINT nIDEvent);
	void EmptyRectangle(HDC hdc, int x1, int y1, int x2, int y2);

	char *GetNextLine(char *start, char *veryEnd, int minLineLength)
	{
		if(strlen(start) <= minLineLength)
			return veryEnd;
		else
		{
			char *nextGap = strchr(start+minLineLength, 32);
			return (nextGap == NULL) ? veryEnd : nextGap+1;
		}
	};

};

#endif

